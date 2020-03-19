#ifndef DUCK_X86_64_H
#define DUCK_X86_64_H

#include <stdint.h>
#include <x86intrin.h>

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
		return __rdtsc();
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
	
	static inline uint64_t read_rflags() {
		uint64_t rflags;
		__asm__ volatile ("pushfq; popq %0" : "=r" (rflags));
		return rflags;
	}
}

#endif
