global ret_from_fork
global kernel_thread_helper

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
%endmacro

section .text
kernel_thread_helper:
    call rsi
    ;后续还得写exit逻辑,这儿暂时不写

ret_from_fork_kernel_thread:
    SWC_POPAD
    call rbx

ret_from_fork:
    SWC_POPAD
    
    iretq