#ifndef _TOYOS_PAGE_TABLE
#define _TOYOS_PAGE_TABLE

#include <kernel/stdint.h>
#include <kernel/ptable/ptable.h>

struct pagetable_64;
typedef struct pagetable_64 pgd_t;
typedef struct pagetable_64 pdpt_t;
typedef struct pagetable_64 pte_t;
typedef struct pagetable_64 pde_t;

struct pagetable_64* get_user_pte(uint64_t addr, pgd_t* pgd);
uintptr_t get_pte_paddr(uintptr_t vaddr, pte_t* pte);
void unmask_pte_w(uintptr_t vaddr, pgd_t* pgd);
#endif