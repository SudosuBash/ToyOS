#include <kernel/drivers/drv.h>
#include <kernel/put.h>

static struct drv_class chr_drv;

static drv_match_table table = {
    PCI_DEVID(0x8086, 0x03f8),
    {0,0}
};

static void chr_drv_init() {
    put_str("chr driver init!\n");
    drv_device_match(&chr_drv, &table);
}

static void chr_drv_probe(struct device* dev) {
    put_str("chr driver probe!\n");
}

static void chr_drv_destroy(struct device* dev) {
    put_str("chr driver destroy!\n");
}

static struct drv_class chr_drv = {
    .init = chr_drv_init,
    .probe = chr_drv_probe,
    .destroy = chr_drv_destroy
};


MODULE_SET_DEVICE(chr_drv);