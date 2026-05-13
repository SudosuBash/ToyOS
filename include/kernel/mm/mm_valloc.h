#ifndef _TOYOS_KERNEL_VALLOC
#define _TOYOS_KERNEL_VALLOC

#include <kernel/base/linklist.h>
#include <kernel/base/rbtree.h>
#include <kernel/stdint.h>
struct kernel_vma {
    uint64_t start;
    uint64_t sz;
    struct rb_node node;
    struct linklist_head sibling;
};


//考虑核间竞争
struct kernel_vma_table {
    struct rb_root vma_root;
    struct linklist_head vma_head;
};
#endif