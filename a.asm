[org 0x7c00]
mov sp, 0x7c00
mov bp, sp
call main
putc:
push bp
mov bp, sp
mov al, [bp + 4]
mov ah, 0x0e
int 0x10
mov sp, bp
pop bp
ret
main:
push bp
mov bp, sp
mov [bp +4], byte 3
mov [bp +4], byte 65
mov al,byte [bp +  4]
push eax
call putc
mov [bp +5], byte 'a'
mov al,byte [bp +  5]
push eax
call putc
times 510-($-$$) db 0
dw 0xaa55
