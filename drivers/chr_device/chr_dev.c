#include <kernel/drivers/drv_frame.h>
#include <kernel/put.h>

static void chr_drv_init(struct device* dev) {
    put_str("chr driver init!\n");
}

static struct drv_class chr_drv = {
    .init = chr_drv_init,
};

MODULE_SET_DEVICE(chr_drv);