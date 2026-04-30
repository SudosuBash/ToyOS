[org 0x7c00]
bits 16

jmp BL_START

bl_crash:
    jmp $
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

rd_mem_info:
    push bp
    mov bp,sp
    push ebx
    push edi
    push esi

    mov ax,0
    mov es,ax
    mov esi,0
    mov di, 0x3004 

rd_mem_ld:
    
    mov ax, 0xe820
    mov edx,0x534D4150
    mov ecx,24
    
    int 15h
    add di,24
    add esi,1
    test ebx,ebx
    jne rd_mem_ld
    
    mov [es:0x3000], esi ;数量
    ;short mem_sz;
    pop esi
    pop edi
    pop ebx
    mov sp,bp
    pop bp
    ret
    
BL_START:
    mov ax,0x7c00
    mov ss,ax
    mov ax,0xb800
    mov gs,ax
    
    mov ax,0
    mov bx,0
    mov cx,0
    mov dx,0
    mov sp,0

    call rd_mem_info
    
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
%include "kloader/rddisk.S"
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
    
    mov eax,0x40000
    push eax
    mov eax,4
    push eax
    mov eax,30
    push eax
    call rd_disc

    jmp 0x40000
times 510 - ($-$$) db 0
dw 0xaa55