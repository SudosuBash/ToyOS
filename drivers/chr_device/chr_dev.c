#include <kernel/drivers/drv.h>
#include <kernel/vfs/vfs.h>
#include <kernel/fs/devicefs.h>
#include <kernel/put.h>
#include <kernel/fault/error.h>
#include <kernel/log/kprintf.h>
#include <kernel/task/task.h>
#include <asm.h>
#include <kernel/base/rio_broadcast.h>

static struct drv_class chr_drv;
static struct rio_reader reader;

static drv_match_table table = {
    PCI_DEVID(0x8086, 0x03f8),
    {0,0}
};

static struct file* drv_do_open(struct directory* dir) {
    struct file* fd = alloc_file(dir);
    if(IS_ERR(fd))
        return ERR_PTR(fd);
    return fd;
}

static stat_t drv_do_write(struct file* file, void* buf, size_t len) {
    char* cbuf = (char*)buf;
    cbuf[len] = 0;
    put_str(cbuf);
    return 0;
}

struct dir_operation oper = {
    .do_open = drv_do_open,
    .do_write = drv_do_write
};

static void chr_drv_init() {
    drv_device_match(&chr_drv, &table);
}

static void chr_drv_probe(struct device* dev) {
    DEVICE_BIND_FILE_OP(dev, oper);
    set_device_type(dev, DRV_CONSOLETYP);
    register_print_reader(&reader);
}

static void chr_drv_destroy(struct device* dev) {
    put_str("[serial driver] serial driver destroyed.\n");
}

static void chr_drv_callback(struct device* dev) {
    char c = get_print_buf(&reader);
    while(c != 0) {
        outb(0x3f8,c);
        c = get_print_buf(&reader);
    }
}

static struct drv_class chr_drv = {
    .init = chr_drv_init,
    .probe = chr_drv_probe,
    .destroy = chr_drv_destroy,
    .start = chr_drv_callback
};

MODULE_SET_DRIVER_OP(chr_drv);