#ifndef DUCK_UTILS_H
#define DUCK_UTILS_H

namespace Utils {
	template <class T1, class T2>
	inline T1 round_up(T1 x, T2 r) {
		return (x + r - 1) / r * r;
	}
	
	template <class T1, class T2>
	inline T1 round_down(T1 x, T2 r) {
		return x / r * r;
	}
}

#endif
