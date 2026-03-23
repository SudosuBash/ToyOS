#include <kernel/stdint.h>
#include <syscall/syscall.h>

syscall_fn* syscall_table;

void init_syscall() {
    extern uint64_t __syscall_table_addr_start;
    syscall_table =(syscall_fn*) &__syscall_table_addr_start;
    
}