#include <kernel/mm/mmap.h>
#include <kernel/mm/mm.h>
#include <kernel/task/task.h>
#include <kernel/mm/mm_user.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mm_page.h>
#include <kernel/syscall/syscall.h>
#include <kernel/fault/error.h>
#include <kernel/fault/fault.h>
#include <kernel/cpu/archimpl.h>

void do_pte_fast_mmap(
    void* paddr, 
    uint16_t flag,
    uint16_t prot,
    pte_t* pte) {
    
    uint64_t addr = (uint64_t)paddr;
    if(prot & PERM_W) pte->rw = 1;
    if(!(prot & PERM_X)) pte->nx = 1;
    if(!(flag & FLAG_KERN_ONLY)) pte->us = 0;
    pte->base_addr = addr >> PAGE_OFFSET;

    struct page* pg = find_page_by_paddr(addr);
    struct page* head = find_head_page(pg);
    if(head == NULL) {
        crash("Aiee, attempt to mmap the reserved area!");
    }
    pg->page_flags |= MM_BUDDY_FLAG_MMAP;
    ref_page(head);
    if(pte->present) {
        barrier();
        invlpg(addr);
    }
}

void* do_mmap(
    void* vaddr,
    uint64_t sz,
    uint16_t flag,
    uint16_t prot) {
        
    struct task_struct* current = CURRENT_PROCESS();
    uintptr_t addr = ((uint64_t)vaddr & PAGE_MASK);
    uintptr_t st = addr;
    uintptr_t ed = PAGE_ROUND_UP(addr + sz);
    struct user_vm_area* area = new_area(st, ed, flag, prot);
    insert_into_vma(area, &current->mm_user);
    return (void*)addr;
}

void* do_remap(
    void* paddr,
    void* vaddr,
    uint64_t sz,
    uint16_t prot
) {
    struct task_struct* current = CURRENT_PROCESS();
    pgd_t* pgd = current->mm_user.pg_root;
    uintptr_t addr = ((uint64_t)vaddr & PAGE_MASK);
    pte_t* pte;

    uintptr_t pst = ((uint64_t)paddr & PAGE_MASK), st = addr;
    uintptr_t ed = PAGE_ROUND_UP(addr + sz);

    for(; st < ed; st+=PAGE_SZ, pst += PAGE_SZ) {
        pte = get_user_pte(st, pgd);
        uint64_t present = pte->present;
        pte->present = 1;
        //这个每次查表的话，其实可以优化，但是优化起来的代码可读性很差，所以还是算了
        if(prot & PERM_W) pte->rw = 1;
        if(!(prot & PERM_X)) pte->nx = 1;
        //pte->us = 0;
        pte->us = 1; //后续完善内存分页机制后, 这个会改为 0, 现在临时用一下
        pte->base_addr = (uint64_t)pst >> PAGE_OFFSET;
        
        // struct page* pg = find_page_by_paddr(pst);
        // struct page* head = find_head_page(pg);
        // if(head == NULL) {
        //     crash("Aiee, attempt to remap the reserved area!");
        // }
        // pg->page_flags |= MM_BUDDY_FLAG_MMAP;
        // ref_page(head);
        if(present) {
            barrier();
            invlpg(st);
        }
    }
    return (void*)(addr & PAGE_MASK);
}


//这个页表是两套的, 内核态的就不应该变
//但是用户态怎么映射的话, 还得靠上面的 do_mmap 记录的区域来管理的
/**
 * 这个函数会将原来映射到 User Space 的区域全部收回来
 * 若没有映射到 User Space 的区域, 则选择不动
 * 查找 VMA 来直接回收 do_munmap 的内存
*/
void do_munmap(
    void* vaddr,
    size_t sz
) {
    struct task_struct* current = CURRENT_PROCESS();

    uint64_t st = (uint64_t) vaddr;
    uint64_t end = st + sz;
}

pgd_t* alloc_pgd() {
    pgd_t* pgd = (pgd_t*)kmalloc(sizeof(pgd_t) * PAGE_PTE_ENTRIES, GFP_ATOMIC);
    memset(pgd,0,sizeof(pgd_t) * PAGE_PTE_ENTRIES);
    return pgd;
}


DEFINE_SYSCALL4(mmap, vaddr, void*, sz, size_t, flag, uint16_t, prot, uint16_t) {
    return (long)do_mmap(vaddr, sz, flag, prot);
}

DEFINE_SYSCALL2(munmap, vaddr, void*, sz, size_t) {
    return 0;
}