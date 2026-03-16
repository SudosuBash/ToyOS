#include <kernel/task/fork.h>
#include <kernel/task/task.h>
#include <kernel/task/task_manager.h>
#include <kernel/mm/mm.h>
#include <kernel/stdlib.h>

static void dup_task_struct(struct task_struct* dst,struct task_struct* src) {
    *dst = *src;
}

static void copy_thread(
    struct task_struct* task, 
    struct task_struct* origin,
    struct arch_regs* regs, 
    uint64_t flag,
    char* name ) {
    memcpy(task->name, name, 20);
    task->flags |= TASK_FORK_FLAG; //打上 fork的标签
    if(flag & CLONE_KERNEL_THREAD) {
        task->flags |= TASK_KERNEL_THREAD_FLAG;
    }
    task->rest_time = SCHED_RR_TS;
    task->kid = 1; //暂时写死
    INIT_LIST_HEAD(&task->sibling);
    task->kstack = kmalloc(PAGE_SZ * PROCESS_STACK_PAGE);
    arch_dup_thread(task, origin, regs, flag);
}

static void copy_process(struct arch_regs* regs, uint64_t flag, char* name) {
    struct cpu_task_manager* manager = get_cpu_manager();
    struct task_struct* current = CURRENT_PROCESS();

    struct task_struct* new_task = kmalloc(sizeof(struct task_struct));
    
    dup_task_struct(new_task, current);

    copy_thread(new_task, current, regs, flag, name);
    manager->class.task_enqueue(new_task);
}

static void do_fork(struct arch_regs* regs, uint64_t flag, char* name) {
    copy_process(regs, flag, name);
}

void clone(struct arch_regs* regs, uint64_t flag, char* name) {
    do_fork(regs, flag, name);
}