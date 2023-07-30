#include <stdint.h>
#include <string.h>

#include <inc/i211.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/memory.hpp>
#include <inc/pci.hpp>
#include <inc/dma.hpp>

using Memory::PAGE_SIZE;

namespace i211 {
	struct RegConfig {
		uint32_t CTRL, STATUS, CTRL_EXT;
		uint32_t RCTL;
		uint32_t TCTL;
		
		uint32_t EIMC;
		
		uint32_t RXPBSIZE, TXPBSIZE;
		
		uint32_t RFCTL;
		
		uint32_t MTA;
		uint32_t RAL, RAH;
		
		uint32_t MRQC;
		
		uint32_t SWSM;
		uint32_t SW_FW_SYNC;
		
		uint32_t UTA;
		
		uint32_t SRRCTL;
		uint32_t RDBAL, RDBAH, RDLEN, RDH, RDT;
		uint32_t RXDCTL;
		
		uint32_t TDBAL, TDBAH, TDLEN, TDH, TDT;
		uint32_t TXDCTL;
		
		uint32_t EEC;
		uint32_t EEMNGCTL;
	};
	
	static const RegConfig regs_i211 = {
		.CTRL = 0x0000,
		.STATUS = 0x0008,
		.CTRL_EXT = 0x0018,
		.RCTL = 0x00100,
		.TCTL = 0x00400,
		
		.EIMC = 0x1528,
		
		.RXPBSIZE = 0x2404,
		.TXPBSIZE = 0x3404,
		
		.RFCTL = 0x5008,
		
		.MTA = 0x5200,
		.RAL = 0x5400,
		.RAH = 0x5404,
		
		.MRQC = 0x5818,
		
		.SWSM = 0x5B50,
		.SW_FW_SYNC = 0x5B5C,
		
		.UTA = 0xA000,
		
		.SRRCTL = 0xC00C,
		.RDBAL = 0xC000,
		.RDBAH = 0xC004,
		.RDLEN = 0xC008,
		.RDH = 0xC010,
		.RDT = 0xC018,
		.RXDCTL = 0xC028,
		
		.TDBAL = 0xE000,
		.TDBAH = 0xE004,
		.TDLEN = 0xE008,
		.TDH = 0xE010,
		.TDT = 0xE018,
		.TXDCTL = 0xE028,
		
		.EEC = 0x12010,
		.EEMNGCTL = 0x12030,
	};
	
	static volatile char i211_dev[0x13000] __attribute__((aligned(PAGE_SIZE)));
	static const RegConfig *regs;
	
	#define READ32(reg) (*(volatile uint32_t *) (i211_dev + regs->reg))
	#define WRITE32(reg, val) (*(volatile uint32_t *) (i211_dev + regs->reg) = (val))
	
	typedef union {
		struct {
			uint64_t packet_addr;
			// TODO: consider RXCTL.RXdataWriteNSEn
			uint64_t header_addr;
		} __attribute__((packed)) rd;
		struct {
			uint64_t padding1;  // not used
			uint32_t status_error;
			uint16_t packet_length;
			uint16_t padding2;  // (not used) vlan_tag
		} __attribute__((packed)) wb;
	} RecvDesc;
	
	typedef union {
		struct {
			uint64_t packet_addr;
			uint32_t cmd_type_len;
			uint32_t paylen_status;
		} __attribute__((packed)) rd;
		struct {
			uint64_t padding1;  // not used
			uint32_t padding2;  // not used
			uint32_t status;
		} __attribute__((packed)) wb;
	} TransDesc;
	
	#define BUFFER_SIZE 2048
	#define RQSIZE ((PAGE_SIZE) / sizeof(RecvDesc))
	#define TQSIZE ((PAGE_SIZE) / sizeof(TransDesc))
	#define RQ_FLUSH_COUNT 32
	#define TQ_FLUSH_COUNT 32
	
	static volatile RecvDesc rq[RQSIZE] __attribute__((aligned(PAGE_SIZE)));
	static volatile TransDesc tq[TQSIZE] __attribute__((aligned(PAGE_SIZE)));
	
	static char *rq_buf;
	static char *tq_buf;
	
	static void prepare_rx_descriptor(int idx) {
		static char header_buf[64] __attribute__((aligned(64)));
		rq[idx].rd.packet_addr = (uint64_t) (rq_buf + idx * BUFFER_SIZE);
		rq[idx].rd.header_addr = (uint64_t) (header_buf);
	}
	
	static void prepare_tx_descriptor(int idx, uint32_t len) {
		tq[idx].rd.packet_addr = (uint64_t) (tq_buf + idx * BUFFER_SIZE);
		
		uint32_t cmd_type_len = 0;
		// - DTALEN (15:0)
		cmd_type_len |= len;
		// - Advanced Transmit Data Descriptor, DTYP (23:20) = 0011b.
		cmd_type_len |= (3u << 20);
		// - DCMD (31:24):
		//   - DEXT (bit 5), Descriptor Extension (1b for advanced mode)
		//   - IFCS (bit 1), Insert FCS
		//   - EOP (bit 0), End of Packet
		cmd_type_len |= ((1u << 5) | (1u << 1) | (1u << 0)) << 24;
		// - STA (35:32)
		//   - DD (bit 0), Descriptor Done
		//     (Not needed)
		// - PAYLEN (63:46)
		//   - equals to DTALEN (?)
		uint32_t paylen_status = len << (46 - 32);
		
		tq[idx].rd.cmd_type_len = cmd_type_len;
		tq[idx].rd.paylen_status = paylen_status;
	}
	
	static struct {
		uint32_t rdt;
		uint32_t next_rx_idx;
		uint32_t tdh, tdt;
		uint32_t next_tx_idx;
	} i211_states;
	
	static void i211_phy_take_ownership() {
		bool need_try_again = false;
		
		start:
		// Set the SWSM.SWESMBI bit (offset 0x5B50 bit 1).
		WRITE32(SWSM, READ32(SWSM) | (1u << 1));
		// Read SWSM; If !SWSM.SWESMBI, go back to last step.
		if (!(READ32(SWSM) & (1u << 1))) goto start;
		// Read SW_FW_SYNC and checks the bit that controls the PHY it wants to own.
		//   If 1 (firmware owns the PHY), try again later.
		if (READ32(SW_FW_SYNC) & (1u << 17)) {  // FW_PHY_SM
			need_try_again = true;
		} else {
			WRITE32(SW_FW_SYNC, READ32(SW_FW_SYNC) | (1u << 1));  // SW_PHY_SM
		}
		// Clear SWSM.SWESMBI.
		WRITE32(SWSM, READ32(SWSM) & ~(1u << 1));
		
		if (need_try_again) {
			Timer::microdelay(1);
			need_try_again = false;
			goto start;
		}
	}
	
	static void i211_phy_release_ownership() {
		start:
		// Set the SWSM.SWESMBI bit (offset 0x5B50 bit 1).
		WRITE32(SWSM, READ32(SWSM) | (1u << 1));
		// Read SWSM; If !SWSM.SWESMBI, go back to last step.
		if (!(READ32(SWSM) & (1u << 1))) goto start;
		// Clear the bit that controls the PHY it wants to own.
		WRITE32(SW_FW_SYNC, READ32(SW_FW_SYNC) & ~(1u << 1));  // SW_PHY_SM
		// Clear SWSM.SWESMBI.
		WRITE32(SWSM, READ32(SWSM) & ~(1u << 1));
	}
	
	static void i211_phy_configure() {
		// TODO
	}
	
	static bool i211_init_rx_queue() {
		WRITE32(RXDCTL, 0);
		
		// 1. Allocate a region of memory for the receive descriptor list.
		rq_buf = (char *) DMA::alloc(RQSIZE * BUFFER_SIZE);
		if (!rq_buf) {
			LERROR("i211: failed to allocate receive buffer");
			return false;
		}
		
		// 2. Receive buffers of appropriate size should be allocated and
		//    pointers to these buffers should be stored in the descriptor ring.
		for (uint64_t i = 0; i < RQSIZE; i++) {
			prepare_rx_descriptor(i);
		}
		
		// 3. Program the descriptor base address with the address of the region.
		WRITE32(RDBAL, (uint32_t) (uint64_t) rq);
		WRITE32(RDBAH, 0);
		
		// 4. Set the length register to the size of the descriptor ring.
		WRITE32(RDLEN, RQSIZE * sizeof(RecvDesc));
		
		// 5. Program SRRCTL.
		//    BSIZEPACKET (6:0): 2 (2KB)
		//    BSIZEHEADER (13:8): 1 (64B)
		//    DESCTYPE (27:25): 001b = Advanced descriptor one buffer.
		uint32_t srrctl = READ32(SRRCTL);
		srrctl &= ~(0x7f << 0);
		srrctl &= ~(0x3f << 8);
		srrctl &= ~(0x7 << 25);
		srrctl |= (2u << 0) | (1u << 8) | (1u << 25);
		WRITE32(SRRCTL, srrctl);
		
		// 6. If header split or header replication is required for this queue,
		//    program the PSRTYPE register according to the required headers.
		//    (Not used)
		
		// 7. Enable the queue by setting RXDCTL.ENABLE.
		WRITE32(RDH, 0);
		WRITE32(RDT, 0);
		//    !!! In the case of queue zero, the enable bit is set by default -
		//        so the ring parameters should be set before RCTL.RXEN is set.
		//    PTHRESH = 8, HTHRESH = 8, WTHRESH = 4
		WRITE32(RXDCTL, (8u << 0) | (8u << 8) | (4u << 16) | (1u << 25));
		
		// 8. Poll the RXDCTL register until the ENABLE bit is set. The tail
		//    should not be bumped before this bit was read as one.
		LDEBUG("i211: waiting for RXDCTL.ENABLE");
		while (1) {
			if (READ32(RXDCTL) & (1u << 25)) break;
			Timer::microdelay(1000);
		}
		
		WRITE32(RDT, RQSIZE - 1);
		i211_states.rdt = RQSIZE - 1;
		i211_states.next_rx_idx = 0;
		
		// 9. Program the direction of packets to this queue according to the
		//    mode selected in the MRQC register. Packets directed to a
		//    disabled queue are dropped.
		WRITE32(MRQC, 0);
		
		return true;
	}
	
	static bool i211_init_tx_queue() {
		WRITE32(TXDCTL, 0);
		
		// 1. Allocate a region of memory for the transmit descriptor list.
		tq_buf = (char *) DMA::alloc(TQSIZE * BUFFER_SIZE);
		if (!tq_buf) {
			LERROR("i211: failed to allocate transmit buffer");
			return false;
		}
		for (uint64_t i = 0; i < TQSIZE; i++) {
			tq[i].wb.status = 0;
		}
		
		// 2. Program the descriptor base address with the address of the region.
		WRITE32(TDBAL, (uint32_t) (uint64_t) tq);
		WRITE32(TDBAH, 0);
		
		// 3. Set the length register to the size of the descriptor ring.
		WRITE32(TDLEN, TQSIZE * sizeof(TransDesc));
		
		// 4. Program the TXDCTL register with the desired Tx descriptor write back policy.
  		//    - Suggested values: WTHRESH = 1b; other 0b
		WRITE32(TXDCTL, (1u << 16));
		
		// 5. If needed, set TDWBAL/TWDBAH to enable head write back.
		//    (Not needed)
		
		// 6. Enable the queue using TXDCTL.ENABLE (queue zero is enabled by default).
		WRITE32(TDH, 0);
		WRITE32(TDT, 0);
		WRITE32(TXDCTL, READ32(TXDCTL) | (1u << 25));
		
		// 7. Poll the TXDCTL register until the ENABLE bit is set.
		LDEBUG("i211: waiting for TXDCTL.ENABLE");
		while (1) {
			if (READ32(TXDCTL) & (1u << 25)) break;
			Timer::microdelay(1000);
		}
		
		i211_states.tdh = 0;
		i211_states.tdt = 0;
		i211_states.next_tx_idx = 0;
		
		return true;
	}
	
	static bool i211_disable_pcie_master() {
		WRITE32(CTRL, READ32(CTRL) | (1u << 2));
		
		const int MASTER_DISABLE_TIMEOUT = 800;
		for (int i = 0; i < MASTER_DISABLE_TIMEOUT; i++) {
			if (!(READ32(STATUS) & (1u << 19))) return true;
			Timer::microdelay(2500);
		}
		
		return false;
	}
	
	static bool i211_init(uint8_t mac[6], bool is_i225) {
		// Master disable algorithm (Section 5.2.3.3) before reset
		if (!i211_disable_pcie_master()) {
			LERROR("i211: PCIe Master Disable failed");
			return false;
		}
		
		// Disable interrupts
		WRITE32(EIMC, 0xFFFFFFFF);
		
		// 4.3.1 Software Reset (RST)
		//   1. Set CTRL.RST bit
		WRITE32(CTRL, READ32(CTRL) | (1u << 26) | (1u << 29));
		//   2. Wait at least 3 ms
		Timer::microdelay(3000 + 500);
		//   3. Verify: EEC.Auto_RD == 1b; STATUS.PF_RST_DONE == set to 1b
		//      TODO: EEC?
		if (!(READ32(STATUS) & (1u << 21))) {
			LERROR("i211: RST failed");
			return false;
		}
		
		// Disable interrupts (again)
		WRITE32(EIMC, 0xFFFFFFFF);
		
		// CTRL_EXT.DRV_LOAD
		WRITE32(CTRL_EXT, READ32(CTRL_EXT) | (1u << 28));
		
		// 4.3.1.1 Bus Master Enable (BME)
		// (Unimplemented)
		
		// 4.3.1.2 PHY Reset
		//   1. Take ownership of the PHY
		i211_phy_take_ownership();
		//   2. CTRL.PHY_RST = 1 (bit 31)
		WRITE32(CTRL, READ32(CTRL) | (1u << 31));
		//   3. Wait 100us (no longer than 100ms)
		Timer::microdelay(100 + 500);
		//   4. CTRL.PHY_RST = 0
		WRITE32(CTRL, READ32(CTRL) & ~(1u << 31));
		//   5. Release ownership of the relevant PHY
		i211_phy_release_ownership();
		//   6. Wait for the CFG_DONE bit (EEMNGCTL.CFG_DONE0).
		//      (Unimplemented)
		//   7. Take the ownership of the relevant PHY.
		i211_phy_take_ownership();
		//   8. Configure the PHY.
		i211_phy_configure();
		//   9. Release ownership of the relevant PHY.
		i211_phy_release_ownership();
		
		// General configuration
		// CTRL: SLU = 1, FRCSPD = 0, FRCDPLX = 0
		WRITE32(CTRL, READ32(CTRL) & ~(1u << 11) & ~(1u << 12));
		WRITE32(CTRL, READ32(CTRL) | (1u << 6));
		
		// Wait for link up
		LDEBUG("i211: Waiting for link up...");
		while (1) {
			if (READ32(STATUS) & (1u << 1)) break;
			Timer::microdelay(100000);
		}
		int status = READ32(STATUS);
		const char *speeds[] = {"10M", "100M", "1000M", "???"};
		LDEBUG("i211: Link up (status = 0x%08x, FD = %d, SPEED = %s)",
			status,
			(status & (1u << 0)) ? 1 : 0,
			speeds[((status >> 6) & 3)]
		);
		
		// Receive initialization
		WRITE32(RCTL, 0);
		//   1. Receive address registers
		WRITE32(RAL, mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24));
		WRITE32(RAH, mac[4] | (mac[5] << 8) | (1u << 31));
		for (int i = 1; i < 16; i++) {
			WRITE32(RAL + i * 8, 0);
			WRITE32(RAH + i * 8, 0);
		}
		//   2. Set up the Multicast Table Array (MTA) by software.
		//      (Zero all entries initially)
		for (int i = 0; i < 128; i++) {
			WRITE32(MTA + i * 4, 0);
		}
		if (is_i225) {
			for (int i = 0; i < 128; i++) {
				WRITE32(UTA + i * 4, 0);
			}
		}
		//   3. Program the RXPBSIZE register so that the total size
		//      formed by the receive packet buffer plus the transmit
		//      packet buffer(s) does not exceed 60 KB.
		if (is_i225) {
			#define I225_RXPBSIZE_DEFAULT	0x000000A2 /* RXPBSIZE default */
			#define I225_TXPBSIZE_DEFAULT	0x04000014 /* TXPBSIZE default */
			WRITE32(RXPBSIZE, I225_RXPBSIZE_DEFAULT);
			WRITE32(TXPBSIZE, I225_TXPBSIZE_DEFAULT);
		}
		//   4. Program RCTL with appropriate values.
		//      !!! Init receive queue first
		if (!i211_init_rx_queue()) return false;
		//      RXEN | BAM | SECRC
		WRITE32(RCTL, (1u << 1) | (1u << 15) | (1u << 26));
		
		// test
		WRITE32(RFCTL, READ32(RFCTL) | 0x00010000);  // IGC_RFCTL_IPV6_EX_DIS
		
		// Transmit initialization
		WRITE32(TCTL, 0);
		//   1. Program the TCTL register according to the MAC behavior needed
		if (!i211_init_tx_queue()) return false;
		//      EN | PSP
		WRITE32(TCTL, (1u << 1) | (1u << 3));
		//   2. Program the TXPBSIZE register so any transmit buffer that is
		//      in use is at least greater to twice the maximum packet size
		//      that might be stored in it. (Section 4.5.9)
		//      (Unimplemented)
		
		return true;
	}
	
	struct InitOptions {
		uint32_t vendor_id, device_id;
		const RegConfig *regs;
		bool is_i225;
	};
	
	bool init(uint8_t mac[6]) {
		LDEBUG_ENTER_RET();
		
		const InitOptions options[] = {
			{ 0x8086, 0x1539, &regs_i211, false },  // I211AT
			{ 0x8086, 0x15f3, &regs_i211, true },   // I225-V
			{ 0, 0, NULL, false },
		};
		
		uint64_t r = -1ull;
		for (int i = 0; options[i].vendor_id; i++) {
			r = PCI::map_device(options[i].vendor_id, options[i].device_id, (uint64_t) i211_dev, sizeof(i211_dev));
			if (r != -1ull) {
				regs = options[i].regs;
				return i211_init(mac, options[i].is_i225);
			}
		}
		
		return false;
	}
	
	void i211_flush_tx() {
		if (i211_states.tdt == i211_states.next_tx_idx) return;
		i211_states.tdt = i211_states.next_tx_idx;
		WRITE32(TDT, i211_states.tdt);
	}
	
	int send(const void *buf, int len) {
		if (len < 0 || len > 1500) {
			return -1;
		}
		
		uint32_t idx = i211_states.next_tx_idx;
		
		if ((idx + 1) % TQSIZE == i211_states.tdh) {
			i211_states.tdh = READ32(TDH);
		}
		
		if ((idx + 1) % TQSIZE == i211_states.tdh) {
			LDEBUG("i211: No free descriptors (idx = %d) while sending %d bytes", idx, len);
			return -1;
		}
		
		// Copy packet
		memcpy(tq_buf + idx * BUFFER_SIZE, buf, len);
		
		// Set up descriptor
		prepare_tx_descriptor(idx, len);
		
		// Advance TDT
		i211_states.next_tx_idx = (i211_states.next_tx_idx + 1) % TQSIZE;
		
		// Flush
		if (i211_states.next_tx_idx % TQ_FLUSH_COUNT == 0) {
			i211_flush_tx();
		}
		
		return 0;
	}
	
	int receive(void *buf) {
		int idx = i211_states.next_rx_idx;
		
		if (!rq[idx].wb.packet_length) {
			return -1;
		}
		
		// Packet Length
		uint32_t length = rq[idx].wb.packet_length;
		if (length > 1500) {
			return -1;
		}
		
		// Copy packet
		memcpy(buf, rq_buf + idx * BUFFER_SIZE, length);
		
		// Clear descriptor
		prepare_rx_descriptor(idx);
		
		// Advance RDT
		i211_states.next_rx_idx = (i211_states.next_rx_idx + 1) % RQSIZE;
		i211_states.rdt = (i211_states.rdt + 1) % RQSIZE;
		if (i211_states.rdt % RQ_FLUSH_COUNT == 0) {
			WRITE32(RDT, i211_states.rdt);
		}
		
		return length;
	}
	
	int flush() {
		i211_flush_tx();
		return 0;
	}
}
