#ifndef _TOYOS_MEM
#define _TOYOS_MEM

#include <kernel/math.h>
#include <kernel/config.h>
#include <kernel/stdint.h>
#include <kernel/atomic/spinlock.h>
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
#define MM_BUDDY_FLAG_RESERVED 0b100

#define PAGE_ROUND_UP(addr) (((addr) + PAGE_SZ - 1) & PAGE_MASK)

#define barrier() arch_barrier()
struct mm_area {
    uint64_t from;
    uint64_t to;
};

struct mm_area_record {
    struct mm_area* area;
    int num;
    
};

struct mm_buddy {
    struct linklist_head groups[MM_BUDDY_MAX_LEVEL];
    spinlock_t buddy_lock;
};

void* kmalloc(size_t sz);
void kfree(void* addr);
struct page* alloc_page(uint64_t pages,int slab);
void free_page(struct page* page,int slab);
void init_mm();
void kmalloc_init();

uint64_t get_mem_all_pages(); //向下取整
uintptr_t get_kernel_end();
uint64_t get_machine_available_mem_sz();
void init_mm_info();
uint64_t get_kern_addr();
uint64_t get_system_mem_alloced();
uint64_t get_system_mem_sum();
struct mm_area_record* get_mem_records();
#endif