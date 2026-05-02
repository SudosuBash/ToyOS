#include <kernel/atomic/rwlock.h>
#include <kernel/cpu/smp.h>

void rwlock_read_lock(rwlock_t* lock) {
    sem_p(&lock->rwlock_readers_mutex);
    if(lock->readers==0)
        sem_p(&lock->rwlock_mutex);
    lock->readers++;
    sem_v(&lock->rwlock_readers_mutex);
}

void rwlock_write_lock(rwlock_t* lock) {
    sem_p(&lock->rwlock_mutex);
}

void rwlock_read_unlock(rwlock_t* lock) {
    sem_p(&lock->rwlock_readers_mutex);
    lock->readers--;
    if(lock->readers == 0)
        sem_p(&lock->rwlock_mutex);
    sem_v(&lock->rwlock_readers_mutex);
}

void rwlock_write_unlock(rwlock_t* lock) {
    sem_v(&lock->rwlock_mutex);
}

void rwlock_init(rwlock_t* lock) {
    sem_init(&lock->rwlock_mutex, 1);
    sem_init(&lock->rwlock_readers_mutex, 1);
    lock->readers = 0;
    barrier();
}