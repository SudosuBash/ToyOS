#ifndef _TOYOS_SCHED_H
#define _TOYOS_SCHED_H

#include <kernel/task/task.h>
struct task_struct;
struct sched_class {
    void (*task_enqueue)(struct task_struct* t);
    struct task_struct* (*task_dequeue)();
    struct task_struct* (*next_task)();
    void (*sched_class_init)();
};
#endif