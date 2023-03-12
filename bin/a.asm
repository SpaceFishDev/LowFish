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
test:
push bp
mov bp, sp
mov bx, bp
add bx, 4
mov [bp + 5], bx
mov al,byte [bp +  5]
push byte eax
call putc
mov sp, bp
pop bp
ret
main:
push bp
mov bp, sp
mov [bp +4], byte 4
mov [bp + 5], byte string0
mov al,byte [bp +  5]
push byte eax
call test
LO0:
mov dl,byte [bp + 4]
mov cl, byte 27
cmp dl,cl
je LOEND0
call readchar
mov [bp +4], byte al
mov dl,byte [bp + 4]
mov cl, byte 13
cmp dl,cl
jne LOEND1
mov [bp +6], byte 10
mov al,byte [bp +  6]
push byte eax
call putc
LOEND1:
mov al,byte [bp +  4]
push byte eax
call putc
mov dl,byte [bp + 4]
mov cl, byte 8
cmp dl,cl
jne LOEND2
mov [bp +7], byte ' '
mov dl, 31
mov cl, 1
add dl, cl
mov [ebp + 7], dl
mov al,byte [bp +  7]
push byte eax
call putc
mov al,byte [bp +  4]
push byte eax
call putc
LOEND2:
jmp LO0
LOEND0:
hlt
times 510-($-$$) db 0
dw 0xaa55
string0:
db `Test`,0
