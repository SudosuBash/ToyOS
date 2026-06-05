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

static struct pid_nr glob_nr;
static struct pid idle_pid = {
    .layer= {{0}}
};
static uint64_t root_bitmap[CONFIG_GROUP_MAX_CNT>>3] = {0};

DEFINE_PERCPU_VAR(idle_percpu, struct task_struct);
DEFINE_PERCPU_VAR(current_process, struct task_struct*);
DEFINE_PERCPU_VAR(user_rsp, uintptr_t);
DEFINE_PERCPU_VAR(kernel_rsp, uintptr_t);

/**
 * 共享资源: Idle 的 PID
 * 独有资源: Idle 的 PCB
 */

static void init_idle_pid() {
    glob_nr.pid_bitmap.size = CONFIG_GROUP_MAX_CNT;
    uint64_t blocks = BITMAP_BLOCKS(&glob_nr.pid_bitmap);
    glob_nr.pid_bitmap.need_space = blocks * sizeof(uint64_t);
    bitmap_bit_to_1(&glob_nr.pid_bitmap, 0);
    glob_nr.pid_bitmap.start_addr = root_bitmap;

    bitmap_bit_to_1(&glob_nr.pid_bitmap, 0);
    hlist_init(&glob_nr.task_hash);

    idle_pid.layer[0].nr = &glob_nr;
    idle_pid.layer[0].pid_num = 0;

    init_pid_ns_layer(&idle_pid.layer[0]);
    idle_pid.pid_layers = 1;
    hlist_insert(&idle_pid.layer[0].nr->task_hash, &idle_pid.layer[0].sibling, 0);
}

static void init_userspace_mm() {
    struct task_struct* idle = THIS_CPU_PTR(idle_percpu);
    INIT_LIST_HEAD(&idle->mm_user.vm_area_link);
    rwlock_init(&idle->mm_user.rwlock);
    idle->mm_user.vm_area_root.rb_node = NULL;
    idle->mm_user.brk = 0;
}

void init_task_cpu() {
    struct task_struct* idle = THIS_CPU_PTR(idle_percpu);
    SET_THIS_CPU_VAR(current_process,idle);
    struct task_struct* current_task = THIS_CPU_VAR(current_process);
    current_task->kstack = alloc_page(2);
    INIT_LIST_HEAD(&idle->sibling);
    barrier();
    rwlock_init(&idle->rwlock);
    init_userspace_mm();
    init_scheduler();
    idle->pid = &idle_pid;
    idle->status = TASK_RUNNING_STAT;
    memcpy(idle->name, "Idle Process", sizeof("Idle Process"));
}

void init_task() {
    init_idle_pid();
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