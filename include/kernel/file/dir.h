#ifndef _TOYOS_KERN_DIR
#define _TOYOS_KERN_DIR

#include <kernel/stdint.h>
#include <kernel/vfs/vfs.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/file/file.h>

#define DIR_TYPE_MNT 1
#define DIR_TYPE_DIR 2
#define DIR_TYPE_FILE 4
#define DIR_TYPE_SOFTLNK 8
#define DIR_TYPE_HARDLNK 16
#define DIR_TYPE_SOCKET 32

#define DIR_FLAG_LOADED 1

#define directory_of(ele) container_of(ele, struct directory, d_sibling)

struct directory {
    char d_name[FILE_DIR_NAME_MAX];
    char d_prefix[FILE_DIR_PREFIX_MAX];

    struct linklist_head d_sibling;
    struct linklist_head d_children;
    struct linklist_head *d_parent;

    struct vfs_inode *d_inode;
    uint16_t d_flag;
};

struct directory* create_dir_node(struct directory* dir, struct vfs_inode* inode, char* name);
struct directory* dir_cache_find(char* path, uint64_t hash);
void dir_cache_add(uint64_t hash, struct directory* dir);
struct directory* find_path_dir_from_root(char* path);
#endif