#ifndef _TOYOS_EARLY_BOOT
#define _TOYIS_EARLY_BOOT

#include <hal/packed_e820.h>
#include <kloader/kloader.h>
#include <hal/pgtable/pgtable_kern.h>

#define PAGE_BIG_ROUND_UP(addr) ((addr) + PG_BIG_PAGE_SZ - 1)  & PG_BIG_PAGE_MASK

void init_pgtable();
//获取内核大小
void init_early_acpi();
void early_init();
#endif