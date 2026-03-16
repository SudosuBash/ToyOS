#ifndef _TOYOS_X86_ARCH_TASK
#define _TOYOS_X86_ARCH_TASK

#define STACK_OFFSET 40

#define switch_to(prev,next) asm volatile ( \
        "pushq %%rbp\n\t" \
        "movq %%rsp, %[new_task_rsp]\n\t" \
        "movq %[new_stack], %%rsp\n\t" \
        "popq %%rbp\n\t" \
        : [new_task_rsp] "=m"(prev->ksp) \
        : [new_stack] "m"(next->ksp) \
        : "memory", "cc" \
    );
//CS, RIP, RFLAGS, SS, RSP
#endif