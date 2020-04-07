#include <ducknet_ether.h>
#include <ducknet_phy.h>
#include <ducknet_arp.h>

#include "ducknet_impl.h"

DucknetMACAddress ducknet_mac;
int ducknet_MTU;

static int (*packet_handle)(DucknetEtherHeader *, int);

int ducknet_ether_init(const DucknetEtherConfig *conf) {
	ducknet_mac = conf->mac;
	ducknet_MTU = 1500;  // ???
	packet_handle = conf->packet_handle;
	return 0;
}

int ducknet_ether_send(DucknetMACAddress dst, ducknet_u16 ethertype, const void *payload, int len) {
	if (len < 0) {
		return -1;
	}
	if (len > ducknet_MTU) {
		return -1;
	}
	DucknetEtherHeader *hdr = (DucknetEtherHeader *) ducknet_sendbuf;
	hdr->dst = dst;
	hdr->src = ducknet_mac;
	hdr->ethertype = ducknet_htons(ethertype);
	memcpy(hdr + 1, payload, len);
	return ducknet_phy_send(hdr, len + sizeof(DucknetEtherHeader));
}

void ducknet_ether_hton(DucknetEtherHeader *hdr) {
	hdr->ethertype = ducknet_htons(hdr->ethertype);
}

static int is_equal(const DucknetMACAddress &a, const DucknetMACAddress &b) {
	return
		*(ducknet_u32 *) a.a == *(ducknet_u32 *) b.a &&
		*(ducknet_u16 *) (a.a + 4) == *(ducknet_u16 *) (b.a + 4);
}

int ducknet_ether_idle() {
	return 0;
}

int ducknet_ether_packet_handle(void *pkt, int len) {
	if (len < (int) sizeof(DucknetEtherHeader)) return -1;
	DucknetEtherHeader *hdr = (DucknetEtherHeader *) pkt;
	ducknet_ether_hton(hdr);
	int r;
	if (packet_handle && ((r = packet_handle(hdr, len - sizeof(DucknetEtherHeader))) < 0)) {
		return r;
	}
	if (!is_equal(hdr->dst, ducknet_mac) && !is_equal(hdr->dst, ducknet_ether_broadcast_addr())) {
		return -1;
	}
	int content_len = len - sizeof(DucknetEtherHeader);
	switch (hdr->ethertype) {
		case DUCKNET_ETHERTYPE_ARP:
			return ducknet_arp_packet_handle(hdr + 1, content_len);
		case DUCKNET_ETHERTYPE_IPv4:
			return ducknet_ipv4_packet_handle(hdr + 1, content_len);
		default:
			return -1;
	}
}
