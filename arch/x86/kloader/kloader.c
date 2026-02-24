#include <kloader/pgtable.h>
#include <kloader/elf_bl.h>
#include <config_arch.h>
#include <kloader/kloader.h>

#define DISC_SECTOR 15
#define RD_COUNT 30
/**
 * 内核加载器(换句话说, 跳板程序)
 * 负责加载内核, 配置初始页表, 进入 64 位, 进入分页模式
*/
//1. 准备分页
//2. 加载内核的 ELF 并读取
//3. 将启动信息存储到一个地方
//4. 进入 64 位
//5. 进入分页模式
struct boot_info;
extern int rd_disc(int rd_count, int sector, void* data);
extern void enter_64bit_mode();
extern void open_pg_mode(uint32_t gdtr, uint64_t kernel_vaddr);

void set_bl_info(struct boot_info* bl);
void prepare_gdt();
static struct gdtr gdtr;

int _start() {
    struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_PADDR;
    //写死BL
    prepare_pde();
    int r = rd_disc(RD_COUNT,DISC_SECTOR,(void*)KERNEL_LDR_ADDR);
    uint64_t kernel_entrance = load_elf((void*)KERNEL_LDR_ADDR,(void*)KERNEL_FINAL_LDR_ADDR);
    
    set_bl_info(bl);
    //设置引导信息
    enter_64bit_mode();
    prepare_gdt();
    
    open_pg_mode((uint32_t)&gdtr,kernel_entrance);
    return 0;
}
static struct gdtr gdtr = {
    .limit = 0,
    .base = KERNEL_GDT_TEMP_VADDR
};
void set_bl_info(struct boot_info* bl) {
    bl->kloader_pg_paddr_top = get_kern_pte_ptr_top();
    bl->kern_sz = 512 * DISC_SECTOR; //定死
    bl->kern_mem_phys_addr = KERNEL_FINAL_LDR_ADDR;
    bl->mem_bitmap_addr = KERNEL_MEM_BITMAP_ADDR;
    bl->phys_mem_info_addr = KERNEL_MEM_INFO_ADDR; //内核位于的内存信息
    bl->kloader_pg_base_addr = KERNEL_TEMP_PG_ADDR;
}

//rsp 初始值 64-bit
//gdtr 的值 32-bit
//kernel vaddr的值

void prepare_gdt() { //临时页表
    uint64_t* addr = (uint64_t*)KERNEL_GDT_ADDR;
    *addr = 0; //第一个必须为0
    struct gdt* gaddr = (struct gdt*)KERNEL_GDT_ADDR + 1;
    struct gdt gdt = {
        .limit = 0xffff,
        .base = 0,
        .base_2 = 0,
        .access_byte = 0b10011010,
        .limit_2 = 0b1111,
        .flags = 0b0010,
        .base = 0
    };
    *gaddr = gdt;
    gaddr+=1;

    gdtr.limit = (uint16_t)((uint32_t)gaddr - KERNEL_GDT_ADDR)-1;
}
