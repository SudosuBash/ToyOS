#include <kernel/task/task.h>
#include <kernel/sched/sched.h>
#include <kernel/config.h>
#include <kernel/mm/mm.h>
#include <kernel/task/fork.h>
#include <kernel/cpu/smp.h>
#include <kernel/irq/irq.h>
#include <kernel/task/task_manager.h>
#include <kernel/irq/timer.h>
#include <kernel/task/pid.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mmap.h>

extern struct sched_class rr_se;
static struct pid_nr* glob_nr;
static struct task_struct idle = {
    .rest_time = SCHED_RR_TS,
    .kstack = NULL,
    .name = "Idle Process"
};

DEFINE_PERCPU_VAR(current_process, struct task_struct*);

static void timer_irq_handler(struct arch_regs* frame) {
    schedule();
}

static void init_pid() {
    uint64_t pid_sz = sizeof(struct pid) + sizeof(struct pid_ns_layer);
    glob_nr = kmalloc(sizeof(struct pid_nr));
    struct pid* new = kmalloc(pid_sz);

    memset(glob_nr, 0, sizeof(struct pid_nr));

    init_new_pid_nr(glob_nr);
    new->layer[0].nr = glob_nr;
    new->layer[0].pid_num = 0;
    init_pid_ns_layer(&new->layer[0], &idle);
    new->pid_layers = 1;
    hlist_insert(&new->layer[0].nr->task_hash, &new->layer[0].sibling, 0);
    idle.pid = new;
}

static void init_userspace_mm() {
    idle.mm_user.pg_root = alloc_pgd();
    INIT_LIST_HEAD(&idle.mm_user.vm_node);
    idle.mm_user.brk = 0;
}

inline struct task_struct* find_by_pid(uint64_t pid) {
    struct pid_ns_layer* layer =  hlist_find(&glob_nr->task_hash, pid, pid_num, sibling, struct pid_ns_layer);
    if(layer == NULL) return NULL;
    return layer->target_task;
}

void init_task() {
    init_task_manager();
    SET_THIS_CPU_VAR(current_process,&idle);
    struct task_struct* current_task = THIS_CPU_VAR(current_process);
    extern void* __pid_0_stack_bottom;
    current_task->kstack = &__pid_0_stack_bottom;
    INIT_LIST_HEAD(&idle.sibling);
    barrier();
    init_pid();
    init_userspace_mm();
    timer_irq_register(timer_irq_handler);
}

inline struct task_struct* get_current_process() {
    return THIS_CPU_VAR(current_process);
}