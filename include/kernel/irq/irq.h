#ifndef _TOYOS_IRQ_H
#define _TOYOS_IRQ_H

#include <kernel/stdint.h>
#include <kernel/base/linklist.h>
#include <hal/hal.h>

#define IRQ_MAX_CNT 256

typedef uint32_t (*irq_handler_t)(struct arch_regs* frame, void* device_data);

struct irq_behavior {
    void* device_data;
    irq_handler_t handler;
    struct linklist_head sibling;
};

struct irq_entrance {
    struct linklist_head head;
};


int irq_register(
    uint64_t num,
    irq_handler_t fn,
    void* device_data
);
void init_irq_cpu();
void init_irq_arch();
int irqs_disabled();
void irq_entrance_fn(struct arch_regs* args, uint64_t irq_number);
void irq_remove(uint64_t num, void* dev_data);
void init_irq();
void init_irq_arch_cpu();
#endif