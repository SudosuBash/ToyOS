#include <kernel/task/exec.h>
#include <kernel/task/task.h>
#include <kernel/mm/mmap.h>
#include <cpu/regs.h>
#include <cpu/gdt.h>
#include <kernel/mm/mm.h>
#include <kernel/stdlib.h>
#include <kernel/syscall/syscall.h>
#include <kernel/cpu/archimpl.h>
void arch_do_exec(
    int (*fn)(void*),
    struct task_struct* target
) {
    disable_irq();
    struct arch_regs* regs = (struct arch_regs*)arch_process_stack_bottom(target->kstack);
    regs-=1;
    regs->rip = 0x400000;
    regs->cs = USER_CS | USER_RPL;
    regs->ss = USER_DS | USER_RPL;
    //其实吧,这个就算改了regs, syscallq照样成立, 反正都是0
    regs->rsp = USER_STACK_POS+PAGE_SZ * 2;
    regs->eflags |= REG_EFLAGS_IF_BIT;
    target->ksp = (uint64_t)regs;
    set_tss_rsp_r0((uint64_t)arch_process_stack_bottom(target));
    load_cr3(VADDR2PHYS(target->mm_user.pg_root));
}