#include <kernel/drivers/drv.h>
#include <kernel/atomic/atomic.h>
#include <kernel/sched/sched_drv.h>
#include <kernel/drivers/drv_bus.h>
#include <kernel/drivers/drv_reminder.h>
#include <kernel/mm/mm.h>
#include <kernel/fault/fault.h>
#include <kernel/timer/timer.h>

extern struct device_bus device_bus;

int kdriver_thread_start(void* pdev) {
    struct device* dev = (struct device*) pdev;
    while(1) {
        dev->d_op.start(dev);
        current_task_switch_stat(TASK_SIGNAL_SLEEP_STAT);
    }
    return 0;
}

void remind_device_type(drv_type type) {
    struct device* dev = NULL;
    struct linklist_head *current;
    barrier();
    if(device_bus.typed_bus[type].head.next== NULL)  //链表为空
        return;
    list_for_entry(&device_bus.typed_bus[type].head, current) {
        dev = container_of(current, struct device, dev_type_node);
        task_switch_stat(dev->task, TASK_RUNNING_STAT);
        activate_driver_scheduler(dev->task->scheduler); //重新激活专用的 dev 调度器
    }
}

void set_device_type(struct device* dev, drv_type type) {
    if(type >= DRV_BUS_TYPE_COUNT)
        return;
    struct device_bus_elem* elem = &device_bus.typed_bus[type];
    list_insert_rcu(&dev->dev_type_node ,list_tail(&elem->head));
    dev->type_devid = type;
    dev->sn_id = elem->count;
    elem->count += 1;
}

void init_drv() {
    init_devicebus();
}