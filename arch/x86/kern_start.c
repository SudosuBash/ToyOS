
#include <config_arch.h>

#include <early_boot.h>
#include <kernel/put.h>
#include <kernel/kstart.h>

int kernel_x86_start() {
    clear_screen();
    put_str("Booting ToyOS (64-Bit)......\n");
    init_pgtable(); //应该在这初始化
    kernel_start(); //正式的内核启动
}