#include <hal/irq/irq.h>
#include <hal/asm.h>
#include <kernel/stdint.h>
#include <kernel/cpu/smp.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/irq/irq.h>
#include <kernel/fault/fault.h>
#include <hal/hal.h>
#include <hal/cpu/cpu.h>
#include <kernel/kernel.h>
#include <hal/hal.h>

/**
 * Interrupt:  
 *      公共资源: 1)idt 门, 2)idt 门对应的物理中断入口点 3) 中断仲裁器
 *      私有资源: irq 真正的入口点函数
 */

static volatile struct idt_gate gate[IRQ_MAX_CNT];
static struct idtr idt_global;

extern void* irq_entry_table[IRQ_MAX_CNT];
DEFINE_PERCPU_VAR(irq_entrance_gate_percpu[IRQ_MAX_CNT], irq_entry_t);
extern struct system_static_data sysdata;

static void init_idt() {
    for(int i=0;i<IRQ_MAX_CNT;i++) {
        uintptr_t irq_entry_ptr = (uintptr_t)irq_entry_table[i];
        gate[i].dpl= 0;
        gate[i].selector = KERNEL_CS;
        gate[i].type = 0xe;
        gate[i].ist = 0;
        gate[i].p = 1;
        
        gate[i].offset_low =  irq_entry_ptr & 0xffff;
        gate[i].offset = irq_entry_ptr  >> 16;
    }
    gate[IRQ_DF_ERR].ist = DF_IRQ_IST_IDX;

    gate[IRQ_GP_ERR].ist = UD_DG_IRQ_IST_IDX;
    gate[IRQ_UD_ERR].ist = UD_DG_IRQ_IST_IDX;
    gate[IRQ_PG_ERR].ist = UD_DG_IRQ_IST_IDX; 
    //UD切换到新栈

    gate[IRQ_DBG_ERR].ist = DBG_IRQ_IST_IDX;
    //单独设置ist

    idt_global.base = (uintptr_t)gate;
    idt_global.limit = sizeof(struct idt_gate) * IRQ_MAX_CNT - 1;

    barrier();
}

static void init_irq_entry() {
    irq_entry_t (*irq_entrance_gate)[IRQ_MAX_CNT] = THIS_CPU_PTR(irq_entrance_gate_percpu);
    for(int i=0;i<IRQ_MAX_CNT;i++) {
        (*irq_entrance_gate)[i] = irq_entrance_fn;
    }
}

inline void irq_single_register(uint64_t num, irq_entry_t func) {
    irq_entry_t (*irq_entrance_gate)[IRQ_MAX_CNT] = THIS_CPU_PTR(irq_entrance_gate_percpu);
    if(num >= IRQ_MAX_CNT)
        return;
    (*irq_entrance_gate)[num] = func;
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
}

void init_irq_arch_cpu() {
    init_irq_entry();
    fault_init();
    lidt(idt_global);
    //中断表格是 percpu 的
    //但是 irq_entry 这个中断响应函数是全局的
}