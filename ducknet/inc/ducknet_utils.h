#ifndef DUCKNET_UTILS_H
#define DUCKNET_UTILS_H

#include <ducknet_types.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	ducknet_u64 tsc_freq;
} DucknetUtilsConfig;

int ducknet_utils_init(const DucknetUtilsConfig *);

int ducknet_utils_idle();


// === time ===

ducknet_time_t ducknet_gettime();
ducknet_time_t ducknet_time_add_ns(ducknet_time_t t, ducknet_u64 ns);
ducknet_time_t ducknet_time_add_us(ducknet_time_t t, ducknet_u64 us);
ducknet_time_t ducknet_time_add_ms(ducknet_time_t t, ducknet_u64 ms);
ducknet_time_t ducknet_time_add_sec(ducknet_time_t t, ducknet_u64 sec);
ducknet_u64 ducknet_time_to_ns(ducknet_time_t);
ducknet_u64 ducknet_time_to_us(ducknet_time_t);
ducknet_u64 ducknet_time_to_ms(ducknet_time_t);
ducknet_u64 ducknet_time_to_sec(ducknet_time_t);

// === htons ===

inline ducknet_u16 ducknet_htons(ducknet_u16 x) {
	return
		((x & 0xff00u) >> 8) |
		((x & 0x00ffu) << 8);
}

inline ducknet_u32 ducknet_htonl(ducknet_u32 x) {
	return
		((x & 0xff000000u) >> 24) |
		((x & 0x00ff0000u) >> 8) |
		((x & 0x0000ff00u) << 8) |
		((x & 0x000000ffu) << 24);
}

inline ducknet_u64 ducknet_htonll(ducknet_u64 x) {
	return
		((x & 0xff00000000000000u) >> 56) |
		((x & 0x00ff000000000000u) >> 40) |
		((x & 0x0000ff0000000000u) >> 24) |
		((x & 0x000000ff00000000u) >> 8) |
		((x & 0x00000000ff000000u) << 8) |
		((x & 0x0000000000ff0000u) << 24) |
		((x & 0x000000000000ff00u) << 40) |
		((x & 0x00000000000000ffu) << 56);
}

// === others ===

ducknet_u16 ducknet_checksum(const void *a, int count, ducknet_u32 start_sum);
ducknet_u32 ducknet_checksum_sum(const void *a, int count);


#ifdef __cplusplus
}
#endif

#endif
