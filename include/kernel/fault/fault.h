#ifndef _TOYOS_FAULT_H
#define _TOYOS_FAULT_H

#include <fault/fault.h>
#include <kernel/irq/irq.h>

struct crash_info {
    const char* func;
    int line;
    const char* filename;
    const char* message;
    const char* condition;
};

#define crash_on_irq(msg, irq_info)  arch_crash_on_irq(msg,irq_info)
#define crash(msg) arch_crash(msg, NULL)
#define assert(condition) do { \
    if(!(condition)) arch_crash("assertion failed: ", (#condition)); \
} while(0)
void fault(struct crash_info* info, struct irq_frame* frame);
void fault_irq(const char* name, struct irq_frame* frame);
void fault_init();
#endif