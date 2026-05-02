#ifndef _TOYOS_BASIC_DRV
#define _TOYOS_BASIC_DRV

#include <kernel/stdint.h>
#include <kernel/base/linklist.h>
#include <kernel/atomic/atomic.h>
#include <kernel/vfs/vfs.h>
#include <kernel/file/dir.h>
#include <kernel/drivers/drv.h>

#define DEVICE_DRV_ID(vendor_id, device_id) ((((uint32_t)(vendor_id)) << 16) | (device_id))
#define PCI_DEVID(vendor_id, device_id) {(vendor_id), (device_id)}

#define driver_of(node) container_of(node, struct driver, sibling)
typedef uint16_t drv_match_table[][2], (*pdrv_match_table)[][2];
struct device;

struct drv_class {
    void (*init)(struct device* device);
    void (*probe)(struct device* device);
    void (*destroy)(struct device* device);
};

struct driver {
    pdrv_match_table drv_match_table;
    struct linklist_head sibling;
    struct drv_class class;
};

struct device {
    struct dir_operation f_op;
    struct drv_class d_op;
    
    atomic_t refcount;
    uint64_t mmio_start_addr;
    uint64_t mmio_start_vaddr;
    uint16_t vendor_id;
    uint16_t device_id;
    
    uint32_t irq;
};

#define MODULE_SET_DEVICE(drv) static struct drv_class* export_drv_sym __attribute__((section(".drv_init"), aligned(sizeof(void*)), used)) = &(drv);

void drv_device_match(struct drv_class* drv, pdrv_match_table table);
void device_try_probe(uint16_t vendor_id, uint16_t device_id);
void init_drv();

#endif