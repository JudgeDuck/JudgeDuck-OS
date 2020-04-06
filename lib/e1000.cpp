#include <stdint.h>
#include <string.h>

#include <inc/e1000.hpp>
#include <inc/logger.hpp>
#include <inc/memory.hpp>
#include <inc/timer.hpp>
#include <inc/pci.hpp>

// kernel pages are 4k-sized
using Memory::PAGE_SIZE;

using Timer::microdelay;

namespace e1000 {
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
	
	#define TQ_FLUSH_COUNT 32
	#define RQ_FLUSH_COUNT 128
	
	#define RQ_DESC_PAGE_COUNT 1
	
	#define TQSIZE ((PAGE_SIZE) / sizeof(TransDesc))
	#define RQSIZE ((PAGE_SIZE * RQ_DESC_PAGE_COUNT) / sizeof(RecvDesc))
	
	static volatile TransDesc tq[TQSIZE] __attribute__((aligned(PAGE_SIZE)));
	static volatile RecvDesc rq[RQSIZE] __attribute__((aligned(PAGE_SIZE)));
	
	static char tq_pages[TQSIZE / 2][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
	static char rq_pages[RQSIZE / 2][PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
	
	static void *tq_addrs[TQSIZE];
	static void *rq_vaddrs[RQSIZE];
	
	static int last_tq_page_id;
	static uint64_t last_tq_page_offset;
	
	static volatile char e1000[0x10000] __attribute__((aligned(PAGE_SIZE)));
	
	static uint32_t e1000_rdt, e1000_rdh;
	static uint32_t e1000_rdt_real;
	static uint32_t e1000_tdt, e1000_tdh;
	static uint32_t e1000_tdt_real;
	
	// MTU = 1500, half page per packet
	static int tq_frag_alloc(uint64_t cnt, void **va, void **pa) {
		cnt += (-cnt) & (64 - 1);  // ???
		if (last_tq_page_offset + cnt <= PAGE_SIZE) {
			*pa = (char *) tq_addrs[last_tq_page_id] + last_tq_page_offset;
			*va = tq_pages[last_tq_page_id] + last_tq_page_offset;
			last_tq_page_offset += cnt;
			return 0;
		} else if (cnt <= PAGE_SIZE) {
			last_tq_page_id = (last_tq_page_id + 1) % (TQSIZE / 2);
			*pa = tq_addrs[last_tq_page_id];
			*va = tq_pages[last_tq_page_id];
			last_tq_page_offset = cnt;
			return 0;
		} else {
			return -1;
		}
	}
	
	static int e1000_init(unsigned maxMTA, uint8_t mac[6]) {
		LDEBUG("e1000 status = %x", *(volatile uint32_t *) (e1000 + 0x8));
		
		const uint32_t tq_pa = (uint32_t) (uint64_t) tq;
		const uint32_t rq_pa = (uint32_t) (uint64_t) rq;
		
		for (uint64_t i = 0; i < TQSIZE; i++) {
			tq_addrs[i] = (void *) tq_pages[i];
		}
		for (uint64_t i = 0; i < RQSIZE; i++) {
			rq_vaddrs[i] = (void *) ((uint64_t) rq_pages[i / 2] + (i % 2) * PAGE_SIZE / 2);
		}
		
		// For allocating fragments
		last_tq_page_id = 0;
		last_tq_page_offset = 0;
		
		// try reset
		*(volatile uint32_t *) (e1000 + 0xd8) = 0xffffffff;
		uint32_t ctrl = *(volatile uint32_t *) (e1000 + 0x0);
		*(volatile uint32_t *) (e1000 + 0x0) = ctrl | 0x04000000;
		microdelay(1);
		*(volatile uint32_t *) (e1000 + 0xd8) = 0xffffffff;
		
		// try phy reset
		ctrl = *(volatile uint32_t *) (e1000 + 0x0);
		*(volatile uint32_t *) (e1000 + 0x0) = ctrl | 0x80000000;
		microdelay(10000);
		*(volatile uint32_t *) (e1000 + 0x0) = ctrl;
		
		// CTRL: force 1Gbps duplex
		*(volatile uint32_t *) (e1000 + 0x0) = (1u << 0) | (0u << 5) | (1u << 6) | (2u << 8) | (1u << 11) | (1u << 12);
		
		LDEBUG("e1000: waiting for link ...");
		while (1) {
			if ((*(volatile uint32_t *) (e1000 + 0x8)) & 0x2) break;  // link up
			microdelay(100000);
		}
		LDEBUG("e1000: link up");
		
		const uint32_t tdbal = tq_pa;
		*(volatile uint32_t *) (e1000 + 0x3800) = tdbal;      // TDBAL
		*(volatile uint32_t *) (e1000 + 0x3804) = 0;          // TDBAH
		*(volatile uint32_t *) (e1000 + 0x3808) = sizeof(tq); // TDLEN
		*(volatile uint32_t *) (e1000 + 0x3810) = 0;          // TDH
		*(volatile uint32_t *) (e1000 + 0x3818) = 0;          // TDT
		e1000_tdt = 0;
		e1000_tdt_real = 0;
		e1000_tdh = 0;
		
		// TXDCTL: GRAN=0 | 1<<22(must=1) | WTHRESH=3 | HTHRESH=4 | PTHRESH=3
		*(volatile uint32_t *) (e1000 + 0x3828) = (0u << 24) | (1u << 22) | (3u << 16) | (4u << 8) | 3u;
		
		// TCTL: EN | PSP | CT=0x10 | COLD=0x40 | MULR
		uint32_t tctl = (1 << 1) | (1 << 3) | (0x10 << 4) | (0x40 << 12) | (1u << 28);
		*(volatile uint32_t *) (e1000 + 0x400) = tctl;        // TCTL
		// TIPG: ???
		*(volatile uint32_t *) (e1000 + 0x410) = 0x00602006;
		
		// MAC address
		uint32_t RAL = 0, RAH = 0;
		for (int i = 0; i < 4; i++) RAL |= (uint32_t) mac[i] << (i * 8);
		for (int i = 0; i < 2; i++) RAH |= (uint32_t) mac[i + 4] << (i * 8);
		*(volatile uint32_t *) (e1000 + 0x5400) = RAL;        // RAL
		uint32_t rah = RAH | (1u << 31);
		*(volatile uint32_t *) (e1000 + 0x5404) = rah;        // RAH
		
		for (unsigned i = 0x5200; i < maxMTA; i += 4) {
			*(volatile uint32_t *)(e1000 + i) = 0;                     // MTA
		}
		
		*(volatile uint32_t *) (e1000 + 0xd8) = 0xffffffff;            // IMC
		*(volatile uint32_t *) (e1000 + 0xc0);  // read ICR
		
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
		
		// RCTL: EN | BAM | BSIZE=2048 | BSEX=0 | SECRC=1
		uint32_t rctl = (1 << 1) | (1 << 15) | (0 << 16) | (0 << 25) | (1 << 26);
		*(volatile uint32_t *) (e1000 + 0x100) = rctl;        // RCTL
		
		LDEBUG("e1000 status = %08x", *(volatile uint32_t *) (e1000 + 0x8));
		
		return 0;
	}
	
	static int _init(uint8_t mac[6]) {
		const uint32_t args[][3] = {
			{ 0x8086, 0x15a3, 0x5280 },  // e1000e
			{ 0x8086, 0x10d3, 0x5280 },  // e1000e
			{ 0x8086, 0x100e, 0x5400 },  // e1000
			{ 0x8086, 0x15bc, 0x5400 },  // I219-V
			{ 0x8086, 0x15b8, 0x5400 },  // I219-V H310CM-ITX/ac
			{ 0, 0, 0 }
		};
		
		uint64_t r = -1ull;
		for (int i = 0; args[i][0]; i++) {
			r = PCI::map_device(args[i][0], args[i][1], (uint64_t) e1000, sizeof(e1000));
			if (r != -1ull) {
				return e1000_init(args[i][2], mac);
			}
		}
		return r;
	}
	
	int send(const void *buf, int cnt) {
		if (cnt < 0 || cnt > 1500) {
			return -1;
		}
		
		if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
			e1000_tdh = *(volatile uint32_t *) (e1000 + 0x3810);
		}
		if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
			return -1;
		}
		
		void *va, *pa;
		if (tq_frag_alloc(cnt, &va, &pa) < 0) {
			return -1;
		}
		memcpy(va, buf, cnt);
		
		TransDesc td;
		memset(&td, 0, sizeof(td));
		td.addr = (uint32_t) (uint64_t) pa;
		td.length = cnt;
		td.cmd &= ~(1 << 5);
		td.cmd |= (1 << 3) | (1 << 0);
		td.cmd |= (1 << 1);
		memcpy((void *) &tq[e1000_tdt], &td, sizeof(td));
		e1000_tdt = (e1000_tdt + 1) % TQSIZE;
		
		if (e1000_tdt % TQ_FLUSH_COUNT == 0) {
			*(volatile uint32_t *) (e1000 + 0x3818) = e1000_tdt;
			e1000_tdt_real = e1000_tdt;
		}
		
		return 0;
	}
	
	int flush() {
		if (e1000_tdt != e1000_tdt_real) {
			*(volatile uint32_t *) (e1000 + 0x3818) = e1000_tdt;
			e1000_tdt_real = e1000_tdt;
		}
		
		return 0;
	}
	
	// returns: length
	int receive(void *bufpage) {
		uint32_t idx = (e1000_rdt + 1) % RQSIZE;
		volatile struct RecvDesc *rd = rq + idx;
		if (!(rd->status & 1)) {
			return -1;
		}
		int len = rd->length;
		if (len < 0) {
			return -1;
		}
		if (len > (int) PAGE_SIZE / 2) {
			len = PAGE_SIZE / 2;
		}
		memcpy(bufpage, rq_vaddrs[idx], len);
		rd->status &= ~1;
		e1000_rdt = idx;
		if (e1000_rdt % RQ_FLUSH_COUNT == 0) {
			*(volatile uint32_t *) (e1000 + 0x2818) = e1000_rdt;  // ???
			e1000_rdt_real = e1000_rdt;
		}
		return len;
	}
	
	bool init(uint8_t mac[6]) {
		LDEBUG_ENTER_RET();
		
		int r = _init(mac);
		if (r < 0) {
			return false;
		} else {
			return true;
		}
	}
}
