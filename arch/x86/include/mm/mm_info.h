#ifndef _TOYOS_X86_MM_INFO
#define _TOYOS_X86_MM_INFO

#include <kernel/stdint.h>

#define LOW_MEM 0x100000

#define MM_PAGE_REL_ADDR(addr) ((addr) - LOW_MEM)
#define MM_PAGE_ABS_ADDR(addr) ((addr) + LOW_MEM)

#define MM_PAGE_PINDEX(paddr) ((paddr) >> PAGE_OFFSET)
uintptr_t get_kernel_end();
uint64_t get_available_mem_sz();
void init_mm_info();
uint64_t get_kern_addr();
#endif