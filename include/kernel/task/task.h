#ifndef _TOYOS_TASK
#define _TOYOS_TASK

#include <kernel/sched/sched.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/cpu/smp.h>

DECLARE_PERCPU_VAR(current_process, struct task_struct*);

#define CURRENT_PROCESS() THIS_CPU_VAR(current_process)

#define TASK_FORK_FLAG 0b1
#define TASK_FORK_MASK ~TASK_FORK_FLAG

#define TASK_KERNEL_THREAD_FLAG 0b10
#define TASK_KERNEL_THREAD_MASK ~TASK_KERNEL_THREAD_FLAG


#define CLONE_KERNEL_THREAD 0b1
struct task_struct {
    char* name;
    void* kstack; 

    struct linklist_head sibling;
    uintptr_t ksp; //内核的sp
    uint64_t flags; //标志位
    uint64_t rest_time; 
    uint64_t kid;
};

extern void kernel_thread_helper(
    void* args,
    int (*fn)(void*)
);
extern void ret_from_fork();
void schedule();
void init_task();
void kernel_thread(int (*fn)(void*), void* args, char* name);
#endif