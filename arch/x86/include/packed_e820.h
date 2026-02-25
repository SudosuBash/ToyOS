#ifndef _TOYOS_PACKED_E820
#define _TOYOS_PACKED_E820

#include <kernel/stdint.h>
#include <config_arch.h>

#define E820_ENTRY_COUNT() ((int)(*KERNEL_MEM_INFO_VADDR))

struct e820_entry {
    uint64_t base_addr;
    uint64_t leng;
    uint32_t type;
    uint32_t extended_attr;
}__attribute__((packed));


#endif