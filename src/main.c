#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/cpu/cpu.h>
#include <kernel/irq/irq.h>
#include <kernel/kstart.h>
#include <kernel/fault/fault.h>
#include <kernel/put.h>
#include <kernel/irq/timer.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/task/task.h>
#include <kernel/task/fork.h>

#include <cpu/regs.h>
#include <cpu/gdt.h>

void rest_init() {
     
}

int thr1(void* test) {
    return 0;
}

void kernel_start() {
    init_irq();
    init_smp();
    init_cpu();
    
    init_mm();
    init_task();
    
    fork();
    while(1) hlt();
}