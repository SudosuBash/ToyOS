#ifndef _TOYOS_ARCH_ABSTRACT
#define _TOYOS_ARCH_ABSTRACT

struct arch_regs;

void hlt();
void enable_irq();
void disable_irq();
void invlpg(uint64_t addr);

void pause();
//其他架构的页表刷新照样用这玩意
#endif