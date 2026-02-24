[org 0x7c00]
bits 16

jmp BL_START

bl_panic:
    jmp $

;al = sector count
;ds:si = target load position
;int 13h
rd_disc_bios:
    push bp
    mov bp,sp
    push ax
    push dx
    mov ah,0x02
    add dl,0x80
    int 0x13
    
    cmp ah,0
    jne bl_panic
    pop dx
    pop ax
    pop bp
    ret

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

    mov al,2
    mov ch,0
    mov cl,2
    mov dh,0
    mov dl,0
    mov bx,0x4000
    call rd_disc_bios
    
    jmp 0:0x4000

times 510 - ($-$$) db 0
dw 0xaa55