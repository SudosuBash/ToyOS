#include <kernel/log/kprintf.h>
#include <kernel/def.h>
#include <kernel/atomic/atomic.h>
#include <kernel/stdlib.h>
#include <kernel/drivers/drv_reminder.h>

static struct rio_broadcast rio_buf;
// static struct rio_buf_queue rio_buf;

void register_print_reader(struct rio_reader *reader) {
    register_reader_broadcast(&rio_buf, reader);
}

char get_print_buf(struct rio_reader* reader) {
    return rio_broadcast_recv(&rio_buf, reader);
}

int kprintf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt); 
    int result = vfprintf(&rio_buf, fmt, ap);
    remind_device_type(DRV_CONSOLETYP);
    va_end(ap); 
    return result;
}

void init_log() {
    init_rio_broadcast(&rio_buf);
    
}