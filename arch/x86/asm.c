#include <asm.h>
#include <kernel/irq/irq.h>
#include <kernel/cpu/archimpl.h>

#define CR0_WP_MASK 0xf000

#define __lfence() asm volatile("lfence")

inline void enable_irq() {
    asm volatile ("sti");
}

inline void disable_irq() {
    asm volatile ("cli");
}

inline void hlt() {
    asm volatile ("hlt");
}

inline void outb(uint16_t port, uint8_t data) {
    asm volatile(
        "outb %0,%1"
        :
        : "a"(data), "Nd"(port)
    );
}

inline void load_cr3(uint64_t addr) {
    asm volatile(
        "movq %0, %%rax\r\n"
        "movq %%rax, %%cr3\r\n"
        :
        : "r"(addr)
        : "rax", "memory"
    );
}

inline void open_cr0_wp() {
    uintptr_t cr0;
    asm volatile(
        "movq %%cr0, %0"
        : "=r"(cr0)
    );
    cr0 &= ~CR0_WP_MASK;
    asm volatile(
        "movq %0, %%cr0"
        :
        :"r"(cr0)
    );
}

inline void close_cr0_wp() {
    uintptr_t cr0;
    asm volatile(
        "movq %%cr0, %0"
        : "=r"(cr0)
    );
    cr0 |= CR0_WP_MASK;
    asm volatile(
        "movq %0, %%cr0"
        :
        :"r"(cr0)
    );
}

inline void lgdt(struct gdtr gdt) {
    asm volatile(
        "lgdt %0"
        : 
        : "m"(gdt)
    );
}

inline void lidt(struct idtr idt) {
    asm volatile(
        "lidt %0"
        : 
        : "m"(idt)
    );
}

inline uint64_t get_cr2() {
    uint64_t pgaddr;
    asm volatile(
        "movq %%cr2, %0"
        : "=r"(pgaddr)
    );
    return pgaddr;
}

inline void invlpg(uint64_t addr) {
    asm volatile(
        "invlpg (%[addr])"
        :
        :
        [addr] "r"(addr)
    );
}