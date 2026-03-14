#include <kernel/atomic/spinlock.h>
#include <kernel/atomic/atomic.h>
#include <kernel/mm/mm.h>

inline void spin_init(spinlock_t* lk) {
    lk->value = 0;
}

void spin_lock(spinlock_t* lk) {
    while(atomic_cas(&lk->value,1,0) == 1) {
        __builtin_ia32_pause();
        arch_barrier();
    }
        
    arch_barrier();
}

void spin_unlock(spinlock_t* lk) {
    barrier();
    lk->value = 0;
}