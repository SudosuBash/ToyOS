#ifndef _TOYOS_DRV_BUS
#define _TOYOS_DRV_BUS

#include <kernel/drivers/drv.h>
#include <kernel/base/htable.h>
#include <kernel/base/linklist.h>
#include <kernel/task/task.h>

#define DRV_BUS_TYPE_COUNT 10

struct device_bus_elem {
    struct linklist_head head;
    struct linklist_head *tail;
    uint64_t count;
};

struct device_bus {
    struct htable_list phys_bus; //物理总线
    struct device_bus_elem typed_bus[DRV_BUS_TYPE_COUNT]; //设备类型总线
    struct task_struct *bus_task;
};



int kdrv_reminder_daemon();
#endif