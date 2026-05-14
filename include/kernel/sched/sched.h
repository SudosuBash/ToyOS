#ifndef _TOYOS_SCHED_H
#define _TOYOS_SCHED_H

#include <kernel/task/task.h>
#include <kernel/stdint.h>
#include <kernel/base/linklist.h>

#define TASK_RUNNING_STAT 1
#define TASK_BLOCKED_SLEEP_STAT 2
#define TASK_SIGNAL_SLEEP_STAT 3

#define SCHED_TIMER_IRQ_VRUNTIME 2000
#define SCHED_LEVEL_L1 10
#define SCHED_LEVEL_L2 20
#define SCHED_LEVEL_L3 30
#define SCHED_LEVEL_L4 40
#define SCHED_LEVEL_L5 50

struct task_struct;
struct scheduler;

typedef uint8_t task_stat_t;

struct sched_class {
    struct task_struct* (*task_sched_next_task)(struct scheduler* sched);
    void (*sched_init)(struct scheduler* sched);
    void (*task_sched_enqueue)(struct scheduler* sched, struct task_struct* t);
    void (*task_fork_enqueue)(struct scheduler* sched, struct task_struct* task);
    void (*task_smp_enqueue)(struct scheduler* sched,struct task_struct* task);
    /* task status trigger */
    void (*task_r_to_ss)(struct task_struct *task);
    void (*task_ss_to_r)(struct task_struct *task);
    void (*task_r_to_bs)(struct task_struct *task);
    void (*task_bs_to_r)(struct task_struct *task);
};

struct scheduler {
    struct linklist_head s_sibling;
    struct sched_class s_class;
    uint64_t s_count;
    uint16_t s_level;
} __attribute__((aligned(64)));;



void sched_task_switch_stat(struct task_struct *task, task_stat_t new_stat);
struct task_struct* pick_next_task();
void register_scheduler(struct scheduler* sched, struct sched_class sc, uint16_t level) ;
void init_scheduler();

void register_idle();
#endif