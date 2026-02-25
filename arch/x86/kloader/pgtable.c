#include <pgtable.h>
#include <kernel/stdint.h>

//申请的pte数量(base_addr)
static void* pte_start_addr;
static void* pde_start_addr;
#define ALLOC_NEW_PTE() ((pte_start_addr) += PAGE_SZ)

struct pagetable_64 pg_default = {
    .present = 0,
    .rw = 1,
    .us = 0,
    .pwt = 0,
    .pcd = 0,
    .a = 0,
    .d = 0,
    .g = 0,
    .avl_1 = 0,
    .base_addr = 0,
    .avl_2 = 0,
    .nx = 0
};

static void link_addr(uint64_t paddr,uint64_t vaddr, uint8_t big_page) {
    uint64_t pml4_index = PML4_OF(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);

    struct pagetable_64* pml4_tab = GET_ENTRY_TABLE(pde_start_addr, pml4_index);
    
    if(pml4_tab->present == 0) {
        *pml4_tab = pg_default;
        pml4_tab->present = 1;
        pml4_tab->base_addr = (uintptr_t)(pte_start_addr) >> 12;
        ALLOC_NEW_PTE();
    }
    uintptr_t pnum = ((uint64_t)pml4_tab->base_addr << 12);
    struct pagetable_64* pdpt_tab = GET_ENTRY_TABLE(pnum, pdpt_index);
    if(pdpt_tab->present == 0) {
        *pdpt_tab = pg_default;
        pdpt_tab->present = 1;
        pdpt_tab->base_addr = (uintptr_t)(pte_start_addr) >> 12;
        ALLOC_NEW_PTE();
    }
    pnum = ((uint64_t)pdpt_tab->base_addr << 12);
    if(!big_page) {
        struct pagetable_64* pde_tab = GET_ENTRY_TABLE(pnum, pde_index);
        if(pde_tab->present == 0) { 
            *pde_tab = pg_default;
            pde_tab->present = 1;
            pde_tab->base_addr = (uintptr_t)(pte_start_addr) >> 12;
            ALLOC_NEW_PTE();
        }

        pnum = ((uint64_t)pde_tab->base_addr << 12);
        struct pagetable_64* pte_tab = GET_ENTRY_TABLE(pnum, pte_index);
        *pte_tab = pg_default;
        pte_tab->present = 1;
        pte_tab->base_addr = paddr >> 12;
    } else {
        struct pagetable_64* pde_tab = GET_ENTRY_TABLE(pnum, pde_index);
        *pde_tab = pg_default;
        pde_tab->present = 1;
        pde_tab->ps = 1; //2MB大页
        pde_tab->base_addr = paddr >> 21;
    }
}

void link_new_pte_addr(uint64_t paddr, uint64_t vaddr) {
    link_addr(paddr,vaddr,0);
}

void link_new_pte_bigpage_addr(uint64_t paddr,uint64_t vaddr) {
    link_addr(paddr,vaddr,1);
}


void prepare_pde(void* pde_start) { //准备最基本的页表
    pde_start_addr = pde_start;
    for(int i=0;i<KERNEL_PTE_ENTRIES;i++) {
        WRITE_PTE_TO_ADDR(pde_start, pg_default);
        pde_start += sizeof(struct pagetable_64);
    }
    pte_start_addr = pde_start;
    pte_start_addr += sizeof(struct pagetable_64) * KERNEL_PTE_ENTRIES;
}