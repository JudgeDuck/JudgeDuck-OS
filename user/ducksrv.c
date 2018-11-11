#include <inc/lib.h>
#include <ip-config.h>

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

bool firstTrans;

uint32_t e1000_rdt, e1000_rdh;
uint32_t e1000_rdt_real;

int e1000_init() {
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
	
	uint32_t tdbal = tq_pa;
	*(volatile uint32_t *) (e1000 + 0x3800) = tdbal;      // TDBAL
	*(volatile uint32_t *) (e1000 + 0x3804) = 0;          // TDBAH
	*(volatile uint32_t *) (e1000 + 0x3808) = sizeof(tq); // TDLEN
	*(volatile uint32_t *) (e1000 + 0x3810) = 0;          // TDH
	*(volatile uint32_t *) (e1000 + 0x3818) = 0;          // TDT
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
	
	unsigned maxMTA = 0x5280;
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
	
	firstTrans = true;
	
	cprintf("e1000 status = %x\n", *(volatile uint32_t *) (e1000 + 0x8));
	
	return 0;
}

int network_init() {
	int r;
	r = sys_map_pci_device(0x8086, 0x10d3, (void *) e1000, sizeof(e1000));
	if (r < 0) {
		return r;
	}
	
	return e1000_init();
}

int network_try_transmit(void *buf, int cnt) {
	uint32_t tdt = *(volatile uint32_t *) (e1000 + 0x3818);
	if (tdt + 1 == TQSIZE) {
		firstTrans = false;
	}
	if (!firstTrans && !(tq[(tdt + 1) % TQSIZE].status & 1)) {
		return -1;
	}
	memcpy(tq_pages[tdt], buf, cnt);
	struct TransDesc td;
	memset(&td, 0, sizeof(td));
	td.addr = (uint32_t) tq_addrs[tdt];
	td.length = cnt;
	td.cmd &= ~(1 << 5);
	td.cmd |= (1 << 3) | (1 << 0);
	tq[tdt] = td;
	tdt = (tdt + 1) % TQSIZE;
	*(volatile uint32_t *) (e1000 + 0x3818) = tdt;
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

char recvbuf[PGSIZE] __attribute__((aligned(PGSIZE)));

uint64_t read_tsc() {
	uint32_t hi, lo;
	__asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
	return (((uint64_t) hi) << 32) | lo;
}

void ducksrv_mainloop() {
	uint64_t transfer = 0;
	uint64_t last_tsc = read_tsc();
	uint32_t cntpkt = 0;
	uint32_t pollcnt = 0;
	uint32_t cntsend = 0;
	while (1) {
		int len = network_try_receive(recvbuf);
		if (++pollcnt % 128 == 0) {
			uint64_t now_tsc = read_tsc();
			if (now_tsc - last_tsc >= 3400000000ull) {
				cprintf("transfer %d KB, %d pkts, %d polls, %d sends\n", (int) (transfer >> 10), cntpkt, (int) pollcnt, (int) cntsend);
				transfer = 0;
				cntpkt = 0;
				pollcnt = 0;
				cntsend = 0;
				last_tsc = now_tsc;
			}
		}
		if (len < 0) {
			continue;
		}
		transfer += len;
		++cntpkt;
	}
}

// ========

void umain(int argc, char **argv) {
	binaryname = "ducksrv";
	
	cprintf("ducksrv starting ...\n");
	
	if (network_init() < 0) {
		cprintf("GG, network init failed\n");
		return;
	}
	cprintf("duck network seems ok\n");
	
	ducksrv_mainloop();
}
