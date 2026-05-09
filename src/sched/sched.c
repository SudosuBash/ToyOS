#include <kernel/sched/sched.h>
#include <kernel/cpu/smp.h>
#include <kernel/task/task_manager.h>
void sched_task_switch_stat(struct task_struct *task, task_stat_t new_stat) {
    preempt_disable();
    struct cpu_task_manager* m = get_cpu_manager();
    switch(new_stat) {
        case TASK_BLOCKED_SLEEP_STAT: {
            if(task->status == TASK_RUNNING_STAT) 
                m->class.swc.task_r_to_bs(task);
            task->status = TASK_BLOCKED_SLEEP_STAT;
        }
        case TASK_SIGNAL_SLEEP_STAT: {
            if(task->status == TASK_RUNNING_STAT) 
                m->class.swc.task_r_to_ss(task);
            task->status = TASK_SIGNAL_SLEEP_STAT;
        }

        case TASK_RUNNING_STAT: {
            if(task->status == TASK_SIGNAL_SLEEP_STAT)  
                m->class.swc.task_ss_to_r(task);
            else if(task->status == TASK_BLOCKED_SLEEP_STAT)
                m->class.swc.task_bs_to_r(task);
            task->status = TASK_RUNNING_STAT;
        }
    }
    preempt_enable();
}

void current_sched_task_switch_stat(task_stat_t new_stat) {
    preempt_disable();
    struct task_struct* current = CURRENT_PROCESS();
    sched_task_switch_stat(current, new_stat);
    preempt_enable();
}