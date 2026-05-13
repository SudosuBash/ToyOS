#ifndef _TOYOS_TASK_MM_USER
#define _TOYOS_TASK_MM_USER

#include <kernel/ptable/ptable.h>
#include <kernel/base/linklist.h>
#include <kernel/base/rbtree.h>
#include <kernel/atomic/rwlock.h>

#define PERM_X 1
#define PERM_W 2
#define FLAG_KERN_ONLY 1

//它描述的是每一个进程单独的 vma, 不需要 refcount 去约束它.
struct user_vm_area {
    struct rb_node sibling;
    struct linklist_head head;
    uintptr_t mem_start;
    uintptr_t mem_end;
    uintptr_t kern_mem_start;
    uint32_t fd;
    uint8_t perm; //设置内存权限
    uint8_t flag; //申请内存的flag
}; 

struct mm_user {
    struct rb_root vm_area_root;
    struct linklist_head vm_area_link;
    pgd_t* pg_root; //根页表
    uintptr_t brk;  
    rwlock_t rwlock;
};

void init_vma_area();
struct user_vm_area* new_area(uintptr_t start, uintptr_t end, uint16_t flag, uint32_t fd, uint16_t prot);
void insert_into_vma(struct user_vm_area *target_area, struct mm_user *user);
void remove_from_vma(struct user_vm_area *victim, struct mm_user *user);
void destroy_vma(struct user_vm_area** victim);
struct user_vm_area* copy_area(struct user_vm_area* area);
struct user_vm_area* find_vm_area(struct mm_user* area, uintptr_t addr);
#endif