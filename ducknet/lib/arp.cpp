#include <ducknet_arp.h>
#include <ducknet_ether.h>
#include <ducknet_phy.h>
#include <ducknet_ipv4.h>
#include <ducknet_utils.h>

#include "ducknet_impl.h"

static int (*packet_handle)(DucknetARPHeader *, int);

const int ARP_TABLE_SIZE = 100;
const int ARP_EXPIRE_TIMEOUT_MS = 30000;
const int ARP_QUERY_TIMEOUT_MS = 1000;

static struct {
	DucknetIPv4Address ip;
	DucknetMACAddress mac;
	ducknet_time_t expire_time;
} arp_table[ARP_TABLE_SIZE];

static int n_arp_entries;

static struct {
	DucknetIPv4Address ip;
	ducknet_time_t expire_time;
} arp_query_table[ARP_TABLE_SIZE];

static int n_arp_queries;

static ducknet_time_t last_idle_time;
static ducknet_u64 idle_delay;

int ducknet_arp_init(const DucknetARPConfig *conf) {
	packet_handle = conf->packet_handle;
	n_arp_entries = 0;
	n_arp_queries = 0;
	
	last_idle_time = ducknet_currenttime;
	idle_delay = ducknet_tsc_freq * 100 / 1000 / 1000;  // 0.1ms
	
	return 0;
}

void ducknet_arp_hton(DucknetARPHeader *hdr) {
	hdr->htype = ducknet_htons(hdr->htype);
	hdr->ptype = ducknet_htons(hdr->ptype);
	hdr->op = ducknet_htons(hdr->op);
}

int ducknet_arp_send(DucknetMACAddress dst, ducknet_u16 op, const void *payload, int len) {
	if (len < 0) {
		return -1;
	}
	if (len + (int) sizeof(DucknetARPHeader) > ducknet_MTU) {
		return -1;
	}
	DucknetEtherHeader *eth_hdr = (DucknetEtherHeader *) ducknet_sendbuf;
	eth_hdr->dst = dst;
	eth_hdr->src = ducknet_mac;
	eth_hdr->ethertype = ducknet_htons(DUCKNET_ETHERTYPE_ARP);
	DucknetARPHeader *arp_hdr = (DucknetARPHeader *) (eth_hdr + 1);
	arp_hdr->htype = ducknet_htons(DUCKNET_HTYPE_ETHERNET);
	arp_hdr->ptype = ducknet_htons(DUCKNET_PTYPE_IPv4);
	arp_hdr->hlen = 6;
	arp_hdr->plen = 4;
	arp_hdr->op = ducknet_htons(op);
	memcpy(arp_hdr + 1, payload, len);
	return ducknet_phy_send(eth_hdr, len + sizeof(DucknetARPHeader) + sizeof(DucknetEtherHeader));
}

int ducknet_arp_idle() {
	static int cnt = 0;
	if (++cnt < 128 && ducknet_currenttime - last_idle_time <= idle_delay) {
		return 0;
	}
	cnt = 0;
	last_idle_time = ducknet_currenttime;
	
	int id;
	
	id = 0;
	for (int i = 0; i < n_arp_entries; i++) {
		if (ducknet_currenttime > arp_table[i].expire_time) {
			continue;
		}
		arp_table[id++] = arp_table[i];
	}
	n_arp_entries = id;
	
	id = 0;
	for (int i = 0; i < n_arp_queries; i++) {
		if (ducknet_currenttime > arp_query_table[i].expire_time) {
			continue;
		}
		arp_query_table[id++] = arp_query_table[i];
	}
	n_arp_queries = id;
	
	return 0;
}

int ducknet_arp_packet_handle(void *pkt, int len) {
	if (len < (int) sizeof(DucknetARPHeader)) return -1;
	DucknetARPHeader *hdr = (DucknetARPHeader *) pkt;
	ducknet_arp_hton(hdr);
	int r;
	if (packet_handle && ((r = packet_handle(hdr, len - (int) sizeof(DucknetARPHeader))) < 0)) {
		return r;
	}
	if (hdr->htype == DUCKNET_HTYPE_ETHERNET && hdr->ptype == DUCKNET_PTYPE_IPv4) {
		if (hdr->hlen != 6) return -1;
		if (hdr->plen != 4) return -1;
		
		typedef struct {
			DucknetMACAddress sha;
			DucknetIPv4Address spa;
			DucknetMACAddress tha;
			DucknetIPv4Address tpa;
		} __attribute__((packed)) Payload;
		Payload payload, payload2;
		
		if (len - (int) sizeof(DucknetARPHeader) < (int) sizeof(payload)) {
			return -1;
		}
		memcpy(&payload, hdr + 1, sizeof(payload));
		payload.spa.addr = ducknet_htonl(payload.spa.addr);
		payload.tpa.addr = ducknet_htonl(payload.tpa.addr);
		
		if (hdr->op == DUCKNET_ARP_REQUEST) {
			if (payload.tpa.addr == ducknet_ip.addr) {
				payload2.tha = payload.sha;
				payload2.tpa.addr = ducknet_htonl(payload.spa.addr);
				payload2.sha = ducknet_mac;
				payload2.spa.addr = ducknet_htonl(payload.tpa.addr);
				ducknet_arp_send(payload.sha, DUCKNET_ARP_REPLY, &payload2, sizeof(payload2));
				return 0;
			} else {
				return -1;
			}
		} else if (hdr->op == DUCKNET_ARP_REPLY) {
			ducknet_arp_update(payload.spa, payload.sha);
			return 0;
		}
	}
	return -1;
}

int ducknet_arp_update(DucknetIPv4Address ip, DucknetMACAddress mac) {
	int l = 0, r = n_arp_entries - 1;
	while (l < r) {
		int mid = (l + r) >> 1;
		if (arp_table[mid].ip.addr < ip.addr) {
			l = mid + 1;
		} else {
			r = mid;
		}
	}
	if (l >= n_arp_entries || arp_table[l].ip.addr != ip.addr) {
		if (n_arp_entries == ARP_TABLE_SIZE) {
			return -1;
		}
		memmove(arp_table + (l + 1), arp_table + l, (n_arp_entries - l) * sizeof(arp_table[0]));
		++n_arp_entries;
	}
	arp_table[l] = {
		.ip = ip,
		.mac = mac,
		.expire_time = ducknet_time_add_ms(ducknet_currenttime, ARP_EXPIRE_TIMEOUT_MS)
	};
	return 0;
}

int ducknet_arp_lookup(DucknetIPv4Address ip, DucknetMACAddress *mac) {
	int l = 0, r = n_arp_entries - 1;
	while (l < r) {
		int mid = (l + r) >> 1;
		if (arp_table[mid].ip.addr < ip.addr) {
			l = mid + 1;
		} else {
			r = mid;
		}
	}
	if (l >= n_arp_entries || arp_table[l].ip.addr != ip.addr) {
		return -1;
	}
	*mac = arp_table[l].mac;
	return 0;
}

int ducknet_arp_query(DucknetIPv4Address ip) {
	int l = 0, r = n_arp_queries - 1;
	while (l < r) {
		int mid = (l + r) >> 1;
		if (arp_query_table[mid].ip.addr < ip.addr) {
			l = mid + 1;
		} else {
			r = mid;
		}
	}
	if (l >= n_arp_queries || arp_query_table[l].ip.addr != ip.addr) {
		if (n_arp_queries == ARP_TABLE_SIZE) {
			return -1;
		}
		memmove(arp_query_table + (l + 1), arp_query_table + l, (n_arp_queries - l) * sizeof(arp_query_table[0]));
		++n_arp_queries;
		arp_query_table[l] = {
			.ip = ip,
			.expire_time = ducknet_time_add_ms(ducknet_currenttime, ARP_QUERY_TIMEOUT_MS)
		};
		
		struct {
			DucknetMACAddress sha;
			DucknetIPv4Address spa;
			DucknetMACAddress tha;
			DucknetIPv4Address tpa;
		} __attribute__((packed)) payload;
		
		payload.sha = ducknet_mac;
		payload.spa.addr = ducknet_htonl(ducknet_ip.addr);
		memset(&payload.tha, 0, sizeof(DucknetMACAddress));
		payload.tpa.addr = ducknet_htonl(ip.addr);
		
		return ducknet_arp_send(ducknet_ether_broadcast_addr(), DUCKNET_ARP_REQUEST, &payload, sizeof(payload));
	}
	return 0;  // Already in query table
}
