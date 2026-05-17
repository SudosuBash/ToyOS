#ifndef _TOYOS_ARCH_ABSTRACT
#define _TOYOS_ARCH_ABSTRACT

#include <hal.h>
struct arch_regs;

void invlpg(uint64_t addr);

//其他架构的页表刷新照样用这玩意
#endif