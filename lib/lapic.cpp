#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <inc/lapic.hpp>
#include <inc/acpi.hpp>
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
		
		lapic = Memory::remap((volatile uint32_t *) ACPI::get_lapic_addr());
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
