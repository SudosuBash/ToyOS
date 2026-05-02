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


void kernel_start() {
    init_irq();
    init_smp();
    init_cpu();
    init_mm();
    init_task();
    init_syscall();
    init_vfs();
    init_drv();
    
    while(1) hlt();
}