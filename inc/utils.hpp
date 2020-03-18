#ifndef DUCK_UTILS_H
#define DUCK_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define unimplemented() \
	{ fprintf(stderr, "%s:%d: %s: Unimplemented!\n", __FILE__, __LINE__, __func__); abort(); }

namespace Utils {
	template <class T1, class T2>
	static inline T1 round_up(T1 x, T2 r) {
		return (x + r - 1) / r * r;
	}
	
	template <class T1, class T2>
	static inline T1 round_down(T1 x, T2 r) {
		return x / r * r;
	}
}

#endif
