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


#endif