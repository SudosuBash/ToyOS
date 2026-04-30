extern __stack_top
extern kernel_x86_start

section .text
global _start

_start:
    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    mov rax,0
    mov rbx,0
    mov rcx,0
    mov rdx,0
    mov rsi,0
    mov rdi,0
    mov rbp,0
    mov rsp, __stack_top
    call kernel_x86_start
