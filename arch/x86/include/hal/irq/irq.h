#ifndef _TOYOS_X86_IRQ
#define _TOYOS_X86_IRQ

#include <kernel/stdint.h>
#include <hal/hal.h>

#define IRQ_DIV_ERR 0
#define IRQ_DBG_ERR 3
#define IRQ_DF_ERR 8
#define IRQ_GP_ERR 13
#define IRQ_PG_ERR 14
#define IRQ_UD_ERR 6

#define DBG_IRQ_IST_IDX 1
#define DF_IRQ_IST_IDX 2
#define UD_DG_IRQ_IST_IDX 5

struct idt_gate {
    uint64_t offset_low:16;
    uint64_t selector:16;
    uint64_t ist:3;
    uint64_t reserved:5;
    uint64_t type:4;
    uint64_t reserved_2:1;
    uint64_t dpl:2;
    uint64_t p:1;
    uint64_t offset:48;
    uint64_t reserved_3:32;
}__attribute__((packed, aligned(16)));

struct idtr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

typedef void (*irq_entry_t)(struct arch_regs* arch, uint64_t num);
void irq_single_register(uint64_t num, irq_entry_t func);
//这个就一个入口函数
#endif