// #include <sys/auxv.h>

.global kern_loader
kern_loader:
	pushq $0  # auxv end
	pushq $0  # envp end
	pushq $0  # argv end
	pushq $0  # argc
	
	jmp _start
