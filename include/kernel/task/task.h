#ifndef _TOYOS_TASK
#define _TOYOS_TASK

#include <kernel/sched/sched.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/cpu/smp.h>
#include <kernel/task/pid.h>
#include <kernel/task/mm_user.h>


#define CURRENT_PROCESS get_current_process
#define TASK_KERNEL_THREAD_FLAG 0b10
#define TASK_KERNEL_THREAD_MASK ~TASK_KERNEL_THREAD_FLAG

#define TASK_STATUS_RUNNING 0b1
#define TASK_STATUS_SLEEP 0b10

struct task_struct {
    char name[30];
    void* kstack; 

    uint64_t status;
    struct linklist_head sibling;
    uintptr_t ksp; //内核的sp
    uint64_t flags; //标志位
    uint64_t rest_time; 
    struct pid* pid;
    struct mm_user mm_user;
};

struct task_struct* get_current_process();
struct task_struct* find_by_pid(uint64_t pid);
void schedule();
void init_task();
void kernel_thread(int (*fn)(void*), void* args, char* name);
#endif