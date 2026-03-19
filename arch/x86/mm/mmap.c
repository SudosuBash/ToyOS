#include <kernel/mm/mmap.h>
#include <pgtable/pgtable_kern.h>
#include <kernel/mm/mm.h>
#include <kernel/task/task.h>
#include <kernel/task/mm_user.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mm_page.h>

static struct pagetable_64* user_set_pgd(struct pagetable_64* pagetable) {
    struct pagetable_64* pgtable = alloc_pgd();
    pagetable->present = 1;
    pagetable->rw = 1;
    pagetable->us = 1;
    pagetable->base_addr = (uintptr_t)(pgtable) >> PAGE_OFFSET;
    return pagetable;
}

struct pagetable_64* get_user_pte(uint64_t addr, pgd_t* pgd) {
    uint64_t index = PML4_OF(addr);
    pdpt_t* pdpt_entry;
    if(!pgd[index].present) {
        pdpt_entry = user_set_pgd(&pgd[index]);
    } else {
        pdpt_entry = (pdpt_t*)((uint64_t)pgd[index].base_addr << PAGE_OFFSET);
    }
    
    pde_t* pde_entry;
    index = PDPT_OF(addr);
    if(!pdpt_entry[index].present) {
        pde_entry = user_set_pgd(&pdpt_entry[index]);
    } else {
        pde_entry = (pde_t*)((uint64_t)pdpt_entry[index].base_addr << PAGE_OFFSET);
    }   

    pte_t* pte_entry;
    index = PDE_OF(addr);
    if(!pde_entry[index].present) {
        pte_entry = user_set_pgd(&pde_entry[index]);
    } else {
        pte_entry = (pte_t*)((uint64_t)pde_entry[index].base_addr << PAGE_OFFSET);
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
    uintptr_t paddr_f = ((uint64_t)paddr & PAGE_MASK);
    pte_t* pte;
    for(uintptr_t st = addr; st < addr + sz; st+=PAGE_SZ) {
        pte = get_user_pte(st, pgd);
        pte->present = 1;
        //这个每次查表的话，其实可以优化，但是优化起来的代码可读性很差，所以还是算了
        if(prot & PERM_W) pte->rw = 1;
        if(!(prot & PERM_X)) pte->nx = 1;
        if(!(flag & FLAG_KERN_ONLY)) pte->us = 1;
        pte->base_addr = (uint64_t)paddr_f >> PAGE_OFFSET;
        paddr_f += PAGE_SZ;

        struct page* pg = find_page_by_vaddr(st);
        ref_page(pg);
    }
    return (void*)(addr & PAGE_MASK);
}

void unmask_pte_w(uintptr_t vaddr, pgd_t* pgd) {
    pte_t* pte = get_user_pte(vaddr,pgd);
    pte->rw = 0;
}

pgd_t* alloc_pgd() {
    pgd_t* pgd = (pgd_t*)kmalloc(sizeof(pgd_t));
    memset(pgd,0,sizeof(pgd_t));
    return pgd;
}