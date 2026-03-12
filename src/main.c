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
    
    void* p = kmalloc(8);
    kfree(p);
    p=0;
    assert(1==2);
    panic("Attempt to kill init!");
    while(1);
}