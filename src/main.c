#include <kernel/mm/mm.h>

#include <kernel/mm/mm_slab.h>
#include <cpu/cpu.h>
#include <irq/irq.h>
#include <kernel/kstart.h>
#include <kernel/fault/fault.h>
#include <kernel/put.h>

void kernel_start() {
    init_cpu();
    init_irq();
    init_mm();
    
    void* ptrs[64]; // 模拟一个小的对象池
    // }
    while(1) {
        void* v1 = kmalloc(4194304); //1024
        void* v2 = kmalloc(1048576); //256
        void* v3 = kmalloc(65536); //16 pages
        void* v4 = kmalloc(8192); //2 pages
        void* v5 = kmalloc(8193); //4 pages
    }
    // void* p = kmalloc(4194304);
    // kfree(p);
    // p=0;
    assert(1==2);
    while(1);
}