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

struct directory;

struct dir_operation {
    struct file* (*do_open)(struct directory* dir);
    struct file* (*do_mkdir)(struct directory* dir, char* name);
    struct file* (*do_mount)(struct directory* root, struct directory* mount);
    void (*do_umount)(struct directory* root);
    void (*do_close)(struct file* file);
};

struct directory {
    char d_name[FILE_DIR_NAME_MAX];
    char d_prefix[FILE_DIR_PREFIX_MAX];

    struct linklist_head d_sibling;
    struct linklist_head d_children;
    struct linklist_head *d_parent;

    struct dir_operation d_oper; // 一组接口
    struct vfs_inode *d_inode;
    uint16_t d_flag;
};

struct directory* create_dir_node(char* name, struct directory* dir, struct vfs_inode* inode);
struct directory* dir_cache_find(char* path, uint64_t hash);
void dir_cache_add(uint64_t hash, struct directory* dir);
struct directory* find_path_dir_from_root(char* path);
stat_t* del_dir_node(struct directory* *dir);

void init_dir_module();
#endif