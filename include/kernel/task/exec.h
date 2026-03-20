#ifndef _TOYOS_TASK_EXEC
#define _TOYOS_TASK_EXEC

#include <kernel/task/task.h>
void arch_do_exec(
    int (*fn)(void*),
    struct task_struct* target
);
void do_exec(int (*fn)(void*));
#endif