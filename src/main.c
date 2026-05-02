#include <kernel/mm/mm.h>
#include <kernel/mm/mm_slab.h>
#include <kernel/cpu/cpu.h>
#include <kernel/irq/irq.h>
#include <kernel/kstart.h>
#include <kernel/fault/fault.h>
#include <kernel/put.h>
#include <kernel/irq/timer.h>
#include <kernel/cpu/archimpl.h>
#include <kernel/task/task.h>
#include <kernel/task/fork.h>
#include <kernel/task/exec.h>
#include <kernel/syscall/syscall.h>
#include <kernel/drivers/drv_frame.h>
#include <kernel/fs/devicefs.h>

struct file* dfs_do_open(struct directory* dir) {
    put_str("Opened the file /devicefs!\n");
}

void test_device() {
    struct device* device = (struct device*)kmalloc(sizeof(struct device));
    device->operation.do_open = dfs_do_open;
    devicefs_mount("/", "devicefs", device);
}

void kernel_start() {
    init_irq();
    init_smp();
    init_cpu();
    init_mm();
    init_task();
    init_syscall();
    init_vfs();

    test_device();
    int* fd = do_open("/devicefs");
    
    while(1) hlt();
}