#include <kernel/mm/mm_page.h>
#include <kernel/mm/mm.h>

void init_page_mem(struct kmem_cache* cache,struct page* pg, uint32_t sz) {
    uintptr_t* pg_mem = (uintptr_t*) get_page_vaddr(pg);
    pg->cache = cache;
    pg->alloced_blocks=0;
    pg->block_sz = sz;
    int page_blks = PAGE_SZ / pg->block_sz;
    pg->blocks=page_blks;
    uintptr_t* ptr = pg_mem;
    for(int i=0;i<page_blks-1;i++) {
        uintptr_t* next_blk = (uintptr_t*)((uint8_t*)ptr + pg->block_sz);
        *(uintptr_t*)ptr = (uintptr_t)next_blk;
        ptr = next_blk;
    }
    *(uintptr_t*)ptr = 0;
    pg->block_start = pg_mem;
}