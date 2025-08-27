;准备进入内核
[org 0x900]
[bits 16]
jmp preparation
gdt:
    gdt_null: dd 0x00000000
              dd 0x00000000 ;第一个段描述符是0，强制规定
    gdt_kernel_code: 
              dd 0x0000FFFF
              dd 0x00CF9A00
    gdt_kernel_data:
              dd 0x0000FFFF
              dd 0x00CF9200
    gdt_kernel_stack:
              dd 0x0000FFFF
              dd 0x00CF9200
    gdt_kernel_8025_mode:
              dd 0x8000FFFF
              dd 0x00CF920B
gdt_end:
gdt_desc:
    dw (gdt_end - gdt - 1) ;cpu按照限制长度+1来计算的
    dd gdt
preparation:
    jmp goto_pr_mode

;ds:si: gdt base
goto_pr_mode:
    cli
    ;load gdt
    lgdt [gdt_desc]
    ;open a20
    in al,0x92
    or al,0x2
    out 0x92,al
    ;finally!
    mov eax,cr0
    or eax,1
    mov cr0,eax
    
    jmp 0x08:pt_mode_start
[bits 32]
%include "io32.S"

pt_mode_start:
    mov ax,0x20
    mov gs,ax ;4
    mov ax,0x10
    mov ds,ax ;2
    mov ax,0x18
    mov ss,ax ;3

    mov eax,0
    mov ebx,0
    mov ecx,0
    mov edx,0
    mov esp,0x2000
    mov esi,0
    
    call cls_32

    mov ax,0
    mov si,pe_mode_str
    call print_string_32
    jmp $

pe_mode_str db "Came into Protection Mode.",0
times 512 - ($-$$) db 0