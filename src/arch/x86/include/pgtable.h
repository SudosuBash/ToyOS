#ifndef _TOYOS_PGTABLE
#define _TOYOS_PGTABLE

#include <stdint.h>

#define KERNEL_TEMP_PG_ADDR (0x200000)
#define KERNEL_PTE_ENTRIES 1024
#define KERNEL_TEMP_PG_VADDR 0xF0000000
#pragma pack(push, 1)

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

void prepare_pde();
void link_new_pte_addr(uint32_t paddr, uint32_t vaddr);
void open_pg_mode(uint32_t addr);
#endif