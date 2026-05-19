#ifndef _TOYOS_TIMER_TSC
#define _TOYOS_TIMER_TSC

#include <kernel/stdint.h>
#include <kernel/irq/irq.h>
#include <hal.h>

typedef void (*timer_handler_t)(struct arch_regs* frame);
uint64_t rdtsc();
void init_timer();
#endif