#ifndef _TOYOS_KERN_FILE
#define _TOYOS_KERN_FILE

#include <kernel/vfs/vfs.h>
#include <kernel/stdint.h>
#include <kernel/atomic/atomic.h>
#include <kernel/kernel.h>

//打开文件表
struct file {
    struct directory* f_dir;
    atomic_t f_ref;

    uint64_t f_modified_time;
    uint64_t f_create_time;
    uint64_t f_read_time;
    uint64_t f_flag;
    uint64_t f_seek_pos;
    
    uint16_t f_perm;
};

int64_t do_open(char* name);
int64_t do_write(int fd, void* buf, size_t len);
struct file* alloc_file(struct directory* dir);
void init_file_env();
#endif