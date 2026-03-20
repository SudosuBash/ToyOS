#ifndef _TOYOS_MM_MMAP
#define _TOYOS_MM_MMAP

#include <kernel/stdint.h>
#include <kernel/ptable/ptable.h>
#include <kernel/task/mm_user.h>

void do_pte_fast_mmap(
    void* paddr, 
    uint64_t flag,
    uint64_t prot,
    pte_t* pte);

void* do_mmap(
    void* paddr,
    void* vaddr,
    uint64_t sz,
    uint64_t flag,
    uint64_t prot
);
pgd_t* alloc_pgd();
#endif