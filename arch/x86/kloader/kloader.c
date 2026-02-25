#include <pgtable.h>
#include <kloader/elf_bl.h>
#include <config_arch.h>
#include <kloader/kloader.h>

#define DISC_SECTOR 15
#define RD_COUNT 60

#define SELF_PADDR 0x40000
#define SELF_SECTOR 3

/**
 * 内核加载器(换句话说, 跳板程序)
 * 负责加载内核, 配置初始页表, 进入 64 位, 进入分页模式
*/
//1. 准备分页
//2. 加载内核的 ELF 并读取
//3. 将启动信息存储到一个地方
//4. 进入 64 位
//5. 进入分页模式
extern int rd_disc(int rd_count, int sector, void* data);
extern void enter_64bit_mode();
extern void open_pg_mode(uint32_t gdtr, uint64_t kernel_vaddr);

void memset(void* addr,uint8_t val, uintptr_t size);
void set_bl_info(struct boot_info* bl);
void prepare_gdt();
static struct gdtr gdtr;
 
int _start() {
    struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_PADDR;
    prepare_pde((void*)KERNEL_TEMP_PG_ADDR);
    
    link_new_pte_bigpage_addr(0,KERNEL_MEM_SA_VADDR);//临时链接前2MB

    link_new_pte_addr(KERNEL_TEMP_PG_ADDR,KERNEL_TEMP_PG_VADDR);
    link_new_pte_addr(KERNEL_BOOT_INFO_PADDR,KERNEL_BOOT_INFO_VADDR); //内存信息
    link_new_pte_addr(SELF_PADDR, SELF_PADDR);//Triple Fault警告
    link_new_pte_addr(KERNEL_GDT_ADDR,KERNEL_GDT_TEMP_VADDR);

    int r = rd_disc(RD_COUNT,DISC_SECTOR,(void*)KERNEL_LDR_ADDR);
    uint64_t kernel_entrance = load_elf((void*)KERNEL_LDR_ADDR,(void*)KERNEL_FINAL_LDR_ADDR,bl);

    memset((void*)KERNEL_LDR_ADDR,0, sizeof(RD_COUNT) << 9); //脏数据清空
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

void memset(void* addr,uint8_t val, uintptr_t size) {
    uint8_t* uaddr = (uint8_t*)addr;
    for(int i=0;i<size;i++) {
        uaddr[i] = val;
    }
}

void set_bl_info(struct boot_info* bl) {
    bl->mem_bitmap_addr = KERNEL_MEM_BITMAP_ADDR;
    bl->phys_mem_info_addr = KERNEL_MEM_INFO_ADDR; //内核位于的内存信息
    bl->kloader_pg_base_addr = KERNEL_TEMP_PG_ADDR;
    bl->kern_ldr_addr = KERNEL_FINAL_LDR_ADDR;
    bl->kern_ldr_vaddr = KERNEL_FINAL_LDR_VADDR;
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
