#include <kernel/stdint.h>
#include <kernel/file/file.h>
#include <kernel/task/task.h>
#include <kernel/file/dir.h>

uint64_t do_open(char* path) {
    struct task_struct* current = CURRENT_PROCESS();
    struct directory* dir = find_path_dir_from_root(path);
    struct file* file = dir->d_oper.do_open(dir);
    return 0;
}

stat_t do_write(int fd, void* buf, size_t len) {
    return 0;
}

stat_t do_mkdir(char* name, uint64_t flag) {
    return 0;
}