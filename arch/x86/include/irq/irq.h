#ifndef _TOYOS_X86_IRQ
#define _TOYOS_X86_IRQ

#include <kernel/stdint.h>

#define IRQ_DIV_ERR 0
#define IRQ_PG_ERR 14
#define IRQ_UD_ERR 6
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

//要倒过来, 因为rsp在栈底
struct irq_frame {
    uint64_t r15,r14,r13,r12,r11,r10,r9,r8;
    uint64_t rbp,rdi,rsi,rdx,rcx,rbx,rax;
    uint64_t irq_num, error_code, rip, cs, eflags, rsp, ss;
}__attribute__((packed));


typedef void (*irq_entry_t)();
//这个就一个入口函数
#endif