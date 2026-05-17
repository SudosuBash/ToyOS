#ifndef _TOYOS_BASIC_DRV
#define _TOYOS_BASIC_DRV

#include <kernel/stdint.h>
#include <kernel/base/linklist.h>
#include <kernel/atomic/atomic.h>
#include <kernel/vfs/vfs.h>
#include <kernel/file/dir.h>
#include <kernel/drivers/drv.h>
#include <kernel/base/htable.h>

#define DEVICE_DRV_ID(vendor_id, device_id) ((((uint32_t)(vendor_id)) << 16) | (device_id))
#define PCI_DEVID(vendor_id, device_id) {(vendor_id), (device_id)}
#define DEVICE_BIND_FILE_OP(dev, file) ((dev)->f_op = (file))
#define driver_of(node) container_of(node, struct driver, sibling)

typedef uint16_t drv_match_table[][2], (*pdrv_match_table)[][2];
struct device;
typedef enum {
    DRV_CONSOLETYP = 0,
    DRV_GRAPHICSTYP = 1,
    DRV_DISCTYP = 2,
    DRV_IOTYP = 3
} drv_type;

struct drv_class {
    void (*init)(struct device* device);
    void (*probe)(struct device* device);
    void (*destroy)(struct device* device);

    void (*msg_callback)(struct device* device); //请求回调函数
};

struct driver {
    pdrv_match_table drv_match_table;
    struct linklist_head sibling;
    struct drv_class class;
};

struct device {
    struct dir_operation f_op;
    struct drv_class d_op;
    struct linklist_head dev_type_node;
    
    struct task_struct* task;
    
    atomic_t refcount;
    uint64_t mmio_start_addr;
    uint64_t mmio_start_vaddr;
    uint16_t vendor_id;
    uint16_t device_id;

    uint16_t type_devid;
    uint16_t sn_id; //序号

    uint32_t irq;
};

#define MODULE_SET_DRIVER_OP(drv) static struct drv_class* export_drv_sym __attribute__((section(".drv_init"), aligned(sizeof(void*)), used)) = &(drv);

void drv_device_match(struct drv_class* drv, pdrv_match_table table);
void device_try_probe(uint16_t vendor_id, uint16_t device_id);
void init_devicebus();
void init_drv();
void set_device_type(struct device* dev, drv_type type);
#endif