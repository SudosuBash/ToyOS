#ifndef _TOYOS_X86_SMP
#define _TOYOS_X86_SMP

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
#endif