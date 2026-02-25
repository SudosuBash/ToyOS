#ifndef _TOYOS_ATOMIC
#define _TOYOS_ATOMIC

#include <kernel/stdint.h>
typedef struct {
    uint64_t count;
} atomic_t;

void atomic_inc(atomic_t *val);
uint8_t atomic_dec_and_test(atomic_t *val);

#endif