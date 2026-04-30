global rd_disc
;保护模式诶, int 13h就别想用了
;32位通过栈传参
;void rd_disc(u8 rd_count, u32 sector, void* data)
rd_disc:
    push ebp
    mov ebp,esp
    push edi
    push esi

    mov eax,[ebp+8] ;rd_count(data)
    mov esi,eax

    test esi,esi
    jne rd_disc_next
    mov esi,256
rd_disc_next:
    mov dx,0x1f2
    out dx,al

    mov eax,[ebp+12] ;sector
    mov dx,0x1f3
    out dx, al

    shr eax,8
    mov dx,0x1f4
    out dx, al

    shr eax,8
    mov dx,0x1f5
    out dx, al
    
    shr eax,8
    or eax, 0b11100000 ; high3 to 0
    and eax, 0b11101111 ;4 to 0
    mov dx,0x1f6
    out dx, al

    mov eax, 0x20
    mov dx,0x1f7
    out dx, al
    mov edi ,[ebp+16] ;addr
rd_disc_loop:
    mov edx,0x1f7
    in al, dx ;status

    mov ecx,eax
    test eax,eax
    js rd_disc_loop ;busy

    mov eax,ecx
    shl eax,7
    test eax,eax
    jns rd_disc_data
    
    mov edx,0x1f1
    mov eax,0
    in al, dx ;error
    jmp rd_disc_end ;ret
rd_disc_data:
    mov eax,ecx
    and eax,0b00001000
    test eax,eax
    jz rd_disc_loop
    mov dx,0x1F0 ;es:edi
    mov ecx,256 ;一个扇区512字节
    cld 
    rep insw

    dec esi
    jne rd_disc_loop

    mov eax,0

rd_disc_end:
    pop esi
    pop edi
    mov esp,ebp
    pop ebp
    ret
    