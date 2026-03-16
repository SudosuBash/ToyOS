#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/fault/fault.h>
#include <kernel/cpu/smp.h>

DEFINE_PERCPU_VAR(cache_per_cpu[10], struct kmem_cache);
// struct kmem_cache* caches[10]; //2^3~2^13

void* kmalloc(size_t sz) {
    struct kmem_cache *caches = THIS_CPU_VAR(cache_per_cpu);
    assert(sz!=0);
    int require_sz = highest_up_1(sz+1);
    //复用这个函数，就不新写了，直接+1代替
    if(require_sz < 3) require_sz = 3; //最小8
    if(require_sz < 13) { //4096 kb
        require_sz-=3;
        void* mem = kmem_cache_alloc(&caches[require_sz]);
        return mem;
    } else { //
        struct page* pg = alloc_page(MM_PAGE_PINDEX(PAGE_ROUND_UP(sz)), 0);
        if(pg == NULL) return 0;
        void* addr = get_page_vaddr(pg);
        return addr;
    }
}

void kfree(void* addr) {
    
    assert(addr!=NULL);
    uintptr_t paddr = (uint64_t)addr;
    struct page* p = find_page_by_vaddr(paddr);

    if(p->cache==NULL) {
        assert(p->in_use);
        free_page(p,0);
    } else {
        kmem_cache_free(addr);
    }
}

void kmalloc_init() {
    struct kmem_cache *caches = THIS_CPU_VAR(cache_per_cpu);
    //不能定义为全局变量了，因为全局变量是一堆核心共享的，这样会直接炸!
    for(int i=0;i<10;i++) {
        int c = (1 << (i+3));
        kmem_cache_init(&caches[i],c);
    }
}