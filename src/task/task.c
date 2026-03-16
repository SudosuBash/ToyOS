#include <kernel/task/task.h>
#include <kernel/sched/sched.h>
#include <kernel/config.h>
#include <kernel/mm/mm.h>
#include <kernel/task/fork.h>
#include <kernel/cpu/smp.h>
#include <kernel/irq/irq.h>
#include <kernel/task/task_manager.h>
#include <kernel/irq/timer.h>

extern struct sched_class rr_se;
static struct task_struct launchd = {
    .rest_time = SCHED_RR_TS,
    .kstack = NULL,
    .name = "Idle Process",
    .kid = 0
};

DEFINE_PERCPU_VAR(current_process, struct task_struct*);

static void timer_irq_handler(struct irq_frame* frame) {
    schedule();
}


void init_task() {
    init_task_manager();
    SET_THIS_CPU_VAR(current_process,&launchd);
    struct task_struct* current_task = THIS_CPU_VAR(current_process);

    extern void* __pid_0_stack_bottom;
    current_task->kstack = &__pid_0_stack_bottom;
    INIT_LIST_HEAD(&launchd.sibling);
    timer_irq_register(timer_irq_handler);
}