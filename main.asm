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
readchar:
push bp
mov bp, sp
mov ah, 0x00
int 0x16
mov sp, bp
pop bp
ret
main:
push bp
mov bp, sp
mov [bp +4], byte 4
LO0:
mov dl,byte [bp + 4]
mov cl,byte [bp + 4]
cmp dl,cl
jne LOEND0
call readchar
mov [bp +4], byte al
mov dl,byte [bp + 4]
mov cl, byte 13
cmp dl,cl
jne LOEND1
mov [bp +5], byte 10
mov al,byte [bp +  5]
push eax
call putc
LOEND1:
mov al,byte [bp +  4]
push eax
call putc
mov dl,byte [bp + 4]
mov cl, byte 8
cmp dl,cl
jne LOEND2
mov [bp +6], byte ' '
mov al,byte [bp +  6]
push eax
call putc
mov al,byte [bp +  4]
push eax
call putc
LOEND2:
jmp LO0
LOEND0:
hlt
times 510-($-$$) db 0
dw 0xaa55
