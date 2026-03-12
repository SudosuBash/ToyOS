#include <kernel/fault/fault.h>
#include <kernel/put.h>
#include <regs.h>
#include <irq/irq.h>
#include <cpu/cpu.h>
static void put_kernel_regs(struct irq_frame* frame) {
    uint64_t cr3;
    asm volatile(
        "movq %%cr3,%0\t\n"
        : "=r"(cr3)
        : 
        : "memory"
    );
    put_str("RAX=");put_hex_zfill(frame->rax,16);
    put_str(" RBX=");put_hex_zfill(frame->rbx,16);
    put_str(" RCX=");put_hex_zfill(frame->rcx,16);
    put_str(" RDX=");put_hex_zfill(frame->rdx,16);
    put_char('\n');

    put_str("RSI=");put_hex_zfill(frame->rsi,16);
    put_str(" RDI=");put_hex_zfill(frame->rdi,16);
    put_str(" RBP=");put_hex_zfill(frame->rbp,16);
    put_str(" RSP=");put_hex_zfill(frame->rsp,16);
    put_char('\n');

    put_str("R8=");put_hex_zfill(frame->r8,16);
    put_str(" R9=");put_hex_zfill(frame->r9,16);
    put_str(" R10=");put_hex_zfill(frame->r10,16);
    put_str(" R11=");put_hex_zfill(frame->r11,16);
    put_char('\n');
    

    put_str("R12=");put_hex_zfill(frame->r12,16);
    put_str(" R13=");put_hex_zfill(frame->r13,16);
    put_str(" R14=");put_hex_zfill(frame->r14,16);
    put_str(" R15=");put_hex_zfill(frame->r15,16);
    put_char('\n');

    put_str("RIP=");put_hex_zfill(frame->rip,16);
    put_str(" RFLAGS=");put_hex_zfill(frame->eflags,8);
    put_str(" CR3=");put_hex_zfill(cr3,16);
    put_char('\n');
}

static void panic_call_trace(struct irq_frame* frame) {
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

static void panic_log_position(struct panic_info* info) {
    put_str("on ");
    put_str(info->func);
    put_str(" at ");
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
static void panic_main(struct panic_info* info, struct irq_frame* frame) {
    put_str("Oops! Kernel has crashed ");
    panic_log_position(info);
    put_char('\n');
    put_kernel_regs(frame);
    put_char('\n');
    panic_call_trace(frame);
    put_char('\n');
    put_str("Halting on critical error...\n");
    
    while(1);
}


static void ud_irq_handler(struct irq_frame* frame) {
    uint32_t cs = frame->cs;
    if(IS_IN_KERN_MODE(cs)) { //内核模式直接嘎
        struct panic_info* msg = (struct panic_info*)frame->r15;
        panic_main(msg, frame);
    }
}

void fault_init() {
    irq_register(IRQ_UD_ERR, ud_irq_handler);
}