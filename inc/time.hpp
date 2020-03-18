#ifndef DUCK_TIME_H
#define DUCK_TIME_H

#include <inc/x86_64.hpp>

// libc compatible
extern "C" time_t time(time_t *) throw();
extern "C" clock_t clock() throw();
#undef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC Time::clocks_per_sec()

namespace Time {
    void set_tsc_per_sec(uint64_t);
    clock_t clocks_per_sec();
    double timef();
}

#endif // !DUCK_TIME_H