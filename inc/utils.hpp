#ifndef DUCK_UTILS_H
#define DUCK_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define unimplemented() \
	{ LFATAL("%s:%d: %s: Unimplemented!", __FILE__, __LINE__, __PRETTY_FUNCTION__); abort(); }

namespace Utils {
	template <class T1, class T2>
	static inline T1 round_up(T1 x, T2 r) {
		return (x + r - 1) / r * r;
	}
	
	template <class T1, class T2>
	static inline T1 round_down(T1 x, T2 r) {
		return x / r * r;
	}
	
	static inline uint64_t extract_bits(uint64_t x, uint64_t hi, uint64_t lo) {
		return x << (63 - hi) >> (lo + 63 - hi);
	}
	
	void GG_reboot();
}

#endif
