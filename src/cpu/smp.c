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

    extern uint8_t __percpu_start,__percpu_end;
    uint64_t percpu_sz = &__percpu_end - &__percpu_start;
    
    void* base_addr = kmalloc(percpu_sz);
    assert(base_addr != NULL);
    set_smp_base_addr((uintptr_t)base_addr);
}

static void init_smp_ist() {
    
}
void init_smp() {
    smp_percpu_init();
}