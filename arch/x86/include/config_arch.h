#ifndef _TOYOS_CONFIG_ARCH_X86
#define _TOYOS_CONFIG_ARCH_X86

#define KERNEL_TEMP_PG_ADDR (0x10000) //临时页表放在 0x10000, 引导系统可以建立15个临时页表
//内核就不止15个了, 因为引导系统被覆盖了
#define KERNEL_TEMP_PG_VADDR 0xfffffffffffff000//内核分页地址
#define KERNEL_MEM_INFO_ADDR 0x3000 //内存信息
#define KERNEL_LDR_ADDR 0x200000 //内核临时加载地址
#define KERNEL_FINAL_LDR_ADDR 0x100000 //内存最终加载地址
#define KERNEL_MEM_BITMAP_ADDR 0x9000 //内存管理bitmap
#define KERNEL_BOOT_INFO_PADDR 0x2000 //启动信息(一个页面的大小)
#define KERNEL_BOOT_INFO_VADDR 0xffffffff81000000 //启动信息映射
#define KERNEL_FINAL_LDR_VADDR 0xffffffff80000000 //启动信息映射
#define KERNEL_PG_SZ 0x1000
#define KERNEL_GDT_ADDR 0x1000 
#define KERNEL_GDT_TEMP_VADDR 0x00000000FFFFE000 //内核虚拟地址
#define KERNEL_MEM_SA_VADDR 0xFFFFF80000000000

#endif