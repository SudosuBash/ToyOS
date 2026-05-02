#ifndef _TOYOS_RQ
#define _TOYOS_RQ

#include <kernel/base/linklist.h>
#include <kernel/sched/sched.h>

struct cpu_task_manager {
    struct linklist_head task_head;
    struct sched_class class;
};
struct cpu_task_manager* get_cpu_manager();
void init_task_manager();
#endif