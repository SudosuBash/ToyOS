#include <kernel/timer/timer.h>
#include <cpu/cpu.h>
#include <kernel/kernel.h>
#include <hal.h>
#include <irq/pic/apic.h>
#include <config_arch.h>
#include <kernel/task/task.h>

inline uint64_t rdtsc() {
    uint32_t eax,edx;

    if(cpu_feature_rdtscp()) {
        asm volatile (
            "rdtscp"
            : "=a"(eax), "=d"(edx)
            : : "rcx"
        );
    } else {
        smp_rmb();
        asm volatile (
            "rdtsc"
            : "=a"(eax), "=d"(edx) : :
        );
    }
    return ((uint64_t)edx << 32) | eax;
}

static void timer_irq(struct arch_regs* regs, uint64_t irq_num) {
    lapic_handle_ok();
    schedule();
}

void init_timer() {
    irq_single_register(APIC_LVT_IVT, timer_irq); 
    //这个快一点
}