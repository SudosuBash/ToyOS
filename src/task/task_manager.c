#include <kernel/task/task_manager.h>
#include <kernel/cpu/smp.h>
#include <kernel/irq/timer.h>
DEFINE_PERCPU_VAR(task_manager, struct cpu_task_manager);

extern struct sched_class rr_se;
inline struct cpu_task_manager* get_cpu_manager() {
    return THIS_CPU_PTR(task_manager);
}

void init_task_manager() {
    struct cpu_task_manager* manager = THIS_CPU_PTR(task_manager);
    INIT_LIST_HEAD(&manager->task_head);
    manager->class = rr_se;
}
