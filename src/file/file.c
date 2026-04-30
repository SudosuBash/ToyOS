#include <kernel/stdint.h>
#include <kernel/file/file.h>

uint64_t do_open(char* name) {
    struct task_struct* current = CURRENT_PROCESS();
    
}

stat_t do_write(int fd, void* buf, size_t len) {

}

stat_t do_mkdir(char* name, uint64_t flag) {
    
}