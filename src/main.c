#include <kernel/mm/mm.h>
#include <kernel/cpu/cpu.h>
#include <kernel/irq/irq.h>
#include <kernel/kstart.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/task/task.h>
#include <kernel/syscall/syscall.h>
#include <kernel/drivers/drv.h>
#include <kernel/fs/devicefs.h>
#include <kernel/log/kprintf.h>
#include <kernel/version.h>

int init() {
    kprintf("Init thread launched, pid = %ld\n", pid_of(CURRENT_PROCESS()));
    while(1);
    return 0;
}

void kernel_start() {
    init_log();
    log_version_info();
    init_irq();
    init_smp();
    init_cpu();
    init_mm();
    
    init_syscall();
    init_task();

    kernel_thread(init, NULL, "init");

    init_vfs();
    init_drv();
    device_try_probe(0x8086, 0x03f8);
    
    enable_irq();
    
    while(1)
        hlt();
}