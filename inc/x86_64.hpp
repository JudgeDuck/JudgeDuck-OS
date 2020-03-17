#ifndef DUCK_X86_64_H
#define DUCK_X86_64_H

#include <stdint.h>

namespace x86_64 {
	inline void outb(int port, uint8_t data) {
		__asm__ volatile ("outb %0, %w1" : : "a" (data), "d" (port));
	}
	
	inline uint8_t inb(int port) {
		uint8_t data;
		__asm__ volatile ("inb %w1, %0" : "=a" (data) : "d" (port));
		return data;
	}
	
	inline void lidt(void *p) {
		__asm__ volatile ("lidt (%0)" : : "r" (p));
	}
	
	inline void lgdt(void *p) {
		__asm__ volatile ("lgdt (%0)" : : "r" (p));
	}
	
	inline void ltr(uint16_t sel) {
		__asm__ volatile ("ltr %0" : : "r" (sel));
	}
	
	inline void sti() {
		__asm__ volatile ("sti");
	}
	
	inline void cli() {
		__asm__ volatile ("cli");
	}
}

#endif
