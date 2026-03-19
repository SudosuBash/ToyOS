#include <pgtable/pgtable_kern.h>
#include <packed_e820.h>
#include <asm.h>

#include <kernel/mm/mm.h>
#include <kernel/put.h>
#include <kernel/irq/irq.h>
#include <early_boot.h>

static struct e820_entry* edr_table;
static uint32_t edr_entry;
static uint64_t kernel_end;
static uint64_t kernel_vstart;

#define KERNTOPADDR(addr) ((addr) - kernel_vstart)
static void pagefault_irq(struct arch_regs* frame) {
    arch_crash_on_irq("Segmentation Fault", frame);
}

uintptr_t get_kernel_end() {
    return kernel_end;
}
//返回可用的字节

inline void arch_barrier() {
    asm volatile("" ::: "memory");
}

void init_mm_info() {
    uint32_t* mem_info_addr = (uint32_t*)PHYS2VADDR(get_mem_info_paddr());
    edr_entry = *mem_info_addr;
    edr_table = (struct e820_entry*)(mem_info_addr+1);
    kernel_vstart = get_kern_vaddr();
    extern uint64_t __kernel_end;
    kernel_end = KERNTOPADDR((uintptr_t)&__kernel_end);
    
    irq_register(IRQ_PG_ERR, pagefault_irq);
}