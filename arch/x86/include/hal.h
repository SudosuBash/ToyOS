#ifndef _TOYOS_X86_HAL
#define _TOYOS_X86_HAL

// hal.h
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
#endif