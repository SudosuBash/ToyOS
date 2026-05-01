#include <irq/irq.h>
#include <irq/pic/pic.h>
#include <asm.h>
#include <kernel/stdint.h>
#include <kernel/cpu/smp.h>
#include <kernel/cpu/archimpl.h>
#include <irq/pic/apic.h>
#include <kernel/irq/irq.h>
#include <kernel/fault/fault.h>
#include <cpu/regs.h>
#include <cpu/cpu.h>

#define IRQ_MAX_CNT 256

static volatile struct idt_gate gate[IRQ_MAX_CNT];
extern irq_entry_t irq_entry_table[IRQ_MAX_CNT];
volatile irq_handler_t irq_handler_fns[IRQ_MAX_CNT];

static void default_irq_handler(struct arch_regs* frame) {
    fault_irq("IRQ Triggered.",frame);
}

int irq_register(
    uint64_t num,
    irq_handler_t fn
) {

    if(num >= IRQ_MAX_CNT) return 0;
    irq_handler_fns[num] = fn;
    return 1;
}

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
        irq_handler_fns[i] = default_irq_handler;
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
void init_irq() {
    disable_irq();
    init_idt();
    init_pic();
    init_apic();
    fault_init();
} 
