#ifndef _TOYOS_BASIC_DRV
#define _TOYOS_BASIC_DRV

#include <kernel/stdint.h>
#include <kernel/data_struct/linklist.h>
#include <kernel/atomic/atomic.h>
#include <kernel/vfs/vfs.h>
#include <kernel/file/dir.h>

enum device_stat {
    UNINITIALIZED = 0,
    PROBED = 1,
    RUNNING = 2,
    SUSPENDED = 3,
    DESTROYED = 4
};

struct device {
    uint16_t vendor_id;
    uint16_t device_id;
    
    uint32_t irq;

    uint64_t mmio_start_addr;
    uint64_t mmio_start_vaddr;
    enum device_stat stat;

    atomic_t refcount;

    struct linklist_head parent;
    struct linklist_head sibling;
    
    struct dir_operation operation;
};
#endif