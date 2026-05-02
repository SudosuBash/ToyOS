#include <kernel/atomic/sem.h>

void sem_p(sem_t* sem) {
    atomic_dec_and_test(&sem->value);
}

void sem_v(sem_t* sem) {
    atomic_inc(&sem->value);
}

void sem_init(sem_t* sem, int64_t val) {
    atomic_set(&sem->value, val);
}