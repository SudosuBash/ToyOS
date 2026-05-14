#include <kernel/timer/timer.h>
#include <cpu/cpu.h>
#include <kernel/kernel.h>
#include <cpu/smp.h>

uint64_t get_current_tstamp() {
    uint32_t eax,edx;
    barrier();
    asm volatile (
        "rdtsc"
        : "=a"(eax), "=d"(edx)
    );
    return ((uint64_t)edx << 32) | eax;
}