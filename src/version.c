#include <kernel/version.h>
#include <generated/version.h>
#include <kernel/log/kprintf.h>

static struct kern_version ver = {
    .build_time = KERNEL_COMPILE_TIME,
    .version = KERNEL_VERSION,
    .version_number = KERNEL_VERSION_NUMBER,
    .platform = KERNEL_PLATFORM
};

void log_version_info() {
    kprintf("Toy OS Kernel version %s %s %s %s\n", ver.version, ver.version_number,ver.platform, ver.build_time);
}