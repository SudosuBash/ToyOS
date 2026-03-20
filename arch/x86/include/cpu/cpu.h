#ifndef __TOYOS_X86_CPU
#define __TOYOS_X86_CPU

#include <kernel/stdint.h>
#include <kernel/cpu/cpu.h>
struct cpuinfo {
    uint8_t step_id;
    uint8_t model;
    uint8_t family_id;
    uint8_t processor_type;
    uint8_t extended_model_id;
    uint8_t extended_family_id;

    uint8_t brand;
    uint8_t cache_line_sz;
    uint8_t logic_proc_maxcount;
    uint8_t apic_id;

    uint8_t fpu_support:1;
    uint8_t tsc_support:1;
    uint8_t cx8_support:1;
    uint8_t apic_support:1;

    uint8_t cmov_support:1;
    uint8_t mmx_support:1;
    uint8_t sse_support:1;
    uint8_t sse2_support:1;
    uint8_t rdtscq_support:1;
    uint8_t htt_support:1;

    uint8_t sse3_support:1;
    uint8_t vmx_support:1;
    uint8_t ssse3_support:1;
    uint8_t fma_support:1;
    uint8_t sse4_1_support:1;
    uint8_t aes_support:1;
    uint8_t avx_support:1;
    uint8_t rdnand_support:1;
};

#define CPL_DPL_MASK 0b11
#define IS_IN_KERN_MODE(cs) (((cs) & CPL_DPL_MASK) == 0)
#define MSR_EFER 0xC0000080
#define EFER_NXE (1ULL << 11)
#endif