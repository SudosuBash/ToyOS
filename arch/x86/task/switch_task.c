#include <kernel/task/task.h>
#include <task/task.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/task/task_manager.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/smp.h>
#include <kernel/asm/attribute.h>

DECLARE_PERCPU_VAR(current_process, struct task_struct*);

void schedule() {
    struct cpu_task_manager* manager = get_cpu_manager();
    struct task_struct* current = CURRENT_PROCESS();
    struct task_struct* tasks = manager->class.next_task();
    manager->class.task_enqueue(current);
    
    if(tasks != NULL) { //存在任务
        SET_THIS_CPU_VAR(current_process, tasks);
        //这percpu的set能给你出问题出到老家去

        barrier();
        tasks->flags &= TASK_FORK_MASK;
        tasks->flags &= TASK_KERNEL_THREAD_MASK;
        switch_to(current,tasks);
    }
}
