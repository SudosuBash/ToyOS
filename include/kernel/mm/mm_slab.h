#ifndef _TOYOS_KMEM_SLAB

#define _TOYOS_KMEM_SLAB

#include <kernel/mm/mm_page.h>
#include <kernel/data_struct/linklist.h>
struct kmem_cache {
    uint32_t block_sz;
    uint32_t need_alloc_blks; //为了尽量少用除法，用它做缓存
    struct linklist_head* full; //完全被占用
    struct linklist_head* partial; //部分被占用
    struct linklist_head* empty; //empty的页表
};

void kmem_cache_free(void* addr);
void init_mm_slab();
void* kmem_cache_alloc(struct kmem_cache* cache);
struct kmem_cache* kmem_cache_get(uint32_t sz);
#endif