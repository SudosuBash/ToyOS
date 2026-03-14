#include <kloader/pgtable.h>
#include <kloader/elf_bl.h>
#include <kernel/config.h>
#include <kloader/kloader.h>
#include <packed_e820.h>

#define DISC_SECTOR 34
#define RD_COUNT 3000

#define SELF_PADDR 0x40000
#define SELF_SECTOR 3 

/**
 * 内核加载器(换句话说, 跳板程序)
 * 负责加载内核, 配置初始页表, 进入 64 位, 进入分页模式
*/
//1. 准备分页
//2. 加载内核的 ELF 并读取
//3. 将启动信息存储到一个地方
//4. 准备 E820
//5. 进入 64 位
//6. 进入分页模式

extern int rd_disc(uint8_t rd_count, int sector, void* data);
extern void enter_64bit_mode();
extern void open_pg_mode(uint32_t gdtr, uint64_t kernel_vaddr);

void memset(void* addr,uint8_t val, uintptr_t size);
void set_bl_info(struct boot_info* bl);
void prepare_gdt();
void rd_disks(uint32_t rd_count, int sector, uintptr_t data);

void init_e820(); //f**k u
static struct gdtr gdtr;
 
int _start() {
    struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_PADDR;
    prepare_pde((void*)KERNEL_TEMP_PG_ADDR);
    
    link_new_pte_bigpage_addr(0,KERNEL_MEM_SA_VADDR);//临时链接前2MB

    link_new_pte_addr(KERNEL_TEMP_PG_ADDR,KERNEL_TEMP_PG_VADDR);
    link_new_pte_addr(SELF_PADDR, SELF_PADDR);//Triple Fault警告
    link_new_pte_addr(SELF_PADDR+0x1000, SELF_PADDR+0x1000);//Triple Fault警告
    link_new_pte_addr(KERNEL_GDT_ADDR,KERNEL_GDT_TEMP_VADDR);

    rd_disks(RD_COUNT,DISC_SECTOR, KERNEL_LDR_ADDR);
    uint64_t kernel_entrance = load_elf((void*)KERNEL_LDR_ADDR,(void*)KERNEL_FINAL_LDR_ADDR,bl);

    init_e820();
    memset((void*)KERNEL_LDR_ADDR,0, sizeof(RD_COUNT) << 9); //脏数据清空
    

    enter_64bit_mode();
    prepare_gdt();
    set_bl_info(bl);
    //设置引导信息
    open_pg_mode((uint32_t)&gdtr,kernel_entrance);
    return 0;
}


void init_e820() {
    uint32_t entries = *(uint32_t*)KERNEL_MEM_INFO_TEMP_ADDR;
    struct e820_entry* edr = (struct e820_entry*)(KERNEL_MEM_INFO_TEMP_ADDR + 4);

    for(int i=0;i<entries;i++) {
        int swapped = 0;
        for(int j=1;j<entries-i;j++) {
            if(edr[j].base_addr < edr[j-1].base_addr) {
                swapped = 1;
                struct e820_entry tmp = edr[j];
                edr[j] = edr[j-1];
                edr[j-1] = tmp;
            }
        }
        if(!swapped) break;
    }
    
    struct e820_entry* new_edr = (struct e820_entry*)(KERNEL_MEM_INFO_ADDR+4);
    int leng = 0;
    new_edr[0] = edr[0];

    for(int i=1;i<entries;i++) {
        while(i<entries && edr[i].base_addr == new_edr[leng].base_addr + new_edr[leng].leng) {
            if(edr[i].type == 2) 
                new_edr[leng].type = 2;
            new_edr[leng].leng += edr[i].leng;
            i++;
        }
        if(i < entries && edr[i].base_addr < new_edr[leng].base_addr + new_edr[leng].leng) {
            struct e820_entry new_e;
            uint64_t right_border_2 = edr[i].base_addr + edr[i].leng;
            uint64_t right_border = new_edr[leng].base_addr + new_edr[leng].leng;

            int new_edr_type = new_edr[leng].type;
            if(edr[i].base_addr != new_edr[leng].base_addr) {
                new_edr[leng].leng = edr[i].base_addr - new_edr[leng].base_addr;
                leng++;
            }
            if(right_border_2 > right_border) {
                new_edr[leng].base_addr = edr[i].base_addr;
                new_edr[leng].leng = right_border - edr[i].base_addr;
                new_edr[leng].extended_attr = edr[i].extended_attr;
                if(edr[i].type == 1 && new_edr_type == 1) {
                    new_edr[leng].type = 1;
                } else new_edr[leng].type = 2;
                
                leng++;
                new_edr[leng].base_addr = right_border;
                new_edr[leng].extended_attr = edr[i].extended_attr;
                new_edr[leng].type = edr[i].type;
                new_edr[leng].leng = right_border_2 - right_border;
            } else if(right_border == right_border_2) {
                new_edr[leng] = edr[i];
            } else {
                new_edr[leng] = edr[i];
                leng++;
                new_edr[leng] = new_edr[leng-1];
                new_edr[leng].leng = right_border - right_border_2;
                new_edr[leng].base_addr = right_border_2;
            }
        } else {
            new_edr[leng] = edr[i];
        }
        leng++;
    }

    *(uint32_t*)(KERNEL_MEM_INFO_ADDR) = leng;
}
void rd_disks(uint32_t rd_count, int sector, uintptr_t data) {
    while(rd_count >= 256) {
        rd_disc(0 ,sector, (void*)data);
        rd_count -= 256;
        sector+=256;
        data+=256 << 9;
    }
    if(rd_count!=0) {
        rd_disc(rd_count,sector, (void*)data);
    }
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
    }; //Kernel Code
    
    *gaddr = gdt;
    gaddr+=1; //8 Kernel Data
    gdt.access_byte = 0b10010110;
    *gaddr = gdt;
    gaddr+=1; //16

    gdtr.limit = (uint16_t)((uint32_t)gaddr - KERNEL_GDT_ADDR)-1;
    gdtr.base = KERNEL_GDT_TEMP_VADDR;
}
