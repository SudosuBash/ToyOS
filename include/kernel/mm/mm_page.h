#ifndef _TOYOS_MM_PAGE
#define _TOYOS_MM_PAGE
#include <kernel/stdint.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/atomic/spinlock.h>

typedef uintptr_t link_next_ptr_t;

struct page {
    struct linklist_head sibling; //位于 slab 的节点位置
    struct linklist_head buddy_sibling; //buddy 的节点位置
    link_next_ptr_t* block_start; //块开始位置
    union {
        struct kmem_cache* cache;
        struct page* page_head;
    };
    atomic_t pg_ref;
    uint32_t alloced_blocks; //已经分配的数量
    uint8_t page_flags;
    uint8_t buddy_level:4;
    uint8_t in_use:1; //是否在buddy system中
};

struct page* find_page_by_paddr(uintptr_t ptr);
struct page* find_page_by_vaddr(uintptr_t ptr);
void* get_page_vaddr(struct page* page);
void init_page_mem(struct kmem_cache* cache,struct page* pg, uint32_t sz);
#endif