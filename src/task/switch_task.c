#include <kernel/task/task.h>
#include <task/task.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/sched/sched.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/smp.h>
#include <kernel/asm/attribute.h>
#include <cpu/gdt.h>
#include <kernel/fault/fault.h>

DECLARE_PERCPU_VAR(current_process, struct task_struct*);
DECLARE_PERCPU_VAR(user_rsp, uintptr_t);
DECLARE_PERCPU_VAR(kernel_rsp, uintptr_t);
DECLARE_PERCPU_VAR(percpu_preempt_count, atomic_t);

void schedule() {
    struct scheduler* scheduler = pick_scheduler();
    assert(scheduler != NULL);

    atomic_t* preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    if(preempt_count->count != 0) //PREEMPT DISABLE
        return;

    struct task_struct* current = CURRENT_PROCESS();

    
    struct task_struct* tasks = scheduler->s_class.next_task(scheduler, current);
    scheduler->s_class.task_enqueue(scheduler, current);
    
    if(tasks != NULL) { //存在任务
        SET_THIS_CPU_VAR(current_process, tasks);
        SET_THIS_CPU_VAR(kernel_rsp, tasks->ksp);

        current->usp = THIS_CPU_VAR(user_rsp); //内核抢占用
        SET_THIS_CPU_VAR(user_rsp, tasks->usp);
        
        uint64_t flag = tasks->flags & TASK_KERNEL_THREAD_FLAG;
        tasks->flags &= TASK_KERNEL_THREAD_MASK;
        tasks->flags &= TASK_RET_FROM_FORK_MASK;

        uint64_t kstack_top = arch_process_stack_bottom(tasks);
        set_tss_rsp_r0(kstack_top);
        barrier();
        
        switch_to(current,tasks, flag);
    }
}
