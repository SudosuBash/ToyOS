#ifndef _TOYOS_ASM
#define _TOYOS_ASM

#include <kernel/stdint.h>
#include <kernel/fault/fault.h>
#include <hal/cpu/gdt.h>
#include <hal/irq/irq.h>

#define ltr(selector) asm volatile ("ltr %0": :"r"((uint16_t)selector))

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);
void outw(uint16_t port, uint16_t data);
uint16_t inw(uint16_t port);
void outl(uint16_t port, uint32_t data);
uint32_t inl(uint16_t port);

#define lgdt(gdt) asm volatile( \
        "lgdt %0" \
        : \
        : "m"(gdt) \
    );

#define lidt(idt) asm volatile( \
        "lidt %0" \
        :  \
        : "m"(idt) \
    );
    
#define load_cr3(paddr)  asm volatile( \
        "movq %0, %%rax\r\n" \
        "movq %%rax, %%cr3\r\n" \
        : \
        : "r"(paddr) \
        : "rax", "memory" \
    );
void open_cr0_wp();
void close_cr0_wp();
void arch_enable_pge();
#endif