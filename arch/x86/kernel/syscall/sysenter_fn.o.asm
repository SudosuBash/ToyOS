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
    push (3*8)
    push r11 ;RFLAGS
    push (4*8) ;CS
    push rcx ;rip
    mov rcx,r10 ; rcx
    sti
    cmp rax, 512
    jge syscall_error
    call [syscall_table + rax * 8]
    jmp syscall_enter_end
syscall_error:
    mov rax, 38
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
