section .bss
section .data
strPrnt:
dd "%d",0
string0:
	dd `Hello, World!\n`,0
[extern printf]
[extern _exit]
section .text
puts:
push ebp
mov ebp, esp
push dword [ebp + 8]
call printf
mov esp, ebp
pop ebp
ret
puti:
push ebp
mov ebp, esp
push dword [ebp + 8]
push dword strPrnt
call printf
mov esp, ebp
pop ebp
ret
exit:
push ebp
mov ebp, esp
push dword [ebp + 8]
call _exit
mov esp, ebp
pop ebp
ret
main:
push ebp
mov ebp, esp
mov [ebp + 0], dword 0
LO0:
mov eax, [ ebp + 0]
mov edx, [ ebp + 0]
cmp edx, eax
jne LOEND1
push dword string0
	call puts
	push dword eax
push dword 32
	call puti
jmp LO0
LOEND1:
	push dword [ebp +  8]
	call exit
mov esp, ebp
pop ebp
ret
