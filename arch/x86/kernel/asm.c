#include <hal/asm.h>
#include <kernel/irq/irq.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/cpu/smp.h>

#define CR0_WP_MASK 0xf000

#define PCI_ADDRESS(bus, device, function, reg) (((1) << 31) | ((bus) << 16) | ((device) << 11) | ((function) << 8) | ((reg) << 2))
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

inline uint64_t read_pci_configuration(uint16_t bus, uint16_t device, uint16_t function, uint16_t reg, uint64_t width) {
    uint64_t base = reg & 0xfc, offset = reg & 0x03;
    assert(width + offset <= 4);
    preempt_disable();
    outl(0xCF8, PCI_ADDRESS(bus, device, function, base));
    uint64_t val = (uint64_t)inl(0xCFC);
    val >>= (offset << 3);
    switch(width) {
        case 8: val = (uint8_t)val; break;
        case 16: val = (uint16_t)val; break;
        case 32: val = (uint32_t)val; break;
    } 
    preempt_enable();
    return val;
}

inline void write_pci_configuration(uint16_t bus, uint16_t device, uint16_t function, uint16_t reg, uint64_t width, uint64_t value) {
    uint64_t base = reg & 0xfc, offset = reg & 0x03;
    assert(width + offset <= 4);
    preempt_disable();
    outl(0xCF8, PCI_ADDRESS(bus, device, function, base));
    uint64_t val = (uint64_t)inl(0xCFC), mask = (uint64_t)(-1);
    value = (value & ((1ULL << width) - 1)) << (offset << 3);
    mask = ~(mask & ((1ULL << width) - 1)) << (offset << 3);
    val &= mask;
    val |= value;
    outl(0xCFC, val);
    preempt_enable();
}