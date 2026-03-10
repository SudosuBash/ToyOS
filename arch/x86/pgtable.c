#include <pgtable.h>
#include <kernel/stdint.h>
#include <kernel/mm/mm.h>
//申请的pte数量(base_addr)
static void* pte_start_addr;
static void* pde_start_addr;

#define ALLOC_NEW_PTE() ((pte_start_addr) += PAGE_SZ)
#define VADDR_TO_PADDR(addr) ((addr) - KERNEL_FINAL_LDR_VADDR + KERNEL_FINAL_LDR_ADDR)
#define PADDR_TO_VADDR(addr) ((addr) + KERNEL_FINAL_LDR_VADDR - KERNEL_FINAL_LDR_ADDR)

struct pagetable_64 pg_default = {
    .present = 0,
    .rw = 1,
    .us = 1,
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

static inline struct pagetable_64* get_pml4(uint64_t vaddr) {
    uint64_t pml4_index = PML4_OF(vaddr);
    return GET_ENTRY_TABLE(pde_start_addr, pml4_index);
}

static inline struct pagetable_64* get_pdpt(uint64_t vaddr) {
    struct pagetable_64* pml4 = get_pml4(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    if(pml4->present == 0) return 0;
    return GET_ENTRY_TABLE(PADDR_TO_VADDR(pml4->base_addr << 12), pdpt_index);
}

static inline struct pagetable_64* get_pte(uint64_t vaddr) {
    struct pagetable_64* pdpt = get_pdpt(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    if(pdpt->present == 0) return 0;
    return GET_ENTRY_TABLE(PADDR_TO_VADDR((uint64_t)pdpt->base_addr << 12), pde_index);
}

static inline struct pagetable_64* get_pde(uint64_t vaddr) {
    struct pagetable_64* pte = get_pte(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);
    if(pte->present == 0) return 0;
    if(pte->ps == 1) return 0;
    return GET_ENTRY_TABLE(PADDR_TO_VADDR((uint64_t)pte->base_addr << 12), pte_index);
}

//这部分为了加速, 就不用上面的函数了
static void link_addr(uint64_t paddr,uint64_t vaddr, uint8_t big_page) {
    uint64_t pml4_index = PML4_OF(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);

    struct pagetable_64* pml4_tab = GET_ENTRY_TABLE(pde_start_addr, pml4_index);
    
    if(pml4_tab->present == 0) {
        *pml4_tab = pg_default;
        pml4_tab->present = 1;
        pml4_tab->us = 0; //这部分对内核页表进行控制
        pml4_tab->base_addr = VADDR_TO_PADDR((uintptr_t)(pte_start_addr)) >> 12;
        ALLOC_NEW_PTE();
    }
    uintptr_t pnum = PADDR_TO_VADDR(pml4_tab->base_addr << 12);
    struct pagetable_64* pdpt_tab = GET_ENTRY_TABLE(pnum, pdpt_index);
    if(pdpt_tab->present == 0) {
        *pdpt_tab = pg_default;
        pdpt_tab->present = 1;
        pdpt_tab->base_addr = VADDR_TO_PADDR((uintptr_t)(pte_start_addr))>>12;
        ALLOC_NEW_PTE();
    }
    pnum = PADDR_TO_VADDR(pdpt_tab->base_addr << 12);
    if(!big_page) {
        struct pagetable_64* pde_tab = GET_ENTRY_TABLE(pnum, pde_index);
        if(pde_tab->present == 0) { 
            *pde_tab = pg_default;
            pde_tab->present = 1;
            pde_tab->base_addr = VADDR_TO_PADDR((uintptr_t)(pte_start_addr))>>12;
            ALLOC_NEW_PTE();
        }

        pnum = PADDR_TO_VADDR((uint64_t)pde_tab->base_addr << 12);
        struct pagetable_64* pte_tab = GET_ENTRY_TABLE(pnum, pte_index);
        *pte_tab = pg_default;
        pte_tab->present = 1;
        pte_tab->us = 0; //Superuser
        pte_tab->base_addr = paddr >> 12;
    } else {
        struct pagetable_64* pde_tab = GET_ENTRY_TABLE(pnum, pde_index);
        *pde_tab = pg_default;
        pde_tab->present = 1;
        pde_tab->ps = 1; //2MB大页
        pde_tab->us = 0; 
        pde_tab->base_addr = paddr >> 12; //不是21
        //坑人时刻: C语言的位域
        // C语言的位域也是小端字节序(更准确的说小端比特序)，成员排列从低到高，成员内部排列从高到低
    }
}


void link_new_pte_addr(uint64_t paddr, uint64_t vaddr) {
    link_addr(paddr,vaddr,0);
}

void link_new_pte_bigpage_addr(uint64_t paddr,uint64_t vaddr) {
    link_addr(paddr,vaddr,1);
}

void set_pml4_us(uint64_t vaddr, uint8_t us) {
    struct pagetable_64* pml4 = get_pml4(vaddr);
    pml4->us = us;
}

void set_pde_us_bigpage(uint64_t vaddr, uint8_t us) {
    struct pagetable_64* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->us = us;
}

void set_pte_us(uint64_t vaddr, uint8_t us) {
    struct pagetable_64* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->us = us;
}

void set_pde_nx_bigpage(uint64_t vaddr, uint8_t nx) {
    struct pagetable_64* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->nx = nx;
}

void set_pte_nx(uint64_t vaddr, uint8_t nx) {
    struct pagetable_64* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->nx = nx;
}

void set_pde_rw_bigpage(uint64_t vaddr, uint8_t nx) {
    struct pagetable_64* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->nx = nx;
}

void set_pte_rw(uint64_t vaddr, uint8_t rw) {
    struct pagetable_64* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->rw = rw;
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

void delete_link(uint64_t vaddr) {
    struct pagetable_64* pde = get_pde(vaddr);
    if(pde->ps) pde->present = 0;
    else {
        struct pagetable_64* pte = get_pte(vaddr);
        pte->present = 0;
    }
}