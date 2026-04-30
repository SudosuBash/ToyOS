#include <kernel/file/dir.h>
#include <kernel/file/path.h>
#include <kernel/fault/errno.h>
#include <kernel/task/task.h>
#include <kernel/stdlib.h>
#include <kernel/data_struct/htable.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/fault/error.h>

static struct htable_list dir_cache;
static struct directory dir; 
static struct vfs_inode root_node = {
    .f_blk_count = 0,
    .f_blk_off = 0,
    .f_create_time = 0,
    .f_flag = DIR_TYPE_DIR
};

//任何节点都是一个 directory 对象
//

DEFINE_PERCPU_VAR(dir_node_allocator, struct kmem_cache*)

static inline void __gen_node_path(struct directory* parent, struct directory* child) {
    size_t d_prefix_len = strlen(parent->d_prefix);
    size_t d_name_len = strlen(parent->d_name);
    memcpy(child->d_prefix, child->d_prefix, d_prefix_len);
    parent->d_prefix[d_prefix_len] = '/';
    memcpy(child->d_prefix + d_prefix_len + 1, parent->d_name, d_name_len);
}

struct directory* create_dir_node(struct directory* dir, struct vfs_inode* inode, char* name) {
    struct directory* new_dir = kmem_cache_alloc(dir_node_allocator);
    if(IS_ERR(new_dir)) 
        return ERR_PTR(new_dir);
        
    new_dir->d_flag = DIR_FLAG_LOADED;
    new_dir->d_inode = inode;
    INIT_LIST_HEAD(&new_dir->d_sibling);
    INIT_LIST_HEAD(&new_dir->d_children);
    new_dir->d_parent = dir;
    memcpy(new_dir->d_name, name, strlen(name));
    __gen_node_path(dir, new_dir);
    
    list_insert(new_dir, &dir->d_children);
    return new_dir;
}

struct directory* dir_cache_find(char* path, uint64_t hash) {
    struct linklist_head* h;
    list_for_entry(&dir_cache.bucket[hash], h) {
        struct directory* d = directory_of(h);
        char* fname = path_file_name(path);
        if(strcmp(fname, d->d_name) && strcmp_l(path, d->d_prefix, (size_t)(fname - path - 1))) {
            return d;
        }
    }
    return NULL;
}

inline void dir_cache_add(uint64_t hash, struct directory* dir) {
    hlist_insert(&dir_cache, &dir->d_sibling, hash);
}

struct directory* find_path_dir_from_root(char* path) {
    return find_path_dir(&dir, path);
}
void dir_init() {

    hlist_init(&dir_cache);
    dir_cache_add(hlist_calc_hash('/'), &dir); //把根目录加入缓存
}