#include <kernel/irq/irq.h>
#include <kernel/base/linklist.h>
#include <kernel/kernel.h>
#include <kernel/cpu/smp.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/mm/mm.h>

DEFINE_PERCPU_VAR(irq_allocator, struct kmem_cache);
static struct irq_entrance irq_entries[IRQ_MAX_CNT];

void irq_entrance_fn(struct arch_regs* args, uint64_t irq_number) {
    
    if(irq_number >= IRQ_MAX_CNT) {
        return;
    }
    struct linklist_head* current;
    list_for_entry(&irq_entries[irq_number].head, current) {
        struct irq_behavior* behavior = container_of(current, struct irq_behavior, sibling);
        behavior->handler(args, behavior->device_data);
    }
}

int irq_register(
    uint64_t num,
    irq_handler_t fn,
    void* device_data
) {
    struct kmem_cache* cache = THIS_CPU_PTR(irq_allocator);
    struct irq_behavior* behavior = kmem_cache_alloc(cache, GFP_KERNEL);
    behavior->handler = fn;
    behavior->device_data = device_data;
    INIT_LIST_HEAD(&behavior->sibling);
    list_insert_rcu(&behavior->sibling, list_tail(&irq_entries->head));
    return num;
}

void irq_remove(uint64_t num, void* dev_data) {
    struct linklist_head* now = NULL;
    struct irq_behavior *behavior = NULL, *target = NULL;
    list_for_entry(&irq_entries[num].head, now) {
        behavior = container_of(now, struct irq_behavior, sibling);
        if(behavior->device_data == dev_data) {
            target = behavior;
            break;
        }
    }

    if(target != NULL) 
        list_del_init(&target->sibling);
}

void init_irq() {
    struct kmem_cache* cache = THIS_CPU_PTR(irq_allocator);
    kmem_cache_init(cache, sizeof(struct irq_behavior));
    disable_irq();
    for(int i=0;i<IRQ_MAX_CNT;i++) {
        INIT_LIST_HEAD(&irq_entries[i].head);
    }
    init_irq_arch();
}