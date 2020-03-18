#ifndef DUCK_X86_64_H
#define DUCK_X86_64_H

#include <stdint.h>

namespace x86_64 {
	static inline void outb(int port, uint8_t data) {
		__asm__ volatile ("outb %0, %w1" : : "a" (data), "d" (port));
	}
	
	static inline uint8_t inb(int port) {
		uint8_t data;
		__asm__ volatile ("inb %w1, %0" : "=a" (data) : "d" (port));
		return data;
	}
	
	static inline void lidt(void *p) {
		__asm__ volatile ("lidt (%0)" : : "r" (p));
	}
	
	static inline void lgdt(void *p) {
		__asm__ volatile ("lgdt (%0)" : : "r" (p));
	}
	
	static inline void ltr(uint16_t sel) {
		__asm__ volatile ("ltr %0" : : "r" (sel));
	}
	
	static inline void sti() {
		__asm__ volatile ("sti");
	}
	
	static inline void cli() {
		__asm__ volatile ("cli");
	}
	
	static inline uint64_t rdtsc() {
		uint32_t eax, edx;
		__asm__ volatile ("rdtsc" : "=a" (eax), "=d" (edx));
		return ((uint64_t) edx << 32) | eax;
	}
	
	static inline void cpuid(uint32_t info,
		uint32_t *eaxp, uint32_t *ebxp, uint32_t *ecxp, uint32_t *edxp) {
		uint32_t eax, ebx, ecx, edx;
		asm volatile("cpuid"
			: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
			: "a" (info));
		if (eaxp) *eaxp = eax;
		if (ebxp) *ebxp = ebx;
		if (ecxp) *ecxp = ecx;
		if (edxp) *edxp = edx;
	}
}

#endif
