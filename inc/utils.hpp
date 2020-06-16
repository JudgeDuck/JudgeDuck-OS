#ifndef DUCK_UTILS_H
#define DUCK_UTILS_H

#include <stdio.h>
#include <stdlib.h>

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
	
	void GG_reboot();
}

#endif
