#include <hal/config_arch.h>
#include <hal/early_boot.h>
#include <kernel/put.h>
#include <kernel/kstart.h>
#include <kernel/log/kprintf.h>
#include <kernel/cpu/smp.h>
#include <kernel/cpu/cpu.h>

extern struct ap_resource_pack* ap_respack;

void kernel_x86_start() {
    clear_screen();
    early_init();
    init_pgtable(); //应该在这初始化
    init_early_acpi();
    kernel_start(); //正式的内核启动
}

void kernel_x86_ap_start(uint64_t cpuid) {
    set_smp_percpu_addr(ap_respack[cpuid].percpu_address);
    kern_ap_start();
}