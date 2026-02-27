#ifndef _TOYOS_X86_SMP_PERCPU
#define _TOYOS_X86_SMP_PERCPU

#define DEFINE_PERCPU_VAR(name, type) __attribute__((section(".percpu"))) type name;

void smp_percpu_init();
#endif