#ifndef __DUCKNET_IMPL_H__
#define __DUCKNET_IMPL_H__

#include <ducknet_types.h>
#include <ducknet_ether.h>
#include <ducknet_ipv4.h>

#ifdef __cplusplus
extern "C" {
#endif

// === protocols ===

const int MAX_MTU = 1500;

extern char ducknet_sendbuf[MAX_MTU + 64];

extern DucknetMACAddress ducknet_mac;
extern int ducknet_MTU;

extern DucknetIPv4Address ducknet_ip;

// === time ===

extern ducknet_time_t ducknet_currenttime;
extern ducknet_u64 ducknet_tsc_freq;

// === string operations ===

#include <string.h>

#ifdef __cplusplus
}
#endif

#endif
