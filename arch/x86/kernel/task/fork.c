#include <kernel/task/fork.h>
#include <kernel/task/task.h>
#include <task/task.h>
#include <kernel/mm/mm.h>
#include <kernel/config.h>
#include <cpu/regs.h>
#include <cpu/gdt.h>
#include <pgtable/pgtable_kern.h>
#include <kernel/mm/mm_user.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mmap.h>
#include <kernel/mm/mm_page.h>
#include <kernel/ptable/ptable.h>
#include <kernel/cpu/archimpl.h>

extern void kernel_thread_helper(
    void* args,
    int (*fn)(void*)
);
//这个用来获取stack的栈顶
//因为有些架构的sp是向下的, 有些是向上的, 所以不能一概而论
inline uintptr_t arch_process_stack_bottom(struct task_struct* task) {
    return (uintptr_t)(task->kstack) + PAGE_SZ * PROCESS_STACK_PAGE;
}

//这个用来获取stack新进程应该指向的 sp 的位置(不是内核栈底!)
static inline uintptr_t arch_process_spp(struct task_struct* task) {
    return arch_process_stack_bottom(task) - STACK_OFFSET;
}

void arch_dup_thread(struct task_struct* task, struct task_struct* origin, struct arch_regs* regs, uint64_t flags) {
    /**
     * 用户态进来的话, regs 是 NULL
     * 内核态 fork 新线程的话(kernel_thread_helper), regs 是设定好的
    */

    struct arch_regs* new_task_stack = (struct arch_regs*)(arch_process_stack_bottom(task));
    //内核栈
    if(regs == NULL) 
        regs = (struct arch_regs*)(arch_process_stack_bottom(origin)); 
    //用户栈CoW, 内核栈重新分配

    if(flags & CLONE_THREAD) { 
        //因为最终返回到内核, 所以返回到的也是内核栈
        regs->rsp = (uintptr_t)new_task_stack;
    }
    new_task_stack -= 1;
    *new_task_stack = *regs;
    new_task_stack->rax = 0; //fork返回值为0
    task->ksp = (uintptr_t)(new_task_stack);
}

void kernel_thread(int (*fn)(void*), void* args, char* name) {
    struct arch_regs regs = {0};
    regs.rip = (uint64_t)kernel_thread_helper;
    regs.rsi = (uint64_t)fn;
    regs.rdi = (uint64_t)args;
    regs.cs = KERNEL_CS;
    regs.ss = KERNEL_DS;
    regs.eflags |= REG_EFLAGS_IF_BIT; //开中断
    clone(&regs, CLONE_THREAD, name);
}

static inline void set_cow(pte_t* old_pte, pte_t* new_pte) {
    old_pte->rw = 0;
    new_pte->rw = 0;
}

void arch_set_mm_user(
    struct task_struct* new_task, 
    struct task_struct* old_task,
    struct user_vm_area* target
) {
    
    rwlock_read_lock(&old_task->mm_user.rwlock);
    rwlock_read_lock(&new_task->mm_user.rwlock);
    pgd_t* old_pgd = old_task->mm_user.pg_root;
    pgd_t* new_pgd = new_task->mm_user.pg_root;
    rwlock_read_unlock(&new_task->mm_user.rwlock);
    rwlock_read_unlock(&old_task->mm_user.rwlock);

    for(uintptr_t p = target->mem_start;p < target->mem_end; p+=PAGE_SZ) {
        pte_t* old_pte = get_user_pte(p, old_pgd);
        pte_t* new_pte = get_user_pte(p, new_pgd);
        
        uintptr_t paddr = get_pte_paddr(p, old_pte);
        user_cow_remap((void*) paddr, new_pte);
        //对新页进行原有页的映射, 设置为和父页一样的地址
        set_cow(old_pte, new_pte);
        
        //后续会减少
        barrier();
        invlpg(p);
        //必须处理，否则父进程继续执行的话会出现问题
    }
}