#ifndef _TOYOS_IRQ_H
#define _TOYOS_IRQ_H

#include <kernel/stdint.h>

struct arch_regs;
typedef void (*irq_handler_t)(struct arch_regs* frame);

int irq_register(uint64_t num, irq_handler_t fn);
void init_irq();
int irqs_disabled();
#endif