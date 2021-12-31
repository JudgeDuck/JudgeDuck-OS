#ifndef DUCK_TIMER_H
#define DUCK_TIMER_H

#include <stdint.h>

#include <inc/x86_64.hpp>

namespace Timer {
	extern uint64_t tsc_freq, tsc_epoch;
	extern uint32_t ext_freq;
	extern uint64_t clk_freq;  // clk_thread Hz
	
	void init();
	
	static inline uint64_t get_tsc() {
		return x86_64::rdtsc();
	}
	
	static inline uint64_t tsc_to_ns(uint64_t tsc) {
		uint64_t q = tsc / tsc_freq;
		uint64_t r = tsc % tsc_freq;
		return q * 1000000000ull + r * 1000000000ull / tsc_freq;
	}
	
	static inline uint64_t clk_to_ns(uint64_t clk) {
		uint64_t q = clk / clk_freq;
		uint64_t r = clk % clk_freq;
		return q * 1000000000ull + r * 1000000000ull / clk_freq;
	}
	
	static inline double tsc_to_secf(uint64_t tsc) {
		return (double) tsc / tsc_freq;
	}
	
	static inline uint64_t tsc_since_epoch() {
		if (!tsc_epoch) return -1;
		return get_tsc() - tsc_epoch;
	}
	
	static inline double secf_since_epoch() {
		if (!tsc_epoch) return -1;
		return (double) tsc_since_epoch() / tsc_freq;
	}
	
	static inline uint64_t ns_since_epoch() {
		if (!tsc_epoch) return -1;
		return tsc_to_ns(tsc_since_epoch());
	}
	
	static inline void microdelay(uint64_t us) {
		uint64_t tsc_target = get_tsc() + us * (tsc_freq / 1000000);
		while ((long long) (get_tsc() - tsc_target) < 0ll);  // prevent overflow
	}
	
	void powersave_sleep(uint64_t ns);
	
	void reset_performance_counters();
	void read_performance_counters(uint64_t &inst,
		uint64_t &clk_thread, uint64_t &clk_ref_tsc);
}

#endif
