#include <inc/vga_buffer.h>

using vga_buffer::writer;

static void call_init_array() {
	extern void (*const __init_array_start)(void), (*const __init_array_end)(void);
	
	uintptr_t a = (uintptr_t) &__init_array_start;
	for (; a < (uintptr_t) &__init_array_end; a += sizeof(void(*)()))
		(*(void (**)(void))a)();
}

extern "C"
void kern_main() {
	// ATTENTION: we have a very small stack and no guard page
	
	call_init_array();
	
	const char *hello = "Hello World!";
	const char color_byte = 0x1f;
	
	char hello_colored[24];
	for (int i = 0; i < 12; i++) {
		hello_colored[i * 2] = hello[i];
		hello_colored[i * 2 - 1] = color_byte;
	}
	
	// write `Hello World!` to the center of the VGA text buffer
	// TODO: import memcpy from libc
	char *buffer_ptr = (char *) (0xb8000 + 1988);
	for (int i = 0; i < 24; i++) {
		buffer_ptr[i] = hello_colored[i];
	}
	
	writer->write_str("Hello\n");
	writer->write_str("World\n");
	
	while (1);
}
