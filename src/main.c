#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/cpu/cpu.h>
#include <kernel/irq/irq.h>
#include <kernel/kstart.h>
#include <kernel/fault/fault.h>
#include <kernel/put.h>
#include <kernel/irq/timer.h>
#include <kernel/cpu/archimpl.h>

void irq() {
    put_str("Hello,World!\n");
}
void kernel_start() {
    init_cpu();
    init_irq();
    init_mm();
    timer_irq_register(irq);
    enable_irq();
    while(1);
}