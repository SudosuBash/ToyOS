#ifndef _TOYOS_PAGE_TABLE
#define _TOYOS_PAGE_TABLE

#include <kernel/stdint.h>
#include <pgtable/pgtable_kern.h>

struct pagetable_64;
typedef struct pagetable_64 pgd_t;
typedef struct pagetable_64 pdpt_t;
typedef struct pagetable_64 pte_t;
typedef struct pagetable_64 pde_t;

pgd_t* get_pgd(uint64_t vaddr);
struct pagetable_64* get_user_pte(uint64_t addr, pgd_t* pgd);
uintptr_t get_pte_paddr(uintptr_t vaddr, pte_t* pte);
void copy_pgd(pgd_t* dest, pgd_t* src);
#define KERN_VADDR_TO_PADDR(addr) ((uintptr_t)(addr) - KERNEL_FINAL_LDR_VADDR + KERNEL_FINAL_LDR_ADDR)
#define KERN_PADDR_TO_VADDR(addr) ((uintptr_t)(addr) + KERNEL_FINAL_LDR_VADDR - KERNEL_FINAL_LDR_ADDR)
#endif