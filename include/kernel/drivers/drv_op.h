#ifndef _TOYOS_DRV_OP
#define _TOYOS_DRV_OP
#include <kernel/stdint.h>
#include <kernel/drivers/drv_basic.h>
//驱动程序类
struct basic_drv_class {
    void (*init)(struct device* device);
    void (*probe)();
    void (*start)();
    void (*destroy)();
};


#endif