#include <kernel/cpu/smp.h>
#include <kernel/stdint.h>
#include <kernel/mm/mm.h>
#include <kernel/fault/fault.h>
#include <kernel/atomic/atomic.h>
DEFINE_PERCPU_VAR(percpu_preempt_count, atomic_t);
static atomic_t* preempt_count;

static void smp_percpu_init() {
    if(is_bsp_core()) {
        extern uint64_t __bsp_percpu_start;
        set_smp_base_addr((uintptr_t)&__bsp_percpu_start);
        return;
    }
}

inline void preempt_enable() {
    atomic_inc(preempt_count);
}

inline void preempt_disable() {
    atomic_dec_and_test(preempt_count);
}

void init_smp() {
    smp_percpu_init();
    preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    atomic_set(preempt_count, 0);
    
}