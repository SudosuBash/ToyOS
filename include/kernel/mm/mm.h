#ifndef _TOYOS_MEM
#define _TOYOS_MEM

#include <kernel/config.h>
#include <kernel/stdint.h>
#include <kernel/data_struct/linklist.h>

#define PAGE_OFFSET 12
#define PAGE_SZ 4096
#define PAGE_MASK ~(PAGE_SZ-1)

#define PHYS2VADDR(addr) ((addr) + (KERNEL_MEM_SA_VADDR))
#define VADDR2PHYS(addr) ((addr) - (KERNEL_MEM_SA_VADDR))

#define MM_BUDDY_MAX_LEVEL 11

#define MM_BUDDY_LEVEL_SIZE(level) (1 << ((level) + PAGE_OFFSET - 1))
#define MM_BUDDY_MAX_LEVEL_SIZE() MM_BUDDY_LEVEL_SIZE(MM_BUDDY_MAX_LEVEL)
#define MM_BUDDY_LEVEL_PAGES(level) (1 << (level - 1))
#define MM_BUDDY_MAX_LEVEL_PAGES(level) MM_BUDDY_LEVEL_PAGES(MM_BUDDY_MAX_LEVEL)
struct mm_buddy {
    struct linklist_head* buddys[MM_BUDDY_MAX_LEVEL];
};

void* kmalloc(size_t sz);
struct page* alloc_page(uint64_t pages);
void free_page(struct page* page);
void init_mm();
#endif