#include <kernel/mm/mm_page.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/mm/mm.h>
#include <mm/mm_info.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/data_struct/general.h>

struct kmem_cache main_cache;

void kmem_cache_free(void* addr) {
    uintptr_t paddr = (uintptr_t)addr;
    struct page* page = find_page_by_vaddr((uintptr_t)addr);
    paddr &= PAGE_MASK;
    *(uintptr_t**)paddr = page->block_start;

    page->block_start = (link_next_ptr_t*) paddr;
    struct kmem_cache* cache = page->cache;
    if(page->blocks == page->alloced_blocks) {
        
        if(&page->sibling == cache->full) //头部的话要next
            link_next(&cache->full);
        list_del(&page->sibling); //尾部的话sibling
        list_head_insert(&page->sibling,&cache->partial);
    }
    page->alloced_blocks--;
    if(page->alloced_blocks == 0) {
        if(&page->sibling == cache->partial)
            link_next(&cache->partial);
        list_del(&page->sibling);
        list_head_insert(&page->sibling,&cache->empty);
    }
}

void* kmem_cache_alloc(struct kmem_cache* cache) {
    if(cache == 0) return 0;
    if(cache->partial) {
        struct page* p = container_of(cache->partial, struct page, sibling);
        void* addr = (void*)p->block_start;

        p->alloced_blocks++;
        p->block_start = (link_next_ptr_t*)(*(p->block_start));
        if(p->block_start == 0) {
            struct linklist_head* prev = cache->partial;
            link_next(&cache->partial);
            list_del(prev);

            list_head_insert(&p->sibling, &cache->full);
        }
        return addr;
    }
cache_empty:
    if(cache->empty) {
        struct page* p = container_of(cache->empty, struct page, sibling);
        void* addr = (void*)p->block_start;
        p->block_start = (link_next_ptr_t*)(*(p->block_start));
        p->alloced_blocks++;
        struct linklist_head* prev = cache->empty;
        link_next(&cache->empty);
        list_del(prev);

        list_head_insert(&p->sibling,&cache->partial);
        return addr;
    } else {
        struct page* new_page = alloc_new_phys_page();
        init_page_mem(cache, new_page, cache->block_sz);
        cache->empty = &new_page->sibling;
        goto cache_empty; //重新分配
    }
    //到这儿说明出BUG了
    return 0;
}

struct kmem_cache* kmem_cache_get(uint32_t sz) {
    struct kmem_cache* mem = (struct kmem_cache*)kmem_cache_alloc(&main_cache);
    return mem;
}

void init_mm_slab() {
    main_cache.block_sz = sizeof(struct kmem_cache);   
}