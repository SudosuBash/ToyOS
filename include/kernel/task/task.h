#ifndef _TOYOS_TASK
#define _TOYOS_TASK

#include <kernel/sched/sched.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/cpu/smp.h>
#include <kernel/task/pid.h>
#include <kernel/mm/mm_user.h>
#include <kernel/file/file.h>

#define CURRENT_PROCESS get_current_process
#define TASK_RET_FROM_FORK_FLAG 0b1

#define TASK_KERNEL_THREAD_FLAG 0b10
#define TASK_KERNEL_THREAD_MASK ~TASK_KERNEL_THREAD_FLAG
#define TASK_RET_FROM_FORK_MASK ~TASK_RET_FROM_FORK_FLAG
#define TASK_STATUS_RUNNING 0b1
#define TASK_STATUS_SLEEP 0b10

#define TASK_MAX_FILES 512

struct user_file_struct {
    uint64_t ufile_index;
    struct file files[TASK_MAX_FILES];
};

struct task_struct {
    char name[30];
    void* kstack; 

    struct linklist_head sibling;
    struct mm_user mm_user;
    struct user_file_struct file_user;
    
    struct pid* pid;
    uint64_t status;
    uintptr_t ksp; //内核的sp
    uintptr_t usp; //用户sp, 用于内核抢占
    uint64_t flags; //标志位
    uint64_t rest_time; 
};

struct task_struct* get_current_process();
extern void ret_from_fork();
void schedule();
void init_task();
void kernel_thread(int (*fn)(void*), void* args, char* name);
uintptr_t arch_process_stack_bottom(struct task_struct* task);
#endif