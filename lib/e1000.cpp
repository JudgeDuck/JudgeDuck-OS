#include <stdint.h>
#include <string.h>

#include <inc/e1000.hpp>
#include <inc/logger.hpp>
#include <inc/memory.hpp>
#include <inc/timer.hpp>
#include <inc/pci.hpp>
#include <inc/dma.hpp>

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
	
	static char *tq_buf;
	static char *rq_buf;
	
	static void *tq_addrs[TQSIZE];
	static void *rq_addrs[RQSIZE];
	
	static volatile char e1000[0x10000] __attribute__((aligned(PAGE_SIZE)));
	
	static uint32_t e1000_rdt, e1000_rdh;
	static uint32_t e1000_rdt_real;
	static uint32_t e1000_tdt, e1000_tdh;
	static uint32_t e1000_tdt_real;
	
	struct RegConfig {
		uint32_t CTRL, STATUS;
		uint32_t ICR, IMS, IMC;
		uint32_t RAL, RAH;
		
		uint32_t TCTL, TIPG;
		uint32_t TDBAL, TDBAH, TDLEN, TDH, TDT, TXDCTL;
		
		uint32_t RCTL;
		uint32_t RDBAL, RDBAH, RDLEN, RDH, RDT, RXDCTL;
	};
	
	static const RegConfig regs_e1000e = {
		.CTRL = 0x0,
		.STATUS = 0x8,
		
		.ICR = 0xc0,
		.IMS = 0xd0,
		.IMC = 0xd8,
		
		.RAL = 0x5400,
		.RAH = 0x5404,
		
		.TCTL = 0x400,
		.TIPG = 0x410,
		
		.TDBAL = 0x3800,
		.TDBAH = 0x3804,
		.TDLEN = 0x3808,
		.TDH = 0x3810,
		.TDT = 0x3818,
		.TXDCTL = 0x3828,
		
		.RCTL = 0x100,
		
		.RDBAL = 0x2800,
		.RDBAH = 0x2804,
		.RDLEN = 0x2808,
		.RDH = 0x2810,
		.RDT = 0x2818,
		.RXDCTL = 0x2828,
	};
	
	#define READ32(reg) (*(volatile uint32_t *) (e1000 + regs_e1000e.reg))
	#define WRITE32(reg, val) (*(volatile uint32_t *) (e1000 + regs_e1000e.reg) = (val))
	
	static int e1000_init(unsigned maxMTA, uint8_t mac[6]) {
		LDEBUG("e1000 status = %x", READ32(STATUS));
		
		const uint32_t tq_pa = (uint32_t) (uint64_t) tq;
		const uint32_t rq_pa = (uint32_t) (uint64_t) rq;
		
		tq_buf = (char *) DMA::alloc(TQSIZE / 2 * PAGE_SIZE);
		rq_buf = (char *) DMA::alloc(RQSIZE / 2 * PAGE_SIZE);
		if (!tq_buf || !rq_buf) {
			LERROR("e1000: DMA::alloc failed");
			return -1;
		}
		
		for (uint64_t i = 0; i < TQSIZE; i++) {
			tq_addrs[i] = (void *) ((uint64_t) tq_buf + i * PAGE_SIZE / 2);
		}
		
		for (uint64_t i = 0; i < RQSIZE; i++) {
			rq_addrs[i] = (void *) ((uint64_t) rq_buf + i * PAGE_SIZE / 2);
			rq[i].addr = (uint32_t) (uint64_t) rq_addrs[i];
		}
		
		WRITE32(TCTL, 0); 	  // clear TCTL
		WRITE32(RCTL, 0); 	  // clear RCTL
		
		// try reset
		WRITE32(IMC, 0xffffffff);
		if (false) WRITE32(CTRL, READ32(CTRL) | 0x04000000);
		microdelay(1);
		WRITE32(IMC, 0xffffffff);
		
		// try phy reset
		uint32_t ctrl = READ32(CTRL);
		if (false) WRITE32(CTRL, ctrl | 0x80000000);
		microdelay(10000);
		if (false) WRITE32(CTRL, ctrl);
		
		// Init link
		ctrl = READ32(CTRL);
		if (false) ctrl &= ~((1u << 11) | (1u << 12));   // No forcing speed or full-duplex
		if (false) ctrl |= 1u << 0 | 0b11 << 8 | 1u << 11 | 1u << 12; // Force 1000Mbps full-duplex
		ctrl |= 1u << 6;   // set link up
		if (false) WRITE32(CTRL, ctrl);
		
		LDEBUG("e1000: waiting for link ...");
		while (1) {
			if (READ32(STATUS) & 0x2) break;  // link up
			microdelay(100000);
		}
		LDEBUG("e1000: link up");
		
		// MAC address
		uint32_t RAL_val = 0, RAH_val = 0;
		for (int i = 0; i < 4; i++) RAL_val |= (uint32_t) mac[i] << (i * 8);
		for (int i = 0; i < 2; i++) RAH_val |= (uint32_t) mac[i + 4] << (i * 8);
		WRITE32(RAL, RAL_val);
		WRITE32(RAH, RAH_val | (1u << 31));
		
		for (unsigned i = 0x5200; i < maxMTA; i += 4) {
			*(volatile uint32_t *)(e1000 + i) = 0;                     // MTA
		}
		
		WRITE32(IMC, 0xffffffff);
		READ32(ICR);
		
		WRITE32(IMS, 0);
		
		// Transmit
		const uint32_t tdbal = tq_pa;
		WRITE32(TDBAL, tdbal);
		WRITE32(TDBAH, 0);
		WRITE32(TDLEN, sizeof(tq));
		WRITE32(TDH, 0);
		WRITE32(TDT, 0);
		
		e1000_tdt = 0;
		e1000_tdt_real = 0;
		e1000_tdh = 0;
		
		// TCTL: EN | PSP | CT=0x10 | COLD=0x40 | MULR
		WRITE32(TXDCTL, 0);
		WRITE32(TCTL, (1 << 1) | (1 << 3) | (0x10 << 4) | (0x40 << 12) | (1u << 28));
		
		// TXDCTL: QUEUE_ENABLE | WTHRESH=3 | HTHRESH=4 | PTHRESH=3
		WRITE32(TXDCTL, (1u << 25) | (3u << 16) | (4u << 8) | 3u);
		
		// TIPG: ???
		WRITE32(TIPG, 0x00602006);
		
		// Receive
		uint32_t rdbal = rq_pa;
		WRITE32(RDBAL, rdbal);
		WRITE32(RDBAH, 0);
		WRITE32(RDLEN, sizeof(rq));
		WRITE32(RDH, 0);
		WRITE32(RDT, RQSIZE - 1);
		
		e1000_rdh = 0;
		e1000_rdt = RQSIZE - 1;
		e1000_rdt_real = e1000_rdt;
		
		// RCTL: EN | BAM | BSIZE=2048 | BSEX=0 | SECRC=1
		WRITE32(RXDCTL, 0);
		WRITE32(RCTL, (1 << 1) | (1 << 15) | (0 << 16) | (0 << 25) | (1 << 26));
		
		// RXDCTL: QUEUE_ENABLE | WTHRESH=3 | HTHRESH=4 | PTHRESH=3
		WRITE32(RXDCTL, (1u << 25) | (3u << 16) | (4u << 8) | 3u);
		
		LDEBUG("e1000 status = %08x", READ32(STATUS));
		
		return 0;
	}
	
	struct InitOptions {
		uint32_t key1, key2;
		uint32_t maxMTA;
	};
	
	static int _init(uint8_t mac[6]) {
		const InitOptions args[] = {
			{ 0x8086, 0x15a3, 0x5280 },  // e1000e
			{ 0x8086, 0x100e, 0x5400 },  // e1000
			{ 0x8086, 0x10d3, 0x5280 },  // e1000e
			{ 0x8086, 0x153a, 0x5280 },  // I217-LM (e1000e)
			{ 0x8086, 0x15bc, 0x5400 },  // I219-V
			{ 0x8086, 0x15b8, 0x5400 },  // I219-V H310CM-ITX/ac
			{ 0x8086, 0x0d55, 0x5400 },  // B460M TUF Gaming
			{ 0x8086, 0x15fa, 0x5400 },  // H510M-HDV/M.2
			{ 0x8086, 0x1a1c, 0x5400 },  // I219-LM (13th gen)
			{ 0, 0, 0 },
		};
		
		uint64_t r = -1ull;
		for (int i = 0; args[i].key1; i++) {
			r = PCI::map_device(args[i].key1, args[i].key2, (uint64_t) e1000, sizeof(e1000));
			if (r != -1ull) {
				return e1000_init(args[i].maxMTA, mac);
			}
		}
		return r;
	}
	
	int send(const void *buf, int cnt) {
		if (cnt < 0 || cnt > 1500) {
			return -1;
		}
		
		if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
			e1000_tdh = READ32(TDH);
		}
		if ((e1000_tdt + 1) % TQSIZE == e1000_tdh) {
			return -1;
		}
		
		void *va = tq_addrs[e1000_tdt];
		void *pa = va;

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
			WRITE32(TDT, e1000_tdt);
			e1000_tdt_real = e1000_tdt;
		}
		
		return 0;
	}
	
	int flush() {
		if (e1000_tdt != e1000_tdt_real) {
			WRITE32(TDT, e1000_tdt);
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
		memcpy(bufpage, rq_addrs[idx], len);
		rd->status &= ~1;
		e1000_rdt = idx;
		if (e1000_rdt % RQ_FLUSH_COUNT == 0) {
			WRITE32(RDT, e1000_rdt);  // ???
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
