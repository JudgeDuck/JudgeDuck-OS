#ifndef TLS_H
#define TLS_H

namespace TLS {
	static inline void set_fs(void *p) {
		__asm__ volatile ("wrfsbase %0" : : "r" (p));
	}
}

#endif
