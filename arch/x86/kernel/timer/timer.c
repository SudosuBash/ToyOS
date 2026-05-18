#include <kernel/timer/timer.h>
#include <cpu/cpu.h>
#include <kernel/kernel.h>
#include <hal.h>
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