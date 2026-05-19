#include <irq/irq.h>
#include <irq/pic/pic.h>
#include <asm.h>
#include <kernel/stdint.h>
#include <kernel/cpu/smp.h>
#include <kernel/cpu/archimpl.h>
#include <irq/pic/apic.h>
#include <kernel/irq/irq.h>
#include <kernel/fault/fault.h>
#include <hal.h>
#include <cpu/cpu.h>
#include <kernel/kernel.h>

static volatile struct idt_gate gate[IRQ_MAX_CNT];
extern void* irq_entry_table[IRQ_MAX_CNT];
volatile irq_entry_t irq_entrance_gate[IRQ_MAX_CNT];

void init_idt() {
    for(int i=0;i<IRQ_MAX_CNT;i++) {
        uintptr_t irq_entry_ptr = (uintptr_t)irq_entry_table[i];
        gate[i].dpl= 0;
        gate[i].selector = KERNEL_CS;
        gate[i].type = 0xe;
        gate[i].ist = 0;
        gate[i].p = 1;
        
        gate[i].offset_low =  irq_entry_ptr & 0xffff;
        gate[i].offset = irq_entry_ptr  >> 16;
        irq_entrance_gate[i] = irq_entrance_fn;
    }
    gate[IRQ_DF_ERR].ist = DF_IRQ_IST_IDX;

    gate[IRQ_GP_ERR].ist = UD_DG_IRQ_IST_IDX;
    gate[IRQ_UD_ERR].ist = UD_DG_IRQ_IST_IDX;
    gate[IRQ_PG_ERR].ist = UD_DG_IRQ_IST_IDX; 
    //UD切换到新栈

    gate[IRQ_DBG_ERR].ist = DBG_IRQ_IST_IDX;
    //单独设置ist
    struct idtr idt;
    idt.base = (uintptr_t)gate;
    idt.limit = sizeof(struct idt_gate) * IRQ_MAX_CNT - 1;
    
    barrier();
    lidt(idt);
}

inline void irq_single_register(uint64_t num, irq_entry_t func) {
    if(num >= IRQ_MAX_CNT)
        return;
    irq_entrance_gate[num] = func;
}

inline int irqs_disabled() {
    uint64_t rfl;
    asm volatile (
        "pushfq\r\n"
        "pop %0\r\n"
        : "=r"(rfl));
    return (rfl & (1 << 9));
}

void init_irq_arch() {
    init_idt();
    init_pic();
    init_apic();
    fault_init();
}
