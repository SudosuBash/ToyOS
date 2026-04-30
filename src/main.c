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

int test_fn(void* test) {
    asm volatile (
        "movq $2, %rax\r\n"
        "syscall"
    );
    while(1);
}

int idle_1(void* test) {
    disable_irq();
    
    do_exec(test_fn);
    return 0;
}

void kernel_start() {
    init_irq();
    init_smp();
    init_cpu();
    
    init_mm();

    init_task();
    
    init_syscall();
    kernel_thread(idle_1, NULL, "Hello, Idle!");
    enable_irq();
    while(1) hlt();
}