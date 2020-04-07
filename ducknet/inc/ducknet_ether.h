#ifndef DUCKNET_ETHER_H
#define DUCKNET_ETHER_H

#include <ducknet_types.h>
#include <ducknet_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUCKNET_ETHERTYPE_IPv4 0x0800
#define DUCKNET_ETHERTYPE_IPv6 0x86dd
#define DUCKNET_ETHERTYPE_ARP 0x0806

typedef struct {
	ducknet_u8 a[6];
} __attribute__((packed)) DucknetMACAddress;

typedef struct {
	DucknetMACAddress dst;
	DucknetMACAddress src;
	ducknet_u16 ethertype;
} __attribute__((packed)) DucknetEtherHeader;

typedef struct {
	DucknetMACAddress mac;
	int (*packet_handle)(DucknetEtherHeader *pkt, int content_len);
} DucknetEtherConfig;

int ducknet_ether_init(const DucknetEtherConfig *);

inline DucknetMACAddress ducknet_ether_broadcast_addr() {
	return (DucknetMACAddress) {{255, 255, 255, 255, 255, 255}};
}

int ducknet_ether_send(DucknetMACAddress dst, ducknet_u16 ethertype, const void *payload, int len);

void ducknet_ether_hton(DucknetEtherHeader *header);

int ducknet_ether_idle();
int ducknet_ether_packet_handle(void *pkt, int len);

#ifdef __cplusplus
}
#endif

#endif
