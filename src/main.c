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
#include <kernel/timer/timer.h>
#include <kernel/acpi/acpi.h>

struct system_static_data sysdata;

void init_smp_data_early_arch();

int init() {
    kprintf("Init thread launched, pid = %ld\n", pid_of(CURRENT_PROCESS()));
    while(1);
    return 0;
}

static void init_resource_early() {
    extern struct system_static_data sysdata;
    uint64_t cores = sysdata.smp_info->local_apic_count;
    sysdata.percpu_start_addr = sysdata.kernel_end;
    sysdata.kernel_end += cores * CONFIG_PERCPU_SZ;
    sysdata.ap_sp = sysdata.kernel_end;
    sysdata.kernel_end += (cores - 1) * STACK_SZ;
    sysdata.page_start = (struct page*)sysdata.kernel_end;
    sysdata.kernel_end += (sysdata.mem_all_pages * sizeof(struct page));
    init_smp_data_early_arch();
}

void kern_early_init() {
    init_log();
    log_version_info();
    init_irq();
    init_resource_early();
    init_mm_early();
}

void kernel_start() { 
    kern_early_init();
    init_mm(); //未来这个会根据 NUMA 分配, 这个暂时先全局
    init_timer();
    
    init_smp();

    init_cpu();
    init_irq_cpu(); 
    init_timer_cpu();
    
    init_mm_cpu();
    
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

void kern_ap_start() {
    init_cpu();
    init_irq_cpu();
    init_timer_cpu();
    init_mm_cpu();
    while(1);
}