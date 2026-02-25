#include <kernel/put.h>
#include <kloader/kloader.h>
#include <config_arch.h>
#include <pgtable.h>
#include <kernel/stdlib.h>
#include <mem.h>
#include <kernel/clock.h>
#include <packed_e820.h>

const struct boot_info* bl = (struct boot_info*)KERNEL_BOOT_INFO_VADDR;

void init_mem() { //暴力映射
    uint64_t kern_ldr_vaddr = bl->kern_ldr_vaddr;
    uint64_t kern_ldr_paddr = bl->kern_ldr_addr;
    uint64_t phys_mem_info_addr = bl->phys_mem_info_addr;
    uint64_t kern_sz = bl->kern_sz;
    extern uintptr_t _pgtable_bottom;
    uint32_t entries = *(uint32_t*)PHYS2VADDR(phys_mem_info_addr);
    struct e820_entry* edr = (struct e820_entry*)PHYS2VADDR(phys_mem_info_addr + 4);
    int index = 0, cnt;
    uintptr_t pg_start, pstart, pend;

    prepare_pde(&_pgtable_bottom); 

    for(int i=0;i<kern_sz;i+=PAGE_SZ) { //自映射
        link_new_pte_addr(kern_ldr_paddr + i, kern_ldr_vaddr + i);
    }

    for(int i=0;i<entries;i++) {
        if(edr[i].type == 1 && edr[i].leng >= edr[index].leng) {
            index = i;
        } 
    }
    put_str("Checked Memory Size = ");
    put_dec(edr[index].leng);
    put_str(" Bytes, Start Addr = ");
    put_hex(edr[index].base_addr);
    put_str(".\n");

    cnt = (edr[index].base_addr + edr[index].leng + PG_BIG_PAGE_SZ - 1) / PG_BIG_PAGE_SZ; //2MB页
    pstart = edr[index].base_addr;
    pend = edr[index].base_addr + (cnt << PG_OFFSET);
    
    for(uintptr_t p = 0; p < pend; p += PG_BIG_PAGE_SZ) {
        link_new_pte_bigpage_addr(p, PHYS2VADDR(p));
    }
    pg_start = (uintptr_t) &_pgtable_bottom;

    asm volatile(
        "movq %0, %%rax\r\n"
        "movq %%rax, %%cr3\r\n"
        :
        : "r"(pg_start - kern_ldr_vaddr + kern_ldr_paddr)
        : "rax", "memory"
    );

    put_str("Virtual Memory Info:\n");
    put_str(" From:\n");
    put_str("  Phys Mem: ");
    put_hex(0);
    put_str(" --> Virtual Mem: ");
    put_hex(PHYS2VADDR(0));
    put_str("\n To:\n");
    put_str("  Phys Mem: ");
    put_hex(pend);
    put_str(" --> Virtual Mem: ");
    put_hex(PHYS2VADDR(pend));
    put_char('\n');
}

void init_clock() {
    put_str("Initializing clock...\n");
    clock_init();
    uint64_t current = clock_current();
    put_str("Clock initialize. current runtime = ");
    put_dec(current);
    put_char('\n');
}

int kernel_x86_start() {
    clear_screen();
    put_str("Booting ToyOS (64-Bit)......\n");
    init_clock();
    init_mem();
    while(1);
}