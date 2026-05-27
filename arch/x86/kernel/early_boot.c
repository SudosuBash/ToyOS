#include <hal/early_boot.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/smp.h>
#include <hal/asm.h>
#include <hal/cpu/cpu.h>
#include <acpica/acpi.h>
#include <kernel/acpi/acpi.h>
#include <kernel/stdlib.h>
#include <kernel/mm/mmap.h>
#include <hal/config_arch.h>

static struct mm_area_record record;
static struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_VADDR;
static volatile uint64_t avl_mem;

extern struct smp_data_acpi_info __smp_apic_data;
extern struct system_static_data sysdata;

static void init_mem_record() {
    uint64_t phys_mem_info_addr = bl->phys_mem_info_addr;
    uint32_t entries = *(uint32_t*)PHYS2VADDR(phys_mem_info_addr);
    struct e820_entry* edr = (struct e820_entry*)PHYS2VADDR(phys_mem_info_addr + 4);

    for(uint32_t i=0;i<entries;i++) {
        if(edr[i].type == 1 && edr[i].base_addr >= LOW_MEM) {
            struct mm_area area;
            area.from = PAGE_BIG_ROUND_UP(edr[i].base_addr);
            area.to = (edr[i].base_addr + edr[i].leng) & PG_BIG_PAGE_MASK;
            record.area[record.num] = area;
            record.num ++;
            for(uint64_t addr = area.from;addr<area.to;addr+=PG_BIG_PAGE_SZ) {
                link_new_pte_bigpage_addr(addr,PHYS2VADDR(addr));
            }
            sysdata.mem_sz +=area.to - area.from;
            sysdata.mem_all_pages = (edr[i].base_addr +edr[i].leng) >> PAGE_OFFSET;
        } else if(edr[i].type == 3) {
            uint64_t start = edr[i].base_addr & PAGE_MASK;
            uint64_t end = PAGE_ROUND_UP(edr[i].base_addr + edr[i].leng);
            
            if(end - start >= PG_BIG_PAGE_SZ) {
                start = edr[i].base_addr & PG_BIG_PAGE_MASK;
                end = PAGE_BIG_ROUND_UP(edr[i].base_addr + edr[i].leng);
                for(uint64_t addr = start; addr < end; addr += PG_BIG_PAGE_SZ) {
                    uint64_t new_addr = PHYS2VADDR_MMIO(addr);
                    link_new_pte_bigpage_addr(addr ,new_addr);
                    
                    set_pde_pcd_bigpage(new_addr, 1);
                    set_pde_pwt_bigpage(new_addr, 1);
                }
            }
        }
    }
    //低端1mb看作 MMIO
    link_new_pte_bigpage_addr(0, PHYS2VADDR_MMIO(0));
    set_pde_pcd_bigpage(PHYS2VADDR_MMIO(0), 1);
    set_pde_pwt_bigpage(PHYS2VADDR_MMIO(0), 1);
    link_new_pte_bigpage_addr(0, 0); //这个恒等映射是为 AP 做的

    record.area[0].from = LOW_MEM; //手动设置为低端内存
}
void init_pgtable() { //暴力映射
    extern uintptr_t __mem_info_bottom;
    record.area = (struct mm_area*)&__mem_info_bottom;

    uint64_t kern_ldr_vaddr = bl->kern_ldr_vaddr;
    uint64_t kern_ldr_paddr = bl->kern_ldr_addr;
    
    uint64_t kern_sz = bl->kern_sz;
    extern uintptr_t __pgtable_bottom;

    prepare_pde(&__pgtable_bottom); 

    init_mem_record();

    for(int i=0;i<kern_sz;i+=PAGE_SZ) { //自映射
        link_new_pte_addr(kern_ldr_paddr + i, kern_ldr_vaddr + i);
    }
    //按照下面这个算法，低端1MB是映射不进去的
    //所以先映射地址为0的页
    barrier();
    uint64_t cr3 = (uint64_t)&__pgtable_bottom - kern_ldr_vaddr + kern_ldr_paddr;
    load_cr3(cr3);
    bl = (struct boot_info*)PHYS2VADDR_MMIO(KERNEL_BOOT_INFO_PADDR);
}

static void parse_madt(ACPI_TABLE_MADT* madt) {
    uint8_t *pheader = (uint8_t*)((uint64_t)madt + sizeof(ACPI_TABLE_MADT)), *header_end = (uint8_t*)((uint64_t)madt + madt->Header.Length);
    sysdata.smp_info = &__smp_apic_data;
    while(pheader < header_end) {
        ACPI_SUBTABLE_HEADER* header = (ACPI_SUBTABLE_HEADER*) pheader;
        switch(header->Type) {
            case ACPI_MADT_TYPE_LOCAL_APIC: {
                sysdata.smp_info->localapic[sysdata.smp_info->local_apic_count++] = (ACPI_MADT_LOCAL_APIC*)header;
                break;
            }
            case ACPI_MADT_TYPE_IO_APIC: {
                sysdata.smp_info->ioapic[sysdata.smp_info->io_apic_count++] = (ACPI_MADT_IO_APIC*)header;
                break;
            }
            case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE: {
                sysdata.smp_info->override[sysdata.smp_info->irq_override_count++] = (ACPI_MADT_INTERRUPT_OVERRIDE*)header;
                break;
            }
        }
        pheader+=header->Length;
    }
}

void init_early_acpi() {
    ACPI_TABLE_RSDP* rsdp = (ACPI_TABLE_RSDP*)PHYS2VADDR_MMIO(bl->acpi_rsdp_pos);
    sysdata.acpi_data.rsdp = rsdp;
    init_acpi();
    ACPI_TABLE_MADT* madt = get_acpi_madt(rsdp);
    parse_madt(madt);
}

void early_init() {
    sysdata.kernel_load_address = bl->kern_ldr_addr;
    sysdata.kernel_size = bl->kern_sz;
    sysdata.kernel_load_vaddr = bl->kern_ldr_vaddr;
    sysdata.mm_area = &record;
    extern uint64_t __kernel_end;
    sysdata.kernel_end = PHYS2VADDR(KERN_VADDR_TO_PADDR((uintptr_t)&__kernel_end));

    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
    //对 8259a 进行屏蔽
}

void init_smp_data_early_arch() {
    sysdata.kernel_exception_stack = sysdata.kernel_end;
    sysdata.kernel_end += sysdata.smp_info->local_apic_count * BST_STACK_SZ * BST_STACK_COUNT_PERCPU 
                            *sysdata.smp_info->io_apic_count;
}