#ifndef _TOYOS_MM_PAGE
#define _TOYOS_MM_PAGE
#include <kernel/stdint.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/atomic/spinlock.h>

typedef uintptr_t link_next_ptr_t;
struct page {
    uint32_t block_sz; //页面分块大小
    uint32_t blocks; //总分配数
    uint32_t alloced_blocks; //已经分配的数量
    struct linklist_head sibling; //位于 slab 的节点位置
    struct linklist_head buddy_sibling; //buddy 的节点位置
    link_next_ptr_t* block_start; //块开始位置
    union {
        struct kmem_cache* cache;
        struct page* page_head;
    };
    uint8_t buddy_level;
    uintptr_t vaddr;
    uint8_t in_use; //是否在buddy system中
    uint8_t page_flags;
};

struct page* find_page_by_vaddr(uintptr_t ptr);
void* get_page_vaddr(struct page* page);
void init_page_mem(struct kmem_cache* cache,struct page* pg, uint32_t sz);
#endif