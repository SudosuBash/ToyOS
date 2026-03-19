#include <kernel/task/fork.h>
#include <kernel/task/task.h>
#include <kernel/task/task_manager.h>
#include <kernel/mm/mm.h>
#include <kernel/stdlib.h>
#include <kernel/put.h>
#include <kernel/task/mm_user.h>
#include <kernel/ptable/ptable.h>
#include <kernel/mm/mmap.h>
#include <kernel/data_struct/bitmap.h>

static void dup_task_struct(struct task_struct* dst,struct task_struct* src) {
    *dst = *src;
}

void copy_mm_user(struct task_struct* task, struct task_struct* old, uint64_t flag) {
    if(old->mm_user.pg_root == NULL) //父线程就是内核线程, return
        return;
    
    if(flag & CLONE_THREAD) //内核线程
        return;
    
    task->mm_user.pg_root = alloc_pgd();
    INIT_LIST_HEAD(&task->mm_user.vm_node);
    // hlist_init(&task->user_area.vm_node);
    
    struct linklist_head* current;
    list_for_entry(&old->mm_user.vm_node,current) {
        struct user_vm_area* old_area = container_of(current, struct user_vm_area, sibling);
        arch_set_mm_user(task, old, old_area);
    }
}

static void copy_thread(
    struct task_struct* task, 
    struct task_struct* origin,
    struct arch_regs* regs, 
    uint64_t flag,
    char* name ) {
    memcpy(task->name, name, 30);
    if(flag & CLONE_THREAD) {
        task->flags |= TASK_KERNEL_THREAD_FLAG;
    }
    task->rest_time = SCHED_RR_TS;
    INIT_LIST_HEAD(&task->sibling);
    task->kstack = kmalloc(PAGE_SZ * PROCESS_STACK_PAGE);
    arch_dup_thread(task, origin, regs, flag);
}


static void copy_process(struct arch_regs* regs, uint64_t flag, char* name) {
    struct cpu_task_manager* manager = get_cpu_manager();
    
    struct task_struct* current = CURRENT_PROCESS();
    struct task_struct* new_task = kmalloc(sizeof(struct task_struct));
    
    dup_task_struct(new_task, current);

    copy_mm_user(new_task, current, flag);
    copy_thread(new_task, current, regs, flag, name);

    alloc_pid(new_task, flag);
    manager->class.task_enqueue(new_task);
}

static void do_fork(struct arch_regs* regs, uint64_t flag, char* name) {
    copy_process(regs, flag, name);
}

void clone(struct arch_regs* regs, uint64_t flag, char* name) {
    do_fork(regs, flag, name);
}

void fork() {
    struct task_struct* current = CURRENT_PROCESS();
    do_fork(NULL, 0, current->name);
    // do_fork(current->, 0, "");
}