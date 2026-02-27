#include <asm.h>
#include <kernel/atomic/atomic_type.h>

#define CR0_WP_MASK 0xf000

#define __lfence() asm volatile("lfence")

inline void outb(uint16_t port, uint8_t data) {
    asm volatile(
        "outb %0,%1"
        :
        : "a"(data), "Nd"(port)
    );
}

void atomic_inc(atomic_t *val) {
    asm volatile (
        "lock incq %0"
        : "+m" (val->count)
        :
        :"cc"
    );
}

uint8_t atomic_dec_and_test(atomic_t* val) {
    uint8_t zf;
    asm volatile (
        "lock decq %0\t\n"
        "setz %1"
        : "+m" (val->count),
          "=q" (zf)
        //这部分必须用zf, 否则引入竞争条件
        :
        :"cc"
    );
    return zf;
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

void lgdt(struct gdtr gdt) {
    asm volatile(
        "lgdt %0"
        : 
        : "m"(gdt)
    );
}

void lidt(struct idtr idt) {
    asm volatile(
        "lidt %0"
        : 
        : "m"(idt)
    );
}

uint64_t get_cr2() {
    uint64_t pgaddr;
    asm volatile(
        "movq %%cr2, %0"
        : "=r"(pgaddr)
    );
    return pgaddr;
}