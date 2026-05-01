#include <kernel/drivers/drv_frame.h>


static void chr_init(struct device* dev) {
    
}


struct drv_class dev = {
    .init = chr_init
};