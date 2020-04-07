#ifndef DUCKNET_UDP_H
#define DUCKNET_UDP_H

#include <ducknet_types.h>
#include <ducknet_utils.h>

#include <ducknet_ipv4.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	ducknet_u16 sport, dport;
	ducknet_u16 length;
	ducknet_u16 checksum;
} __attribute__((packed)) DucknetUDPHeader;

typedef struct {
	int (*packet_handle)(DucknetIPv4Address src, DucknetIPv4Address dst, DucknetUDPHeader *, int);
} DucknetUDPConfig;

int ducknet_udp_init(const DucknetUDPConfig *);

void ducknet_udp_hton(DucknetUDPHeader *);

int ducknet_udp_send(DucknetIPv4Address dst, ducknet_u16 dport, ducknet_u16 sport, const void *payload, int len);

int ducknet_udp_idle();
int ducknet_udp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, void *pkt, int len);

#ifdef __cplusplus
}
#endif

#endif
