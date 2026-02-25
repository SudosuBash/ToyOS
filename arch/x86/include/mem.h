#ifndef _TOYOS_MEM
#define _TOYOS_MEM

#include <config_arch.h>

#define PHYS2VADDR(addr) ((addr) + (KERNEL_MEM_SA_VADDR))
#define VADDR2PHYS(addr) ((addr) - (KERNEL_MEM_SA_VADDR))

#endif