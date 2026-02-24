#ifndef _TOYOS_TASK
#define _TOYOS_TASK

#include <sched/sched.h>
#include <regs.h>
struct task_struct {
    struct sched_class se_class; //调度器
    char* name;
    void* kstack; 
    reg_t context;
};
#endif