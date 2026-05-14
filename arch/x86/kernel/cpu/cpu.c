#include <cpu/cpu.h>
#include <kernel/cpu/smp.h>
#include <asm.h>
#include <cpu/gdt.h>
#include <cpu/msr_base.h>
#include <kernel/fault/fault.h>
#include <kernel/kernel.h>

DEFINE_PERCPU_VAR(cpuinfo, struct cpuinfo);
DEFINE_PERCPU_VAR(proc_id, uint8_t);


void set_smp_base_addr(uintptr_t base) {
    *(uintptr_t*)base = base; //非常重要! 否则找不到base地址.
    uint32_t eax = base & 0xffffffff;
    uint32_t edx = base >> 32;
    uint32_t ecx = MSR_GS_BASE_ADDR;

    barrier();
    asm volatile (
        "wrmsr": : "a"(eax), "d"(edx), "c"(ecx):
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

static void get_cpu_feature() {
    struct cpuinfo* info = THIS_CPU_PTR(cpuinfo);
    uint8_t* id = THIS_CPU_PTR(proc_id);

    uint64_t eax = 1,ebx,ecx,edx;
    asm volatile ("cpuid" 
        :"=b"(ebx),"=c"(ecx),"=d"(edx), "+a"(eax)
        );
    
    barrier();
    info->step_id = CPUID_STEP_ID(eax);
    info->model = CPUID_MODEL_ID(eax);
    info->family_id = CPUID_FAMILY(eax);
    info->processor_type = CPUID_PROC_TYPE(eax);
    info->extended_model_id = CPUID_EXT_MODEL(eax);
    info->extended_family_id = CPUID_EXT_FAMILY(eax);

    barrier();
    info->fpu_support = CPUID_FPU(edx);
    info->de_support = CPUID_DE(edx);
    info->pse_support = CPUID_PSE(edx);
    info->tsc_support = CPUID_TSC(edx);
    info->msr_support = CPUID_MSR(edx);
    info->cx8_support = CPUID_CX8(edx);
    info->apic_support = CPUID_APIC(edx);
    info->cmov_support = CPUID_CMOV(edx);
    info->mmx_support = CPUID_MMX(edx);
    info->fxsr_support = CPUID_FXSR(edx);
    info->sse_support = CPUID_SSE(edx);
    info->sse2_support = CPUID_SSE2(edx);
    info->pbe_support = CPUID_PBE(edx);
    info->htt_support = CPUID_HTT(edx);
    barrier();
    info->vmx_support = CPUID_VMX(ecx);
    info->pclmulqdq_support = CPUID_PCLMULQDQ(ecx);
    info->fma_support = CPUID_FMA(ecx);
    info->cmpxchg16b_support = CPUID_CMPXCHG16B(ecx);
    info->x2apic_support = CPUID_X2APIC(ecx);
    info->rdnand_support = CPUID_RDNAND(ecx);
    info->hypv_support = CPUID_HYPV(ecx);

    *id = APIC_ID(ebx);

    asm volatile ("cpuid" 
    :"=b"(ebx),"=c"(ecx),"=d"(edx)
    :"a"(0x80000001));

    info->rdtscq_support = CPUID_RDTSCP(edx);
}

void arch_barrier() {
    asm volatile("" ::: "memory");
}

inline bool cpu_feature_rdtscq() {
    struct cpuinfo* info = THIS_CPU_PTR(cpuinfo);
    return info->rdtscq_support;
}

void init_cpu() {
    init_gdt();
    init_ist();
    get_cpu_feature();
    open_cr0_wp();
    efer_set();
    arch_enable_pge();
}


inline uint16_t smp_processor_id() {
    return THIS_CPU_VAR(proc_id);
}