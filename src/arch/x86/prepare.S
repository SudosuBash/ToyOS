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
%include "rddisk.S"
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
    
    mov eax,0x10000
    push eax
    mov eax,4
    push eax
    mov eax,6
    push eax
    call rd_disc

    jmp 0x10000

pe_mode_str db "Came into Protection Mode.",0
times 1024 - ($-$$) db 0