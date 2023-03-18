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
mov [bp +5], byte 0
mov [bp +6], byte 0
LO0:
mov dl,byte [bp + 4]
mov cl, byte 27
cmp dl,cl
je LOEND0
mov dl, byte[bp + 5]
mov cl, 1
add dl, cl
mov [ebp + 5], dl
call readchar
mov [bp +4], byte al
mov dl,byte [bp + 4]
mov cl, byte 13
cmp dl,cl
jne LOEND1
mov [bp +7], byte 10
mov [bp +5], byte 0
mov dl, byte[bp + 6]
mov cl, 1
add dl, cl
mov [ebp + 6], dl
mov al,byte [bp +  7]
push byte eax
call putc
LOEND1:
mov dl,byte [bp + 5]
mov cl, byte -1
cmp dl,cl
jne LOEND2
mov dl,byte [bp + 4]
mov cl, byte 8
cmp dl,cl
jne LOEND3
mov [bp +4], byte 'b'
mov dl, byte[bp + 5]
mov cl, 1
add dl, cl
mov [ebp + 5], dl
LOEND3:
LOEND2:
mov al,byte [bp +  4]
push byte eax
call putc
mov dl,byte [bp + 4]
mov cl, byte 8
cmp dl,cl
jne LOEND4
mov [bp +8], byte ' '
mov dl, 31
mov cl, 1
add dl, cl
mov [ebp + 8], dl
mov al,byte [bp +  8]
push byte eax
call putc
mov al,byte [bp +  4]
push byte eax
call putc
LOEND4:
jmp LO0
LOEND0:
hlt
times 510-($-$$) db 0
dw 0xaa55