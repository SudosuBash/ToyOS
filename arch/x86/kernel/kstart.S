#include <generated/offset.h>
#include <hal/config_arch.h>

extern __stack_top
extern kernel_x86_start
extern kernel_x86_ap_start
extern ap_respack

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
    jmp kernel_x86_start

section .text_ap_start
_ap_start:
    mov rax, (0xfee00020 + MMIO_MEM_SA_VADDR)
    mov eax, [rax]
    shr eax, 24

    mov rbx, [ap_respack]
    mov rdx, rax
    imul rdx, size_ap_resource
    add rbx, rdx
    mov rsp, [rbx + offset_ap_resource_rsp]

    mov rdi, rax
    mov ax,0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov rax, 0
    mov rbx, 0
    mov rcx, 0
    mov rdx, 0
    mov rsi, 0
    mov rbp, 0

    jmp kernel_x86_ap_start