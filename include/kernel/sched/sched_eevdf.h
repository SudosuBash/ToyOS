#ifndef _TOYOS_SCHED_EEVDF
#define _TOYOS_SCHED_EEVDF

#include <kernel/sched/sched.h>
#include <kernel/base/rbtree.h>

struct sched_eevdf {
    struct scheduler scheduler;

    uint64_t vcputime;
    struct rb_root running_task;
    struct rb_root sleeping_task;
    uint64_t eevdf_sum_weigh;
};

#endif