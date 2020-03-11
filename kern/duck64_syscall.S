.global __duck64__syscall
__duck64__syscall:
	# callee-saved: r12-r15, rbp, rbx
	# caller-saved: rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11
	# syscall-abi-in: [store rflags in r11; store rip in rcx]
	# syscall-abi-out: destroys rax, rcx, r11; [restore rflags in r11; return to rcx]
	# function call => return address is already on stack
	
	# caller-saved registers [for calling handler]
	pushq %rdx
	pushq %rsi
	pushq %rdi
	pushq %r8
	pushq %r9
	pushq %r10
	
	pushq %rax  # syscall number
	movq %r10, %rcx   # the fourth argument
	
	call __duck64__syscall_handler
	
	popq %r11   # pop syscall number (garbage)
	
	# caller-saved registers [restoring]
	popq %r10
	popq %r9
	popq %r8
	popq %rdi
	popq %rsi
	popq %rdx
	
	ret
