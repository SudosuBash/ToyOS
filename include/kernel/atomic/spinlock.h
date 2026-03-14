#ifndef _TOYOS_ATOMIC_LOCK
#define _TOYOS_ATOMIC_LOCK

#include <kernel/atomic/atomic.h>

typedef struct {
    volatile uint64_t value;
} spinlock_t;

void spin_init(spinlock_t* lk);
void spin_lock(spinlock_t* lk);
void spin_unlock(spinlock_t* lk);
#endif