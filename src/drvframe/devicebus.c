#include <kernel/drivers/drv.h>
#include <kernel/atomic/atomic.h>
#include <kernel/sched/sched_drv.h>
#include <kernel/drivers/drv_bus.h>
#include <kernel/drivers/drv_reminder.h>
#include <kernel/mm/mm.h>
#include <kernel/timer/timer.h>

#define MAX_TIMER 10000000
extern struct device_bus device_bus;
static struct wakeup_queue queue;

int kdrv_minder_daemon() {
    struct task_struct* task = CURRENT_PROCESS();
    device_bus.bus_task = task;
    switch_to_drv_sched(task); //切换到 drv 调度器
    
    while(1) { //轮询
        spin_lock(&queue.tail_lock);
        spin_lock(&queue.head_lock);
        if(&queue.head == queue.tail) { //没消息了
            spin_unlock(&queue.head_lock);
            spin_unlock(&queue.tail_lock);
            current_task_switch_stat(TASK_SIGNAL_SLEEP_STAT);
            continue;
        }

        struct linklist_head *head = list_head(&queue.head), *current;
        struct device* dev;
        struct device_wakeup_reminder* reminder;
        while(head != &queue.head) {
            reminder = container_of(head, struct device_wakeup_reminder, sibling);
            list_for_entry(&device_bus.typed_bus[reminder->type].head, current) {
                dev = container_of(current, struct device, dev_type_node);
                dev->d_op.msg_callback(dev);
                //暂时先这么写
            }
            queue.tail = head;
            list_del_init(head);
            kfree(reminder);
            head = list_head(&queue.head);
        }
        queue.tail = &queue.head;
        spin_unlock(&queue.head_lock);
        spin_unlock(&queue.tail_lock);
    }
}

void remind_device_type(drv_type type) {
    spin_lock(&queue.tail_lock);
    struct device_wakeup_reminder* reminder = kmalloc(sizeof(struct device_wakeup_reminder), GFP_KERNEL);
    INIT_LIST_HEAD(&reminder->sibling);
    list_insert_rcu(&reminder->sibling, queue.tail);
    reminder->type = type;
    queue.tail = &reminder->sibling;
    barrier();
    task_switch_stat(device_bus.bus_task, TASK_RUNNING_STAT);
    activate_driver_scheduler(device_bus.bus_task->scheduler); //重新激活
    spin_unlock(&queue.tail_lock);
}

void set_device_type(struct device* dev, drv_type type) {
    if(type >= DRV_BUS_TYPE_COUNT)
        return;
    struct device_bus_elem* elem = &device_bus.typed_bus[type];
    list_insert_rcu(&dev->dev_type_node ,elem->tail);
    dev->type_devid = type;
    dev->sn_id = elem->count;
    elem->count += 1;
}

void init_drv() {
    init_devicebus();
    INIT_LIST_HEAD(&queue.head);
    queue.tail = &queue.head;
}