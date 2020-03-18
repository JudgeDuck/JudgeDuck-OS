#ifndef DUCK_TIMER_H
#define DUCK_TIMER_H

#include <inc/x86_64.hpp>

namespace Timer {
	extern uint64_t tsc_freq, tsc_epoch;
	extern uint32_t ext_freq;
	
	void init();
	
	static inline uint64_t get_tsc() {
		return x86_64::rdtsc();
	}
	
	static inline uint64_t tsc_to_ns(uint64_t tsc) {
		uint64_t q = tsc / tsc_freq;
		uint64_t r = tsc % tsc_freq;
		return q * 1000000000ull + r * 1000000000ull / tsc_freq;
	}
	
	static inline uint64_t tsc_since_epoch() {
		if (!tsc_epoch) return -1;
		return get_tsc() - tsc_epoch;
	}
	
	static inline double secf_since_epoch() {
		if (!tsc_epoch) return -1;
		return (double) tsc_since_epoch() / tsc_freq;
	}
}

#endif
