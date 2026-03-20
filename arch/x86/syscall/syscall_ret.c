
#include <kernel/mm/mm.h>
#include <kernel/task/task.h>
#include <asm.h>
#define pop_frame() ({ \
    asm volatile ( \
        "popq %r15\r\n" \
        "popq %r14\r\n" \
        "popq %r13\r\n" \
        "popq %r12\r\n" \
        "popq %r11\r\n" \
        "popq %r10\r\n" \
        "popq %r9\r\n" \
        "popq %r8\r\n" \
        "popq %rbp\r\n" \
        "popq %rdi\r\n" \
        "popq %rsi\r\n" \
        "popq %rdx\r\n" \
        "popq %rcx\r\n" \
        "popq %rbx\r\n" \
        "popq %rax\r\n" \
        "addq $16,%rsp\r\n" \
        "iretq\r\n" \
    ); \
})

void ret_to_user() {
    
    struct task_struct* current = CURRENT_PROCESS();
    uint64_t ksp = current->ksp;
    struct arch_regs* regs = (struct arch_regs*)current->kstack;
    load_cr3(VADDR2PHYS(current->mm_user.pg_root));
    asm volatile(
        "movq %[rsp],%%rsp\r\n"
        : 
        : [rsp] "r"(ksp)
    );
    pop_frame();
}