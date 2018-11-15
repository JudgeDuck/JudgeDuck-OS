#include <inc/lib.h>
#include <ip-config.h>
#include <ducknet.h>

// ==== duck e1000 driver ====

struct TransDesc {
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));

struct RecvDesc {
	uint64_t addr;
	uint16_t length;
	uint16_t chksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
} __attribute__((packed));

#define TQSIZE ((PGSIZE) / sizeof(struct TransDesc))
#define RQSIZE ((PGSIZE) / sizeof(struct RecvDesc))
volatile struct TransDesc tq[TQSIZE] __attribute__((aligned(PGSIZE)));
volatile struct RecvDesc rq[RQSIZE] __attribute__((aligned(PGSIZE)));
char tq_pages[TQSIZE][PGSIZE] __attribute__((aligned(PGSIZE)));
char rq_pages[RQSIZE][PGSIZE] __attribute__((aligned(PGSIZE)));
void *tq_addrs[TQSIZE];

volatile char e1000[0x10000] __attribute__((aligned(PGSIZE)));

uint32_t e1000_rdt, e1000_rdh;
uint32_t e1000_rdt_real;
uint32_t e1000_tdt, e1000_tdh;
uint32_t e1000_tdt_real;

int e1000_init(unsigned maxMTA) {
	cprintf("e1000 status = %x\n", *(volatile uint32_t *) (e1000 + 0x8));
	
	uint32_t tq_pa, rq_pa;
	int r;
	r = sys_page_alloc_with_pa(0, (void *) tq, PTE_U | PTE_P | PTE_W, &tq_pa);
	if (r < 0) {
		return r;
	}
	r = sys_page_alloc_with_pa(0, (void *) rq, PTE_U | PTE_P | PTE_W, &rq_pa);
	if (r < 0) {
		return r;
	}
	
	for (int i = 0; i < TQSIZE; i++) {
		uint32_t pa;
		r = sys_page_alloc_with_pa(0, (void *) tq_pages[i], PTE_U | PTE_P | PTE_W, &pa);
		if (r < 0) {
			return r;
		}
		tq_addrs[i] = (void *) pa;
	}
	
	for (int i = 0; i < RQSIZE; i++) {
		uint32_t pa;
		r = sys_page_alloc_with_pa(0, (void *) rq_pages[i], PTE_U | PTE_P | PTE_W, &pa);
		if (r < 0) {
			return r;
		}
		rq[i].addr = pa;
	}
	
	// CTRL: force 1Gbps duplex
	*(volatile uint32_t *) (e1000 + 0x0) = (1u << 0) | (0u << 5) | (1u << 6) | (2u << 8) | (1u << 11) | (1u << 12);
	
	uint32_t tdbal = tq_pa;
	*(volatile uint32_t *) (e1000 + 0x3800) = tdbal;      // TDBAL
	*(volatile uint32_t *) (e1000 + 0x3804) = 0;          // TDBAH
	*(volatile uint32_t *) (e1000 + 0x3808) = sizeof(tq); // TDLEN
	*(volatile uint32_t *) (e1000 + 0x3810) = 0;          // TDH
	*(volatile uint32_t *) (e1000 + 0x3818) = 0;          // TDT
	e1000_tdt = 0;
	e1000_tdt_real = 0;
	e1000_tdh = 0;
	
	uint32_t tctl = (1 << 1) | (1 << 3) | (0x10 << 4) | (0x40 << 12);
	*(volatile uint32_t *) (e1000 + 0x400) = tctl;        // TCTL
	// ??? *(volatile uint32_t *) (e1000 + 0x410) = 0xa | (0x8 << 10) | (0xc << 20);    // TIPG
	*(volatile uint32_t *) (e1000 + 0x410) = 0x00602006;
	
	uint8_t mac[6] = DUCK_MAC;
	uint32_t RAL = 0, RAH = 0;
	for (int i = 0; i < 4; i++) RAL |= (uint32_t) mac[i] << (i * 8);
	for (int i = 0; i < 2; i++) RAH |= (uint32_t) mac[i + 4] << (i * 8);
	
	*(volatile uint32_t *) (e1000 + 0x5400) = RAL; // RAL
	uint32_t rah = RAH | (1u << 31);
	*(volatile uint32_t *) (e1000 + 0x5404) = rah;        // RAH
	
	for (unsigned i = 0x5200; i < maxMTA; i++) {
		e1000[i] = 0;                         // MTA
	}
	
	*(volatile uint32_t *) (e1000 + 0xd0) = 0;            // IMS
	uint32_t rdbal = rq_pa;
	*(volatile uint32_t *) (e1000 + 0x2800) = rdbal;      // RDBAL
	*(volatile uint32_t *) (e1000 + 0x2804) = 0;          // RDBAH
	*(volatile uint32_t *) (e1000 + 0x2808) = sizeof(rq); // RDLEN
	*(volatile uint32_t *) (e1000 + 0x2810) = 0;          // RDH
	*(volatile uint32_t *) (e1000 + 0x2818) = RQSIZE - 1; // RDT
	e1000_rdh = 0;
	e1000_rdt = RQSIZE - 1;
	e1000_rdt_real = e1000_rdt;
	
	uint32_t rctl = (1 << 1) | (1 << 15) | (3 << 16) | (1 << 25) | (1 << 26);
	*(volatile uint32_t *) (e1000 + 0x100) = rctl;        // RCTL
	
	cprintf("e1000 status = %x\n", *(volatile uint32_t *) (e1000 + 0x8));
	
	return 0;
}

int network_init() {
	const uint32_t args[][3] = {
		{0x8086, 0x15a3, 0x5280},  // e1000e
		{0x8086, 0x10d3, 0x5280},  // e1000e
		{0x8086, 0x100e, 0x5400},  // e1000
		{0, 0, 0}
	};
	
	int r = -1;
	for (int i = 0; args[i][0]; i++) {
		r = sys_map_pci_device(args[i][0], args[i][1], (void *) e1000, sizeof(e1000));
		if (r >= 0) {
			return e1000_init(args[i][2]);
		}
	}
	return r;
}

int network_try_transmit(const void *buf, int cnt) {
	if (cnt < 0 || cnt > 1500) {
		return -1;
	}
	
	if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
		e1000_tdh = *(volatile uint32_t *) (e1000 + 0x3810);
	}
	if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
		return -1;
	}
	
	memcpy(tq_pages[e1000_tdt], buf, cnt);
	struct TransDesc td;
	memset(&td, 0, sizeof(td));
	td.addr = (uint32_t) tq_addrs[e1000_tdt];
	td.length = cnt;
	td.cmd &= ~(1 << 5);
	td.cmd |= (1 << 3) | (1 << 0);
	td.cmd |= (1 << 1);
	tq[e1000_tdt] = td;
	e1000_tdt = (e1000_tdt + 1) % TQSIZE;
	
	if ((e1000_tdt - e1000_tdt_real + TQSIZE) % TQSIZE == TQSIZE / 2) {
		*(volatile uint32_t *) (e1000 + 0x3818) = e1000_tdt;
		e1000_tdt_real = e1000_tdt;
	}
	
	return 0;
}

int network_flush() {
	if (e1000_tdt != e1000_tdt_real) {
		*(volatile uint32_t *) (e1000 + 0x3818) = e1000_tdt;
		e1000_tdt_real = e1000_tdt;
	}
	
	return 0;
}

// returns: length
int network_try_receive(void *bufpage) {
	uint32_t idx = (e1000_rdt + 1) % RQSIZE;
	if (idx == e1000_rdh) {
		e1000_rdh = *(volatile uint32_t *) (e1000 + 0x2810);
	}
	if (idx == e1000_rdh) {
		return -1;
	}
	volatile struct RecvDesc *rd = rq + idx;
	int len = rd->length;
	if (len < 0) {
		return -1;
	}
	if (len > PGSIZE) {
		len = PGSIZE;
	}
	memcpy(bufpage, rq_pages[idx], len);
	rd->status &= ~1;
	e1000_rdt = idx;
	if ((e1000_rdt - e1000_rdt_real + RQSIZE) % RQSIZE == RQSIZE / 2) {
		*(volatile uint32_t *) (e1000 + 0x2818) = e1000_rdt;  // ???
		e1000_rdt_real = e1000_rdt;
	}
	return len;
}

// ========

uint64_t tsc_freq;

int my_idle() {
	return 0;
}

int my_packet_handle(void *pkt, int len) {
	return 0;
}

int my_ether_packet_handle(DucknetEtherHeader *hdr, int content_len) {
	return 0;
}

int my_arp_packet_handle(DucknetARPHeader *hdr, int len) {
	return 0;
}

int my_ipv4_packet_handle(DucknetIPv4Header *hdr, int content_len) {
	return 0;
}

int my_icmp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, DucknetICMPHeader *hdr, int len) {
	return 0;
}

int my_udp_packet_handle(DucknetIPv4Address src, DucknetIPv4Address dst, DucknetUDPHeader *hdr, int len) {
	ducknet_udp_send(src, hdr->sport, hdr->dport, hdr + 1, len);
	return -1;
}

void umain(int argc, char **argv) {
	binaryname = "ducksrv";
	
	cprintf("ducksrv starting ...\n");
	
	if (sys_get_tsc_frequency(&tsc_freq) < 0) {
		cprintf("GG, get tsc freq failed\n");
		return;
	}
	cprintf("tsc freq = %llu\n", tsc_freq);
	
	if (network_init() < 0) {
		cprintf("GG, network init failed\n");
		return;
	}
	cprintf("duck network seems ok\n");
	
	DucknetMACAddress my_mac = (DucknetMACAddress) {.a = DUCK_MAC};
	DucknetIPv4Address my_ip;
	if (ducknet_parse_ipv4(IP, &my_ip) < 0) {
		cprintf("GG, invalid IP address\n");
		return;
	}
	cprintf("IP = %s\n", IP);
	
	DucknetConfig conf = {
		.utils = {
			.tsc_freq = tsc_freq
		},
		.phy = {
			.send = network_try_transmit,
			.recv = network_try_receive,
			.flush = network_flush,
			.packet_handle = my_packet_handle
		},
		.ether = {
			.mac = my_mac,
			.packet_handle = my_ether_packet_handle
		},
		.arp = {
			.packet_handle = my_arp_packet_handle
		},
		.ipv4 = {
			.ip = my_ip,
			.packet_handle = my_ipv4_packet_handle
		},
		.icmp = {
			.packet_handle = my_icmp_packet_handle
		},
		.udp = {
			.packet_handle = my_udp_packet_handle
		},
		.idle = my_idle
	};
	
	if (ducknet_init(&conf) < 0) {
		cprintf("GG, libducknet init failed\n");
		return;
	}
	cprintf("libducknet seems ok\n");
	ducknet_mainloop();
}
