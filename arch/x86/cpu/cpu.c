#include <cpu/cpu.h>
#include <cpu/smp_percpu.h>
#include <asm.h>
#include <gdt.h>

#include <kernel/fault/fault.h>
DEFINE_PERCPU_VAR(cpuinfo, struct cpuinfo);

void init_cpu() {
    init_gdt();
    open_cr0_wp();
    fault_init();
}