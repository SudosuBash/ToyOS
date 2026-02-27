
#include <config_arch.h>

#include <kernel/put.h>
#include <kernel/kstart.h>
struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_VADDR;

int kernel_x86_start() {
    clear_screen();
    put_str("Booting ToyOS (64-Bit)......\n");
    kernel_start(); //正式的内核启动
}