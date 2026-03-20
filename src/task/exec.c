#include <kernel/task/exec.h>
#include <kernel/task/task.h>
#include <kernel/ptable/ptable.h>
#include <kernel/mm/mmap.h>
#include <kernel/mm/mm.h>
#include <kernel/syscall/syscall.h>

//高半部分直接复制
static void copy_pgd(pgd_t* dest, pgd_t* src) {
    for(uint64_t p =(PAGE_PTE_ENTRIES >> 1); p < PAGE_PTE_ENTRIES; p++) {
        dest[p] = src[p];
    }
}

void do_exec(int (*fn)(void*)) {
    struct task_struct* current = CURRENT_PROCESS();
    pgd_t* new_pgd = alloc_pgd();
    current->mm_user.pg_root = new_pgd;
    struct linklist_head *task_lnk;
    // list_for_entry(&current->mm_user.vm_node, task_lnk) {
    //     // struct vma* v = container_of(task_lnk, );
        
    //     // kfree(task_lnk);
    //     //这儿要写释放逻辑, 代码先略
    //     //确保原来的代码已经释放了
    // }

    // kfree(pgd);
    void* stack = kmalloc(2 * PAGE_SZ);
    pgd_t* kern_pgd = get_pgd(0);
    copy_pgd(new_pgd, kern_pgd);
    do_mmap((void*)VADDR2PHYS(stack),(void*)USER_STACK_POS, PAGE_SZ*2, 0, PERM_W);
    do_mmap((void*)KERN_VADDR_TO_PADDR(fn), (void*)0x400000, PAGE_SZ*10, 0, PERM_X);

    // do_mmap((void*)KERN_VADDR_TO_PADDR(ret_to_user), ret_to_user, PAGE_SZ, 0, PERM_X);
    //跳板代码

    arch_do_exec(fn, current);
}