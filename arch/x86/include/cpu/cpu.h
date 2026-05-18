#ifndef __TOYOS_X86_CPU
#define __TOYOS_X86_CPU

#include <kernel/stdint.h>
#include <kernel/cpu/cpu.h>
struct cpuinfo {
    uint8_t step_id:4;
    uint8_t model:4;
    uint8_t family_id:4;
    uint8_t processor_type:2;
    uint8_t extended_model_id:4;
    uint8_t extended_family_id;

    uint8_t brand;
    uint8_t cache_line_sz;
    uint8_t logic_proc_maxcount;
    

    uint8_t fpu_support:1;
    uint8_t de_support:1;
    uint8_t pse_support:1;
    uint8_t tsc_support:1;
    uint8_t msr_support:1;
    uint8_t cx8_support:1;
    uint8_t apic_support:1;

    uint8_t cmov_support:1;
    uint8_t mmx_support:1;
    uint8_t sse_support:1;
    uint8_t sse2_support:1;
    uint8_t rdtscp_support:1;
    uint8_t htt_support:1;
    uint8_t fxsr_support:1;
    uint8_t pbe_support:1;

    uint8_t vmx_support:1;
    uint8_t fma_support:1;
    uint8_t cmpxchg16b_support:1;
    uint8_t x2apic_support:1;
    uint8_t aes_support:1;
    uint8_t avx_support:1;
    uint8_t rdnand_support:1;
    uint8_t pclmulqdq_support:1;
    uint8_t hypv_support:1;
};

#define CPL_DPL_MASK 0b11
#define IS_IN_KERN_MODE(cs) (((cs) & CPL_DPL_MASK) == 0)

#define EFER_NXE (1ULL << 11)
#define EFER_SCE (1ULL << 0)

#define CPUID_STEP_ID(eax) ((eax) & (0xf))
#define CPUID_MODEL_ID(eax) (((eax) >> 4) & (0xf))
#define CPUID_FAMILY(eax) (((eax) >> 8) & (0xf))
#define CPUID_PROC_TYPE(eax) (((eax) >> 12) & (0x3))
#define CPUID_EXT_MODEL(eax) (((eax) >> 16) & (0x3))
#define CPUID_EXT_FAMILY(eax) (((eax) >> 20) & (0xff))

#define CPUID_FPU(edx) ((edx) & 1)
#define CPUID_DE(edx) (((edx) >> 2) & 1)
#define CPUID_PSE(edx) (((edx) >> 3) & 1)
#define CPUID_TSC(edx) (((edx) >> 4) & 1)
#define CPUID_MSR(edx) (((edx) >> 5) & 1)
#define CPUID_CX8(edx) (((edx) >> 8) & 1)
#define CPUID_APIC(edx) (((edx) >> 9) & 1)
#define CPUID_CMOV(edx) (((edx) >> 15) & 1)
#define CPUID_MMX(edx) (((edx) >> 23) & 1)
#define CPUID_FXSR(edx) (((edx) >> 24) & 1)
#define CPUID_SSE(edx) (((edx) >> 25) & 1)
#define CPUID_SSE2(edx) (((edx) >> 26) & 1)
#define CPUID_PBE(edx) (((edx) >> 31) & 1)
#define CPUID_HTT(edx) (((edx) >> 28) & 1)

#define CPUID_VMX(ecx) (((ecx) >> 5) & 1)
#define CPUID_PCLMULQDQ(ecx) (((ecx) >> 1) & 1)
#define CPUID_FMA(ecx) (((ecx) >> 12) & 1)
#define CPUID_CMPXCHG16B(ecx) (((ecx) >> 13) & 1)
#define CPUID_X2APIC(ecx) (((ecx) >> 21) & 1)
#define CPUID_RDNAND(ecx) (((ecx) >> 30) & 1)
#define CPUID_HYPV(ecx) (((ecx) >> 31) & 1)

#define CPUID_RDTSCP(edx) (((edx) >> 27) & 1)

#define APIC_ID(ebx) (((ebx) >> 24))
#endif