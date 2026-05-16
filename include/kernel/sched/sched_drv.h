#ifndef _TOYOS_SCHED_DRV_H
#define _TOYOS_SCHED_DRV_H

#include <kernel/base/linklist.h>
#include <kernel/sched/sched.h>
struct sched_fifo_queue {
    struct linklist_head head;
    struct linklist_head* tail;
};

struct sched_drv_fifo {
    struct sched_fifo_queue run_queue;
    struct linklist_head sleep;
    struct scheduler scheduler;
};

void switch_to_drv_sched(struct task_struct* task);
void register_drv_sched();
#endif