#ifndef _TOYOS_PGTABLE
#define _TOYOS_PGTABLE

#include <kernel/config.h>
#include <kernel/stdint.h>

#define PML4_OF(addr) (((addr) >> 39 ) & 511)
#define PDPT_OF(addr) (((addr) >> 30) & 511)
#define PDE_OF(addr) (((addr) >> 21) & 511)
#define PTE_OF(addr) (((addr) >> 12) & (0x1FF))
#define POFFSET_OF(addr) ((addr) & (0x1ff))

#define GET_ENTRY_TABLE(pde, pde_index, type) (&((type*)(pde))[(pde_index)])
#define WRITE_PTE_TO_ADDR(addr, pte) (*(struct pagetable_64*)(addr) = (pte))
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

void prepare_pde(void* pde_start);
void link_new_pte_addr(uint64_t paddr, uint64_t vaddr);
void link_new_pte_bigpage_addr(uint64_t paddr,uint64_t vaddr);
void set_pgd_us(uint64_t vaddr, uint8_t us);

void set_pde_us_bigpage(uint64_t vaddr, uint8_t us);
void set_pte_us(uint64_t vaddr, uint8_t us);
void set_pde_nx_bigpage(uint64_t vaddr, uint8_t nx);
void set_pte_nx(uint64_t vaddr, uint8_t nx);
void set_pde_rw_bigpage(uint64_t vaddr, uint8_t rw);
void set_pte_rw(uint64_t vaddr, uint8_t rw);
void set_pde_pcd_bigpage(uint64_t vaddr, uint8_t pcd);
void set_pte_pcd(uint64_t vaddr, uint8_t pcd);
void set_pde_pwt_bigpage(uint64_t vaddr, uint8_t pwt);
void set_pte_pwt(uint64_t vaddr, uint8_t pwt);

void delete_link(uint64_t vaddr);

#endif