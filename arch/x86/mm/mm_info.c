#include <pgtable.h>
#include <packed_e820.h>
#include <asm.h>

#include <kernel/mm/mm.h>
#include <kernel/put.h>
#include <irq/irq.h>
#include <early_boot.h>

static struct e820_entry* edr_table;
static uint32_t edr_entry;
static uint32_t avl_mem_index;
static uint64_t kernel_end;
static uint64_t kernel_vstart;

#define KERNTOPADDR(addr) ((addr) - kernel_vstart)
static void pagefault_irq(struct irq_frame* frame) {
    put_str("Page Fault Error Info: \n");
    put_str("  Status is ");
    put_hex(frame->error_code);
    put_str(",\n  Vaddr is ");
    uint64_t pg_addr = get_cr2();
    put_hex(pg_addr);
    put_char('\n');
    while(1);
}

uintptr_t get_kernel_end() {
    return kernel_end;
}
//返回可用的字节
uint64_t get_available_mem_sz() {
    struct e820_entry entry = edr_table[avl_mem_index];
    return entry.leng;
}

void init_mm_info() {
    uint32_t* mem_info_addr = (uint32_t*)PHYS2VADDR(get_mem_info_paddr());
    edr_entry = *mem_info_addr;
    edr_table = (struct e820_entry*)(mem_info_addr+1);
    avl_mem_index = get_avl_mem_index();
    
    kernel_vstart = get_kern_vaddr();
    extern uint64_t __kernel_end;
    kernel_end = KERNTOPADDR((uintptr_t)&__kernel_end);
    
    irq_register(IRQ_PG_ERR, pagefault_irq);
}