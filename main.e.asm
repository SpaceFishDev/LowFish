section .bsssection .data
string0:
	dd `abc`,0
section .text
print:
push ebp
mov ebp, esp
mov esp, ebp
pop ebp
ret
main:
push ebp
mov ebp, esp
	push dword string0
	push dword [ebp + 8]
	call print
mov esp, ebp
pop ebp
ret
