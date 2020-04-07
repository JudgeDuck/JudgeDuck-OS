#include <ducknet_ipv4.h>
#include <ducknet_icmp.h>
#include <ducknet_udp.h>
#include <ducknet_ether.h>
#include <ducknet_arp.h>
#include <ducknet_phy.h>
#include <ducknet_utils.h>

#include "ducknet_impl.h"

static int (*packet_handle)(DucknetIPv4Header *, int);

DucknetIPv4Address ducknet_ip;

const int TOSEND_TABLE_SIZE = 64;
const int TOSEND_TIMEOUT_MS = 5000;

static struct {
	DucknetIPv4Address dst_ip;
	ducknet_time_t expire_time;
	int pkt_len;
	char pkt[MAX_MTU + 64];
} tosend_table[TOSEND_TABLE_SIZE];

static ducknet_u32 tosend_free[(TOSEND_TABLE_SIZE + 31) >> 5];

static ducknet_time_t last_idle_time;
static ducknet_u64 idle_delay;

int ducknet_ipv4_init(const DucknetIPv4Config *conf) {
	ducknet_ip = conf->ip;
	packet_handle = conf->packet_handle;
	
	memset(tosend_free, 0, sizeof(tosend_free));
	for (int i = 0; i < TOSEND_TABLE_SIZE; i++) {
		tosend_free[i >> 5] |= 1u << (i & 31);
	}
	
	last_idle_time = ducknet_currenttime;
	idle_delay = ducknet_tsc_freq * 100 / 1000 / 1000;  // 0.1ms
	
	return 0;
}

void ducknet_ipv4_hton(DucknetIPv4Header *hdr) {
	ducknet_u8 tmp = hdr->version;
	hdr->version = hdr->ihl;
	hdr->ihl = tmp;
	hdr->length = ducknet_htons(hdr->length);
	hdr->id = ducknet_htons(hdr->id);
	hdr->flags = ducknet_htons(hdr->flags);  // fragmentation NOT supported yet
	hdr->src.addr = ducknet_htonl(hdr->src.addr);
	hdr->dst.addr = ducknet_htonl(hdr->dst.addr);
}

static int ipv4_build_packet(void *pkt, DucknetIPv4Address dst, ducknet_u8 protocol, const void *payload, int len) {
	if (len < 0) {
		return -1;
	}
	if (len + (int) sizeof(DucknetIPv4Header) > ducknet_MTU) {
		return -1;
	}
	
	DucknetEtherHeader *eth_hdr = (DucknetEtherHeader *) pkt;
	// Not assigning eth dst
	eth_hdr->src = ducknet_mac;
	eth_hdr->ethertype = ducknet_htons(DUCKNET_ETHERTYPE_IPv4);
	
	DucknetIPv4Header *ipv4_hdr = (DucknetIPv4Header *) (eth_hdr + 1);
	ipv4_hdr->version = 4;
	ipv4_hdr->ihl = 5;
	ipv4_hdr->tos = 0;
	ipv4_hdr->length = len + sizeof(DucknetIPv4Header);
	ipv4_hdr->id = 2333;
	ipv4_hdr->flags = 0;
	ipv4_hdr->ttl = 233;
	ipv4_hdr->protocol = protocol;
	ipv4_hdr->src = ducknet_ip;
	ipv4_hdr->dst = dst;
	ipv4_hdr->checksum = 0;
	ducknet_ipv4_hton(ipv4_hdr);
	
	ipv4_hdr->checksum = ducknet_checksum(ipv4_hdr, sizeof(DucknetIPv4Header), 0);
	
	memcpy(ipv4_hdr + 1, payload, len);
	
	return 0;
}

static int get_lower_bit_index(ducknet_u32 x) {
	for (int i = 0; ; i++) {
		if ((x >> i) & 1) {
			return i;
		}
	}
}

static int find_tosend_free() {
	for (int i = 0; i < (int) (sizeof(tosend_free) / sizeof(tosend_free[0])); i++) {
		if (tosend_free[i]) {
			return (i << 5) | get_lower_bit_index(tosend_free[i]);
		}
	}
	return -1;
}

int ducknet_ipv4_send(DucknetIPv4Address dst, ducknet_u8 protocol, const void *payload, int len) {
	DucknetMACAddress dst_mac;
	if (ducknet_arp_lookup(dst, &dst_mac) >= 0) {
		int r;
		if ((r = ipv4_build_packet(ducknet_sendbuf, dst, protocol, payload, len)) < 0) {
			return r;
		}
		DucknetEtherHeader *eth_hdr = (DucknetEtherHeader *) ducknet_sendbuf;
		eth_hdr->dst = dst_mac;
		return ducknet_phy_send(eth_hdr, len + sizeof(DucknetIPv4Header) + sizeof(DucknetEtherHeader));
	}
	int r;
	if ((r = ducknet_arp_query(dst)) < 0) {
		// Do nothing ???
	}
	int id = find_tosend_free();
	if (id == -1) {
		return -1;
	}
	if ((r = ipv4_build_packet(tosend_table[id].pkt, dst, protocol, payload, len)) < 0) {
		return r;
	}
	tosend_table[id].dst_ip = dst;  // TODO routing
	tosend_table[id].expire_time = ducknet_time_add_ms(ducknet_currenttime, TOSEND_TIMEOUT_MS);
	tosend_table[id].pkt_len = len + sizeof(DucknetIPv4Header) + sizeof(DucknetEtherHeader);
	tosend_free[id >> 5] &= ~(1u << (id & 31));
	return 0;
}

int ducknet_ipv4_idle() {
	static int cnt = 0;
	if (++cnt < 1024 && ducknet_currenttime - last_idle_time <= idle_delay) {
		return 0;
	}
	cnt = 0;
	last_idle_time = ducknet_currenttime;
	
	for (int i = 0; i < TOSEND_TABLE_SIZE; i++) {
		if (tosend_free[i >> 5] & (1u << (i & 31))) {
			continue;
		}
		if (ducknet_currenttime > tosend_table[i].expire_time) {
			tosend_free[i >> 5] |= 1u << (i & 31);
			continue;
		}
		DucknetMACAddress dst_mac;
		if (ducknet_arp_lookup(tosend_table[i].dst_ip, &dst_mac) >= 0) {
			tosend_free[i >> 5] |= 1u << (i & 31);
			DucknetEtherHeader *eth_hdr = (DucknetEtherHeader *) tosend_table[i].pkt;
			eth_hdr->dst = dst_mac;
			ducknet_phy_send(eth_hdr, tosend_table[i].pkt_len);
		} else {
			ducknet_arp_query(tosend_table[i].dst_ip);
		}
	}
	return 0;
}

int ducknet_ipv4_packet_handle(void *pkt, int len) {
	if (len < (int) sizeof(DucknetIPv4Header)) return -1;
	DucknetIPv4Header *hdr = (DucknetIPv4Header *) pkt;
	ducknet_ipv4_hton(hdr);
	int r;
	if (packet_handle && ((r = packet_handle(hdr, len - (int) sizeof(DucknetIPv4Header))) < 0)) {
		return r;
	}
	if (hdr->version != 4) return -1;
	if (hdr->ihl != 5) return -1;  // TODO support ihl > 5
	// No tos checking
	if (hdr->length > (unsigned) len) return -1;
	if ((hdr->flags & ~DUCKNET_IPv4_FLAGS_DF) != 0) return -1;  // TODO support fragmentation
	if (!(~hdr->dst.addr == 0 || hdr->dst.addr == ducknet_ip.addr)) {
		return -1;
	}
	int content_len = hdr->length - (int) sizeof(DucknetIPv4Header);
	switch (hdr->protocol) {
		case DUCKNET_IPv4_ICMP:
			return ducknet_icmp_packet_handle(hdr->src, hdr->dst, hdr + 1, content_len);
		case DUCKNET_IPv4_UDP:
			return ducknet_udp_packet_handle(hdr->src, hdr->dst, hdr + 1, content_len);
		default:
			return -1;
	}
}

int ducknet_parse_ipv4(const char *s, DucknetIPv4Address *addr) {
	int len = strlen(s);
	int cnt = 0;
	for (int i = 0; i < len; i++) {
		if (s[i] == '.') ++cnt;
	}
	if (cnt != 3) {
		return -1;
	}
	ducknet_u8 a[4] = {0, 0, 0, 0};
	int idx = 0;
	bool has_num = false;
	for (int i = 0; i < len; i++) {
		if (s[i] == '.') {
			if (!has_num) {
				return -1;
			}
			++idx;
			has_num = false;
		} else {
			if (s[i] < '0' || s[i] > '9') {
				return -1;
			}
			if (((int) a[idx]) * 10 + s[i] - 48 > 255) {
				return -1;
			}
			a[idx] = a[idx] * 10 + s[i] - 48;
			has_num = true;
		}
	}
	*addr = (DucknetIPv4Address) {
		.a = {a[3], a[2], a[1], a[0]}
	};
	return 0;
}
