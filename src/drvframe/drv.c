#include <kernel/drivers/drv.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/fault/error.h>
#include <kernel/stdlib.h>
#include <kernel/base/htable.h>
#include <kernel/put.h>
#include <kernel/mm/mm.h>
#include <kernel/drivers/drv_bus.h>
#include <kernel/task/task.h>
#include <kernel/sched/sched_drv.h>

extern struct drv_class *__drv_init_start, *__drv_init_end;

DEFINE_PERCPU_VAR(percpu_device_allocator, struct kmem_cache);
DEFINE_PERCPU_VAR(percpu_driver_allocator, struct kmem_cache);

struct device_bus device_bus;

static struct device* new_device() {
    struct kmem_cache* device_allocator = THIS_CPU_PTR(percpu_device_allocator);
    struct device* dev = kmem_cache_alloc(device_allocator, GFP_KERNEL);
    if(IS_ERR(dev))
        return dev;

    memset(dev, 0, sizeof(struct device));
    INIT_LIST_HEAD(&dev->dev_type_node);
    return dev;
}

static struct driver* new_driver() {
    struct kmem_cache* driver_allocator = THIS_CPU_PTR(percpu_driver_allocator);
    struct driver* dv = kmem_cache_alloc(driver_allocator, GFP_KERNEL);
    if(IS_ERR(dv))
        return dv;

    memset(dv, 0, sizeof(struct driver));
    INIT_LIST_HEAD(&dv->sibling);
    return dv;
}

void drv_device_match(struct drv_class* drv, pdrv_match_table table) {
    uint32_t index = 0, device_id, vendor_id, id;
    struct driver *drv_d;
    do {
        device_id = (*table)[index][1];
        vendor_id = (*table)[index][0];
        if(device_id == 0) 
            break;

        drv_d = new_driver();
        drv_d->drv_match_table = table;
        drv_d->class = *drv;

        id = DEVICE_DRV_ID(vendor_id, device_id);
        hlist_insert_rcu(&device_bus.phys_bus, &drv_d->sibling, id);
        index++;
    } while(device_id != 0);
}

void device_try_probe(uint16_t vendor_id, uint16_t device_id) {
    uint32_t id = hlist_calc_hash(DEVICE_DRV_ID(vendor_id, device_id));
    struct linklist_head* current;
    list_for_entry(&device_bus.phys_bus.bucket[id], current) {
        struct driver* drv = driver_of(current);
        uint64_t m_device_id, m_vendor_id, index = 0;
        
        do {
            m_device_id = (*drv->drv_match_table)[index][1];
            m_vendor_id = (*drv->drv_match_table)[index][0];

            if(m_device_id == 0) 
                break;

            if(m_device_id == device_id && m_vendor_id == vendor_id) {
                struct device *new_dev = new_device();
                new_dev->d_op = drv->class;
                new_dev->device_id = device_id;
                new_dev->vendor_id = vendor_id;
                struct task_struct* task = kernel_thread(kdriver_thread_start, (void*)new_dev, "kworkerd");
                switch_to_drv_sched(task);
                task_switch_stat(task, TASK_SIGNAL_SLEEP_STAT);
                new_dev->task = task;
                barrier(); //必须加 barrier, 若先执行 probe 再赋值 task, remind 函数中会产生竞争条件
                new_dev->d_op.probe(new_dev);
            }
            index++;
        } while(m_device_id != 0);
    }
}

void init_devicebus() {
    struct kmem_cache* device_allocator = THIS_CPU_PTR(percpu_device_allocator);
    struct kmem_cache* driver_allocator = THIS_CPU_PTR(percpu_driver_allocator);
    kmem_cache_init(device_allocator, sizeof(struct device));
    kmem_cache_init(driver_allocator, sizeof(struct driver));

    hlist_init(&device_bus.phys_bus);

    for(int i=0;i<DRV_BUS_TYPE_COUNT;i++) {
        INIT_LIST_HEAD(&device_bus.typed_bus[i].head);
        device_bus.typed_bus[i].tail = &device_bus.typed_bus[i].head;
        device_bus.typed_bus[i].count = 0;
    }

    struct drv_class* *d_st = &__drv_init_start;
    struct drv_class* *d_ed = &__drv_init_end;
    
    for(; d_st < d_ed; d_st++) {
        struct device* dev = new_device();
        struct drv_class* op = *d_st;
        if(IS_ERR(dev))
            put_str("WARN: Driver load fail!\n");
        dev->d_op = *op;
        op->init(dev);
    }
}