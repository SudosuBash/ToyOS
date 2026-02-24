#ifndef _TOYOS_X86_PGTABLE
#define _TOYOS_X86_PGTABLE
#include <stdint.h>
struct pagetable {
    int present:1;
    int rw:1;
    int us:1;
    int pwt:1;
    int pcd:1;
    int a:1;
    int d:1;
    int ps:1;
    int g:1;
    int avl:3;
    int base_addr:20;
};

#define WRITE_PTE_TO_ADDR(addr, pte) (*(struct pagetable*)(addr) = (pte))

void unlink_pte_vaddr(uint32_t vaddr);
void link_new_pte_addr(uint32_t paddr, uint32_t vaddr, struct pagetable pgtab);
void init_pagetable(uint32_t kern_pg_addr_top, uint32_t pg_base_addr);
//初始化内核页表
#endif