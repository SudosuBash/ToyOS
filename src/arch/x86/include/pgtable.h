#ifndef _TOYOS_PGTABLE
#define _TOYOS_PGTABLE

#define KERNEL_PG_ADDR (0x105000)
#define KERNEL_PTE_ENTRIES 1024

#pragma pack(push, 1)

struct pte {
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

#define WRITE_PTE_TO_ADDR(addr, pte) (*(struct pte*)(addr) = (pte))

void prepare_pde();
#endif