#ifndef _TOYOS_IRQ_H
#define _TOYOS_IRQ_H

#include <kernel/stdint.h>

struct irq_frame;
typedef void (*irq_handler_t)(struct irq_frame* frame);

int irq_register(uint64_t num, irq_handler_t fn);
void init_irq();
#endif