#ifndef DUCKNET_IPv4_H
#define DUCKNET_IPv4_H

#include <ducknet_types.h>
#include <ducknet_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUCKNET_IPv4_ICMP 1
#define DUCKNET_IPv4_TCP 6
#define DUCKNET_IPv4_UDP 17

#define DUCKNET_IPv4_FLAGS_DF 0x4000

typedef struct {
	union {
		ducknet_u32 addr;
		ducknet_u8 a[4];
	};
} __attribute__((packed)) DucknetIPv4Address;

typedef struct {
	ducknet_u8 version : 4;
	ducknet_u8 ihl : 4;
	ducknet_u8 tos;
	ducknet_u16 length;
	ducknet_u16 id;
	ducknet_u16 flags;  // TODO support fragmentation
	ducknet_u8 ttl, protocol;
	ducknet_u16 checksum;
	DucknetIPv4Address src, dst;
} __attribute__((packed)) DucknetIPv4Header;

typedef struct {
	DucknetIPv4Address ip;
	int (*packet_handle)(DucknetIPv4Header *, int);
} DucknetIPv4Config;

int ducknet_ipv4_init(const DucknetIPv4Config *);

void ducknet_ipv4_hton(DucknetIPv4Header *);

int ducknet_ipv4_send(DucknetIPv4Address dst, ducknet_u8 protocol, const void *payload, int len);

int ducknet_ipv4_idle();
int ducknet_ipv4_packet_handle(void *pkt, int len);

int ducknet_parse_ipv4(const char *s, DucknetIPv4Address *);

#ifdef __cplusplus
}
#endif

#endif
