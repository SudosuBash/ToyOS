#ifndef _TOYOS_TIMER_TSC
#define _TOYOS_TIMER_TSC

#include <kernel/stdint.h>
#include <kernel/irq/irq.h>
#include <hal/hal.h>

typedef void (*timer_handler_t)(struct arch_regs* frame);
uint64_t rdtsc();
void init_timer_cpu();
void spin_wait(uint64_t useconds);
void init_timer();
#endif