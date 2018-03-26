.text
.globl read_eip
read_eip:
	popl %eax
	jmp *%eax