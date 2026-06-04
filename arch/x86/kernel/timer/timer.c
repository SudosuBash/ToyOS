#include <kernel/timer/timer.h>
#include <hal/cpu/cpu.h>
#include <kernel/kernel.h>
#include <hal/hal.h>
#include <kernel/acpi/acpi_apic.h>
#include <hal/config_arch.h>
#include <kernel/task/task.h>
#include <kernel/acpi/acpi_hpet.h>
#include <kernel/log/kprintf.h>

extern struct system_static_data sysdata;

static uint64_t lapic_us;

inline uint64_t rdtsc() {
    uint32_t eax,edx;

    barrier();
    asm volatile (
        "rdtsc"
        : "=a"(eax), "=d"(edx) : :
    );
    return ((uint64_t)edx << 32) | eax;
}

void spin_wait(uint64_t useconds) {
    uint64_t initial = rdtsc();
    while(rdtsc()-initial < sysdata.tsc_us * useconds);
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
    uint64_t prev, last, tsc_sum = 0;
    for(int i=0;i<8;i++) {
        lapic_write(APIC_INITIAL_COUNT_OFFSET, ~0);
        prev = rdtsc();
        hpet_spin_wait(4096);
        last = rdtsc();
        uint32_t end_tick = lapic_read(APIC_CURRENT_COUNT_OFFSET);
        res += (((~0) - end_tick) >> 12);
        tsc_sum += ((last - prev) >> 12);
    }
    lapic_us = res >> 3;
    return tsc_sum >> 3;
}

void init_apic_timer() {
    lapic_write(APIC_SVR_OFFSET,APIC_SVR_VALUE(APIC_IVT));
    lapic_write(APIC_TPR_OFFSET, 0);
    lapic_write(APIC_DCR_OFFSET, APIC_FREQ_NUM);
    
    uint32_t check = check_apic();
    sysdata.tsc_us = check;
    kprintf("TSC: 1us equals cpu tsc %llu.\n", check);
    lapic_write(APIC_LVT_OFFSET, (APIC_LVT_PERODIC_VALUE) | APIC_LVT_IVT);
}

void init_timer_cpu() {
    irq_single_register(APIC_LVT_IVT, timer_irq); 
    lapic_write(APIC_INITIAL_COUNT_OFFSET, lapic_us  * CONFIG_IRQ_MS * 1000);
    //这个快一点
}

void init_timer() {
    init_apic_timer();
}