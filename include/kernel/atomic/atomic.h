#ifndef _TOYOS_ATOMIC
#define _TOYOS_ATOMIC

#include <kernel/stdint.h>
typedef struct {
    volatile uint64_t count;
} atomic_t;

uint64_t atomic_cas(volatile uint64_t* dest,uint64_t src, uint64_t target);
void atomic_inc(atomic_t *val);
uint8_t atomic_dec_and_test(atomic_t *val);

#endif