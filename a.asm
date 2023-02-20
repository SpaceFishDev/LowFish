section .bss
section .data
string0:
	dd `Hello, World!`,0
[extern printf]
section .text
print:
push ebp
mov ebp, esp
push dword [ebp + 8]
call printf
mov esp, ebp
pop ebp
ret
main:
push ebp
mov ebp, esp
push dword string0
	call print
mov esp, ebp
pop ebp
ret
