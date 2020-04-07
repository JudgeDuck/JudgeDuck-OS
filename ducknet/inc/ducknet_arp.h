#ifndef DUCKNET_ARP_H
#define DUCKNET_ARP_H

#include <ducknet_types.h>
#include <ducknet_utils.h>

#include <ducknet_ether.h>
#include <ducknet_ipv4.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUCKNET_HTYPE_ETHERNET 0x1
#define DUCKNET_PTYPE_IPv4 0x0800

#define DUCKNET_ARP_REQUEST 1
#define DUCKNET_ARP_REPLY 2

typedef struct {
	ducknet_u16 htype;
	ducknet_u16 ptype;
	ducknet_u8 hlen;
	ducknet_u8 plen;
	ducknet_u16 op;
} __attribute__((packed)) DucknetARPHeader;

typedef struct {
	int (*packet_handle)(DucknetARPHeader *, int);
} DucknetARPConfig;

int ducknet_arp_init(const DucknetARPConfig *);

void ducknet_arp_hton(DucknetARPHeader *);

int ducknet_arp_send(DucknetMACAddress dst, ducknet_u16 op, const void *payload, int len);

int ducknet_arp_idle();
int ducknet_arp_packet_handle(void *pkt, int len);

int ducknet_arp_update(DucknetIPv4Address, DucknetMACAddress);
int ducknet_arp_lookup(DucknetIPv4Address, DucknetMACAddress *);
int ducknet_arp_query(DucknetIPv4Address);

#ifdef __cplusplus
}
#endif

#endif
