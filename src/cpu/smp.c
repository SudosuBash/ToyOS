#include <kernel/cpu/smp.h>
#include <kernel/stdint.h>
#include <kernel/mm/mm.h>
#include <kernel/fault/fault.h>
#include <kernel/atomic/atomic.h>
DEFINE_PERCPU_VAR(percpu_preempt_count, atomic_t);

static void bsp_percpu_init() {
    extern uint64_t __bsp_percpu_start;
    set_smp_base_addr((uintptr_t)&__bsp_percpu_start);
}

inline void preempt_enable() {
    atomic_t* preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    atomic_dec_and_test(preempt_count);
}

inline void preempt_disable() {
    atomic_t* preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    atomic_inc(preempt_count);
}

void init_bst_smp() {
    bsp_percpu_init();
    atomic_t* preempt_count = THIS_CPU_PTR(percpu_preempt_count);
    atomic_set(preempt_count, 0);
}