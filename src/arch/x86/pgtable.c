#include <pgtable.h>

void prepare_pde() {
    struct pte pte;
    void* addr = (void*)KERNEL_PG_ADDR;

    void* pg_addr = sizeof(struct pte) * 1024;
    for(int i=0;i<1024;i++) {
        WRITE_PTE_TO_ADDR(addr,pte);
    }
}