#ifndef _TOYOS_X86_HAL
#define _TOYOS_X86_HAL

#include <hal/pgtable/pgtable_kern.h>
#include <hal/fault/fault.h>
#include <hal/config_arch.h>
#include <hal/mm/mm_info.h>
// hal/hal.h
// 所有架构相关的macro全部丢这
#define ARCH_GET_PERCPU_VAR(var) ({ \
    uintptr_t __percpu_var_addr__; \
    asm volatile ( \
        "movq %%gs:0, %[addr]\n\t" \
        "addq %[off], %[addr]" \
        : [addr] "=&r" (__percpu_var_addr__) \
        : [off] "r" ((uintptr_t)&(var)) \
        : "cc" \
    ); \
    (typeof(&(var)))(__percpu_var_addr__); \
})

#define ARCH_SET_THIS_CPU_VAR(var, value) *ARCH_GET_PERCPU_VAR(var) = value;
//C语言中var[0]严格等于*(var+0), 所以这样是合法的.

#define barrier() asm volatile("" ::: "memory")
#define smp_mb() asm volatile("mfence" ::: "memory")
#define smp_rmb() asm volatile("lfence" ::: "memory")
#define smp_wmb() asm volatile("sfence" ::: "memory")

#define pause() asm volatile("pause")
#define enable_irq() asm volatile ("sti")
#define disable_irq() asm volatile ("cli")
#define hlt() asm volatile ("hlt")
#define invlpg(address) asm volatile( \
        "invlpg (%[addr])" \
        : \
        : \
        [addr] "r"((address)) \
    );

#define REG_EFLAGS_IF_BIT (1<<9)
#define REG_EFLAGS_IF_MASK ~REG_EFLAGS_IF_BIT

struct arch_regs {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t eflags;
    uint64_t rsp;
    uint64_t ss;
};


struct pagetable_64;
typedef struct pagetable_64 pgd_t;
typedef struct pagetable_64 pdpt_t;
typedef struct pagetable_64 pte_t;
typedef struct pagetable_64 pde_t;
#endif