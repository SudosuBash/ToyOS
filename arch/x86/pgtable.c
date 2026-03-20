#include <kernel/ptable/ptable.h>
#include <kernel/stdint.h>
#include <kernel/mm/mm.h>
#include <pgtable/pgtable_kern.h>
//申请的pte数量(base_addr)
static volatile void* pte_start_addr;
static volatile void* pde_start_addr;

#define ALLOC_NEW_PTE() ((pte_start_addr) += PAGE_SZ)

pgd_t pg_default = {
    .present = 0,
    .rw = 1,
    .us = 0,
    .pwt = 0,
    .pcd = 0,
    .a = 0,
    .d = 0,
    .g = 1,
    .avl_1 = 0,
    .base_addr = 0,
    .avl_2 = 0,
    .nx = 0
};

pte_t* pte_default = (pte_t*)&pg_default;
pgd_t* pml4_default = &pg_default;
pde_t* pde_default = (pgd_t*)&pg_default;
pdpt_t* pdpt_default = (pgd_t*)&pg_default;

inline pgd_t* get_pgd(uint64_t vaddr) {
    uint64_t pml4_index = PML4_OF(vaddr);
    return GET_ENTRY_TABLE(pde_start_addr, pml4_index, pgd_t);
}

static inline pdpt_t* get_pdpt(uint64_t vaddr) {
    pgd_t* pml4 = get_pgd(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    if(pml4->present == 0) return 0;
    return GET_ENTRY_TABLE(KERN_PADDR_TO_VADDR(pml4->base_addr << 12), pdpt_index, pdpt_t);
}

static inline pde_t* get_pde(uint64_t vaddr) {
    pdpt_t* pte = get_pdpt(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);
    if(pte->present == 0) return 0;
    if(pte->ps == 1) return 0;
    return GET_ENTRY_TABLE(KERN_PADDR_TO_VADDR((uint64_t)pte->base_addr << 12), pte_index, pde_t);
}


static inline pte_t* get_pte(uint64_t vaddr) {
    pde_t* pde = get_pde(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    if(pde->present == 0) return 0;
    return GET_ENTRY_TABLE(KERN_PADDR_TO_VADDR((uint64_t)pde->base_addr << 12), pde_index, pte_t);
}

//这部分为了加速, 就不用上面的函数了
static void link_addr(uint64_t paddr,uint64_t vaddr, uint8_t big_page) {
    uint64_t pml4_index = PML4_OF(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);

    pgd_t* pml4_tab = GET_ENTRY_TABLE(pde_start_addr, pml4_index, pgd_t);
    
    if(pml4_tab->present == 0) {
        *pml4_tab = pg_default;
        pml4_tab->present = 1;
        pml4_tab->us = 0; //这部分对内核页表进行控制
        pml4_tab->base_addr = KERN_VADDR_TO_PADDR((uintptr_t)(pte_start_addr)) >> 12;
        ALLOC_NEW_PTE();
    }
    uintptr_t pnum = KERN_PADDR_TO_VADDR(pml4_tab->base_addr << 12);
    pdpt_t* pdpt_tab = GET_ENTRY_TABLE(pnum, pdpt_index, pdpt_t);
    if(pdpt_tab->present == 0) {
        *pdpt_tab = *pdpt_default;
        pdpt_tab->present = 1;
        pdpt_tab->base_addr = KERN_VADDR_TO_PADDR((uintptr_t)(pte_start_addr))>>12;
        ALLOC_NEW_PTE();
    }
    pnum = KERN_PADDR_TO_VADDR(pdpt_tab->base_addr << 12);
    if(!big_page) {
        pde_t* pde_tab = GET_ENTRY_TABLE(pnum, pde_index, pde_t);
        if(pde_tab->present == 0) { 
            *pde_tab = *pde_default;
            pde_tab->present = 1;
            pde_tab->base_addr = KERN_VADDR_TO_PADDR((uintptr_t)(pte_start_addr))>>12;
            ALLOC_NEW_PTE();
        }

        pnum = KERN_PADDR_TO_VADDR((uint64_t)pde_tab->base_addr << 12);
        pte_t* pte_tab = GET_ENTRY_TABLE(pnum, pte_index, pte_t);
        *pte_tab = *pte_default;
        pte_tab->present = 1;
        pte_tab->us = 0; //Superuser
        pte_tab->base_addr = paddr >> 12;
    } else {
        pde_t* pde_tab = GET_ENTRY_TABLE(pnum, pde_index, pde_t);
        *pde_tab = *pde_default;
        pde_tab->present = 1;
        pde_tab->ps = 1; //2MB大页
        pde_tab->us = 0; 
        pde_tab->base_addr = paddr >> 12; //不是21
        //坑人时刻: C语言的位域
        // C语言的位域也是小端字节序(更准确的说小端比特序)，成员排列从低到高，成员内部排列从高到低
    }
    barrier();
}


void link_new_pte_addr(uint64_t paddr, uint64_t vaddr) {
    link_addr(paddr,vaddr,0);
}

void link_new_pte_bigpage_addr(uint64_t paddr,uint64_t vaddr) {
    link_addr(paddr,vaddr,1);
}

void set_pgd_us(uint64_t vaddr, uint8_t us) {
    pgd_t* pml4 = get_pgd(vaddr);
    pml4->us = us;
}

void set_pde_us_bigpage(uint64_t vaddr, uint8_t us) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->us = us;
}

void set_pte_us(uint64_t vaddr, uint8_t us) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->us = us;
}

void set_pde_nx_bigpage(uint64_t vaddr, uint8_t nx) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->nx = nx;
}

void set_pte_nx(uint64_t vaddr, uint8_t nx) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->nx = nx;
}

void set_pde_rw_bigpage(uint64_t vaddr, uint8_t nx) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->nx = nx;
}

void set_pte_rw(uint64_t vaddr, uint8_t rw) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->rw = rw;
}

void set_pde_pcd_bigpage(uint64_t vaddr, uint8_t pcd) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->pcd = pcd;
}

void set_pte_pcd(uint64_t vaddr, uint8_t pcd) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->pcd = pcd;
}

void set_pde_pwt_bigpage(uint64_t vaddr, uint8_t pwt) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->pwt = pwt;
}

void set_pte_pwt(uint64_t vaddr, uint8_t pwt) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->pwt = pwt;
}

void prepare_pde(void* pde_start) { //准备最基本的页表
    pde_start_addr = pde_start;
    for(int i=0;i<PAGE_PTE_ENTRIES;i++) {
        WRITE_PTE_TO_ADDR(pde_start, pg_default);
        pde_start += sizeof(pde_t);
    }
    pte_start_addr = pde_start;
    // pte_start_addr += sizeof(struct pagetable_64) * KERNEL_PTE_ENTRIES;
}

void delete_link(uint64_t vaddr) {
    pde_t* pde = get_pde(vaddr);
    if(pde->ps) pde->present = 0;
    else {
        pte_t* pte = get_pte(vaddr);
        pte->present = 0;
    }
}