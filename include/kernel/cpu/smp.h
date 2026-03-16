#ifndef _TOYOS_X86_SMP_PERCPU
#define _TOYOS_X86_SMP_PERCPU

#include <kernel/stdint.h>
#include <cpu/smp.h>

#define DEFINE_PERCPU_VAR(name, type) __attribute__((section(".percpu"))) type name;
#define DECLARE_PERCPU_VAR(name, type) extern __attribute__((section(".percpu"))) type name;

#define THIS_CPU_PTR(var) ARCH_GET_PERCPU_VAR(var)
#define THIS_CPU_VAR(var) *(THIS_CPU_PTR(var))
#define SET_THIS_CPU_VAR(name, value) ARCH_SET_THIS_CPU_VAR(name, value)
void set_smp_base_addr(uintptr_t base);
uint8_t is_bsp_core();
#endif