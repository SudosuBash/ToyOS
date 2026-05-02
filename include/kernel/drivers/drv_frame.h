#ifndef _TOYOS_DRV_OP
#define _TOYOS_DRV_OP

#include <kernel/stdint.h>
#include <kernel/drivers/drv.h>

//驱动程序类
struct drv_class {
    void (*init)(struct device* device);
    void (*probe)(struct device* device);
    void (*start)(struct device* device);
    void (*destroy)(struct device* device);
};

#define MODULE_SET_DEVICE(drv) static struct drv_class* export_drv_sym __attribute__((section(".drv_init"), aligned(sizeof(void*)), used)) = &(drv);

void init_drv();
#endif