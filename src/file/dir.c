#include <kernel/file/dir.h>
#include <kernel/file/path.h>
#include <kernel/fault/errno.h>
#include <kernel/task/task.h>
#include <kernel/stdlib.h>
#include <kernel/data_struct/htable.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/fault/error.h>
#include <kernel/cpu/smp.h>
#include <kernel/atomic/spinlock.h>

static struct htable_list dir_cache;
static struct directory dir; 
static struct vfs_inode root_node = {
    .f_blk_count = 0,
    .f_blk_off = 0,
    .f_create_time = 0,
    .f_ftype = DIR_TYPE_DIR,
    .f_flag = 0
};
static spinlock_t dir_lock;

static struct kmem_cache* dir_node_allocator;
//任何节点都是一个 directory 对象

DEFINE_PERCPU_VAR(dir_node_percpu_allocator, struct kmem_cache)

static inline void __gen_node_path(struct directory* parent, struct directory* child) {
    size_t d_prefix_len = strlen(parent->d_prefix);
    size_t d_name_len = strlen(parent->d_name);
    memcpy(child->d_prefix, parent->d_prefix, d_prefix_len);

    child->d_prefix[d_prefix_len] = '/';
    if(d_name_len == 0) 
        d_prefix_len --; //rootdir 创建文件的话, 直接覆盖'/'就好.
    memcpy(child->d_prefix + d_prefix_len + 1, parent->d_name, d_name_len);
    child->d_prefix[d_prefix_len+d_name_len+1] = 0; //字符串结尾
}

struct directory* create_dir_node(char* name, struct directory* dir, struct vfs_inode* inode) {
    struct directory* new_dir = kmem_cache_alloc(dir_node_allocator);
    if(IS_ERR(new_dir)) 
        return ERR_PTR(new_dir);

    new_dir->d_flag = DIR_FLAG_LOADED;
    new_dir->d_inode = inode;
    INIT_LIST_HEAD(&new_dir->d_sibling);
    INIT_LIST_HEAD(&new_dir->d_children);
    new_dir->d_parent = &dir->d_sibling;
    memset(new_dir->d_name, 0, sizeof(new_dir->d_name));
    memset(new_dir->d_prefix, 0, sizeof(new_dir->d_prefix));
    //为了防止这个d_name和d_prefix不清空造成的内核敏感数据泄露, 还是需要清空的
    memcpy(new_dir->d_name, name, strlen(name));
    __gen_node_path(dir, new_dir);
    
    list_insert(&new_dir->d_sibling, &dir->d_children);
    return new_dir;
}

//直接删了得了, 这个只能删除最后一层的节点, 否则拒绝删除
stat_t* del_dir_node(struct directory* *dir) {
    if(!list_empty(&(*dir)->d_children))
        return ERR_PTR(ENOEMP);
    list_del_init(&(*dir)->d_sibling);
    (*dir)->d_parent = NULL;
    kmem_cache_free(*dir);
    *dir = NULL;
    return 0;
}

struct directory* dir_cache_find(char* path, uint64_t hash) {
    struct linklist_head* h;
    list_for_entry(&dir_cache.bucket[hash], h) {
        struct directory* d = directory_of(h);
        char* fname = path_file_name(path);
        if(strcmp(fname, d->d_name) == 0 && strcmp_l(path, d->d_prefix, (size_t)(fname - path - 1)) == 0) {
            return d;
        }
    }
    return NULL;
}

inline void dir_cache_add(uint64_t hash, struct directory* dir) {
    hlist_insert_rcu(&dir_cache, &dir->d_sibling, hash);
}

struct directory* find_path_dir_from_root(char* path) {
    return find_path_dir(&dir, path);
}

static void __init_root_dir() {
    INIT_LIST_HEAD(&dir.d_children);
    INIT_LIST_HEAD(&dir.d_sibling);
    dir.d_parent = &dir.d_sibling;
    memset(dir.d_prefix, 0, sizeof(dir.d_prefix));
    memset(dir.d_name, 0, sizeof(dir.d_name));
    dir.d_prefix[0] = '/';
    dir.d_inode = &root_node;
    dir.d_flag = DIR_FLAG_LOADED;
    smp_wmb(); //全 cpu 可见
}

void init_dir_module() {
    dir_node_allocator = THIS_CPU_PTR(dir_node_percpu_allocator);
    kmem_cache_init(dir_node_allocator, sizeof(struct kmem_cache));

    __init_root_dir();
    hlist_init(&dir_cache);
    smp_wmb();
    dir_cache_add(hlist_calc_hash('/'), &dir); //把根目录加入缓存
}