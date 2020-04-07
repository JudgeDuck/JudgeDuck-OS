#ifndef DUCKNET_H
#define DUCKNET_H

#include <ducknet_types.h>
#include <ducknet_utils.h>

#include <ducknet_phy.h>
#include <ducknet_ether.h>
#include <ducknet_arp.h>
#include <ducknet_ipv4.h>
#include <ducknet_icmp.h>
#include <ducknet_udp.h>

#ifdef __cplusplus
extern "C" {
#endif

// ducknet: map Ethernet, ARP, IP, ICMP, UDP to low level interfaces

typedef struct {
	DucknetUtilsConfig utils;
	DucknetPhyConfig phy;
	DucknetEtherConfig ether;
	DucknetARPConfig arp;
	DucknetIPv4Config ipv4;
	DucknetICMPConfig icmp;
	DucknetUDPConfig udp;
	int (*idle)();
} DucknetConfig;

int ducknet_init(const DucknetConfig *);

int ducknet_flush();

int ducknet_mainloop();
int ducknet_step();
int ducknet_idle();
int ducknet_packet_handle(void *pkt, int len);

#ifdef __cplusplus
}
#endif

#endif
