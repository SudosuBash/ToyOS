#ifndef _TOYOS_KERN_FILE
#define _TOYOS_KERN_FILE

#include <kernel/vfs/vfs.h>
#include <kernel/stdint.h>
#include <kernel/atomic/atomic.h>

#define FILE_DIR_NAME_MAX 256
#define FILE_DIR_PREFIX_MAX 4096

struct file;

struct file_operation {
    struct file* (*open)(char* path);
    void (*close)(struct file* file);
};


//打开文件表
struct file {
    struct file_operation f_ops;
    struct vfs_inode* f_inode;
    char f_name[FILE_DIR_NAME_MAX];
    char f_path[FILE_DIR_PREFIX_MAX];
    char f_suffix[FILE_DIR_NAME_MAX];
    atomic_t f_ref;

    uint64_t f_modified_time;
    uint64_t f_create_time;
    uint64_t f_read_time;
    uint64_t f_flag;
    uint64_t f_seek_pos;
    
    size_t f_fsize;
    uint16_t f_perm;
};

uint64_t do_open(char* name);
stat_t do_write(int fd, void* buf, size_t len);
#endif