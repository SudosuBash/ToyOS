#include <kernel/stdint.h>
#include <hal/syscall/syscall.h>
#include <hal/asm.h>
#include <hal/cpu/msr_base.h>
#include <kernel/mm/mm.h>

#include <kernel/cpu/smp.h>
#include <generated/syscall_id.inc>
extern void syscall_enter();
static inline void set_syscall_star() {
    uint32_t rdx = KERNEL_CS | ((USER_CS - 16) << 16);
    asm volatile(
        "wrmsr"
        : 
        : "a"(0), "d"(rdx),
        "c"(MSR_IA32_STAR)
    );
}
static inline void set_syscall_addr() {
    uint64_t fn = (uint64_t)syscall_enter;
    barrier();
    asm volatile(
        "wrmsr"
        : 
        : "a"(fn & 0xffffffff), "d"(fn >> 32),
        "c"(MSR_IA32_LSTAR)
    );
}
void init_syscall() {
    set_syscall_addr();
    set_syscall_star();
}