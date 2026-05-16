#ifndef _TOYOS_ATOMIC
#define _TOYOS_ATOMIC

#include <kernel/stdint.h>
typedef struct {
    uint64_t count;
} atomic_t;

uint64_t atomic_cas(uint64_t* dest,int64_t src, int64_t target);
void atomic_inc(atomic_t *val);
uint8_t atomic_dec_and_test(atomic_t *val);
void atomic_set(atomic_t* val, int64_t new);
#endif