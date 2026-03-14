#ifndef _TOYOS_ATOMIC_SEM
#define _TOYOS_ATOMIC_SEM

#include <kernel/atomic/atomic.h>
typedef struct {
    atomic_t value;
} sem_t;

void sem_p(sem_t* sem);
void sem_v(sem_t* sem);
#endif