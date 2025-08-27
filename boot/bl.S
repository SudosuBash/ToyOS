[org 0x7c00]
bits 16

jmp BL_START
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
    mov [fs:bx], al
    mov [fs:bx+1], ah
    
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
    add ah,1 ; line + 1
    cmp ah,50h ; line < 80?

    jl cls_next ; next
    sub ah,50h ; ah -= 50h
    add al,1
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
; bx = char* strptr
; cx = length
print_string:
    push bp
    mov bp,sp
    push ax
    push bx
    push cx

print_str_start:
    
    
    pop cx
    pop bx
    pop ax
    pop bp
    ret
    
BL_START:
    mov ax,0x1000
    mov ss,ax
    mov ax,0xb800
    mov fs,ax
    
    mov ax,0
    mov bx,0
    mov cx,0
    mov dx,0
    mov sp,0x7c00

    call cls
    mov ah,1
    mov bl,0x48
    mov bh,0x7

    call print_char
    jmp $

hello_toyos db "Hello,ToyOS!",0
times 510 - ($-$$) db 0
dw 0xaa55