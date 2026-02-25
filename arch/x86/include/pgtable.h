#ifndef _TOYOS_KLOADER_PGTABLE
#define _TOYOS_KLOADER_PGTABLE

#include <kernel/stdint.h>
#include <config_arch.h>

#define PAGE_OFFSET 12
#define PAGE_SZ 4096
#define PAGE_MASK ~(PAGE_SZ-1)

#define PML4_OF(addr) (((addr) >> 39 ) & 511)
#define PDPT_OF(addr) (((addr) >> 30) & 511)
#define PDE_OF(addr) (((addr) >> 21) & 511)
#define PTE_OF(addr) (((addr) >> 12) & (0x1FF))

#define GET_ENTRY_TABLE(pde, pde_index) (&((struct pagetable_64*)(pde))[(pde_index)])

#define KERNEL_PTE_ENTRIES 512
#define PG_OFFSET 21
#define PG_BIG_PAGE_SZ 2097152

#pragma pack(push, 1)

struct pagetable_64 {
    uint64_t present:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t d:1;
    uint64_t ps:1;
    uint64_t g:1;
    uint64_t avl_1:3;
    uint64_t base_addr:40;
    uint64_t avl_2:11;
    uint64_t nx:1;
}__attribute__((packed));

#define WRITE_PTE_TO_ADDR(addr, pte) (*(struct pagetable_64*)(addr) = (pte))

void prepare_pde(void* pde_start);
void link_new_pte_addr(uint64_t paddr, uint64_t vaddr);
void link_new_pte_bigpage_addr(uint64_t paddr,uint64_t vaddr);
#endif
