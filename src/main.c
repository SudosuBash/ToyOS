#include <kernel/mm/mm.h>

#include <kernel/mm/mm_slab.h>
#include <cpu/cpu.h>
#include <irq/irq.h>
#include <kernel/kstart.h>

void kernel_start() {
    init_cpu();
    init_irq();
    init_mm();

    struct kmem_cache* cache = kmem_cache_get(16);
    struct kmem_cache* cache2 = kmem_cache_get(16);
    kmem_cache_free(cache);
    kmem_cache_free(cache2);
    while(1);
}