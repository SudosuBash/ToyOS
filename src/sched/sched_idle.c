#include <kernel/sched/sched.h>
#include <kernel/cpu/smp.h>

DEFINE_PERCPU_VAR(sched, struct scheduler);
DECLARE_PERCPU_VAR(idle_percpu, struct task_struct);

static struct task_struct* idle_next_task(struct scheduler* sched) {
    struct task_struct* idle = THIS_CPU_PTR(idle_percpu);
    return idle;
}

static void idle_swc_stat_empty(struct task_struct* task) {
    return;
}

static void idle_enqueue_empty(struct scheduler* sched, struct task_struct* task) {
    return;
}

static void idle_nice_change_empty(struct scheduler* sched, struct task_struct* task, int8_t value) {
    return;
}

static void idle_sched_init(struct scheduler* sched) {
    struct task_struct* idle = THIS_CPU_PTR(idle_percpu);
    idle->scheduler = sched;
}

static struct sched_class idle_class = {
    .task_sched_next_task = idle_next_task,
    .task_r_to_s = idle_swc_stat_empty,
    .task_s_to_r = idle_swc_stat_empty,
    .task_fork_enqueue = idle_enqueue_empty,
    .task_smp_enqueue = idle_enqueue_empty,
    .task_sched_enqueue = idle_enqueue_empty,
    .sched_init = idle_sched_init,
    .task_nice_changed = idle_nice_change_empty
};

void register_idle() {
    struct scheduler* this_sched = THIS_CPU_PTR(sched);
    register_scheduler(this_sched, idle_class, SCHED_PRIO_LOOOOOW);
}