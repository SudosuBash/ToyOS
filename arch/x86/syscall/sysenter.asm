#include <cpu/gdt.h>
extern syscall_table
extern ret_to_user

syscall_enter:
    swapgs ;切换成内核态gs
    mov [gs:USER_RSP], rsp ;切换rsp
    mov rsp, [gs:KERNEL_RSP]
    push USER_DS
    push [gs:USER_RSP] ;rsp
    push r11 ;RFLAGS
    push USER_CS ;CS
    push rcx ;rip

    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    call [syscall_table + rax * 8]

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    pop rcx ;rip
    add rsp,8 ;跳过CS
    pop r11 ;RFLAGS
    add rsp,16 ;跳过RSP,SS
    ;mov [gs:KERNEL_RSP], rsp ;这甚至都不用写
    mov rsp, [gs:USER_RSP]
    swapgs
    sysretq