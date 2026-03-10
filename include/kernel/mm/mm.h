#ifndef _TOYOS_MEM
#define _TOYOS_MEM

#include <kernel/config.h>
#include <kernel/stdint.h>
#include <kernel/data_struct/linklist.h>

#define PAGE_OFFSET 12
#define PAGE_SZ 4096
#define PAGE_MASK ~(PAGE_SZ-1)

#define PHYS2VADDR(addr) ((addr) + (KERNEL_MEM_SA_VADDR))
#define VADDR2PHYS(addr) ((addr) - (KERNEL_MEM_SA_VADDR))

struct page* alloc_new_phys_page();
void init_mm();
#endif