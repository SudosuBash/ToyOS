#include <kernel/mm/mmap.h>
#include <pgtable/pgtable_kern.h>
#include <kernel/mm/mm.h>
#include <kernel/task/task.h>
#include <kernel/task/mm_user.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mm_page.h>
#include <asm.h>

static struct pagetable_64* user_set_pgd(struct pagetable_64* pagetable) {
    struct pagetable_64* pgtable = alloc_pgd();
    uint64_t vaddr = (uintptr_t)(pgtable);
    pagetable->present = 1;
    pagetable->rw = 1;
    pagetable->us = 1;
    pagetable->base_addr = VADDR2PHYS(vaddr) >> PAGE_OFFSET;
    return (struct pagetable_64*)vaddr;
}

struct pagetable_64* get_user_pte(uint64_t addr, pgd_t* pgd, uint64_t flag) {
    uint64_t index = PML4_OF(addr);
    pdpt_t* pdpt_entry;
    
    if(!pgd[index].present) {
        pdpt_entry = user_set_pgd(&pgd[index]);
        
    } else {
        uint64_t base_addr = PHYS2VADDR((uint64_t)pgd[index].base_addr << PAGE_OFFSET);
        pdpt_entry = (pdpt_t*)(base_addr);
    }
    if(flag & FLAG_KERN_ONLY) pgd[index].us = 0;
    
    pde_t* pde_entry;
    index = PDPT_OF(addr);
    if(!pdpt_entry[index].present) {
        pde_entry = user_set_pgd(&pdpt_entry[index]);
    } else {
        uint64_t base_addr = PHYS2VADDR((uint64_t)pdpt_entry[index].base_addr << PAGE_OFFSET);
        pde_entry = (pde_t*)(base_addr);
    }   

    pte_t* pte_entry;
    index = PDE_OF(addr);
    if(!pde_entry[index].present) {
        pte_entry = user_set_pgd(&pde_entry[index]);
    } else {
        uint64_t base_addr = PHYS2VADDR((uint64_t)pde_entry[index].base_addr << PAGE_OFFSET);
        pte_entry = (pte_t*)(base_addr);
    }
    index = PTE_OF(addr);
    return &pte_entry[index];

} 

inline uintptr_t get_pte_paddr(uintptr_t vaddr, pte_t* pte) {
    return pte->base_addr << PAGE_OFFSET;
}

void do_pte_fast_mmap(
    void* paddr, 
    uint64_t flag,
    uint64_t prot,
    pte_t* pte) {
    
    uintptr_t addr = (uint64_t)paddr;
    if(prot & PERM_W) pte->rw = 1;
    if(prot & PERM_X == 0) pte->nx = 1;
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
    void* paddr,
    void* vaddr,
    uint64_t sz,
    uint64_t flag,
    uint64_t prot
) {
    struct task_struct* current = CURRENT_PROCESS();
    pgd_t* pgd = current->mm_user.pg_root;
    uintptr_t addr = ((uint64_t)vaddr & PAGE_MASK);
    pte_t* pte;
    for(uintptr_t st = addr, pst = (uint64_t)paddr; st < addr + sz; st+=PAGE_SZ, pst += PAGE_SZ) {
        pte = get_user_pte(st, pgd, flag );
        uint64_t present = pte->present;
        pte->present = 1;
        pte->nx = 0;
        //这个每次查表的话，其实可以优化，但是优化起来的代码可读性很差，所以还是算了
        if(prot & PERM_W) pte->rw = 1;
        if(!(prot & PERM_X)) pte->nx = 1;
        if(!(flag & FLAG_KERN_ONLY)) pte->us = 1;
        pte->base_addr = (uint64_t)pst >> PAGE_OFFSET;
        
        struct page* pg = find_page_by_paddr(pst);
        struct page* head = find_head_page(pg);
        if(head == NULL) {
            crash("Aiee, attempt to mmap the reserved area!");
        }
        pg->page_flags |= MM_BUDDY_FLAG_MMAP;
        ref_page(head);
        if(present) {
            barrier();
            invlpg(st);
        }
    }
    return (void*)(addr & PAGE_MASK);
}

pgd_t* alloc_pgd() {
    pgd_t* pgd = (pgd_t*)kmalloc(sizeof(pgd_t) * PAGE_PTE_ENTRIES);
    memset(pgd,0,sizeof(pgd_t) * PAGE_PTE_ENTRIES);
    return pgd;
}