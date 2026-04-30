#include <early_boot.h>
#include <kernel/mm/mm.h>
#include <mm/mm_info.h>
#include <asm.h>

static struct mm_area_record record;
static struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_VADDR;
static volatile uint64_t mem_all_pages; //不包括最上面的那一页,也就是mem_all_pages页是不包括进去的
static volatile uint64_t avl_mem;

inline uint64_t get_machine_available_mem_sz() {
    return avl_mem;
}

inline struct mm_area_record* get_mem_records() {
    return &record;
}

inline uint64_t get_mem_info_paddr() {
    return bl->phys_mem_info_addr;
}

inline uint64_t get_kern_vaddr() {
    return bl->kern_ldr_vaddr;
}

inline uint64_t get_kern_sz() {
    return bl->kern_sz;
}

inline uint64_t get_kern_addr() {
    return bl->kern_ldr_addr;
}

inline uint64_t get_mem_all_pages() {
    return mem_all_pages; 
}
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
            avl_mem+=area.to - area.from;
            mem_all_pages = (edr[i].base_addr +edr[i].leng) >> PAGE_OFFSET;
        }
    }

    
    link_new_pte_bigpage_addr(0, PHYS2VADDR(0));
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
    load_cr3((uint64_t)&__pgtable_bottom - kern_ldr_vaddr + kern_ldr_paddr);

}