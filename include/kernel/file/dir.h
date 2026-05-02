#ifndef _TOYOS_KERN_DIR
#define _TOYOS_KERN_DIR

#include <kernel/stdint.h>
#include <kernel/vfs/vfs.h>
#include <kernel/base/linklist.h>
#include <kernel/file/file.h>
#include <kernel/atomic/rwlock.h>

#define DIR_TYPE_MNT 1
#define DIR_TYPE_DIR 2
#define DIR_TYPE_FILE 4
#define DIR_TYPE_SOFTLNK 8
#define DIR_TYPE_HARDLNK 16
#define DIR_TYPE_SOCKET 32

#define directory_of(ele) container_of(ele, struct directory, d_sibling)

struct directory;

struct dir_operation {
    struct file* (*do_open)(struct directory* dir);
    struct file* (*do_mkdir)(struct directory* dir, char* name);
    struct file* (*do_mount)(struct directory* root, struct directory* mount);
    stat_t* (*do_write)(struct file* file, void* data, size_t len);
    stat_t* (*do_read)(struct file* file);
    stat_t* (*do_umount)(struct directory* root);
    stat_t* (*do_close)(struct file* file);
};

struct directory {
    struct linklist_head d_sibling;
    struct linklist_head *d_parent;
    uint64_t d_sub_dirs;
    struct dir_operation d_oper; // 驱动程序需要完善的一组接口
    struct vfs_inode *d_inode;
    uint16_t d_flag;

    rwlock_t d_lock;
};

struct directory* create_dir_node(char* name, struct directory* dir, struct vfs_inode* inode);
struct directory* dir_cache_find(char* name, struct directory* dir);
void dir_cache_add(struct directory* dir);
stat_t* del_dir_node(struct directory* *dir);
void init_dir_module();
struct directory* find_path_dir(struct directory* dir, char* path);
#endif