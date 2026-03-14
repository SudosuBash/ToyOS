#ifndef _TOYOS_X86_MM_INFO
#define _TOYOS_X86_MM_INFO

#include <kernel/stdint.h>

#define arch_barrier() asm volatile("" ::: "memory")

#define LOW_MEM 0x100000

#define MM_PAGE_REL_ADDR(addr) ((addr) - LOW_MEM)
#define MM_PAGE_ABS_ADDR(addr) ((addr) + LOW_MEM)
#define PHYS2VADDR(addr) ((addr) + (KERNEL_MEM_SA_VADDR))
#define PHYS2VADDR_MMIO(addr) ((addr) + (MMIO_MEM_SA_VADDR))
#define VADDR2PHYS_MMIO(addr) ((addr) - (MMIO_MEM_SA_VADDR))
#define VADDR2PHYS(addr) ((addr) - (KERNEL_MEM_SA_VADDR))
#define MM_PAGE_PINDEX(paddr) (MM_PAGE_REL_ADDR(paddr) >> PAGE_OFFSET)
#define MM_PAGE_VINDEX(vaddr) (MM_PAGE_PINDEX(VADDR2PHYS(vaddr)))
//内核从0x100000开始, 先转换为物理地址, 再转换为内核的偏移, 最后计算 index
#define PAGE_OFFSET 12
#define PAGE_SZ (1<<PAGE_OFFSET)
#define PAGE_MASK ~(PAGE_SZ-1)

#endif