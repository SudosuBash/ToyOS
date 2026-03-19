#ifndef _TOYOS_EARLY_BOOT
#define _TOYIS_EARLY_BOOT

#include <packed_e820.h>
#include <kloader/kloader.h>
#include <pgtable/pgtable_kern.h>

#define PAGE_BIG_ROUND_UP(addr) ((addr) + PG_BIG_PAGE_SZ - 1)  & PG_BIG_PAGE_MASK

void init_pgtable();
//获取内存信息
uint64_t get_mem_info_paddr();
//获得内核加载处的开始 vaddr
uint64_t get_kern_vaddr();
//获取内核大小
uint64_t get_kern_sz();
#endif