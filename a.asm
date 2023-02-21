section .bss
section .data
strPrnt:
dd "%d",0
string0:
	dd `Here`,0
[extern printf]
[extern exit]
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
call exit
mov esp, ebp
pop ebp
ret
main:
push ebp
mov ebp, esp
mov [ebp + 0], dword 3
mov [ebp + 4], dword 5
push eax
mov eax, [ ebp + 4]
mov edx, [ ebp + 0]
cmp edx, eax
je LO0
push dword string0
	call puts
LO0:
push dword 0
	call exit
mov esp, ebp
pop ebp
ret
