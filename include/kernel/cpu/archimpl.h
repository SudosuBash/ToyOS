#ifndef _TOYOS_ARCH_ABSTRACT
#define _TOYOS_ARCH_ABSTRACT

#include <hal.h>
struct arch_regs;

uintptr_t get_pf_addr();
uintptr_t get_pgroot();

//其他架构的页表刷新照样用这玩意
#endif