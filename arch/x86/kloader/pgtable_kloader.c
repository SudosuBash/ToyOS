#include <kloader/pgtable.h>
#include <stdint.h>

//申请的pte数量(base_addr)
static void* pte_start_addr = (void*)(KERNEL_TEMP_PG_ADDR + sizeof(struct pagetable_64) * KERNEL_PTE_ENTRIES);

#define SELF_PADDR 0x20000
#define SELF_SECTOR 3

#define ALLOC_NEW_PTE() ((pte_start_addr) += PAGE_SZ)
static struct pagetable_64 pagetable = {
    .present = 0,
    .rw = 1,
    .us = 0,
    .pwt = 1,
    .pcd = 0,
    .a = 0,
    .d = 0,
    .g = 0,
    .avl_1 = 0,
    .base_addr = 0,
    .avl_2 = 0,
    .nx = 0
};

void link_new_pte_addr(uint64_t paddr, uint64_t vaddr) {
    uint64_t pml4_index = PML4_OF(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);

    struct pagetable_64* pml4_tab = GET_ENTRY_TABLE(KERNEL_TEMP_PG_ADDR, pml4_index);
    
    if(pml4_tab->present == 0) {
        *pml4_tab = pagetable;
        pml4_tab->present = 1;
        pml4_tab->base_addr = (uint32_t)(pte_start_addr) >> 12;
        ALLOC_NEW_PTE();
    }
    struct pagetable_64* pdpt_tab = GET_ENTRY_TABLE((pml4_tab->base_addr << 12), pdpt_index);
    if(pdpt_tab->present == 0) {
        *pdpt_tab = pagetable;
        pdpt_tab->present = 1;
        pdpt_tab->base_addr = (uint32_t)(pte_start_addr) >> 12;
        ALLOC_NEW_PTE();
    }

    struct pagetable_64* pde_tab = GET_ENTRY_TABLE((pdpt_tab->base_addr << 12), pde_index);
    if(pde_tab->present == 0) {
        *pde_tab = pagetable;
        pde_tab->present = 1;
        pde_tab->base_addr = (uint32_t)(pte_start_addr) >> 12;
        ALLOC_NEW_PTE();
    }
    
    struct pagetable_64* pte_tab = GET_ENTRY_TABLE((pde_tab->base_addr << 12), pte_index);
    *pte_tab = pagetable;
    pte_tab->present = 1;
    pte_tab->base_addr = paddr >> 12;
}


void prepare_pde() { //准备最基本的页表
    void* addr = (void*)KERNEL_TEMP_PG_ADDR;
    for(int i=0;i<KERNEL_PTE_ENTRIES;i++) {
        WRITE_PTE_TO_ADDR(addr, pagetable);
        addr += sizeof(struct pagetable_64);
    }
    link_new_pte_addr(KERNEL_TEMP_PG_ADDR,KERNEL_TEMP_PG_VADDR);
    link_new_pte_addr(KERNEL_BOOT_INFO_PADDR,KERNEL_BOOT_INFO_VADDR); //内存信息
    link_new_pte_addr(SELF_PADDR, SELF_PADDR);//Triple Fault警告
    link_new_pte_addr(KERNEL_GDT_ADDR,KERNEL_GDT_TEMP_VADDR);
}

uint64_t get_kern_pte_ptr_top() {
    return (uint64_t)pte_start_addr;
}