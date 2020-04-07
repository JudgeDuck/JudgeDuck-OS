#include <ducknet_udp.h>
#include <ducknet_ipv4.h>

#include "ducknet_impl.h"

static int (*packet_handle)(DucknetIPv4Address, DucknetIPv4Address, DucknetUDPHeader *, int);

int ducknet_udp_init(const DucknetUDPConfig *conf) {
	packet_handle = conf->packet_handle;
	return 0;
}

void ducknet_udp_hton(DucknetUDPHeader *hdr) {
	hdr->sport = ducknet_htons(hdr->sport);
	hdr->dport = ducknet_htons(hdr->dport);
	hdr->length = ducknet_htons(hdr->length);
}

int ducknet_udp_send(DucknetIPv4Address dst, ducknet_u16 dport, ducknet_u16 sport, const void *payload, int len) {
	if (len < 0) {
		return -1;
	}
	if (len + (int) sizeof(DucknetUDPHeader) + (int) sizeof(DucknetIPv4Header) > ducknet_MTU) {
		return -1;
	}
	static char buf[MAX_MTU + 64];
	DucknetUDPHeader *hdr = (DucknetUDPHeader *) buf;
	hdr->sport = sport;
	hdr->dport = dport;
	hdr->length = len + sizeof(DucknetUDPHeader);
	hdr->checksum = 0;
	ducknet_udp_hton(hdr);
	memcpy(hdr + 1, payload, len);
	
	struct {
		DucknetIPv4Address src, dst;
		ducknet_u8 zeros;
		ducknet_u8 protocol;
		ducknet_u16 length;
	} __attribute__((packed)) ph;
	
	int udp_len = len + sizeof(DucknetUDPHeader);
	
	ph.src.addr = ducknet_htonl(ducknet_ip.addr);
	ph.dst.addr = ducknet_htonl(dst.addr);
	ph.zeros = 0;
	ph.protocol = DUCKNET_IPv4_UDP;
	ph.length = ducknet_htons(udp_len);
	
	ducknet_u32 tmp_sum = ducknet_checksum_sum(&ph, sizeof(ph));
	hdr->checksum = ducknet_checksum(hdr, udp_len, tmp_sum);
	
	return ducknet_ipv4_send(dst, DUCKNET_IPv4_UDP, hdr, udp_len);
}

int ducknet_udp_idle() {
	return 0;
}

int ducknet_udp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, void *pkt, int len) {
	if (len < (int) sizeof(DucknetUDPHeader)) return -1;
	DucknetUDPHeader *hdr = (DucknetUDPHeader *) pkt;
	ducknet_udp_hton(hdr);
	int r;
	if (packet_handle && ((r = packet_handle(src, dst, hdr, len - (int) sizeof(DucknetUDPHeader))) < 0)) {
		return r;
	}
	return 0;
}
