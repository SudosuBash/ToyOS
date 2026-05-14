#include <kernel/atomic/sem.h>
#include <kernel/task/task.h>

void sem_p(sem_t* sem) {
    struct task_struct* current = CURRENT_PROCESS();
    if(!atomic_dec_and_test(&sem->value)) {
        current->scheduler->s_class.task_r_to_ss(current);
    }
}

void sem_v(sem_t* sem) {
    atomic_inc(&sem->value);
}

void sem_init(sem_t* sem, int64_t val) {
    atomic_set(&sem->value, val);
}