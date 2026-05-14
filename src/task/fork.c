#include <kernel/task/fork.h>
#include <kernel/task/task.h>
#include <kernel/sched/sched.h>
#include <kernel/stdlib.h>
#include <kernel/put.h>
#include <kernel/mm/mm_user.h>
#include <kernel/mm/mm.h>
#include <kernel/ptable/ptable.h>
#include <kernel/mm/mmap.h>
#include <kernel/syscall/syscall.h>
#include <kernel/def.h>
#include <kernel/sched/sched_eevdf.h>

extern struct task_struct idle;

static void dup_task_struct(struct task_struct* dst,struct task_struct* src) {
    *dst = *src;
}

static void copy_mm_user(struct task_struct* task, struct task_struct* old, uint64_t flag) {
    struct linklist_head* current;
    
    INIT_LIST_HEAD(&task->mm_user.vm_area_link); //必须在前面init, 否则内核线程直接无法初始化了, exec 会导致错误
    rwlock_read_lock(&task->mm_user.rwlock);
    if(old->mm_user.pg_root == NULL) {//父线程就是内核线程, return
        rwlock_read_unlock(&task->mm_user.rwlock);
        return;
    }
    
    if(flag & CLONE_THREAD) {//内核线程
        rwlock_read_unlock(&task->mm_user.rwlock);
        return;
    }
    
    rwlock_init(&task->rwlock);
    task->mm_user.pg_root = alloc_pgd();
    task->mm_user.vm_area_root.rb_node = NULL;//从空树开始插

    pgd_t* kern_pgd = get_pgd(0);
    copy_pgd(task->mm_user.pg_root, kern_pgd);

    list_for_entry(&old->mm_user.vm_area_link, current) {
        struct user_vm_area* old_area = container_of(current, struct user_vm_area, head);
        struct user_vm_area* area = copy_area(old_area);
        insert_into_vma(area, &task->mm_user);
        arch_set_mm_user(task, old, area);
    }
    
    rwlock_read_unlock(&task->mm_user.rwlock);
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
    task->flags |= TASK_RET_FROM_FORK_MASK;
    
    INIT_LIST_HEAD(&task->sibling);
    task->kstack = kmalloc(PAGE_SZ * PROCESS_STACK_PAGE, GFP_KERNEL);
#if defined(CONFIG_EEVDF)
    if(origin == &idle) { //idle 进程的 fork 直接变成 eevdf
        set_eevdf_sched(task);
    }
    task->vdeadtime = 0;
#endif
    arch_dup_thread(task, origin, regs, flag);
}


static void copy_process(struct arch_regs* regs, uint64_t flag, char* name) {
    preempt_disable();
    struct task_struct* current = CURRENT_PROCESS();

    struct task_struct* new_task = (struct task_struct*) kmalloc(sizeof(struct task_struct), GFP_KERNEL);
    
    dup_task_struct(new_task, current);

    copy_mm_user(new_task, current, flag);
    copy_thread(new_task, current, regs, flag, name);
    
    alloc_pid(new_task, flag);

    struct scheduler* se = new_task->scheduler;
    se->s_class.task_fork_enqueue(se, new_task);
    preempt_enable();
}

static void do_fork(struct arch_regs* regs, uint64_t flag, char* name) {
    copy_process(regs, flag, name);
}

void clone(struct arch_regs* regs, uint64_t flag, char* name) {
    do_fork(regs, flag, name);
}

DEFINE_SYSCALL(fork) {
    struct task_struct* current = CURRENT_PROCESS();
    do_fork(NULL, 0, current->name);
    return 0;
}