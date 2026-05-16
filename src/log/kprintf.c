#include <kernel/log/kprintf.h>
#include <kernel/def.h>
#include <kernel/atomic/atomic.h>
#include <kernel/stdlib.h>
#include <kernel/drivers/drv_reminder.h>

static struct rio_buf_queue rio_buf;

char get_print_buf() {
    return read_from_buf(&rio_buf);
}

int kprintf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt); 
    int result = vfprintf(&rio_buf, fmt, ap);
    remind_device_type(DRV_CONSOLETYP);
    va_end(ap); 
    return result;
}