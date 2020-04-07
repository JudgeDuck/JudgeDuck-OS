#ifndef DUCKNET_ICMP_H
#define DUCKNET_ICMP_H

#include <ducknet_types.h>
#include <ducknet_utils.h>

#include <ducknet_ipv4.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUCKNET_ICMP_ECHO_REPLY 0
#define DUCKNET_ICMP_ECHO_REQUEST 8

typedef struct {
	ducknet_u8 type;
	ducknet_u8 code;
	ducknet_u16 checksum;
	ducknet_u32 rest;
} __attribute__((packed)) DucknetICMPHeader;

typedef struct {
	int (*packet_handle)(DucknetIPv4Address src, DucknetIPv4Address dst, DucknetICMPHeader *, int);
} DucknetICMPConfig;

int ducknet_icmp_init(const DucknetICMPConfig *);

void ducknet_icmp_hton(DucknetIPv4Header *);

int ducknet_icmp_idle();
int ducknet_icmp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, void *pkt, int len);

#ifdef __cplusplus
}
#endif

#endif
