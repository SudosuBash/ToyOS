#ifndef _TOYOS_X86_GDT
#define _TOYOS_X86_GDT

#include <kernel/stdint.h>

struct gdt {
    uint64_t limit:16;
    uint64_t base_1:16;
    uint64_t base_2:8;
    uint64_t access_byte:8;
    uint64_t limit_2:4;
    uint64_t flags:4;
    uint64_t base:8;
};

struct gdtr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

#endif