global ret_from_fork
global kernel_thread_helper

%macro SWC_POPAD 0
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
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