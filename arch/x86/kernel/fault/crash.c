#include <kernel/fault/fault.h>
#include <kernel/put.h>
#include <hal/hal.h>
#include <kernel/irq/irq.h>
#include <hal/cpu/cpu.h>
#include <kernel/mm/mm.h>
#include <kernel/atomic/spinlock.h>
#include <kernel/cpu/archimpl.h>
#include <hal/irq/irq.h>

static uint64_t cr2;
static uint64_t cr3;
static spinlock_t crash_spin;

static void put_kernel_regs(struct arch_regs* frame) {
    put_str("Registers info:\n");
    put_str(" RAX=");put_hex_zfill(frame->rax,16);
    put_str(" RBX=");put_hex_zfill(frame->rbx,16);
    put_str(" RCX=");put_hex_zfill(frame->rcx,16);
    put_str(" RDX=");put_hex_zfill(frame->rdx,16);
    put_char('\n');

    put_str(" RSI=");put_hex_zfill(frame->rsi,16);
    put_str(" RDI=");put_hex_zfill(frame->rdi,16);
    put_str(" RBP=");put_hex_zfill(frame->rbp,16);
    put_str(" RSP=");put_hex_zfill(frame->rsp,16);
    put_char('\n');

    put_str(" R8=");put_hex_zfill(frame->r8,16);
    put_str(" R9=");put_hex_zfill(frame->r9,16);
    put_str(" R10=");put_hex_zfill(frame->r10,16);
    put_str(" R11=");put_hex_zfill(frame->r11,16);
    put_char('\n');
    

    put_str(" R12=");put_hex_zfill(frame->r12,16);
    put_str(" R13=");put_hex_zfill(frame->r13,16);
    put_str(" R14=");put_hex_zfill(frame->r14,16);
    put_str(" R15=");put_hex_zfill(frame->r15,16);
    put_char('\n');

    put_str(" RIP=");put_hex_zfill(frame->rip,16);
    put_str(" RFLAGS=");put_hex_zfill(frame->eflags,8);
    put_str(" CR2=");put_hex_zfill(cr2,16);
    put_str(" CR3=");put_hex_zfill(cr3,16);
    put_char('\n');
}

static void crash_call_trace(struct arch_regs* frame) {
    put_str("Call trace (address):");
    uint64_t rbp = frame->rbp;
    while(rbp != 0) {
        put_char('\n'); 
        put_str(" - ");
        put_hex(*(uint64_t*)(rbp+8)); //RIP
        
        rbp = *(uint64_t*)(rbp);
    }
    put_char('\n');
}


static void print_mem_info() {
    uint64_t page_avl = get_system_mem_alloced();
    uint64_t page_all = get_system_mem_sum();
    uint64_t mem_avl = page_avl << PAGE_OFFSET;
    uint64_t mem_all = page_all << PAGE_OFFSET;
    uint64_t mem_percent = get_mem_alloc_percentage();
    put_str("RAM info:\n");
    if(mem_percent == MEM_INIT_ERROR_PERCENTAGE) {
        put_str(" Error: Memory init failure...");
    } else {
        put_str(" allocated: ");
        put_dec(mem_avl);
        put_str(" Bytes / all: ");
        put_dec(mem_all);
        put_str(" Bytes (");
        put_dec(mem_percent);
        put_str("%)\n");
        put_str(" pages: used ");
        put_dec(page_avl);
        put_str(" / all ");
        put_dec(page_all);
    }

    put_char('\n');
}

static void crash_log_position(struct crash_info* info) {
    put_str("on <");
    put_str(info->func);
    put_str("> at ");
    put_str(info->filename);
    put_str(":");
    put_dec(info->line);
    put_char('\n');
    put_str("Reason: ");
    put_str(info->message);
    if(info->condition) {
        put_str(info->condition);
    }
    put_char('\n');
}

static void crash_irq_position(struct crash_info* info,struct arch_regs* frame, uint64_t irq_num) {
    put_str("on irq ");
    put_dec(irq_num);
    if(irq_num == IRQ_PG_ERR) {
        put_str(" (Page Fault on ");
        put_hex_zfill(cr2,16);
        put_str(")");
    }
    put_str(".\n");
    put_str("Reason: ");
    put_str(info->message);
    put_char('\n');
}
static void do_fault(struct crash_info* info, struct arch_regs* frame, uint64_t irq) {
    spin_lock(&crash_spin);
    asm volatile(
        "movq %%cr2,%0\t\n"
        : "=r"(cr2): : "memory"
    );
    
    asm volatile(
        "movq %%cr3,%0\t\n"
        : "=r"(cr3) : : "memory"
    );
    put_str("!!! KERNEL PANIC\n");
    put_str("Oops! Kernel has crashed ");

    if(irq == 0) {
        crash_log_position(info);
    } else {
        crash_irq_position(info,frame, irq);
    }

    put_str("\n");
    print_mem_info();
    put_char('\n');
    put_kernel_regs(frame);
    put_char('\n');
    crash_call_trace(frame);
    put_char('\n');
    put_str("Halting on critical error...\n");
    hlt();
}

void fault_irq(const char* name, struct arch_regs* frame, uint64_t irq_num) {
    disable_irq();
    struct crash_info info = {
        .message = name,
        .condition = NULL,
        .filename = NULL,
        .func = NULL,
        .line = 0
    };
    do_fault(&info, frame, irq_num);
}

static void gp_fault_trigger(struct arch_regs* arch, uint64_t num) {
    fault_irq("General Protection Fault", arch, num);
}

void fault(struct crash_info* info, struct arch_regs* frame) {
    disable_irq();
    do_fault(info, frame, 0);
}


static void ud_irq_handler(struct arch_regs* frame, uint64_t irq_num) {
    uint32_t cs = frame->cs;
    if(IS_IN_KERN_MODE(cs)) { //内核模式直接嘎
        struct crash_info* msg = (struct crash_info*)frame->r15;
        fault(msg, frame);
    }
}


void warn(const char* message) {
    put_str("[WARN] ");
    put_str(message);
    put_char('\n');
}

void fault_init() {
    irq_single_register(IRQ_UD_ERR, ud_irq_handler);
    irq_single_register(IRQ_GP_ERR, gp_fault_trigger);
}













































#include <kernel/syscall/syscall.h>
extern int __arch_check_stack_canary(long long expected, long long actual);
DEFINE_SYSCALL2(reserved, expected, long long, actual, long long) {
    asm volatile ("jmp .label\t\n"
    ".asciz \"WARN: INCOMPLETE AND EMPTY SYSCALL!\" \n\t"
    ".labeI:\n\t"
    "movq $38,%rax \n\t"
    "retq \n\t"
    ".label:");
    
    //Stack Canary Check, to avoid error.
    //Temporary, incomplete.
    return __arch_check_stack_canary(expected, actual); //Stack 
}