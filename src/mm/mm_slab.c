#include <kernel/mm/mm_page.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/mm/mm.h>
#include <mm/mm_info.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/data_struct/general.h>
#include <kernel/fault/fault.h>

#define MM_SLAB_EXPANSION_VOODOO 10
struct kmem_cache main_cache;

void kmem_cache_free(void* addr) {
    assert(addr != NULL);
    uintptr_t paddr = (uintptr_t)addr;
    struct page* page = find_page_by_vaddr((uintptr_t)addr);
    paddr &= PAGE_MASK;
    *(uintptr_t**)paddr = page->block_start;

    page->block_start = (link_next_ptr_t*) paddr;
    struct kmem_cache* cache = page->cache;
    if(page->blocks == page->alloced_blocks) {
        list_del_init(&page->sibling); //尾部的话sibling
        list_insert(&page->sibling,&cache->partial);
    }
    page->alloced_blocks--;
    if(page->alloced_blocks == 0) {
        list_del_init(&page->sibling);
        list_insert(&page->sibling,&cache->partial);
    }
}

void* kmem_cache_alloc(struct kmem_cache* cache) {
    assert(cache != NULL);
   
cache_partial:
    if(!list_empty(&cache->partial)) {
        struct page* p = container_of(cache->partial.next, struct page, sibling);
        void* addr = (void*)p->block_start;
        p->alloced_blocks++;
        p->block_start = (link_next_ptr_t*)(*(p->block_start));
        if(p->block_start == 0) {
            list_del(cache->partial.next);
            list_insert(&p->sibling, &cache->full);
        }
        return addr;
    }
cache_empty:
    if(!list_empty(&cache->empty)) {
        struct page* p = container_of(cache->empty.next, struct page, sibling);
        list_del(cache->empty.next);
        list_insert(&p->sibling,&cache->partial);
        goto cache_partial;
    } else {
        int alloc_count = ((cache->block_sz * MM_SLAB_EXPANSION_VOODOO) + PAGE_SZ - 1) & PAGE_MASK;
        alloc_count>>=PAGE_OFFSET;
        struct page* new_page = alloc_page(alloc_count);

        if(new_page == NULL) return NULL; //分配失败直接返回0
        init_page_mem(cache, new_page, cache->block_sz);
        INIT_LIST_HEAD(&new_page->sibling);
        list_insert(&new_page->sibling,&cache->empty);
        goto cache_empty; //重新分配
    }
    panic("kmem_cache_alloc() went to a wrong place!");
    return NULL;
}

struct kmem_cache* kmem_cache_get(uint32_t sz) {
    struct kmem_cache* mem = (struct kmem_cache*)kmem_cache_alloc(&main_cache);
    mem->block_sz = sz;
    INIT_LIST_HEAD(&mem->partial);
    INIT_LIST_HEAD(&mem->full);
    INIT_LIST_HEAD(&mem->empty);
    return mem;
}

void init_mm_slab() {
    main_cache.block_sz = sizeof(struct kmem_cache);   
    INIT_LIST_HEAD(&main_cache.partial);
    INIT_LIST_HEAD(&main_cache.full);
    INIT_LIST_HEAD(&main_cache.empty);
}