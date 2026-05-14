#include <cpu/gdt_config.h>
#include <kernel/config.h>
#include <kernel/fault/errno.h>
[bits 64]
extern ret_to_user
extern user_rsp
extern kernel_rsp
extern syscall_table

global syscall_enter

syscall_enter:
    cli
    swapgs ;切换成内核态gs
    mov [gs:user_rsp], rsp ;切换rsp
    mov rsp, [gs:kernel_rsp]
    push USER_DS
    push r11 ;RFLAGS
    push USER_CS ;CS
    push rcx ;rip
    mov rcx,r10 ; rcx

    sti
    cmp rax, SYSCALL_COUNT
    jge syscall_error
    call [syscall_table + rax * 8]
    jmp syscall_enter_end

syscall_error:
    mov rax, ENOSYS
syscall_enter_end:
    cli

    mov r10,rcx
    pop rcx ;rip
    add rsp,8 ;跳过CS
    pop r11 ;RFLAGS
    add rsp,16 ;跳过RSP,SS
    ;mov [gs:KERNEL_RSP], rsp 
    ; 栈守恒, 否则绝对出错了
    mov rsp, [gs:user_rsp] ;切换回 user_rsp
    swapgs
    o64 sysret