#ifndef _TOYOS_CLOCK

#define _TOYOS_CLOCK

#include <kernel/stdint.h>
void clock_init();
uint64_t clock_current();
#endif