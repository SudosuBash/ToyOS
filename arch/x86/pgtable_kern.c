#include <pgtable_kern.h>
#include <config_arch.h>
#include <stdint.h>
#include <stdlib.h>

#define PDE_OF(addr) ((addr) >> 22)
#define PTE_OF(addr) (((addr) >> 12) & (0x3FF))
#define GET_PDE_TABLE(pde_index) (struct pagetable*)(KERNEL_TEMP_PG_VADDR + (pde_index) * sizeof(struct pagetable))


//申请的pte数量(base_addr)
static void* pte_start_addr = 0;
static struct pagetable* pg_base_vaddr;
struct pagetable pdefault = {
    .present = 0,
    .rw = 1,
    .us = 0,
    .pwt = 1,
    .pcd = 0,
    .a = 0,
    .d = 0,
    .g = 0,
    .avl = 0,
    .base_addr = 0
};
#define ALLOC_NEW_PTE() {\
    memset(pte_start_addr,0,KERNEL_PG_SZ); \
    (pte_start_addr) += KERNEL_PG_SZ; \
    }

#define PAGE_PTE_OF(vaddr) ((0xffc00000) | (((vaddr) >> 12) << 2))
//获取页表的最佳方式


void link_new_pte_addr(uint32_t paddr, uint32_t vaddr, struct pagetable pgtab) {

    uint32_t pde_index = PDE_OF(vaddr);

    struct pagetable* pde_tab = GET_PDE_TABLE(pde_index);
    
    if(pde_tab->present == 0) {
        pde_tab->present = 1;
        pde_tab->base_addr = (uint32_t)(pte_start_addr) >> 12;
        ALLOC_NEW_PTE();
    }
    
    struct pagetable* pte_tab = (struct pagetable*)PAGE_PTE_OF(vaddr);
    //vaddr已经包含pte index
    *pte_tab = pgtab;
    pte_tab->present = 1;
    pte_tab->base_addr = paddr >> 12;
}

void unlink_pte_vaddr(uint32_t vaddr) {
    uint32_t pde_index = PDE_OF(vaddr);
    struct pagetable* pde_tab = GET_PDE_TABLE(pde_index);

    if(pde_tab->present == 0) return;
    struct pagetable* pte_tab = (struct pagetable*)PAGE_PTE_OF(vaddr);
    pte_tab->present = 0;
    pte_tab->base_addr = 0;
}

void init_pagetable(uint32_t kern_pg_addr_top, uint32_t pg_base_addr) {
    pg_base_vaddr = (struct pagetable*) pg_base_addr;
    pte_start_addr = (void*)kern_pg_addr_top;
}