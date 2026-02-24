#include <put.h>
#include <kloader/kloader.h>
#include <config_arch.h>
#include <pgtable_kern.h>
#include <packed_e820.h>
const struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_VADDR;
int kernel_x86_start() {
    
    clear_screen();
    put_str("[0.000000] Booting ToyOS (64-Bit)......\n");
    put_str("[0.000000] Hello, world!\n");
    put_str("[0.000000] Hello, world!\n");
    while(1);
}