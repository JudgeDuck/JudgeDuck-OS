.global __duck32__syscall
__duck32__syscall:
	# backup caller-saved registers
	pushl %edx
	pushl %ecx
	
	# put arguments onto the stack
	pushl %eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	
	call __duck32__syscall_handler
	
	# restore stack
	addl $28, %esp
	
	# restore caller-saved registers
	popl %ecx
	popl %edx
	
	ret
