global long_mode_start

section .text
bits 64
long_mode_start:
	; load 0 into all data segment registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call enable_sse

    extern kern_main
    call kern_main

    hlt

enable_sse:
    ; clear the CR0.EM bit (bit 2)
    mov rax, cr0
    and rax, ~(1 << 2)
    mov cr0, rax

    ; set the CR0.MP bit (bit 1)
    mov rax, cr0
    or rax, 1 << 1
    mov cr0, rax

    ; set the CR4.OSFXSR bit (bit 9)
    mov rax, cr4
    or rax, 1 << 9
    mov cr4, rax

    ; set the CR4.OSXMMEXCPT bit (bit 10)
    mov rax, cr4
    or rax, 1 << 10
    mov cr4, rax
    
    ; set the CR4.OSXSAVE bit (bit 18)
    mov rax, cr4
    or rax, 1 << 18
    mov cr4, rax
    
    push rax
    push rcx
    xor rcx, rcx
    xgetbv  ; Load XCR0 register
    or rax, 7  ; Set AVX, SSE, X87 bits
    xsetbv  ; Save back to XCR0
    pop rcx
    pop rax

    ret
