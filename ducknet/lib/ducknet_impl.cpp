#include "ducknet_impl.h"

char ducknet_sendbuf[MAX_MTU + 64];

ducknet_time_t ducknet_currenttime;
ducknet_u64 ducknet_tsc_freq;
