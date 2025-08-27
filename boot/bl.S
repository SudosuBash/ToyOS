[org 0x7c00]
bits 16

jmp BL_START

;ax = cursor_position
next_cursor:
    add ah,1 ; line + 1
    cmp ah,50h ; line < 80?

    jl next_cursor_end ; next
    sub ah,50h ; line -= 50h
    add al,1
next_cursor_end:
    ret

; ah = col
; al = line
; bl = char c
; bh = color
print_char:
    push bp
    mov bp,sp
    ; final pos = B8000H + ax * 160(A0H) + bx * 2
    push ax
    push bx
    push cx

    mov cx,ax
    mov cl,ch
    mov ch,0; got col
    add cx,cx; col * 2

    mov bl,0xa0
    mul bl ; line * 160
    add ax,cx ; final pos
    mov bx,ax ; move to bx

    mov ax,word [bp-4]
    mov [gs:bx], al
    mov [gs:bx+1], ah

    
    pop cx
    pop bx
    pop ax
    pop bp
    ret

cls:
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov cx,0xfa0
    mov ax,0
    mov bx,0

cls_start:
    call print_char
    call next_cursor
cls_next:
    loop cls_start

    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret

; al = start_col
; ah = start_row
; ds:si = char* strptr
; the end of string must be zero.
print_string:
    push bp
    mov bp,sp
    push ax
    push bx
    push si

print_str_start:
    mov bl,[si]
    mov bh,0x7
    call print_char
    inc si
    call next_cursor
    cmp bl,0
    jne print_str_start

    pop si
    pop bx
    pop ax
    pop bp
    ret

bl_panic:
    call cls
    mov si,bl_panic_str
    mov ax,0
    call print_string
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

    call cls ;clear screen

    mov al,1
    mov ch,0
    mov cl,2
    mov dh,0
    mov dl,0
    mov bx,0x900
    call rd_disc_bios

    mov ax,0
    mov si,hello_toyos
    
    call print_string
    jmp 0:0x900

hello_toyos db "ToyOS Bootloader.",0
bl_panic_str db "!!!!!!!! Bootloader Panic! !!!!!!!!!",0
times 510 - ($-$$) db 0
dw 0xaa55