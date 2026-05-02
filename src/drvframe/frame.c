#include <kernel/drivers/drv_frame.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/fault/error.h>
#include <kernel/stdlib.h>

#include <kernel/put.h>

extern struct drv_class *__drv_init_start, *__drv_init_end;

DEFINE_PERCPU_VAR(percpu_device_allocator, struct kmem_cache);
struct kmem_cache* device_allocator;


static struct device* new_device() {
    struct device* dev = kmem_cache_alloc(device_allocator);
    if(IS_ERR(dev))
        return dev;

    memset(dev, 0, sizeof(struct device));
    INIT_LIST_HEAD(&dev->sibling);

    return dev;
}
void init_drv() {
    device_allocator = THIS_CPU_PTR(percpu_device_allocator);
    kmem_cache_init(device_allocator, sizeof(struct device));

    struct drv_class* *d_st = &__drv_init_start;
    struct drv_class* *d_ed = &__drv_init_end;
    
    for(; d_st < d_ed; d_st++) {
        struct device* dev = new_device();
        if(IS_ERR(dev))
            put_str("WARN: Driver load fail!\n");
        (*d_st)->init(dev);
    }
}
