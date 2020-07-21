#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <inc/lapic.hpp>
#include <inc/memory.hpp>
#include <inc/pic.hpp>
#include <inc/x86_64.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/utils.hpp>

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
	
	struct ACPI_RDSP {
		uint8_t signature[8];
		uint8_t checksum;
		uint8_t oem_id[6];
		uint8_t revision;
		uint32_t rsdt_addr_phys;
		uint32_t length;
		uint64_t xsdt_addr_phys;
		uint8_t xchecksum;
		uint8_t reserved[3];
	} __attribute__((packed));
	
	struct ACPI_desc_header {
		uint8_t signature[4];
		uint32_t length;
		uint8_t revision;
		uint8_t checksum;
		uint8_t oem_id[6];
		uint8_t oem_tableid[8];
		uint32_t oem_revision;
		uint8_t creator_id[4];
		uint32_t creator_revision;
	} __attribute__((packed));
	
	struct ACPI_RSDT {
		struct ACPI_desc_header header;
		uint32_t entry[0];
	} __attribute__((packed));
	
	struct ACPI_MADT {
		struct ACPI_desc_header header;
		uint32_t lapic_addr_phys;
		uint32_t flags;
		uint8_t table[0];
	} __attribute__((packed));
	
	struct ACPI_FADT {
		struct   ACPI_desc_header header;
		uint32_t FirmwareCtrl;
		uint32_t DSDT;
		uint8_t  Reserved;
		uint8_t  PreferredPowerManagementProfile;
		uint16_t SCI_Interrupt;
		uint32_t SMI_CommandPort;
		uint8_t  ACPI_Enable;
		uint8_t  ACPI_Disable;
		uint8_t  S4BIOS_REQ;
		uint8_t  PSTATE_Control;
		uint32_t PM1aEventBlock;
		uint32_t PM1bEventBlock;
		uint32_t PM1aControlBlock;
		// more fields are omitted
	} __attribute__((packed));
	
	static uint8_t sum(void *addr, int len) {
		uint8_t sum = 0;
		for (int i = 0; i < len; i++) {
			sum += ((uint8_t *) addr) [i];
		}
		return sum;
	}
	
	static void switch_to_acpi_mode(ACPI_FADT *fadt) {
		if (fadt->SMI_CommandPort == 0) {
			return;
		}
		
		if (fadt->ACPI_Enable == 0) {
			return;
		}
		
		x86_64::outb(fadt->SMI_CommandPort, fadt->ACPI_Enable);
		while ((x86_64::inw(fadt->PM1aControlBlock) & 1) == 0);
		
		LDEBUG("Switched to ACPI mode");
	}
	
	static ACPI_RDSP * scan_rdsp(uint32_t base, uint32_t len) {
		for (char *p = (char *) (uint64_t) base; len >= sizeof(ACPI_RDSP); len -= 4, p += 4) {
			if (memcmp(p, "RSD PTR ", 8) == 0 && sum(p, 20) == 0) {
				return (ACPI_RDSP *) p;
			}
		}
		return (ACPI_RDSP *) 0;  
	}
	
	static ACPI_RDSP * find_rdsp() {
		ACPI_RDSP *rdsp;
		uint32_t pa;
		pa = * ((uint16_t *) (uint64_t) 0x40e) << 4;
		if (pa && (rdsp = scan_rdsp(pa, 1024))) {
			return rdsp;
		}
		return scan_rdsp(0xe0000, 0x20000);
	}
	
	volatile uint32_t *lapic;
	
	static inline void lapicw(int index, int value) {
		lapic[index] = value;
		lapic[ID];  // wait for write to finish, by reading
	}
	
	void timer_disable() {
		lapicw(TICR, -1);
	}
	
	extern "C" {
		extern uint64_t __lapic_timer_cnt;
	}
	
	void timer_single_shot_ns(uint64_t ns) {
		uint64_t to_set = (__uint128_t) ns * Timer::ext_freq / ((uint64_t) TIMER_EX * 1000000000);
		assert(to_set < (uint64_t) 4000000000);
		__lapic_timer_cnt = 1;
		lapicw(TIMER, SINGLESHOT | (IRQ_OFFSET + IRQ_TIMER));
		lapicw(TICR, to_set);
	}
	
	void timer_periodic_ns(uint64_t ns, uint64_t cnt) {
		uint64_t to_set = (__uint128_t) ns * Timer::ext_freq / ((uint64_t) TIMER_EX * 1000000000);
		assert(to_set < (uint64_t) 4000000000);
		__lapic_timer_cnt = cnt;
		lapicw(TIMER, PERIODIC | (IRQ_OFFSET + IRQ_TIMER));
		lapicw(TICR, to_set);
	}
	
	void eoi() {
		lapicw(EOI, 0);
	}
	
	static void detect_ext_freq() {
		// TODO: Detect ext freq from tsc freq
		unimplemented();
	}
	
	void init() {
		LDEBUG_ENTER_RET();
		
		ACPI_RDSP *rdsp = find_rdsp();
		ACPI_RSDT *rsdt = (ACPI_RSDT *) (uint64_t) (rdsp->rsdt_addr_phys);
		ACPI_MADT *madt = 0;
		ACPI_FADT *fadt = 0;
		int count = (rsdt->header.length - sizeof(ACPI_RSDT)) / 4;
		for (int i = 0; i < count; i++) {
			ACPI_desc_header *hdr = (ACPI_desc_header *) (uint64_t) rsdt->entry[i];
			if (memcmp(hdr->signature, "APIC", 4) == 0) {
				madt = (ACPI_MADT *) hdr;
			} else if (memcmp(hdr->signature, "FACP", 4) == 0) {
				fadt = (ACPI_FADT *) hdr;
			}
		}
		assert(madt != NULL);
		
		if (fadt) {
			switch_to_acpi_mode(fadt);
		}
		
		lapic = Memory::remap((volatile uint32_t *) (uint64_t) madt->lapic_addr_phys);
		LDEBUG("remapped lapic = %p", lapic);
		
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
		
		if (Timer::ext_freq == 0) {
			detect_ext_freq();
		}
	}
}
