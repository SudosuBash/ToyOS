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

struct tss {
    uint64_t reserved_1:4;
    uint64_t rsp_ring0:8;
    uint64_t rsp_ring1:8;
    uint64_t rsp_ring2:8;
    uint64_t reserved_2:8;
    uint64_t ist[7];
    uint64_t reserved:16;
    uint64_t io_map:16;
};

struct gdtr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

void init_gdt();
void reload_gdt();
void assign_tss_percpu();

#endif