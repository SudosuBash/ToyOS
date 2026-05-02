#ifndef _TOYOS_DRV_DEVICEFS
#define _TOYOS_DRV_DEVICEFS

#include <kernel/file/dir.h>
#include <kernel/drivers/drv.h>
#include <kernel/kernel.h>

#define DEVFS_MAGIC (0x2 <<  MAGIC_NUMBER)

struct directory* devicefs_mount(char* path, char* name, struct device* dev) ;
stat_t* devicefs_unmount(char* path, struct device* dev);
void init_devicefs();

#endif