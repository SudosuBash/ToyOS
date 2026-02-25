#ifndef _TOYOS_DRV_OP
#define _TOYOS_DRV_OP
#include <kernel/stdint.h>
//驱动程序类
struct basic_drv_class {
    void (*init)();
    void (*probe)();
    void (*start)();
    void (*destroy)();
};


#endif