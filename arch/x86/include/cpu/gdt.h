#ifndef _TOYOS_X86_GDT
#define _TOYOS_X86_GDT

#include <kernel/stdint.h>
#include <cpu/gdt_config.h>

struct gdt_desc {
    uint16_t limit;
    uint16_t base_1;
    uint8_t base_2;
    uint8_t access_byte;
    uint8_t limit_2:4;
    uint8_t flags:4;
    uint8_t base_3;
}__attribute__((packed));

struct tss_gdt_desc {
    struct gdt_desc desc;
    uint32_t high_32;
    uint32_t reserved;
}__attribute__((packed));

struct tss {
    uint32_t reserved_1;
    uint64_t rsp_ring0;
    uint64_t rsp_ring1;
    uint64_t rsp_ring2;
    uint64_t reserved_2;
    uint64_t ist[7];
    uint64_t reserved_3;
    uint16_t reserved_4;
    uint16_t io_map;
}__attribute__((packed));

union x86_gdt_tss_desc {
    struct gdt_desc gdts[2];
    struct tss_gdt_desc tss;
};


struct gdtr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

void init_gdt();
void reload_gdt();
void set_tss_rsp_r0(uintptr_t addr);
void set_tss_ist(uint8_t index,uint64_t addr);
#endif