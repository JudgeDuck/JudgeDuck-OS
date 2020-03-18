#ifndef DUCK_TIME_H
#define DUCK_TIME_H

#include <inc/timer.hpp>

extern "C" time_t time(time_t *) throw();
extern "C" clock_t clock() throw();

#ifdef CLOCKS_PER_SEC
#undef CLOCKS_PER_SEC
#endif

#define CLOCKS_PER_SEC Timer::tsc_freq

#endif // !DUCK_TIME_H
