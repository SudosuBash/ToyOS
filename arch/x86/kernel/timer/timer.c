#include <kernel/timer/timer.h>
#include <hal/cpu/cpu.h>
#include <kernel/kernel.h>
#include <hal/hal.h>
#include <kernel/acpi/acpi_apic.h>
#include <hal/config_arch.h>
#include <kernel/task/task.h>
#include <kernel/acpi/acpi_hpet.h>
#include <kernel/log/kprintf.h>

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
    lapic_write(APIC_EOI_OFFSET, 0);
    schedule();
}

static uint32_t check_apic() { //校准 APIC 时钟
    lapic_write(APIC_LVT_OFFSET, 0x20);
    lapic_write(APIC_INITIAL_COUNT_OFFSET, 0);

    barrier();
    uint64_t res = 0;
    for(int i=0;i<8;i++) {
        lapic_write(APIC_INITIAL_COUNT_OFFSET, ~0);
        hpet_spin_wait(1);
        uint32_t end_tick = lapic_read(APIC_CURRENT_COUNT_OFFSET);
        res += ((~0) - end_tick);
    }
    res >>= 3;
    return res;
}

void init_apic_timer() {
    lapic_write(APIC_SVR_OFFSET,APIC_SVR_VALUE(APIC_IVT));
    lapic_write(APIC_TPR_OFFSET, 0);
    lapic_write(APIC_DCR_OFFSET, APIC_FREQ_NUM);

    uint32_t check = check_apic();
    kprintf("APIC: 1us requires ticks %llu.\n", check);
    lapic_write(APIC_LVT_OFFSET, (APIC_LVT_PERODIC_VALUE) | APIC_LVT_IVT);
    lapic_write(APIC_INITIAL_COUNT_OFFSET, check * CONFIG_IRQ_MS * 1000);
}

void init_timer() {
    irq_single_register(APIC_LVT_IVT, timer_irq); 
    init_apic_timer();
    //这个快一点
}