#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <inc/lapic.hpp>
#include <inc/memory.hpp>
#include <inc/pic.hpp>
#include <inc/x86_64.hpp>
#include <inc/smbios.hpp>

using x86_64::inb;
using x86_64::outb;
using PIC::IRQ_OFFSET;
using PIC::IRQ_TIMER;

// Local APIC registers, divided by 4 for use as uint32_t[] indices.
#define ID      (0x0020/4)   // ID
#define VER     (0x0030/4)   // Version
#define TPR     (0x0080/4)   // Task Priority
#define EOI     (0x00B0/4)   // EOI
#define SVR     (0x00F0/4)   // Spurious Interrupt Vector
	#define ENABLE     0x00000100   // Unit Enable

#define ESR     (0x0280/4)   // Error Status
#define ICRLO   (0x0300/4)   // Interrupt Command
	#define INIT       0x00000500   // INIT/RESET
	#define STARTUP    0x00000600   // Startup IPI
	#define DELIVS     0x00001000   // Delivery status
	#define ASSERT     0x00004000   // Assert interrupt (vs deassert)
	#define DEASSERT   0x00000000
	#define LEVEL      0x00008000   // Level triggered
	#define BCAST      0x00080000   // Send to all APICs, including self.
	#define OTHERS     0x000C0000   // Send to all APICs, excluding self.
	#define BUSY       0x00001000
	#define FIXED      0x00000000

#define ICRHI   (0x0310/4)   // Interrupt Command [63:32]
#define TIMER   (0x0320/4)   // Local Vector Table 0 (TIMER)
	#define X1         0x0000000B   // divide counts by 1
	#define X128       0x0000000A   // divide counts by 128
	#define SINGLESHOT 0x00000000   // Single shot
	#define PERIODIC   0x00020000   // Periodic

#define PCINT   (0x0340/4)   // Performance Counter LVT
#define LINT0   (0x0350/4)   // Local Vector Table 1 (LINT0)
#define LINT1   (0x0360/4)   // Local Vector Table 2 (LINT1)
#define ERROR   (0x0370/4)   // Local Vector Table 3 (ERROR)
	#define MASKED     0x00010000   // Interrupt masked

#define TICR    (0x0380/4)   // Timer Initial Count
#define TCCR    (0x0390/4)   // Timer Current Count
#define TDCR    (0x03E0/4)   // Timer Divide Configuration

#define TIMER_EX 1
#define TIMER_EX_T X1

namespace LAPIC {
	
	struct MP {                         // floating pointer [MP 4.1]
		uint8_t signature[4];           // "_MP_"
		uint32_t physaddr;              // phys addr of MP config table
		uint8_t length;                 // 1
		uint8_t specrev;                // [14]
		uint8_t checksum;               // all bytes must add up to 0
		uint8_t type;                   // MP system config type
		uint8_t imcrp;
		uint8_t reserved[3];
	} __attribute__((packed));
	
	struct MPConf {                     // configuration table header [MP 4.2]
		uint8_t signature[4];           // "PCMP"
		uint16_t length;                // total table length
		uint8_t version;                // [14]
		uint8_t checksum;               // all bytes must add up to 0
		uint8_t product[20];            // product id
		uint32_t oemtable;              // OEM table pointer
		uint16_t oemlength;             // OEM table length
		uint16_t entry;                 // entry count
		uint32_t lapicaddr;             // address of local APIC
		uint16_t xlength;               // extended table length
		uint8_t xchecksum;              // extended table checksum
		uint8_t reserved;
		uint8_t entries[0];             // table entries
	} __attribute__((packed));
	
	static uint8_t sum(void *addr, int len) {
		uint8_t sum = 0;
		for (int i = 0; i < len; i++) {
			sum += ((uint8_t *) addr) [i];
		}
		return sum;
	}
	
	// Look for an MP structure in the len bytes at physical address addr.
	static MP * mpsearch1(void *a, int len) {
		MP *mp = (MP *) a, *end = (MP *) ((char *) a + len);
		while (mp < end) {
			if (memcmp(mp->signature, "_MP_", 4) == 0 && sum(mp, sizeof(*mp)) == 0) {
				return mp;
			}
			mp++;
		}
		return NULL;
	}

	// Search for the MP Floating Pointer Structure, which according to
	// [MP 4] is in one of the following three locations:
	// 1) in the first KB of the EBDA;
	// 2) if there is no EBDA, in the last KB of system base memory;
	// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
	static MP * mpsearch() {
		// The BIOS data area lives in 16-bit segment 0x40.
		uint8_t *bda = (uint8_t *) (uint64_t) (0x40 << 4);
		uint32_t p;
		MP *mp;
		
		// [MP 4] The 16-bit segment of the EBDA is in the two bytes
		// starting at byte 0x0E of the BDA.  0 if not present.
		if ((p = * (uint16_t *) (bda + 0x0E))) {
			p <<= 4;	// Translate from segment to PA
			if ((mp = mpsearch1((void *) (uint64_t) p, 1024))) {
				return mp;
			}
		} else {
			// The size of base memory, in KB is in the two bytes
			// starting at 0x13 of the BDA.
			p = * (uint16_t *) (bda + 0x13) * 1024;
			if ((mp = mpsearch1((void *) (uint64_t) (p - 1024), 1024))) {
				return mp;
			}
		}
		return mpsearch1((void *) (uint64_t) 0xF0000, 0x10000);
	}

	// Search for an MP configuration table.  For now, don't accept the
	// default configurations (physaddr == 0).
	// Check for the correct signature, checksum, and version.
	static MPConf * mpconfig(MP **pmp) {
		MPConf *conf;
		MP *mp;
		if ((mp = mpsearch()) == 0) return NULL;
		if (mp->physaddr == 0 || mp->type != 0) {
			printf("SMP: Default configurations not implemented\n");
			return NULL;
		}
		conf = (MPConf *) (uint64_t) mp->physaddr;
		if (memcmp(conf, "PCMP", 4) != 0) {
			printf("SMP: Incorrect MP configuration table signature\n");
			return NULL;
		}
		if (sum(conf, conf->length) != 0) {
			printf("SMP: Bad MP configuration checksum\n");
			return NULL;
		}
		if (conf->version != 1 && conf->version != 4) {
			printf("SMP: Unsupported MP version %d\n", conf->version);
			return NULL;
		}
		if ((sum((uint8_t *) conf + conf->length, conf->xlength) + conf->xchecksum) & 0xff) {
			printf("SMP: Bad MP configuration extended checksum\n");
			return NULL;
		}
		*pmp = mp;
		return conf;
	}
	
	static volatile uint32_t *lapic;
	
	static inline void lapicw(int index, int value) {
		lapic[index] = value;
		lapic[ID];  // wait for write to finish, by reading
	}
	
	void timer_disable() {
		lapicw(TICR, -1);
	}
	
	void timer_single_shot_ns(uint64_t ns) {
		uint64_t to_set = ns * SMBIOS::ext_clock_freq / TIMER_EX / 1000;
		assert(to_set < (uint64_t) 4000000000);
		lapicw(TICR, to_set);
	}
	
	void eoi() {
		lapicw(EOI, 0);
	}
	
	void init() {
		printf("LAPIC::init()\n");
		
		MP *mp;
		MPConf *conf = mpconfig(&mp);
		assert(conf != NULL);
		
		lapic = Memory::remap((volatile uint32_t *) (uint64_t) conf->lapicaddr);
		printf("remapped lapic = %p\n", lapic);
		
		// Enable local APIC; mask spurious interrupt vector.
		lapicw(SVR, ENABLE | MASKED);
		
		// The timer repeatedly counts down at bus frequency
		// from lapic[TICR] and then issues an interrupt.  
		// If we cared more about precise timekeeping,
		// TICR would be calibrated using an external time source.
		lapicw(TDCR, TIMER_EX_T);
		lapicw(TIMER, SINGLESHOT | (IRQ_OFFSET + IRQ_TIMER));
		
		// Leave LINT0 of the BSP enabled so that it can get
		// interrupts from the 8259A chip.
		//
		// According to Intel MP Specification, the BIOS should initialize
		// BSP's local APIC in Virtual Wire Mode, in which 8259A's
		// INTR is virtually connected to BSP's LINTIN0. In this mode,
		// we do not need to program the IOAPIC.
		// No need in single-core OS.
		
		// Disable NMI (LINT1) on all CPUs
		lapicw(LINT1, MASKED);
		
		// Disable performance counter overflow interrupts
		// on machines that provide that interrupt entry.
		if (((lapic[VER] >> 16) & 0xFF) >= 4) {
			lapicw(PCINT, MASKED);
		}
		
		// Mask error interrupt
		lapicw(ERROR, MASKED);
		
		// Clear error status register (requires back-to-back writes).
		lapicw(ESR, 0);
		lapicw(ESR, 0);

		// Ack any outstanding interrupts.
		lapicw(EOI, 0);

		// Send an Init Level De-Assert to synchronize arbitration ID's.
		lapicw(ICRHI, 0);
		lapicw(ICRLO, BCAST | INIT | LEVEL);
		while (lapic[ICRLO] & DELIVS);

		// Enable interrupts on the APIC (but not on the processor).
		lapicw(TPR, 0);
	}
}
