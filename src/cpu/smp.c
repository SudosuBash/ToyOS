#include <kernel/cpu/smp.h>
#include <kernel/stdint.h>
#include <kernel/mm/mm.h>
#include <kernel/fault/fault.h>

static void smp_percpu_init() {
    if(is_bsp_core()) {
        extern uint64_t __bsp_percpu_start;
        set_smp_base_addr((uintptr_t)&__bsp_percpu_start);
        return;
    }
}


void init_smp() {
    smp_percpu_init();
}