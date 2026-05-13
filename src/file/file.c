#include <kernel/stdint.h>
#include <kernel/file/file.h>
#include <kernel/file/dir.h>
#include <kernel/task/task.h>
#include <kernel/fault/error.h>
#include <kernel/atomic/spinlock.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/mm/mm.h>

//long的话便于传递ERR_PTR指针, 返回 fd
DEFINE_PERCPU_VAR(percpu_file_allocator, struct kmem_cache);

int64_t do_open(char* path) {
    struct task_struct* current = CURRENT_PROCESS();
    struct directory* dir = find_path_dir(NULL, path);

    if(IS_ERR(dir))
        return ERR(dir);

    if(dir->d_oper.do_open == NULL) 
        return ERR(EDRVERR);

    struct file* file = dir->d_oper.do_open(dir);
    if(IS_ERR(file))
        return ERR(file);
    
    atomic_inc(&file->f_ref);

    //这个场景使用读写锁, rcu 的话开销太大
    rwlock_write_lock(&current->rwlock);
    uint64_t index = current->file_user.ufile_index;
    current->file_user.ufile_index+= 1;
    current->file_user.files[index] = file;
    rwlock_write_unlock(&current->rwlock);

    return index;
}

int64_t do_write(int fd, void* buf, size_t len) {
    struct task_struct* current = CURRENT_PROCESS();
    struct file* file;
    if(fd >= TASK_MAX_FILES)
        return ERR(ENOEXT);
    
    int64_t stat;
    rwlock_read_lock(&current->rwlock);
    file = current->file_user.files[fd];
    if(file->f_dir->d_oper.do_write == 0) {
        rwlock_read_unlock(&current->rwlock);
        return ERR(EDRVERR);
    }
    stat = file->f_dir->d_oper.do_write(file, buf, len);
    rwlock_read_unlock(&current->rwlock);
    return stat;
}

int32_t do_mkdir(char* name, uint64_t flag) {
    return 0;
}

struct file* alloc_file(struct directory* dir) {
    struct kmem_cache* file_allocator = THIS_CPU_PTR(percpu_file_allocator);
    struct file* fd = kmem_cache_alloc(file_allocator, GFP_KERNEL);

    if(IS_ERR(fd))
        return ERR_PTR(fd);

    fd->f_create_time = dir->d_inode->f_create_time;
    fd->f_modified_time = dir->d_inode->f_last_modified_time;
    fd->f_read_time = dir->d_inode->f_read_time;

    atomic_set(&fd->f_ref, 0);
    fd->f_dir = dir;

    return fd;
}

void init_file_env() {
    struct kmem_cache* file_allocator = THIS_CPU_PTR(percpu_file_allocator);
    kmem_cache_init(file_allocator, sizeof(struct file));
}