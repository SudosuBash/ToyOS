#include <hal/cpu/cpu.h>
#include <kernel/cpu/smp.h>
#include <hal/asm.h>
#include <hal/cpu/gdt.h>
#include <hal/cpu/msr_base.h>
#include <kernel/fault/fault.h>
#include <kernel/kernel.h>
#include <kernel/log/kprintf.h>
#include <kernel/acpi/acpi_apic.h>
#include <kernel/acpi/acpi_hpet.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/cpu/smp.h>

DEFINE_PERCPU_VAR(cpuinfo, struct cpuinfo);
DEFINE_PERCPU_VAR(proc_id, uint8_t);

extern struct system_static_data sysdata;

void set_smp_percpu_addr(uintptr_t base) {
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
    uint64_t smpid = smp_processor_id();
    uint64_t cpu_base_ist = sysdata.kernel_exception_stack + smpid * BST_STACK_SZ * BST_STACK_COUNT_PERCPU;
    for(int i=0;i<BST_STACK_COUNT_PERCPU;i++) 
        set_tss_ist(i, cpu_base_ist + BST_STACK_SZ * i);
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

    info->rdtscp_support = CPUID_RDTSCP(edx);
    kprintf("CPU %d: rdtscp feature %s.\n",smp_processor_id(), info->rdtscp_support ? "supported" : "unsupported");
}

inline bool cpu_feature_rdtscp() {
    struct cpuinfo* info = THIS_CPU_PTR(cpuinfo);
    return info->rdtscp_support;
}

void init_cpu() {
    init_gdt();
    get_cpu_feature();
    init_ist();
    open_cr0_wp();
    efer_set();
    arch_enable_pge();
}

void arch_ap_boot(uint8_t cpuid) {
    lapic_write(APIC_ICR_HIGH_OFFSET, cpuid << 24);
    lapic_write(APIC_ICR_LOW_OFFSET, APIC_ICR_LOW_DELIVERY_INIT | APIC_ICR_LOW_LEVEL_ASSERT);
    hpet_spin_wait(10000);
    lapic_write(APIC_ICR_LOW_OFFSET, APIC_ICR_LOW_DELIVERY_SIPI | APIC_ICR_LOW_LEVEL_ASSERT | (AP_START_ADDR >> 12));
    hpet_spin_wait(10);
}

inline uint16_t smp_processor_id() {
    return THIS_CPU_VAR(proc_id);
}