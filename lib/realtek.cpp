#include <stdint.h>
#include <string.h>

#include <inc/realtek.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/memory.hpp>
#include <inc/pci.hpp>
#include <inc/dma.hpp>

#include <assert.h>
#include <immintrin.h>

using Memory::PAGE_SIZE;

namespace realtek {
	#define RTL_CR 0x37
	#define RTL_CR_RST (1 << 4)
	#define RTL_RESET_MAX_WAIT_MS 100
	#define RTL_CR_TE (1 << 2)
	#define RTL_CR_RE (1 << 3)
	
	#define RTL_RMS 0xda
	#define RTL_RX_MAX_LEN 1536
	
	#define RTL_CPCR 0xe0
	#define RTL_CPCR_MULRW (1 << 3)
	#define RTL_CPCR_CPRX (1 << 1)
	#define RTL_CPCR_CPTX (1 << 0)
	#define RTL_CPCR_DAC (1 << 4)
	#define RTL_CPCR_VLAN (1 << 6)
	
	#define RTL_IDR0 0x00
	
	#define RTL_PHYAR 0x60
	#define RTL_PHYAR_FLAG (1 << 31)
	#define RTL_PHYAR_VALUE(flag, reg, data) ((flag) | ((reg) << 16) | (data))
	#define RTL_PHYAR_DATA(value) ((value) & 0xffff)
	
	#define RTL_MII_MAX_WAIT_US 500
	
	#define RTL_PHYSTATUS 0x6c
	
	#define MII_BMCR 0x00
	#define MII_BMCR_RESET (1 << 15)
	#define MII_BMCR_ANENABLE (1 << 12)
	#define MII_BMCR_ANRESTART (1 << 9)
	#define MII_RESET_MAX_WAIT_MS 500
	
	#define MII_CTRL1000 0x09
	#define MII_CTRL1000_ADV_1000FULL (1 << 9)
	#define MII_CTRL1000_ADV_1000HALF (1 << 8)
	
	#define MII_MMD_DATA 0x0e
	
	#define RTL_TNPDS 0x20
	#define RTL_RDSAR 0xe4
	
	#define RTL_MAR0 0x08
	#define RTL_MAR4 0x0c
	
	#define RTL_TCR 0x40
	#define RTL_TCR_MXDMA_MASK (0x7 << 8)
	#define RTL_TCR_MXDMA_DEFAULT (0x7 << 8)
	#define TX_NOCRC (1 << 16)
	
	#define RTL_RCR 0x44
	#define RTL_RCR_STOP_WORKING (1 << 24)
	#define RTL_RCR_RXFTH_MASK (0x7 << 13)
	#define RTL_RCR_RXFTH_DEFAULT (0x7 << 13)
	#define RTL_RCR_RBLEN_MASK (0x3 << 11)
	#define RTL_RCR_RBLEN_DEFAULT (0x0 << 11)
	#define RTL_RCR_MXDMA_MASK (0x7 << 8)
	#define RTL_RCR_MXDMA_DEFAULT (0x7 << 8)
	#define RTL_RCR_WRAP (1 << 7)
	#define RTL_RCR_AB (1 << 3)
	#define RTL_RCR_AM (1 << 2)
	#define RTL_RCR_APM (1 << 1)
	#define RTL_RCR_AAP (1 << 0)
	
	#define RTL_TPOLL 0x38
	#define RTL_TPOLL_NPQ (1 << 6)
	
	#define RTL_ISR 0x3e
	
	static volatile char rtl_dev[0x1000] __attribute__((aligned(PAGE_SIZE)));
	
	static bool _debug = false;
	static uint32_t _tmp_reg, _tmp_val;
	
	#define _READ(size, reg) ( \
		_debug ? \
			( \
				_tmp_reg = (reg), \
				_tmp_val = (*(volatile size *) (rtl_dev + _tmp_reg)), \
				LINFO("rtl_read " #reg "(0x%x) -> 0x%x", _tmp_reg, _tmp_val), \
				_tmp_val \
			) \
		: \
			(*(volatile size *) (rtl_dev + (reg))) \
	)
	
	#define _WRITE(size, reg, val) ( \
		_debug ? \
			( \
				_tmp_reg = (reg), \
				_tmp_val = (val), \
				LINFO("rtl_write " #reg "(0x%x) <- 0x%x", _tmp_reg, _tmp_val), \
				(*(volatile size *) (rtl_dev + _tmp_reg)) = _tmp_val \
			) \
		: \
			(*(volatile size *) (rtl_dev + (reg)) = (val)) \
	)
	
	#define READ8(reg) _READ(uint8_t, reg)
	#define READ16(reg) _READ(uint16_t, reg)
	#define READ32(reg) _READ(uint32_t, reg)
	
	#define WRITE8(reg, val) _WRITE(uint8_t, reg, val)
	#define WRITE16(reg, val) _WRITE(uint16_t, reg, val)
	#define WRITE32(reg, val) _WRITE(uint32_t, reg, val)
	
	struct Descriptor {
		uint16_t length;
		uint16_t flags;
		uint32_t reserved;
		uint64_t address;
	} __attribute__((packed));
	
	#define RTL_DESC_FLAG_OWN (1 << 15)
	#define RTL_DESC_FLAG_EOR (1 << 14)
	#define RTL_DESC_FLAG_FS (1 << 13)
	#define RTL_DESC_FLAG_LS (1 << 12)
	#define RTL_DESC_LENGTH_MASK 0x3fff
	
	#define N_RX_DESC 4
	#define N_TX_DESC 4
	
	static volatile Descriptor *rx_desc;
	static volatile Descriptor *tx_desc;
	
	#define BUFFER_SIZE RTL_RX_MAX_LEN
	
	static volatile char *rx_buffer;
	static volatile char *tx_buffer;
	
	static int rx_idx;
	static int tx_idx;
	
	static inline void barrier() {
		__asm__ volatile ("lock; addq $0, 0(%%rsp)" : : : "memory");
	}
	
	static bool rtl_mii_read(uint8_t reg, uint16_t *data) {
		WRITE32(RTL_PHYAR, RTL_PHYAR_VALUE(0, reg, 0));
		
		for (int i = 0; i < RTL_MII_MAX_WAIT_US; i++) {
			uint32_t value = READ32(RTL_PHYAR);
			if (value & RTL_PHYAR_FLAG) {
				*data = RTL_PHYAR_DATA(value);
				return true;
			}
			Timer::microdelay(1);
		}
		
		return false;
	}
	
	static bool rtl_mii_write(uint8_t reg, uint16_t data) {
		WRITE32(RTL_PHYAR, RTL_PHYAR_VALUE(RTL_PHYAR_FLAG, reg, data));
		
		for (int i = 0; i < RTL_MII_MAX_WAIT_US; i++) {
			uint32_t value = READ32(RTL_PHYAR);
			if (!(value & RTL_PHYAR_FLAG)) {
				return true;
			}
			Timer::microdelay(1);
		}
		
		return false;
	}
	
	static bool rtl_mii_reset() {
		if (!rtl_mii_write(MII_BMCR, MII_BMCR_RESET | MII_BMCR_ANENABLE)) {
			LERROR("realtek: failed to reset MII");
			return false;
		}
		
		for (int i = 0; i < MII_RESET_MAX_WAIT_MS; i++) {
			uint16_t bmcr;
			if (!rtl_mii_read(MII_BMCR, &bmcr)) {
				LERROR("realtek: failed to read MII_BMCR");
				return false;
			}
			if (bmcr & MII_BMCR_RESET) {
				Timer::microdelay(1000);
				continue;
			}
			bmcr |= MII_BMCR_ANENABLE | MII_BMCR_ANRESTART;
			if (!rtl_mii_write(MII_BMCR, bmcr)) {
				LERROR("realtek: failed to restart MII");
				return false;
			}
			return true;
		}
		
		LERROR("realtek: MII reset timed out");
		return false;
	}
	
	static bool rtl_phy_speed() {
		uint16_t ctrl1000;
		if (!rtl_mii_read(MII_CTRL1000, &ctrl1000)) {
			LWARN("realtek: failed to read MII_CTRL1000");
			return false;
		}
		ctrl1000 |= MII_CTRL1000_ADV_1000FULL | MII_CTRL1000_ADV_1000HALF;
		if (!rtl_mii_write(MII_CTRL1000, ctrl1000)) {
			LWARN("realtek: failed to write MII_CTRL1000");
			return false;
		}
		return true;
	}
	
	static bool rtl_mii_restart() {
		uint16_t bmcr;
		if (!rtl_mii_read(MII_BMCR, &bmcr)) {
			LERROR("realtek: failed to read MII_BMCR");
			return false;
		}
		bmcr |= MII_BMCR_ANENABLE | MII_BMCR_ANRESTART;
		if (!rtl_mii_write(MII_BMCR, bmcr)) {
			LERROR("realtek: failed to restart MII");
			return false;
		}
		return true;
	}
	
	static void prepare_rx_descriptor(int i) {
		memset((void *) (uint64_t) &rx_desc[i], 0, sizeof(Descriptor));
		rx_desc[i].address = (uint64_t) rx_buffer + i * BUFFER_SIZE;
		rx_desc[i].length = BUFFER_SIZE;
		for (int j = 0; j < 233; j++) {
			rx_buffer[i * BUFFER_SIZE + j] = 0;
		}
		barrier();
		uint16_t flags = RTL_DESC_FLAG_OWN;
		if (i + 1 == N_RX_DESC) {
			flags |= RTL_DESC_FLAG_EOR;
		}
		rx_desc[i].flags = flags;
		barrier();
	}
	
	static void prepare_tx_descriptor(int i, int length) {
		memset((void *) (uint64_t) &tx_desc[i], 0, sizeof(Descriptor));
		tx_desc[i].address = (uint64_t) tx_buffer + i * BUFFER_SIZE;
		tx_desc[i].length = length;
		barrier();
		uint16_t flags = RTL_DESC_FLAG_OWN | RTL_DESC_FLAG_FS | RTL_DESC_FLAG_LS;
		if (i + 1 == N_TX_DESC) {
			flags |= RTL_DESC_FLAG_EOR;
		}
		tx_desc[i].flags = flags;
		barrier();
	}
	
	static bool rtl_init(uint8_t mac[6]) {
		// 1. realtek_init_ring
		
		rx_desc = (volatile Descriptor *) (uint64_t) DMA::alloc(N_RX_DESC * sizeof(Descriptor));
		if (!rx_desc) {
			LERROR("realtek: failed to allocate rx descriptors");
			return false;
		}
		
		tx_desc = (volatile Descriptor *) (uint64_t) DMA::alloc(N_TX_DESC * sizeof(Descriptor));
		if (!tx_desc) {
			LERROR("realtek: failed to allocate tx descriptors");
			return false;
		}
		
		rx_buffer = (volatile char *) (uint64_t) DMA::alloc(N_RX_DESC * BUFFER_SIZE);
		if (!rx_buffer) {
			LERROR("realtek: failed to allocate rx buffer");
			return false;
		}
		
		tx_buffer = (volatile char *) DMA::alloc(N_TX_DESC * BUFFER_SIZE);
		if (!tx_buffer) {
			LERROR("realtek: failed to allocate tx buffer");
			return false;
		}
		
		memset((void *) (uint64_t) rx_desc, 0, N_RX_DESC * sizeof(Descriptor));
		memset((void *) (uint64_t) tx_desc, 0, N_TX_DESC * sizeof(Descriptor));
		
		rx_idx = 0;
		tx_idx = 0;
		
		// 2. realtek_reset
		
		WRITE8(RTL_CR, RTL_CR_RST);
		
		for (int i = 0; i < RTL_RESET_MAX_WAIT_MS; i++) {
			if (!(READ8(RTL_CR) & RTL_CR_RST)) break;
			Timer::microdelay(1000);
		}
		
		if (READ8(RTL_CR) & RTL_CR_RST) {
			LERROR("realtek: reset failed");
			return false;
		}
		
		// 3. realtek_detect && get hw addr
		//   - [Set RMS]
		//   - [Check RMS]
		//   - [Set C+ Cmd Reg]
		//   - [Set hw addr]
		
		WRITE16(RTL_RMS, RTL_RX_MAX_LEN);
		if (READ16(RTL_RMS) != RTL_RX_MAX_LEN) {
			LERROR("realtek: failed to set RMS");
			return false;
		}
		
		uint16_t cpcr = READ16(RTL_CPCR);
		cpcr |= RTL_CPCR_MULRW | RTL_CPCR_CPRX | RTL_CPCR_CPTX;
		cpcr |= RTL_CPCR_DAC;
		cpcr &= ~RTL_CPCR_VLAN;
		WRITE16(RTL_CPCR, cpcr);
		
		for (int i = 0; i < 6; i++) {
			WRITE8(RTL_IDR0 + i, mac[i]);
		}
		
		// 4. realtek_phy_reset
		//   - [mii_reset]
		//   - [realtek_phy_speed][ignore errors]
		//   - [clear MII_MMD_DATA][ignore errors]
		//   - [mii_restart]
		
		if (!rtl_mii_reset()) {
			LERROR("realtek: failed to reset MII");
			return false;
		}
		
		rtl_phy_speed();
		
		rtl_mii_write(MII_MMD_DATA, 0);
		
		if (!rtl_mii_restart()) {
			LERROR("realtek: failed to restart MII");
			return false;
		}
		
		// 5. wait for link up
		//    - 6: TxFlow
		//    - 5: RxFlow
		//    - 4: 1000MF
		//    - 3: 100M
		//    - 2: 10M
		//    - 1: LinkSts
		//    - 0: FullDup
		
		LDEBUG("realtek: Waiting for link up...");
		
		uint8_t phystatus;
		while (1) {
			phystatus = READ8(RTL_PHYSTATUS);
			if (phystatus & (1 << 1)) break;
			Timer::microdelay(1000);
		}
		
		const char *speed = "???";
		if (phystatus & 0x10) {
			speed = "1000M";
		} else if (phystatus & 0x08) {
			speed = "100M";
		} else if (phystatus & 0x04) {
			speed = "10M";
		}
		
		LDEBUG("realtek: Link up (status: 0x%02x, speed: %s, duplex: %s)",
			phystatus, speed, (phystatus & 0x01) ? "full" : "half");
		
		// 6. Program ring address
		
		WRITE32(RTL_RDSAR, (uint32_t) (uint64_t) rx_desc);
		WRITE32(RTL_RDSAR + 4, (uint32_t) ((uint64_t) rx_desc >> 32));
		
		WRITE32(RTL_TNPDS, (uint32_t) (uint64_t) tx_desc);
		WRITE32(RTL_TNPDS + 4, (uint32_t) ((uint64_t) tx_desc >> 32));
		
		// 7. Accept all packets
		
		WRITE32(RTL_MAR0, 0xffffffff);
		WRITE32(RTL_MAR4, 0xffffffff);
		
		// 8. Enable transmitter and receiver
		
		WRITE8(RTL_CR, RTL_CR_TE | RTL_CR_RE);
		
		// 9. Configure receiver
		
		uint32_t rcr = READ32(RTL_RCR);
		rcr &= ~(RTL_RCR_STOP_WORKING | RTL_RCR_RXFTH_MASK | RTL_RCR_RBLEN_MASK |
			RTL_RCR_MXDMA_MASK);
		rcr |= (RTL_RCR_RXFTH_DEFAULT | RTL_RCR_RBLEN_DEFAULT | RTL_RCR_MXDMA_DEFAULT |
			RTL_RCR_WRAP | RTL_RCR_AB | RTL_RCR_AM | RTL_RCR_APM | RTL_RCR_AAP);
		WRITE32(RTL_RCR, rcr);
		
		// 10. Configure transmitter
		
		uint32_t tcr = READ32(RTL_TCR);
		tcr &= ~RTL_TCR_MXDMA_MASK;
		tcr |= RTL_TCR_MXDMA_DEFAULT;
		tcr &= ~TX_NOCRC;
		WRITE32(RTL_TCR, tcr);
		
		// 11. Fill rx descriptors
		
		for (int i = 0; i < N_RX_DESC; i++) {
			prepare_rx_descriptor(i);
		}
		
		return true;
	}
	
	struct InitOptions {
		uint32_t vendor_id, device_id;
		int region;
	};
	
	bool init(uint8_t mac[6]) {
		LDEBUG_ENTER_RET();
		
		const InitOptions options[] = {
			{ 0x10ec, 0x8168, 2 },  // RTL8111/8168
			{ 0, 0, 0 },
		};
		
		uint64_t r;
		for (int i = 0; options[i].vendor_id; i++) {
			r = PCI::map_device(options[i].vendor_id, options[i].device_id,
				(uint64_t) rtl_dev, sizeof(rtl_dev), options[i].region);
			if (r != -1ull) {
				return rtl_init(mac);
			}
		}
		
		return false;
	}
	
	int send(const void *buf, int len) {
		if (len < 0 || len > 1500) {
			return -1;
		}
		
		if (tx_desc[tx_idx].flags & RTL_DESC_FLAG_OWN) {
			return -1;
		}
		
		int idx = tx_idx;
		tx_idx = (tx_idx + 1) % N_TX_DESC;
		
		memcpy((void *) (uint64_t) (tx_buffer + idx * BUFFER_SIZE), buf, len);
		
		prepare_tx_descriptor(idx, len);
		
		WRITE8(RTL_TPOLL, RTL_TPOLL_NPQ);
		
		return 0;
	}
	
	int receive(void *buf) {
		uint16_t isr = READ16(RTL_ISR);
		WRITE16(RTL_ISR, isr);
		
		if (rx_desc[rx_idx].flags & RTL_DESC_FLAG_OWN) {
			return -1;
		}
		
		int idx = rx_idx;
		rx_idx = (rx_idx + 1) % N_RX_DESC;
		
		uint16_t flags = rx_desc[idx].flags;
		
		int length = rx_desc[idx].length & RTL_DESC_LENGTH_MASK;
		
		if (flags & (1 << 5)) {
			LERROR("realtek[%d]: Error in packet", idx);
			prepare_rx_descriptor(idx);
			return -1;
		}
		
		if (length > 1500) {
			prepare_rx_descriptor(idx);
			return -1;
		}
		
		memcpy(buf, (void *) (uint64_t) (rx_buffer + idx * BUFFER_SIZE), length);
		
		prepare_rx_descriptor(idx);
		
		return length;
	}
	
	int flush() {
		return 0;
	}
}
