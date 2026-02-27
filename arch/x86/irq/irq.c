#include <irq/irq.h>
#include <asm.h>
#include <kernel/stdint.h>

#define IRQ_MAX_CNT 256

static struct idt_gate gate[IRQ_MAX_CNT];
extern irq_entry_t irq_entry_table[IRQ_MAX_CNT];
irq_handler_t irq_handler_fns[IRQ_MAX_CNT];
int irq_register(
    uint64_t num,
    irq_handler_t fn
) {

    if(num > 255) return 0;
    irq_handler_fns[num] = fn;
    return 1;
}

void init_irq() {
    struct idt_gate igate = {0};
    igate.dpl= 0;
    igate.selector = CS_SELECTOR;
    igate.type = 0xe;
    igate.ist = 0;
    igate.p = 1;
    

    for(int i=0;i<IRQ_MAX_CNT;i++) {
        uintptr_t irq_entry_ptr = (uintptr_t)irq_entry_table[i];
        igate.offset_low =  irq_entry_ptr & 0xffff;
        igate.offset = irq_entry_ptr  >> 16;
        gate[i] = igate;
    }

    struct idtr idt;
    idt.base = (uintptr_t)gate;
    idt.limit = sizeof(struct idt_gate) * IRQ_MAX_CNT - 1;
    
    lidt(idt);
}