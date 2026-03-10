#ifndef _TOYOS_EARLY_BOOT
#define _TOYIS_EARLY_BOOT

#include <packed_e820.h>
#include <kloader/kloader.h>
#include <pgtable.h>

//获取可用内存的 e820 索引
uint32_t get_avl_mem_index();

void init_pgtable();
//获取内存信息
uint64_t get_mem_info_paddr();
//获得内核加载处的开始 vaddr
uint64_t get_kern_vaddr();
//获取内核大小
uint64_t get_kern_sz();
uint64_t get_kern_addr();
#endif