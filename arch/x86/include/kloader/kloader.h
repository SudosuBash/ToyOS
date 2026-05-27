#ifndef _TOYOS_KLOADER_KLOADER
#define _TOYOS_KLOADER_KLOADER

#include <kernel/stdint.h>
#include <hal/cpu/gdt.h>
struct boot_info {
    volatile uint64_t phys_mem_info_addr; //物理内存信息
    volatile uint64_t kloader_pg_base_addr; //分页模式基地址
    volatile uint64_t kern_sz;
    volatile uint64_t kern_ldr_addr;
    volatile uint64_t kern_ldr_vaddr;
    volatile uint32_t acpi_rsdp_pos;
};
#endif