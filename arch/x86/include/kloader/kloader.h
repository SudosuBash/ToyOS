#ifndef _TOYOS_KLOADER_KLOADER
#define _TOYOS_KLOADER_KLOADER

#include <kernel/stdint.h>
struct boot_info {
    uint64_t phys_mem_info_addr; //物理内存信息
    uint64_t mem_bitmap_addr; //内存位图信息
    uint64_t kloader_pg_base_addr; //分页模式基地址
    uint64_t kern_sz;
    uint64_t kern_ldr_addr;
    uint64_t kern_ldr_vaddr;
};


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