#include <kernel/mm/mm_page.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/mm/mm.h>
#include <mm/mm_info.h>
#include <kernel/base/linklist.h>
#include <kernel/kernel.h>
#include <kernel/fault/fault.h>
#include <kernel/put.h>
#include <kernel/fault/error.h>

#define MM_SLAB_EXPANSION_VOODOO 10
struct kmem_cache main_cache;

void kmem_cache_free(void* addr) {
    assert(addr != NULL);
    uintptr_t paddr = (uintptr_t)addr;
    struct page* page = find_page_by_vaddr((uintptr_t)addr);
    if(!(page->page_flags & MM_BUDDY_FLAG_TAIL)) {
        page = find_head_page(page);
    }
    
    struct kmem_cache* cache = page->cache;

    spin_lock(&cache->cache_lock);
    paddr &= PAGE_MASK;
    *(uintptr_t**)paddr = page->block_start;
    
    if(page->block_start == 0) {
        list_del_init(&page->sibling); //尾部的话sibling
        list_insert(&page->sibling,&cache->partial);
    }

    page->block_start = (link_next_ptr_t*) paddr;
    page->alloced_blocks--;
    if(page->alloced_blocks == 0) {
        list_del_init(&page->sibling);
        atomic_dec_and_test(&cache->cache_using_blks);
        spin_unlock(&cache->cache_lock);
        free_page(page);
    } else {
        spin_unlock(&cache->cache_lock);
    }
}

void* kmem_cache_alloc(struct kmem_cache* cache, uint64_t flag) {
    assert(cache != NULL);
    if(flag & GFP_ATOMIC)
        preempt_disable();
    barrier();
    spin_lock(&cache->cache_lock);
cache_partial:
    if(!list_empty(&cache->partial)) {
        struct page* p = container_of(cache->partial.next, struct page, sibling);
        void* addr = (void*)p->block_start;
        p->alloced_blocks++;
        p->block_start = (link_next_ptr_t*)(*(p->block_start));
        //page不加锁, 因为page或者归buddy或者归slab管, 并且这两个还不可能同时管
        //所以只要保证buddy和slab不用同时访问的, page就不用锁.
        if(p->block_start == 0) { //full
            list_del_init(cache->partial.next);
        }
        spin_unlock(&cache->cache_lock);
        if(flag & GFP_ATOMIC)
            preempt_enable();
        return addr;
    }
    spin_unlock(&cache->cache_lock);
    int alloc_count = ((cache->block_sz * MM_SLAB_EXPANSION_VOODOO) + PAGE_SZ - 1) & PAGE_MASK;
    alloc_count>>=PAGE_OFFSET;

    struct page* new_page = alloc_page(alloc_count);
    if(IS_ERR(new_page)) {
        if(flag & GFP_ATOMIC)
            preempt_enable();
        barrier();
        return ERR_PTR(new_page);
    }
    spin_lock(&cache->cache_lock);
    init_page_mem(cache, new_page, cache->block_sz);
    atomic_inc(&cache->cache_using_blks);
    //不存在页分配失败的情况, 因为分配失败的时候全部 kernel panic 了 :(        
    goto cache_partial; //重新分配

    crash("kmem_cache_alloc() went to a wrong place!");
}

void kmem_cache_init(struct kmem_cache* cache, uint32_t sz) {
    cache->block_sz = sz;
    INIT_LIST_HEAD(&cache->partial);
    spin_init(&cache->cache_lock);
    atomic_set(&cache->cache_using_blks, 0);
}

struct kmem_cache* kmem_cache_get(uint32_t sz) {
    struct kmem_cache* mem = (struct kmem_cache*)kmem_cache_alloc(&main_cache, GFP_KERNEL);
    if(IS_ERR(mem))
        return ERR_PTR(mem);
    kmem_cache_init(mem,sz);
    return mem;
}

void kmem_cache_destroy(struct kmem_cache* *cache) {
    assert(cache!=NULL);
    struct kmem_cache* target = *cache;
    assert(target!=NULL);
    if(target->cache_using_blks.count != 0) {
        warn("SLUB ALLOCATOR: attempt to release a using cache!");
        put_str("   On Object ");
        put_hex((uintptr_t)target);
        put_char('\n');
        return;
    }
    kfree(*cache);
    *cache = NULL;
    smp_wmb();
}

void init_mm_slab() {
    kmem_cache_init(&main_cache,sizeof(struct kmem_cache));
    main_cache.block_sz = sizeof(struct kmem_cache);   
    INIT_LIST_HEAD(&main_cache.partial);
    spin_init(&main_cache.cache_lock);
}