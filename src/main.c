#include <kernel/mm/mm.h>

#include <kernel/mm/mm_slab.h>
#include <cpu/cpu.h>
#include <irq/irq.h>
#include <kernel/kstart.h>
#include <kernel/fault/fault.h>

void kernel_start() {
    init_cpu();
    init_irq();
    init_mm();

    //测试 SLUB 分配器
    // struct kmem_cache* cache = kmem_cache_get(4096);
    // struct kmem_cache* cache2 = kmem_cache_get(4096);
    // void* addr = kmem_cache_alloc(cache);
    // kmem_cache_free(addr);
    // kmem_cache_free(cache);
    // kmem_cache_free(cache2);
    // struct page* c = alloc_page(3);
    // struct page* c2 = alloc_page(3);
    // struct page* c3 = alloc_page(3);
    // free_page(c3);
    
    // free_page(c);
    // free_page(c2);

    void* p = kmalloc(8);
    kfree(p);
    p=0;
    assert(1==2);
    panic("Attempt to kill init!");
    while(1);
}