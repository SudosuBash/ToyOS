#include <kernel/stdint.h>
#include <kernel/file/file.h>
#include <kernel/file/dir.h>
#include <kernel/task/task.h>
#include <kernel/fault/error.h>
#include <kernel/atomic/spinlock.h>

//long的话便于传递ERR_PTR指针, 返回 fd
int32_t* do_open(char* path) {
    struct task_struct* current = CURRENT_PROCESS();
    struct directory* dir = find_path_dir(NULL, path);

    if(IS_ERR(dir))
        return ERR_PTR(dir);

    struct file* file = dir->d_oper.do_open(dir);
    if(IS_ERR(file))
        return ERR_PTR(file);
    
    //这个场景使用读写锁, rcu 的话开销太大
    rwlock_write_lock(&current->rwlock);
    uint64_t index = current->file_user.ufile_index;
    current->file_user.ufile_index+= 1;
    current->file_user.files[index] = file;
    rwlock_write_unlock(&current->rwlock);

    return (int32_t*) &current->file_user.ufile_index;
}

stat_t do_write(int fd, void* buf, size_t len) {
    return 0;
}

stat_t do_mkdir(char* name, uint64_t flag) {
    return 0;
}