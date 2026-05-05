#include <cpu/cpu.h>
#include <kernel/cpu/smp.h>
#include <asm.h>
#include <cpu/gdt.h>
#include <cpu/msr_base.h>
#include <kernel/fault/fault.h>
DEFINE_PERCPU_VAR(cpuinfo, struct cpuinfo);

void set_smp_base_addr(uintptr_t base) {
    *(uintptr_t*)base = base; //非常重要! 否则找不到base地址.
    uint32_t eax = base & 0xffffffff;
    uint32_t edx = base >> 32;
    uint32_t ecx = MSR_GS_BASE_ADDR;

    barrier();
    asm volatile (
        "wrmsr"
        : 
        : "a"(eax),
          "d"(edx),
          "c"(ecx)
        :
    );
}

void init_ist() {
    if(is_bsp_core()) {
        extern uint64_t __bsp_ist0,
                    __bsp_ist1,
                    __bsp_ist2,
                    __bsp_ist3,
                    __bsp_ist4;
                    
        set_tss_ist(0, (uint64_t)&__bsp_ist0);
        set_tss_ist(1, (uint64_t)&__bsp_ist1);
        set_tss_ist(2, (uint64_t)&__bsp_ist2);
        set_tss_ist(3, (uint64_t)&__bsp_ist3);
        set_tss_ist(4, (uint64_t)&__bsp_ist4);
    }
}


static void efer_set() {
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(MSR_EFER));
    low |= EFER_NXE | EFER_SCE;
    asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(MSR_EFER));
}
void init_cpu() {
    init_gdt();
    init_ist();
    open_cr0_wp();
    efer_set();
    arch_enable_pge();
}

inline int irqs_disabled() {
    uint64_t rfl;
    asm volatile (
        "pushfq\r\n"
        "pop %0\r\n"
        : "=r"(rfl));
    return (rfl & (1 << 9));
}
void arch_barrier() {
    asm volatile("" ::: "memory");
}