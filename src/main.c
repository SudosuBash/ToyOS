#include <mem.h>
#include <cpu/cpu.h>
#include <irq/irq.h>
#include <kernel/kstart.h>

void kernel_start() {
    init_pgtable();
    init_cpu();
    init_irq();
    uint64_t* vaddr = 0xffffffff7ffffff0;
    *vaddr = 1;//测试中断门
    while(1);
}