[bits 64]
#include <hal/asm-offset.h>

extern irq_entrance_gate
%macro IRQ_JMP_PUSHAD 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro IRQ_JMP_POPAD 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp,8
    iretq
%endmacro


%macro IRQ_ERR 1
    global irq_entry_%1
    irq_entry_%1:
        IRQ_JMP_PUSHAD
        
        mov rsi, [rsp + IRQ_CS_OFFSET]
        and rsi, 3
        jz irq_entry_call_%1
        swapgs

    irq_entry_call_%1:
        mov rdi,rsp ;传入指针
        mov rsi, %1
        call [irq_entrance_gate + %1 * 8]
        mov rdi,rsp
        
        mov rsi, [rsp + IRQ_CS_OFFSET]
        and rsi, 3
        jz irq_entry_end_%1
        swapgs

    irq_entry_end_%1:
        IRQ_JMP_POPAD
%endmacro

%macro IRQ_NO_ERR 1
    global irq_entry_%1
    irq_entry_%1:
        push 0
        IRQ_JMP_PUSHAD

        mov rsi, [rsp + IRQ_CS_OFFSET]
        and rsi, 3
        jz irq_entry_call_%1
        swapgs

    irq_entry_call_%1:
        mov rdi,rsp ;传入指针
        mov rsi, %1
        call [irq_entrance_gate + %1 * 8]
        mov rdi,rsp

        mov rsi, [rsp + IRQ_CS_OFFSET]
        and rsi, 3
        jz irq_entry_end_%1
        swapgs

    irq_entry_end_%1:
        IRQ_JMP_POPAD
%endmacro

IRQ_NO_ERR 0
IRQ_NO_ERR 1
IRQ_NO_ERR 2
IRQ_NO_ERR 3
IRQ_NO_ERR 4
IRQ_NO_ERR 5
IRQ_NO_ERR 6
IRQ_NO_ERR 7
IRQ_ERR 8
IRQ_NO_ERR 9
IRQ_ERR 10
IRQ_ERR 11
IRQ_ERR 12
IRQ_ERR 13
IRQ_ERR 14
IRQ_NO_ERR 15
IRQ_NO_ERR 16
IRQ_ERR 17
IRQ_NO_ERR 18
IRQ_NO_ERR 19
IRQ_NO_ERR 20
IRQ_ERR 21
IRQ_NO_ERR 22
IRQ_NO_ERR 23
IRQ_NO_ERR 24
IRQ_NO_ERR 25
IRQ_NO_ERR 26
IRQ_NO_ERR 27
IRQ_NO_ERR 28
IRQ_ERR 29
IRQ_ERR 30
%assign i 31
%rep (256-31)
IRQ_NO_ERR i
%assign i i+1
%endrep

section .data
global irq_entry_table
irq_entry_table:
%assign i 0
%rep 256
    dq irq_entry_%+i
%assign i i+1
%endrep