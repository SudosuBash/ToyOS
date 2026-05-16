#include <kernel/atomic/sem.h>
#include <kernel/task/task.h>

void sem_p(sem_t* sem) {
    if(!atomic_dec_and_test(&sem->value)) {
        current_task_switch_stat(TASK_SIGNAL_SLEEP_STAT);
    }
}

void sem_v(sem_t* sem) {
    atomic_inc(&sem->value);
}

void sem_init(sem_t* sem, int64_t val) {
    atomic_set(&sem->value, val);
}