#ifndef _TOYOS_SCHED_EEVDF
#define _TOYOS_SCHED_EEVDF

#include <kernel/sched/sched.h>
#include <kernel/base/rbtree.h>

#define EEVDF_FLAG_TASK_SELECTED 1

struct sched_eevdf {
    struct scheduler scheduler;
    
    uint64_t vcputime;
    uint64_t last_timestamp;
    struct rb_root run_task_queue;
    struct rb_root sleep_task_queue;
    uint64_t eevdf_sum_weigh;

    uint64_t e_flag;
}__attribute__((aligned(64)));

void register_eevdf();
void set_eevdf_sched(struct task_struct* task);
#endif