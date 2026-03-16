#include <kernel/atomic/spinlock.h>
#include <kernel/atomic/atomic.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/archimpl.h>
inline void spin_init(spinlock_t* lk) {
    lk->value = 0;
}

void spin_lock(spinlock_t* lk) {
    while(atomic_cas(&lk->value,1,0) == 1) {
        __builtin_ia32_pause();
    }
    barrier();
}

void spin_unlock(spinlock_t* lk) {
    barrier();
    lk->value = 0;
}