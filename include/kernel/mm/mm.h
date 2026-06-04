#ifndef _TOYOS_MEM
#define _TOYOS_MEM

#include <kernel/config.h>
#include <kernel/stdint.h>
#include <kernel/atomic/spinlock.h>
#include <kernel/base/linklist.h>
#include <hal/hal.h>
#include <kernel/def.h>
#include <kernel/cpu/smp.h>


#define MM_BUDDY_MAX_LEVEL 11

#define MM_BUDDY_LEVEL_SIZE(level) (1 << ((level) + PAGE_OFFSET - 1))
#define MM_BUDDY_MAX_LEVEL_SIZE() MM_BUDDY_LEVEL_SIZE(MM_BUDDY_MAX_LEVEL)
#define MM_BUDDY_LEVEL_PAGES(level) (1 << (level - 1))
#define MM_BUDDY_MAX_LEVEL_PAGES(level) MM_BUDDY_LEVEL_PAGES(MM_BUDDY_MAX_LEVEL)

#define MM_BUDDY_FLAG_HEAD 0b1
#define MM_BUDDY_FLAG_TAIL 0b10
#define MM_BUDDY_FLAG_RESERVED 0b100
#define MM_BUDDY_FLAG_MMAP 0b1000

#define PAGE_ROUND_UP(addr) (((addr) + PAGE_SZ - 1) & PAGE_MASK)
#define PHYS2VADDR(addr) ((uint64_t)(addr) + (KERNEL_MEM_SA_VADDR))
#define PHYS2VADDR_MMIO(addr) ((uint64_t)(addr) + (MMIO_MEM_SA_VADDR))
#define VADDR2PHYS_MMIO(addr) ((uint64_t)(addr) - (MMIO_MEM_SA_VADDR))
#define VADDR2PHYS(addr) ((uintptr_t)(addr) - (KERNEL_MEM_SA_VADDR))

#define USER_STACK_POS 0x7fffffffe000
#define STACK_SZ 2 * PAGE_SZ

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

#define GFP_KERNEL 0x1
#define GFP_ATOMIC 0x2

void* kmalloc(size_t sz, uint64_t flag);
void kfree(void* addr);
struct page* alloc_page(uint64_t pages);
void free_page(struct page* page);
void init_mm();
void kmalloc_init();

struct page* find_head_page(struct page* page);
void ref_page(struct page* page);
uint8_t unref_and_test_page(struct page* page);
uint64_t get_mem_alloc_percentage();
void init_mm_arch();
uint64_t get_system_mem_alloced();
uint64_t get_system_mem_sum();
void* kzalloc(size_t sz, uint64_t flag);
void* early_kmalloc(size_t sz);
void init_mm_early();
void init_mm_cpu();
#endif