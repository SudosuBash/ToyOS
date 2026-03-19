#ifndef _TOYOS_TASK_MM_USER
#define _TOYOS_TASK_MM_USER

#include <kernel/ptable/ptable.h>
#include <kernel/data_struct/linklist.h>

#define PERM_X 1
#define PERM_W 2
#define FLAG_KERN_ONLY 1
struct user_vm_area {
    struct linklist_head sibling;
    uintptr_t mem_start;
    uintptr_t mem_end;
    uint8_t perm; //设置内存权限
    uint8_t flag; //申请内存的flag
};

struct mm_user {
    pgd_t* pg_root; //根页表
    uintptr_t brk;  
    struct linklist_head vm_node;
};


#endif