[bits 32]
;ax = cursor_position
next_cursor_32:
    add ah,1 ; line + 1
    cmp ah,50h ; line < 80?

    jl next_cursor_end_32 ; next
    sub ah,50h ; line -= 50h
    add al,1
next_cursor_end_32:
    call set_cursor
    ret

; ah = col
; al = line
; bl = char c
; bh = color
print_char_32:
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

cls_32:
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov cx,0xfa0
    mov ax,0
    mov bx,0

cls_start_32:
    call print_char_32
    call next_cursor_32
cls_next:
    loop cls_start_32

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
print_string_32:
    push bp
    mov bp,sp
    push ax
    push bx
    push si

print_str_start_32:
    mov bl,[si]
    mov bh,0x7
    call print_char_32
    inc si
    call next_cursor_32
    cmp bl,0
    jne print_str_start_32

    pop si
    pop bx
    pop ax
    pop bp
    ret

;ah = col
;al = line
set_cursor:
    push eax
    push ebx
    push edx

    mov ebx,0
    mov bl,al

    imul ebx,80
    mov al,ah
    mov ah,0
    add ebx,eax ;get cursor pos

    cmp ebx,0x7d0
    jnl set_cursor_done
    
    ;set high 8
    mov dx,0x3D4
    mov al,0x0E
    out dx,al
    mov al,bh ;high 8
    mov dx,0x3d5
    out dx,al ;high 8

    ;set low 8
    mov dx,0x3d4 ;3d4:vga register
    mov al,0x0f
    out dx,al ;f register:low
    mov al,bl ;low 8
    mov dx,0x3d5
    out dx,al ;low 8

set_cursor_done:
    pop edx
    pop ebx
    pop eax
    ret