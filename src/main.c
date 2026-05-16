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
#include <kernel/drivers/drv.h>
#include <kernel/fs/devicefs.h>
#include <kernel/stdlib.h>
#include <kernel/drivers/drv_bus.h>
#include <kernel/log/kprintf.h>

int test_fn(void* test) {
    asm volatile (
        "movq $3, %rax\r\n"
        "movq $20, %rdi\r\n"
        "movq $256, %rsi\r\n"
        "movq $2, %r10\r\n"
        "movq $2, %rdx\r\n"
        "syscall"
    );
    *(uint64_t*)(0x7fff00) = 1;
    while(1);
}

int idle_1(void* test) {
    // disable_irq();
    if((uint32_t) test == 1) {
        kernel_thread(idle_1, 2, "Idle 2");
        do_exec(test_fn);
    } else if((uint32_t) test == 2) {
        kernel_thread(idle_1, 3, "Idle 3");
    }

    while(1) {
        if(test==1) {
            put_str("1\n");
        }
    }
    return 0;
}

void kernel_start() {
    init_irq();
    init_smp();
    init_cpu();
    init_mm();
    init_task();
    init_syscall();
    init_vfs();
    init_drv();
    device_try_probe(0x8086, 0x03f8);
    kernel_thread(kdrv_minder_daemon, NULL, "kdriverd");
    enable_irq();
    kprintf("%s", "Hello, world!\n");
    // kernel_thread(idle_1, 1, "Idle 1");
    // enable_irq();
    // 
    // int fd = do_open("/test_file");

    // struct rio_buf_queue buf;
    // fprintf(&buf, "%s%d", "Hello, printk!", 114514);
    // enable_irq();
    int n = 0;
    while(n<100000) {
        kprintf("%d\n", n);
        n++;
    }
    while(1);
}