#ifndef _TOYOS_MEM
#define _TOYOS_MEM

#include <kernel/config.h>
#include <kernel/stdint.h>
#include <kernel/data_struct/linklist.h>
#include <mm/mm_info.h>

#define NULL ((void*)0)

#define MM_BUDDY_MAX_LEVEL 11

#define MM_BUDDY_LEVEL_SIZE(level) (1 << ((level) + PAGE_OFFSET - 1))
#define MM_BUDDY_MAX_LEVEL_SIZE() MM_BUDDY_LEVEL_SIZE(MM_BUDDY_MAX_LEVEL)
#define MM_BUDDY_LEVEL_PAGES(level) (1 << (level - 1))
#define MM_BUDDY_MAX_LEVEL_PAGES(level) MM_BUDDY_LEVEL_PAGES(MM_BUDDY_MAX_LEVEL)

#define MM_BUDDY_FLAG_HEAD 0b1
#define MM_BUDDY_FLAG_TAIL 0b10

struct mm_buddy {
    struct linklist_head buddys[MM_BUDDY_MAX_LEVEL];
};

void* kmalloc(size_t sz);
void kfree(void* addr);
struct page* alloc_page(uint64_t pages);
void free_page(struct page* page);
void init_mm();
void kmalloc_init();
//向上取整
int highest_page_up_1(uint64_t x);
int highest_page_1(uint64_t x);

uintptr_t get_kernel_end();
uint64_t get_available_mem_sz();
void init_mm_info();
uint64_t get_kern_addr();
#endif