#ifndef _TOYOS_DRV_REMINDER
#define _TOYOS_DRV_REMINDER

#include <kernel/base/linklist.h>
#include <kernel/atomic/spinlock.h>
#include <kernel/task/task.h>
#include <kernel/drivers/drv_bus.h>
struct device_wakeup_reminder {
    drv_type type;
    struct linklist_head sibling;
};

struct wakeup_queue {
    struct linklist_head head;
    struct linklist_head* tail;
    
    spinlock_t head_lock;
    spinlock_t tail_lock;
};

void remind_device_type(drv_type type);
#endif  