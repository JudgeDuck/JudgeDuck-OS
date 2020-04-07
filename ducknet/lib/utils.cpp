#include <ducknet_utils.h>

#include "ducknet_impl.h"

static ducknet_u64 tsc_freq;

int ducknet_utils_init(const DucknetUtilsConfig *conf) {
	if (conf->tsc_freq == 0) {
		return -1;
	}
	tsc_freq = conf->tsc_freq;
	ducknet_tsc_freq = tsc_freq;
	ducknet_currenttime = ducknet_gettime();
	return 0;
}

int ducknet_utils_idle() {
	ducknet_currenttime = ducknet_gettime();
	return 0;
}

ducknet_time_t ducknet_gettime() {
	ducknet_u64 ret;
	__asm__ volatile ("rdtsc" : "=A"(ret));
	return ret;
}

static inline ducknet_u64 mul_div(ducknet_u64 a, ducknet_u64 b, ducknet_u64 d) {
	ducknet_u64 x = a / d;
	ducknet_u64 y = a % d;
	ducknet_u64 u = b / d;
	ducknet_u64 v = b % d;
	return x * u * d + x * v + y * u + y * v / d;
}

ducknet_time_t ducknet_time_add_ns(ducknet_time_t t, ducknet_u64 ns) {
	return t + mul_div(ns, tsc_freq, 1000000000u);
}

ducknet_time_t ducknet_time_add_us(ducknet_time_t t, ducknet_u64 us) {
	return t + mul_div(us, tsc_freq, 1000000u);
}

ducknet_time_t ducknet_time_add_ms(ducknet_time_t t, ducknet_u64 ms) {
	return t + mul_div(ms, tsc_freq, 1000u);
}

ducknet_time_t ducknet_time_add_sec(ducknet_time_t t, ducknet_u64 sec) {
	return t + sec * tsc_freq;
}

ducknet_u64 ducknet_time_to_ns(ducknet_time_t t) {
	return mul_div(t, 1000000000u, tsc_freq);
}

ducknet_u64 ducknet_time_to_us(ducknet_time_t t) {
	return mul_div(t, 1000000u, tsc_freq);
}

ducknet_u64 ducknet_time_to_ms(ducknet_time_t t) {
	return mul_div(t, 1000u, tsc_freq);
}

ducknet_u64 ducknet_time_to_sec(ducknet_time_t t) {
	return t / tsc_freq;
}

ducknet_u32 ducknet_checksum_sum(const void *a, int count) {
	ducknet_u32 sum = 0;
	
	while (count > 1) {
		sum += *(ducknet_u16 *) a;
		a = ((char *) a) + 2;
		count -= 2;
	}
	
	if (count) {
		sum += *(ducknet_u8 *) a;
	}
	
	return sum;
}

ducknet_u16 ducknet_checksum(const void *a, int count, ducknet_u32 sum) {
	sum += ducknet_checksum_sum(a, count);
	
	while (sum >> 16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}
	
	return ~sum;
}
