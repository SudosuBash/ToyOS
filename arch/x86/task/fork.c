#include <kernel/task/fork.h>
#include <kernel/task/task.h>
#include <task/task.h>
#include <kernel/mm/mm.h>
#include <kernel/config.h>
#include <cpu/regs.h>
#include <cpu/gdt.h>

//这个用来获取stack的栈顶
//因为有些架构的sp是向下的, 有些是向上的, 所以不能一概而论
static inline uintptr_t arch_process_stack_bottom(struct task_struct* task) {
    return (uintptr_t)(task->kstack) + PAGE_SZ * PROCESS_STACK_PAGE;
}

//这个用来获取stack新进程应该指向的 sp 的位置(不是内核栈底!)
static inline uintptr_t arch_process_spp(struct task_struct* task) {
    return arch_process_stack_bottom(task) - STACK_OFFSET;
}

void arch_dup_thread(struct task_struct* task, struct task_struct* origin, struct arch_regs* regs, uint64_t flags) {
    //用户栈CoW, 内核栈重新分配
    struct arch_regs* new_task_stack = (struct arch_regs*)(arch_process_stack_bottom(task));
    if(flags & CLONE_KERNEL_THREAD) {
        regs->rsp = (uint64_t)new_task_stack;
    }

    new_task_stack -= 1;
    *new_task_stack = *regs;
    new_task_stack->rax = 0; //fork返回值为0

    task->ksp = (uintptr_t)(new_task_stack);
    task->ksp -= sizeof(uintptr_t);
    *(uintptr_t*)(task->ksp) = (uintptr_t)ret_from_fork;
    //这样ret后直接返回到ret_from_fork
    
    task->ksp -= sizeof(uintptr_t);
    *(uintptr_t*)(task->ksp) = (uintptr_t)task->ksp; //rbp
    //给rbp留的
}

void kernel_thread(int (*fn)(void*), void* args, char* name) {
    struct arch_regs regs = {0};
    regs.rip = (uint64_t)kernel_thread_helper;
    regs.rsi = (uint64_t)fn;
    regs.rdi = (uint64_t)args;
    regs.cs = KERNEL_CS;
    regs.ss = KERNEL_DS;
    regs.rflags |= REG_EFLAGS_IF_BIT; //开中断
    clone(&regs, CLONE_KERNEL_THREAD, name);
}