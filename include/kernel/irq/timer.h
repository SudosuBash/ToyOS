#ifndef _TOYOS_IRQ_TIMER
#define _TOYOS_IRQ_TIMER

#include <kernel/irq/irq.h>

typedef void (*timer_handler_t)(struct arch_regs* frame);
void timer_irq_register(timer_handler_t fn);
#endif