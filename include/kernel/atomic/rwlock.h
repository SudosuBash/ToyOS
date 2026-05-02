#ifndef _TOYOS_ATOMIC_RWLOCK
#define _TOYOS_ATOMIC_RWLOCK

#include <kernel/atomic/spinlock.h>
#include <kernel/atomic/sem.h>

typedef struct {
    uint64_t readers;
    sem_t rwlock_readers_mutex;
    sem_t rwlock_mutex;
} rwlock_t;

void rwlock_read_lock(rwlock_t* lock);
void rwlock_write_lock(rwlock_t* lock);
void rwlock_read_unlock(rwlock_t* lock);
void rwlock_write_unlock(rwlock_t* lock);
void rwlock_init(rwlock_t* lock);
#endif
