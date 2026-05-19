#include <kernel/task/task.h>
#include <kernel/sched/sched.h>
#include <kernel/config.h>
#include <kernel/mm/mm.h>
#include <kernel/task/fork.h>
#include <kernel/cpu/smp.h>
#include <kernel/irq/irq.h>
#include <kernel/sched/sched.h>
#include <kernel/timer/timer.h>
#include <kernel/task/pid.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mmap.h>
#include <kernel/base/htable.h>
#include <kernel/fault/error.h>
#include <kernel/stdint.h>
#include <kernel/syscall/syscall.h>

extern struct sched_class rr_se;
static struct pid_nr* glob_nr;
struct task_struct idle = {
    .vruntime = 0,
    .kstack = NULL,
    .name = "Idle Process",
    .status = TASK_RUNNING_STAT
};

DEFINE_PERCPU_VAR(current_process, struct task_struct*);
DEFINE_PERCPU_VAR(user_rsp, uintptr_t);
DEFINE_PERCPU_VAR(kernel_rsp, uintptr_t);

static void init_pid() {
    uint64_t pid_sz = sizeof(struct pid) + sizeof(struct pid_ns_layer);
    glob_nr = kmalloc(sizeof(struct pid_nr), GFP_KERNEL);
    struct pid* new = kmalloc(pid_sz, GFP_KERNEL);

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
    INIT_LIST_HEAD(&idle.mm_user.vm_area_link);
    rwlock_init(&idle.mm_user.rwlock);
    idle.mm_user.vm_area_root.rb_node = NULL;
    idle.mm_user.brk = 0;
}

void init_task() {
    SET_THIS_CPU_VAR(current_process,&idle);
    struct task_struct* current_task = THIS_CPU_VAR(current_process);
    extern void* __pid_0_stack_bottom;
    current_task->kstack = (void*)PHYS2VADDR(KERN_VADDR_TO_PADDR(&__pid_0_stack_bottom));
    
    INIT_LIST_HEAD(&idle.sibling);
    barrier();
    rwlock_init(&idle.rwlock);
    init_pid();
    init_userspace_mm();
    init_scheduler();
}

inline struct task_struct* get_current_process() {
    return THIS_CPU_VAR(current_process);
}

DEFINE_SYSCALL1(nice, value, int8_t) {
    if(value < -20 || value > 20)
        return ERR(EINVAL);
    preempt_disable();
    struct task_struct* current = CURRENT_PROCESS();
    current->scheduler->s_class.task_nice_changed(current->scheduler, current, value);
    preempt_enable();
    return 0;
}