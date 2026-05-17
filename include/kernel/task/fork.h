#ifndef _TOYOS_TASK_FORK
#define _TOYOS_TASK_FORK

#include <kernel/task/task.h>

struct arch_regs;

#define CLONE_THREAD 0b1
#define CLONE_NEW_PID_NS 0b10

void arch_set_mm_user(
    struct task_struct* new_task, 
    struct task_struct* old_task,
    struct user_vm_area* target
);

void arch_dup_thread(struct task_struct* task, struct task_struct* origin, struct arch_regs* regs, uint64_t flags);
struct task_struct* clone(struct arch_regs* regs, uint64_t flag, char* name);
#endif