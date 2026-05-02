#include <kernel/file/dir.h>
#include <kernel/fault/errno.h>
#include <kernel/task/task.h>
#include <kernel/stdlib.h>
#include <kernel/base/htable.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/fault/error.h>
#include <kernel/cpu/smp.h>
#include <kernel/atomic/spinlock.h>

static struct htable_list dir_cache;
static struct directory root_dir; 
static struct vfs_inode root_node = {
    .f_blk_count = 0,
    .f_blk_off = 0,
    .f_create_time = 0,
    .f_ftype = DIR_TYPE_DIR,
    .f_flag = 0
};
static rwlock_t dir_lock;

static struct kmem_cache* dir_node_allocator;
//任何节点都是一个 directory 对象

DEFINE_PERCPU_VAR(dir_node_percpu_allocator, struct kmem_cache)


struct directory* create_dir_node(char* name, struct directory* dir, struct vfs_inode* inode) {
    struct directory* new_dir = kmem_cache_alloc(dir_node_allocator);
    if(IS_ERR(new_dir)) 
        return ERR_PTR(new_dir);

    rwlock_init(&new_dir->d_lock);
    new_dir->d_inode = inode;
    new_dir->d_sub_dirs = 0;
    INIT_LIST_HEAD(&new_dir->d_sibling);
    new_dir->d_parent = &dir->d_sibling;
    memset(new_dir->d_inode->f_name, 0, sizeof(new_dir->d_inode->f_name));
    //为了防止这个f_name和d_prefix不清空造成的内核敏感数据泄露, 还是需要清空的
    memcpy(new_dir->d_inode->f_name, name, strlen(name));

    rwlock_write_lock(&dir->d_lock);
    dir->d_sub_dirs++;
    rwlock_write_unlock(&dir->d_lock);

    dir_cache_add(new_dir);
    return new_dir;
}

//直接删了得了, 这个只能删除最后一层的节点, 否则拒绝删除
stat_t* del_dir_node(struct directory* *dir) {

    rwlock_write_lock(&(*dir)->d_lock);
    if((*dir)->d_sub_dirs != 0) {
        rwlock_write_unlock(&(*dir)->d_lock);
        return ERR_PTR(ENOEMP);
    }
    list_del_init(&(*dir)->d_sibling);
    rwlock_write_unlock(&(*dir)->d_lock);

    (*dir)->d_parent = NULL;
    kmem_cache_free(*dir);
    *dir = NULL;
    return 0;
}

struct directory* dir_cache_find(char* name, struct directory* dir) {
    struct linklist_head* h;
    uint64_t hash = hlist_calc_hash((uint64_t)dir);
    list_for_entry(&dir_cache.bucket[hash], h) {
        struct directory* d = directory_of(h);
        if(strcmp(name, d->d_inode->f_name) == 0 && d->d_parent == dir) {
            return d;
        }
    }
    return NULL;
}

inline void dir_cache_add(struct directory* dir) {
    uint64_t hash = hlist_calc_hash((uint64_t)dir->d_parent); //directory 指针
    hlist_insert_rcu(&dir_cache, &dir->d_sibling, hash);
}

static void __init_root_dir() {
    INIT_LIST_HEAD(&root_dir.d_sibling);
    rwlock_init(&root_dir.d_lock);
    root_dir.d_parent = &root_dir.d_sibling;
    root_dir.d_inode = &root_node;
    smp_wmb(); //全 cpu 可见
}

static struct directory* __find_path_dir(struct directory* dir, char* name) {
    struct directory* target = dir_cache_find(name, dir);
    if(target == NULL) {
        struct vfs_inode* in = dir->d_inode->f_op.vfs_get_subdir(dir->d_inode, name);
        if(in == NULL)
            return ERR_PTR(ENOEXT);
        target = create_dir_node(name, dir, in);
    }
    
    return target;
}

struct directory* find_path_dir(struct directory* dir, char* path) {
    struct directory* root;
    char* pend;

    pend = path + strlen(path);
    if(dir == NULL)
        dir = &root_dir;

    if(path[0] == '/') {
        dir = &root_dir;
        path++; //忽略第一个字符
        if(path == pend)  //root?
            return dir;
    }

    while(pend-path != 1 && *(pend-1) == '/')
        *(pend--) = 0;

    root = dir;
    
    for(char* p = path; p < pend; p++) {
        char fname[FILE_DIR_NAME_MAX] = {0};
        char* fp = fname;

        if(!(root->d_inode->f_ftype & DIR_TYPE_DIR))
            return ERR_PTR(ENOEXT);

        while(*p && *p!='/') 
            *(fp++) = *(p++);

        root = __find_path_dir(root,fname);
        if(IS_ERR(root)) 
            return root;
    }

    dir_cache_add(root);
    return root;
}

void init_dir_module() {
    dir_node_allocator = THIS_CPU_PTR(dir_node_percpu_allocator);
    kmem_cache_init(dir_node_allocator, sizeof(struct kmem_cache));

    __init_root_dir();
    hlist_init(&dir_cache);
    smp_wmb();
    dir_cache_add(&root_dir); //把根目录加入缓存
}