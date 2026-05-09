#ifndef _TOYOS_SCHED_H
#define _TOYOS_SCHED_H

#include <kernel/task/task.h>
#include <kernel/stdint.h>
#define TASK_RUNNING_STAT 1
#define TASK_BLOCKED_SLEEP_STAT 2
#define TASK_SIGNAL_SLEEP_STAT 3

struct task_struct;

typedef uint8_t task_stat_t;

struct sched_task_swc {
    void (*task_r_to_ss)(struct task_struct *task);
    void (*task_ss_to_r)(struct task_struct *task);
    void (*task_r_to_bs)(struct task_struct *task);
    void (*task_bs_to_r)(struct task_struct *task);
};

struct sched_class {
    void (*task_enqueue)(struct task_struct* t);
    struct task_struct* (*task_dequeue)();
    struct task_struct* (*next_task)();
    void (*sched_class_init)();

    struct sched_task_swc swc;
};

void sched_task_switch_stat(struct task_struct *task, task_stat_t new_stat);
#endif