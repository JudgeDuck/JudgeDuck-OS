#include <inc/time.hpp>
#include <inc/x86_64.hpp>
#include <inc/smbios.hpp>

using x86_64::rdtsc;

static uint64_t tsc_per_sec;
static uint64_t tsc_at_set_tsc_per_sec;

namespace Time {
    void set_tsc_per_sec(uint64_t new_tsc_per_sec) {
        tsc_per_sec = new_tsc_per_sec;
        tsc_at_set_tsc_per_sec = rdtsc();
    }
    clock_t clocks_per_sec() {
        return tsc_per_sec;
    }
    double timef() {
        if (!tsc_per_sec)
            return -1;
        return (double) (rdtsc() - tsc_at_set_tsc_per_sec) / tsc_per_sec;
    }
}
extern "C" clock_t clock() throw() {
    if (!tsc_per_sec)
        return -1;
    return rdtsc() - tsc_at_set_tsc_per_sec;
}
extern "C" time_t time(time_t* timer) throw() {
    clock_t c = clock();
    if (c == -1)
        return -1;
    time_t ret = c / CLOCKS_PER_SEC;
    if (timer)
        *timer = ret;
    return ret;
}