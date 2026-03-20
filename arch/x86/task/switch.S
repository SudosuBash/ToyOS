global ret_from_fork
global kernel_thread_helper
global execve_to_user_mode

%macro SWC_POPAD 0
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
    add rsp,16
%endmacro

section .text
kernel_thread_helper:
    call rsi
    ;后续还得写exit逻辑,这儿暂时不写
    ret

ret_from_fork:
    SWC_POPAD
    iretq

execve_to_user_mode:
    SWC_POPAD
    iretq