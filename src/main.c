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
#include <kernel/task/exec.h>
#include <kernel/syscall/syscall.h>
#include <kernel/drivers/drv.h>
#include <kernel/fs/devicefs.h>
#include <kernel/stdlib.h>
#include <kernel/drivers/drv_bus.h>
#include <kernel/log/kprintf.h>

static void output_stat() {
    kprintf("Toy OS Kernel - Demo 12.418.0\n");
    kprintf("Checked Memory: %d Bytes\n", xget_machine_available_mem_sz());
    kprintf("2026 SudosuBash\n");
}

int init() {
    device_try_probe(0x8086, 0x03f8);
    output_stat();
    while(1);
    return 0;
}

void kernel_start() {
    init_irq();
    init_smp();
    init_cpu();
    init_mm();
    init_task();
    init_syscall();
    init_vfs();
    init_drv();

    kernel_thread(init, NULL, "init");
    enable_irq();
    
    while(1)
        hlt();
}