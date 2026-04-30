#ifndef _TOYOS_DRV_DEVICEFS
#define _TOYOS_DRV_DEVICEFS

#include <kernel/file/dir.h>
#include <kernel/drivers/drv_basic.h>

void devicefs_mount(char* dir, char* name, struct device* dev);

#endif