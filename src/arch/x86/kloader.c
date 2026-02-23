#include <pgtable.h>
#include <kloader.h>
#define KERNEL_LDR_ADDR 0x300000
#define KERNEL_FINAL_LDR_ADDR 0x100000

#define DISC_SECTOR 10
#define RD_COUNT 20
/**
 * 内核加载器
*/
//1. 准备分页
//2. 加载内核的 ELF
extern int rd_disc(int rd_count, int sector, void* data);

int _start() {
    prepare_pde();
    int r = rd_disc(RD_COUNT,DISC_SECTOR,(void*)KERNEL_LDR_ADDR);
    Elf32_Addr addr = load_elf((void*)KERNEL_LDR_ADDR,(void*)KERNEL_FINAL_LDR_ADDR);
    open_pg_mode(addr);
    return 0;
}