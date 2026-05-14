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

int test_fn(void* test) {
    asm volatile (
        "movq $0, %rax\r\n"
        "movq $0x7fff00, %rdi\r\n"
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
    } else if((uint32_t) test == 2) {
        kernel_thread(idle_1, 3, "Idle 3");
    }

    // do_exec(test_fn);
    while(1);
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

    kernel_thread(idle_1, 1, "Idle 1");
    enable_irq();
    device_try_probe(0x8086, 0x03f8);
    int fd = do_open("/test_file");

    char* test = "Hello, printk!\n";
    do_write(fd, test, strlen(test));
    enable_irq();
    while(1) hlt();
}