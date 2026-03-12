#ifndef _TOYOS_X86_MM_INFO
#define _TOYOS_X86_MM_INFO

#include <kernel/stdint.h>

#define LOW_MEM 0x100000

#define MM_PAGE_REL_ADDR(addr) ((addr) - LOW_MEM)
#define MM_PAGE_ABS_ADDR(addr) ((addr) + LOW_MEM)
#define PHYS2VADDR(addr) ((addr) + (KERNEL_MEM_SA_VADDR))
#define VADDR2PHYS(addr) ((addr) - (KERNEL_MEM_SA_VADDR))
#define MM_PAGE_PINDEX(paddr) ((paddr) >> PAGE_OFFSET)

#define PAGE_OFFSET 12
#define PAGE_SZ 4096
#define PAGE_MASK ~(PAGE_SZ-1)

#endif