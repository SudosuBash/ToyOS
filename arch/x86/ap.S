;AP 启动
#include <hal/config_arch.h>
[org 0x20000]
[bits 16]
jmp AP_BL_START
AP_BL_START:
    cli
    mov ax,0
    mov ds,ax
    lgdt [cs:gdt_desc]
    in al,0x92
    or al,0x2
    out 0x92,al
    mov eax,cr0
    or eax,1
    mov cr0,eax
    db 0x66, 0xea
    dd AP_INTO_64BIT_MODE
    dw 0x18
[bits 32]
AP_INTO_64BIT_MODE:
    mov ax, 0x20
    mov ds, ax
    mov ss, ax
    mov eax, [AP_REAL_MODE_DATA_ADDR]
    mov cr3, eax

    mov eax,cr4
    or eax, 32
    mov cr4,eax

    mov ecx, 0xc0000080
    rdmsr
    or eax, 256
    wrmsr

    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax
    
    jmp 0x08:AP_64BIT_MODE
[bits 64]

AP_64BIT_MODE:
    mov rax,(KERNEL_FINAL_LDR_VADDR)
    jmp rax

align 8
gdt:
    gdt_null: dd 0x00000000
              dd 0x00000000 ;第一个段描述符是0，强制规定
    gdt_kernel_code_64: 
              dd 0x0000FFFF
              dd 0x00AF9A00
    gdt_kernel_data_64:
              dd 0x0000FFFF
              dd 0x00AF9200
    gdt_kernel_code: ;0x8
              dd 0x0000FFFF
              dd 0x00CF9A00
    gdt_kernel_data:
              dd 0x0000FFFF
              dd 0x00CF9200
gdt_end:
gdt_desc:
    dw (gdt_end - gdt - 1) ;cpu按照限制长度+1来计算的
    dd gdt