#include <inc/ctime.hpp>

using x86_64::rdtsc;

extern "C" clock_t clock() throw() {
	if (!CLOCKS_PER_SEC) {
		return -1;
	}
	return Timer::tsc_since_epoch();
}

extern "C" time_t time(time_t* timer) throw() {
	clock_t c = clock();
	if (c == -1) {
		return -1;
	}
	time_t ret = c / CLOCKS_PER_SEC;
	if (timer) {
		*timer = ret;
	}
	return ret;
}
