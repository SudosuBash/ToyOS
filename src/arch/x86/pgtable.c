#include <pgtable.h>
#include <stdint.h>
#include <kloader.h>

#define PDE_OF(addr) ((addr) >> 22)
#define PTE_OF(addr) (((addr) >> 12) & (0x3FF))

#define GET_PDE_TABLE(pde_index) (struct pagetable*)(KERNEL_TEMP_PG_ADDR + (pde_index) * sizeof(struct pagetable))
#define KERNEL_STACK_VADDR 0xFFFFFFFF
#define KERNEL_STACK_PHYS 0x100000
#define SELF_PADDR 0x10000
#define SELF_SECTOR 3
static void* pte_start_addr = (void*)(KERNEL_TEMP_PG_ADDR + sizeof(struct pagetable) * 1024);
//申请的pte数量(base_addr)

#define ALLOC_NEW_PTE() ((pte_start_addr) += PAGE_SZ)
static struct pagetable pagetable = {
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

void link_new_pte_addr(uint32_t paddr, uint32_t vaddr) {
    uint32_t pde_index = PDE_OF(vaddr);
    uint32_t pte_index = PTE_OF(vaddr);
    struct pagetable* pde_tab = GET_PDE_TABLE(pde_index);
    
    if(pde_tab->present == 0) {
        pde_tab->present = 1;
        pde_tab->base_addr = (uint32_t)(pte_start_addr) >> 12;
        ALLOC_NEW_PTE();
    }
    
    struct pagetable* pte_tab = (struct pagetable*)(pde_tab->base_addr << 12);
    pte_tab[pte_index] = pagetable;
    pte_tab[pte_index].present = 1;
    pte_tab[pte_index].base_addr = paddr >> 12;
}

void prepare_pde() { //准备最基本的页表
    void* addr = (void*)KERNEL_TEMP_PG_ADDR;
    for(int i=0;i<1024;i++) {
        struct pagetable npte = pagetable;
        WRITE_PTE_TO_ADDR(addr, npte);
        addr += sizeof(struct pagetable);
    }
    link_new_pte_addr(KERNEL_STACK_PHYS,KERNEL_STACK_VADDR);//内核栈
    link_new_pte_addr(KERNEL_TEMP_PG_ADDR,KERNEL_TEMP_PG_VADDR); //页目录表本身的页目录表
    link_new_pte_addr(SELF_PADDR, SELF_PADDR);//自己的引导系统也得加上去!
}

void open_pg_mode(uint32_t addr) { //临时页表
    asm volatile (
        "movl %0, %%esp\n\t"
        "movl %1, %%cr3\n\t"
        "movl %%cr0, %%eax\n\t"
        "orl $0x80000000, %%eax\n\t"
        "movl %%eax, %%cr0\n\t"
        "jmp *%2\n\t"
        "ret\n\t"
        :
        : "g"(KERNEL_STACK_VADDR),
          "r"(KERNEL_TEMP_PG_ADDR),
          "r"(addr)
        : "eax", "memory"
    );
}