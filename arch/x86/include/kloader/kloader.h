#ifndef _TOYOS_KLOADER_KLOADER
#define _TOYOS_KLOADER_KLOADER

#include <stdint.h>
struct boot_info {
    uint32_t phys_mem_info_addr; //物理内存信息
    uint32_t mem_bitmap_addr; //内存位图信息
    uint32_t kloader_pg_paddr_top; //已经分到的物理内存页的上限地址
    uint32_t kloader_pg_base_addr; //分页模式基地址
    uint32_t kern_mem_phys_addr; //内核位于的物理内存
    uint32_t kern_sz;
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
const int s = sizeof(struct gdtr);
#endif