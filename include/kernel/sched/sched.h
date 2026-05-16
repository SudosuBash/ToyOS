#ifndef _TOYOS_SCHED_H
#define _TOYOS_SCHED_H

#include <kernel/task/task.h>
#include <kernel/stdint.h>
#include <kernel/base/linklist.h>

#define TASK_RUNNING_STAT 1
#define TASK_BLOCKED_SLEEP_STAT 2
#define TASK_SIGNAL_SLEEP_STAT 3

#define SCHED_TIMER_IRQ_VRUNTIME 2000

#define SCHED_PRIO_LOOOOOW 100
#define SCHED_PRIO_LOOW 50
#define SCHED_PRIO_LOW 40
#define SCHED_PRIO_MEDIUM 30
#define SCHED_PRIO_HIGH 20
#define SCHED_PRIO_HIIGH 10
#define SCHED_PRIO_DRV 9
#define SCHED_PRIO_HIIIIIGH 3

#define SCHED_FLAG_DRIVERTYPE 1
#define SCHED_FLAG_TEMPORATORY_REMOVED 2

struct task_struct;
struct scheduler;

typedef uint8_t task_stat_t;

struct sched_class {
    struct task_struct* (*task_sched_next_task)(struct scheduler* sched);
    void (*task_smp_dequeue)(struct scheduler* scheduler, struct task_struct* task);
    void (*sched_init)(struct scheduler* sched);
    void (*task_sched_enqueue)(struct scheduler* sched, struct task_struct* t);
    void (*task_fork_enqueue)(struct scheduler* sched, struct task_struct* task);
    void (*task_smp_enqueue)(struct scheduler* sched,struct task_struct* task);
    void (*task_nice_changed)(struct scheduler* sched, struct task_struct* task, int8_t value);
    /* task status trigger */
    void (*task_r_to_s)(struct task_struct *task);
    void (*task_s_to_r)(struct task_struct *task);
};

struct scheduler {
    struct linklist_head s_sibling;
    struct sched_class s_class;
    uint64_t s_count;
    uint64_t s_flag;
    uint16_t s_level;
    
} __attribute__((aligned(64)));;

void task_switch_stat(struct task_struct *task, task_stat_t new_stat);
void current_task_switch_stat(task_stat_t new_stat);
struct task_struct* pick_next_task();
void register_scheduler(struct scheduler* sched, struct sched_class sc, uint16_t level) ;
void init_scheduler();
void activate_driver_scheduler(struct scheduler* sched);
void register_idle();
#endif