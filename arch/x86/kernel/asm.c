#include <asm.h>
#include <kernel/irq/irq.h>
#include <kernel/cpu/archimpl.h>

#define CR0_WP_MASK 0xf000

inline void outb(uint16_t port, uint8_t data) {
    asm volatile(
        "outb %0,%1"
        :
        : "a"(data), "d"(port)
    );
}

inline void outw(uint16_t port, uint16_t data) {
    asm volatile(
        "outw %0,%1"
        :
        : "a"(data), "d"(port)
    );
}

inline void outl(uint16_t port, uint32_t data) {
    asm volatile(
        "outl %0,%1"
        :
        : "a"(data), "d"(port)
    );
}

inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile (
        "inb %1, %0" : "=a"(value) : "d"(port)
    );
    return value;
}

inline uint16_t inw(uint16_t port) {
    uint16_t value;
    asm volatile (
        "inw %1, %0" : "=a"(value) : "d"(port)
    );
    return value;
}

inline uint32_t inl(uint16_t port) {
    uint32_t value;
    asm volatile (
        "inl %1, %0" : "=a"(value) : "d"(port)
    );
    return value;
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

inline uintptr_t get_pf_addr() {
    uintptr_t pgaddr;
    asm volatile(
        "movq %%cr2, %0"
        : "=r"(pgaddr)
    );
    return pgaddr;
}

inline uintptr_t get_pgroot() {
    uintptr_t pgaddr;
    asm volatile(
        "movq %%cr3, %0"
        : "=r"(pgaddr)
    );
    return pgaddr;
}

inline void arch_enable_pge() {
    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 7);
    asm volatile("mov %0, %%cr4" : : "r"(cr4) : "memory");
}