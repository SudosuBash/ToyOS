global enter_64bit_mode
global open_pg_mode
enter_64bit_mode:
    mov eax,cr4
    or eax, 32
    mov cr4,eax
    mov ecx, 0xc0000080
    rdmsr
    or eax, 256
    wrmsr
    ret
[bits 32]
open_pg_mode:
    mov ebp,esp
    mov ecx,[ebp+4] ;gdt
    mov ebx,[ebp+8] ;kernel vaddr low4
    mov edi,[ebp+12] ;kernel high4
    mov edx, (0x10000)
    mov cr3, edx
    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax
    lgdt [ecx]
    jmp 0x08:switch_to_64
[bits 64]
switch_to_64:
    shl rdi,32
    or rdi,rbx ;edi: kernel
    mov [seg_data], rdi
    mov word [seg_data+8], 8
    mov rax,0
    mov rbx,0
    mov rcx,0
    mov rdx,0
    mov rsi,0
    lea rdi,[seg_data]
    jmp far [rdi]
seg_data:
    dw 0
    dq 0
