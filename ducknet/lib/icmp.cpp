#include <ducknet_icmp.h>
#include <ducknet_ipv4.h>

#include "ducknet_impl.h"

static int (*packet_handle)(DucknetIPv4Address, DucknetIPv4Address, DucknetICMPHeader *, int);

int ducknet_icmp_init(const DucknetICMPConfig *conf) {
	packet_handle = conf->packet_handle;
	return 0;
}

void ducknet_icmp_hton(DucknetICMPHeader *) {
	// Do nothing
}

int ducknet_icmp_idle() {
	return 0;
}

int ducknet_icmp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, void *pkt, int len) {
	if (len < (int) sizeof(DucknetICMPHeader)) return -1;
	DucknetICMPHeader *hdr = (DucknetICMPHeader *) pkt;
	ducknet_icmp_hton(hdr);
	int r;
	if (packet_handle && ((r = packet_handle(src, dst, hdr, len - (int) sizeof(DucknetICMPHeader))) < 0)) {
		return r;
	}
	switch (hdr->type) {
		case DUCKNET_ICMP_ECHO_REQUEST:
			if (hdr->code != 0) {
				return -1;
			}
			hdr->type = DUCKNET_ICMP_ECHO_REPLY;
			hdr->checksum = 0;
			hdr->checksum = ducknet_checksum(hdr, len, 0);
			ducknet_ipv4_send(src, DUCKNET_IPv4_ICMP, hdr, len);
			return 0;
		default:
			return -1;
	}
}
