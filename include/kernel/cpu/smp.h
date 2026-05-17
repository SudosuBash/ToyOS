#ifndef _TOYOS_X86_SMP_PERCPU
#define _TOYOS_X86_SMP_PERCPU

#include <kernel/stdint.h>
#include <hal.h>

#define DEFINE_PERCPU_VAR(name, type) __attribute__((section(".percpu"))) type name;
#define DECLARE_PERCPU_VAR(name, type) extern __attribute__((section(".percpu"))) type name;

#define THIS_CPU_PTR(var) ARCH_GET_PERCPU_VAR(var)
#define THIS_CPU_VAR(var) *(THIS_CPU_PTR(var))
#define SET_THIS_CPU_VAR(name, value) ARCH_SET_THIS_CPU_VAR(name, value)

#define READ_ONCE(var) (*(volatile typeof(&(var))*)&(var))
#define WRITE_ONCE(var, val) (*(volatile typeof(&(var)))&(var)) = (val);

void set_smp_base_addr(uintptr_t base);
uint8_t is_bsp_core();
void arch_barrier();
void preempt_enable();
void preempt_disable();
#endif