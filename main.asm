[org 0x7c00]
mov sp, 0x7c00
mov bp, sp
mov ah, 0x00
mov al, 0x13
int 0x10
call main
put_pixel:
push bp
mov bp, sp
mov ah, 0x0C
mov al, byte 13
mov bh, byte 0
mov cx, [bp + 4]
mov dx, [bp + 6]
int 0x10
mov ax, 0
mov sp, bp
pop bp
ret
mov sp, bp
pop bp
ret
main:
push bp
mov bp, sp
mov [bp +4], word 5
mov [bp +6], word 10
mov ax,word [bp +  4]
push eax
mov ax,word [bp +  6]
push eax
call put_pixel
hlt
times 510-($-$$) db 0
dw 0xaa55
