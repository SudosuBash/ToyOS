#ifndef _TOYOS_X86_ARCH_TASK
#define _TOYOS_X86_ARCH_TASK

#define STACK_OFFSET 40

#define switch_to(prev, next, rff_flag) asm volatile ( \
        "pushq %%rbp\n\t" \
        "pushq %%rbx\n\t" \
        "pushq %%r12\n\t" \
        "pushq %%r13\n\t" \
        "pushq %%r14\n\t" \
        "pushq %%r15\n\t" \
        "movq %%rsp, %[new_task_rsp]\n\t" \
        "movq %[new_stack], %%rsp\n\t" \
        "testq %[ret_from_fork_flag], %[ret_from_fork_flag]\n\t" \
        "jnz ret_from_fork\n\t" \
        "popq %%r15\n\t" \
        "popq %%r14\n\t" \
        "popq %%r13\n\t" \
        "popq %%r12\n\t" \
        "popq %%rbx\n\t" \
        "popq %%rbp\n\t" \
        : [new_task_rsp] "=m"(prev->ksp) \
        : [new_stack] "m"(next->ksp), \
          [ret_from_fork_flag] "r"((rff_flag)) \
        : "memory", "cc" \
    );
//CS, RIP, RFLAGS, SS, RSP
#endif