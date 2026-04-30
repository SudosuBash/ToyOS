#include <kernel/task/exec.h>
#include <kernel/task/task.h>
#include <kernel/ptable/ptable.h>
#include <kernel/mm/mmap.h>
#include <kernel/mm/mm.h>
#include <kernel/syscall/syscall.h>


void do_exec(int (*fn)(void*)) {
    struct task_struct* current = CURRENT_PROCESS();
    pgd_t* new_pgd = alloc_pgd();
    current->mm_user.pg_root = new_pgd;

    struct linklist_head *task_lnk;
    struct user_vm_area *target_vma;
    list_for_entry(&current->mm_user.vm_area_link, task_lnk) {
        target_vma = container_of(task_lnk, struct user_vm_area, head);
        remove_from_vma(target_vma, &current->mm_user);
        destroy_vma(&target_vma);
    }

    // kfree(pgd);

    void* stack = kmalloc(2 * PAGE_SZ);
    pgd_t* kern_pgd = get_pgd(0);
    copy_pgd(new_pgd, kern_pgd);
    
    do_mmap((void*)KERN_VADDR_TO_PADDR(fn), (void*)0x400000, 4096 * 10, 0, PERM_X);
    do_mmap((void*)VADDR2PHYS(stack), (void*)USER_STACK_POS, 2 * PAGE_SZ, 0, PERM_W);
    
    arch_do_exec(fn, current);
}