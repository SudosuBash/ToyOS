#ifndef _TOYOS_KLOADER_KLOADER
#define _TOYOS_KLOADER_KLOADER

#include <kernel/stdint.h>
#include <gdt.h>
struct boot_info {
    uint64_t phys_mem_info_addr; //物理内存信息
    uint64_t kloader_pg_base_addr; //分页模式基地址
    uint64_t kern_sz;
    uint64_t kern_ldr_addr;
    uint64_t kern_ldr_vaddr;
};
#endif