#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/mm/mm_user.h>
#include <kernel/kernel.h>
#include <kernel/cpu/smp.h>

DEFINE_PERCPU_VAR(vma_pool, struct kmem_cache);

inline struct user_vm_area* copy_area(struct user_vm_area* area) {
    struct user_vm_area* area1 = new_area(area->mem_start, area->mem_end, area->flag, area->perm);
    return area1;
}

inline struct user_vm_area* new_area(uintptr_t start, uintptr_t end, uint16_t flag, uint16_t prot) {
    struct kmem_cache* pool = THIS_CPU_PTR(vma_pool);
    struct user_vm_area* area = (struct user_vm_area*)kmem_cache_alloc(pool, GFP_KERNEL);
    area->flag = flag;
    area->perm = prot;
    area->mem_start = start;
    area->mem_end = end;
    INIT_LIST_HEAD(&area->head);
    rb_init_node(&area->sibling);
    return area;
}

void insert_into_vma(struct user_vm_area *target_area, struct mm_user *user) {
    struct rb_node *curr, *parent;
    struct user_vm_area *area;

    INIT_LIST_HEAD(&target_area->head);
    list_insert_rcu(&target_area->head, &user->vm_area_link);
    rb_init_node(&target_area->sibling);
    if(user->vm_area_root.rb_node == NULL) {
        rb_link_node(&target_area->sibling, NULL, &user->vm_area_root.rb_node);
        rb_insert_color(user->vm_area_root.rb_node, &user->vm_area_root);
        return;
    }

    curr = user->vm_area_root.rb_node;
    while(curr) {
        parent = curr;
        area = container_of(curr, struct user_vm_area, sibling);
        if(area->mem_end > target_area->mem_end) 
            curr = curr->rb_left;
        else 
            curr = curr->rb_right;
    }
    if(area->mem_end > target_area->mem_end) 
        rb_link_node(&target_area->sibling, parent, &parent->rb_left);
    else 
        rb_link_node(&target_area->sibling, parent, &parent->rb_right);
    rb_insert_color(&target_area->sibling, &user->vm_area_root);
}

void remove_from_vma(struct user_vm_area *victim, struct mm_user *user) {
    struct rb_node *curr = user->vm_area_root.rb_node;
    list_del(&victim->head);
    if(!RB_EMPTY_ROOT(&user->vm_area_root))
        rb_erase(curr, &user->vm_area_root);
}

void destroy_vma(struct user_vm_area** victim) {
    kmem_cache_free(*victim);
    *victim = NULL;
}

struct user_vm_area* find_vm_area(struct mm_user* area, uintptr_t addr) {
    struct rb_node* curr;
    struct user_vm_area *varea;
    curr = area->vm_area_root.rb_node;
    while(curr) {
        varea = container_of(curr, struct user_vm_area, sibling);
        if(varea->mem_start <= addr && varea->mem_end >= addr) {
            return varea;
        } else if(varea->mem_start > addr) 
            curr = curr->rb_left;
        else
            curr = curr->rb_right;
    }
    return NULL;
}

void init_vma_area() {
    struct kmem_cache* pool = THIS_CPU_PTR(vma_pool);
    kmem_cache_init(pool, sizeof(struct user_vm_area));
}