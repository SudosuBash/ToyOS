#ifndef _TOYOS_MM_MMAP
#define _TOYOS_MM_MMAP

#include <kernel/stdint.h>
#include <kernel/ptable/ptable.h>
#include <kernel/mm/mm_user.h>

void* do_mmap(
    void* vaddr,
    uint64_t sz,
    uint16_t flag,
    uint32_t fd,
    uint16_t prot);

void* do_remap(
    void* paddr,
    void* vaddr,
    uint64_t sz,
    uint16_t prot
);

void user_cow_remap(
    void* paddr, 
    pte_t* pte);

pgd_t* alloc_pgd();
#endif