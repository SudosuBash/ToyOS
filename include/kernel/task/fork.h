#ifndef _TOYOS_TASK_FORK
#define _TOYOS_TASK_FORK

#include <cpu/regs.h>
#include <kernel/task/task.h>
void arch_dup_thread(struct task_struct* task, struct task_struct* origin, struct arch_regs* regs, uint64_t flags);
void clone(struct arch_regs* regs, uint64_t flag, char* name);
#endif