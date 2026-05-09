#include <pgtable/pgtable_kern.h>
#include <packed_e820.h>
#include <asm.h>
#include <cpu/cpu.h>
#include <kernel/mm/mm.h>
#include <kernel/put.h>
#include <kernel/irq/irq.h>
#include <early_boot.h>
#include <cpu/regs.h>
#include <kernel/fault/error.h>
#include <kernel/task/task.h>
#include <kernel/mm/mm_user.h>
#include <kernel/cpu/archimpl.h>

static struct e820_entry* edr_table;
static uint32_t edr_entry;
static uint64_t kernel_end;
static uint64_t kernel_vstart;

#define KERNTOPADDR(addr) ((addr) - kernel_vstart)

inline static void init_user_page(pte_t* pte,struct user_vm_area* area, uintptr_t mem) {
    pte->rw = (area->perm & PERM_W) >> 1;
    pte->us = 1;
    pte->present = 1;
    pte->nx = !(area->perm & PERM_X);
    pte->base_addr = (VADDR2PHYS(mem)) >> PAGE_OFFSET;
}

static void pagefault_irq(struct arch_regs* frame) {
    if(!(frame->error_code & PG_ERR_PERM))
        arch_crash_on_irq("Page Fault", frame);
    
    if(frame->error_code & PG_ERR_PRESENT) 
        return; //此处应该发出 SIGSEGV 信号
    
    uint64_t addr = get_cr2();
    struct task_struct* current = CURRENT_PROCESS();
    rwlock_read_lock(&current->rwlock);
    struct user_vm_area* area = find_vm_area(&current->mm_user, addr);
    if(area == NULL) {
        rwlock_read_unlock(&current->rwlock);
        return; //此处应该发出 SIGSEGV 信号
    }

    void* mem = alloc_page(1);
    pte_t* pte = get_user_pte(addr, current->mm_user.pg_root);
    init_user_page(pte, area, (uintptr_t)mem);
    invlpg(addr);
    rwlock_read_unlock(&current->rwlock);
}

uintptr_t get_kernel_end() {
    return kernel_end;
}
//返回可用的字节

void init_mm_info() {
    uint32_t* mem_info_addr = (uint32_t*)PHYS2VADDR(get_mem_info_paddr());
    edr_entry = *mem_info_addr;
    edr_table = (struct e820_entry*)(mem_info_addr+1);
    kernel_vstart = get_kern_vaddr();
    extern uint64_t __kernel_end;
    kernel_end = KERNTOPADDR((uintptr_t)&__kernel_end);
    
    irq_register(IRQ_PG_ERR, pagefault_irq);
}