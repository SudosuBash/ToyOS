#include <kernel/task/exec.h>
#include <kernel/task/task.h>
#include <kernel/mm/mmap.h>
#include <hal.h>
#include <cpu/gdt.h>
#include <kernel/mm/mm.h>
#include <kernel/stdlib.h>
#include <asm.h>
#include <kernel/cpu/archimpl.h>
void arch_do_exec(
    int (*fn)(void*),
    struct task_struct* target
) {
    disable_irq();
    struct arch_regs* regs = (struct arch_regs*)arch_process_stack_top(target);
    set_tss_rsp_r0((uint64_t)arch_process_stack_top(target));
    load_cr3(VADDR2PHYS(target->mm_user.pg_root));
    regs-=1;
    target->ksp = (uint64_t)regs;
    regs->rip = 0x400000;
    regs->cs = USER_CS | USER_RPL;
    regs->ss = USER_DS | USER_RPL;
    //其实吧,这个就算改了regs, syscallq照样成立, 反正都是0
    regs->rsp = USER_STACK_POS+PAGE_SZ * 2;
    regs->eflags |= REG_EFLAGS_IF_BIT;
    
    
    //后续会加入手动返回逻辑, 因为内核本身不走 syscall 路径
    asm volatile (
        "movq %[value], %%rsp\r\n"
        "swapgs\r\n"
        "jmpq *%[fork]"
        :
        :
        [value] "r"(regs),
        [fork] "r"((uint64_t)ret_from_fork)
    );
}