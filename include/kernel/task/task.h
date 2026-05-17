#ifndef _TOYOS_TASK
#define _TOYOS_TASK

#include <kernel/sched/sched.h>
#include <kernel/base/linklist.h>
#include <kernel/cpu/smp.h>
#include <kernel/task/pid.h>
#include <kernel/mm/mm_user.h>
#include <kernel/file/file.h>
#include <kernel/atomic/rwlock.h>
#include <kernel/config.h>

#define CURRENT_PROCESS get_current_process
#define TASK_RET_FROM_FORK_FLAG 0b1

#define TASK_KERNEL_THREAD_FLAG 0b10
#define TASK_KERNEL_THREAD_MASK ~TASK_KERNEL_THREAD_FLAG
#define TASK_RET_FROM_FORK_MASK ~TASK_RET_FROM_FORK_FLAG

#define TASK_MAX_FILES 512

struct user_file_struct {
    uint32_t ufile_index;
    struct file* files[TASK_MAX_FILES];
};

struct task_struct {
    char name[30];
    void* kstack; 

    struct linklist_head sibling;
    struct mm_user mm_user;
    struct user_file_struct file_user;
    
    struct pid* pid;
    struct scheduler* scheduler;
    uint64_t status;
    uintptr_t ksp; //内核的sp
    uintptr_t usp; //用户sp, 用于内核抢占
    uint64_t flags; //标志位
    uint64_t vruntime; 
    uint64_t last_runtime; //物理层面的上次运行开始时间
#if defined(CONFIG_EEVDF)
    uint64_t vdeadtime;
    struct rb_node rb_node;
    uint64_t request_time;
    uint64_t min_vdeadtime;
#endif
    rwlock_t rwlock;
    int8_t nice_level;
}__attribute__((aligned(64)));

struct task_struct* get_current_process();
extern void ret_from_fork();
void schedule();
void init_task();
struct task_struct* kernel_thread(int (*fn)(void*), void* args, char* name);
uintptr_t arch_process_stack_top(struct task_struct* task);
#endif