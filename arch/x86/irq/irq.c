#include <irq/irq.h>
#include <asm.h>
#include <kernel/stdint.h>

#define IRQ_MAX_CNT 256

static volatile struct idt_gate gate[IRQ_MAX_CNT];
extern irq_entry_t irq_entry_table[IRQ_MAX_CNT];
volatile irq_handler_t irq_handler_fns[IRQ_MAX_CNT];
int irq_register(
    uint64_t num,
    irq_handler_t fn
) {

    if(num > 255) return 0;
    irq_handler_fns[num] = fn;
    return 1;
}

void init_irq() {
    for(int i=0;i<IRQ_MAX_CNT;i++) {
        uintptr_t irq_entry_ptr = (uintptr_t)irq_entry_table[i];
        gate[i].dpl= 0;
        gate[i].selector = CS_SELECTOR;
        gate[i].type = 0xe;
        gate[i].ist = 0;
        gate[i].p = 1;
        
        gate[i].offset_low =  irq_entry_ptr & 0xffff;
        gate[i].offset = irq_entry_ptr  >> 16;
    }

    struct idtr idt;
    idt.base = (uintptr_t)gate;
    idt.limit = sizeof(struct idt_gate) * IRQ_MAX_CNT - 1;
    
    barrier();
    lidt(idt);
}   