#include <early_boot.h>
#include <kernel/mm/mm.h>
#include <asm.h>
static struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_VADDR;
static uint32_t max_mem_index;

inline uint32_t get_avl_mem_index() {
    return max_mem_index;
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

void init_pgtable() { //暴力映射
    uint64_t kern_ldr_vaddr = bl->kern_ldr_vaddr;
    uint64_t kern_ldr_paddr = bl->kern_ldr_addr;
    uint64_t phys_mem_info_addr = bl->phys_mem_info_addr;
    uint64_t kern_sz = bl->kern_sz;
    extern uintptr_t __pgtable_bottom;

    uint32_t entries = *(uint32_t*)PHYS2VADDR(phys_mem_info_addr);
    struct e820_entry* edr = (struct e820_entry*)PHYS2VADDR(phys_mem_info_addr + 4);

    int index = 0, cnt;
    uintptr_t pg_start, pstart, pend;

    prepare_pde(&__pgtable_bottom); 

    for(int i=0;i<kern_sz;i+=PAGE_SZ) { //自映射
        link_new_pte_addr(kern_ldr_paddr + i, kern_ldr_vaddr + i);
    }

    for(int i=0;i<entries;i++) {
        if(edr[i].type == 1 && edr[i].leng >= edr[index].leng) {
            index = i;
        } 
    }

    max_mem_index = index;

    cnt = (edr[index].base_addr + edr[index].leng + PG_BIG_PAGE_SZ - 1) / PG_BIG_PAGE_SZ; //2MB页
    pstart = edr[index].base_addr;
    pend = edr[index].base_addr + (cnt << PG_OFFSET);
    
    for(uintptr_t p = 0; p < pend; p += PG_BIG_PAGE_SZ) {
        link_new_pte_bigpage_addr(p, PHYS2VADDR(p));
    }
    load_cr3((uint64_t)&__pgtable_bottom - kern_ldr_vaddr + kern_ldr_paddr);

}