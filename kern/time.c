#include <kern/time.h>
#include <inc/assert.h>
#include <inc/types.h>
#include <inc/stdio.h>
#include <kern/timer.h>

static unsigned int ticks;

volatile int tsc_measurement_running;

static uint64_t tsc_start;
static uint64_t tsc_end;
static uint64_t tsc_freq;

static inline uint64_t read_tsc() {
	uint64_t ret;
	__asm__ volatile ("rdtsc" : "=A"(ret));
	return ret;
}

void tsc_measurement_start() {
	cprintf("Detecting tsc freq ... ");
	tsc_start = read_tsc();
	lapic_timer_single_shot(1000000000);
	tsc_measurement_running = 1;
	__asm__ volatile("sti");
	while (1);
}

void tsc_measurement_end() {
	tsc_end = read_tsc();
	tsc_freq = tsc_end - tsc_start;
	tsc_measurement_running = 0;
}

uint64_t get_tsc_frequency() {
	return tsc_freq;
}

void
time_init(void)
{
	ticks = 0;
}

// This should be called once per timer interrupt.  A timer interrupt
// fires every 10 ms.
void
time_tick(void)
{
	ticks++;
	if (ticks * 10 < ticks)
		panic("time_tick: time overflowed");
}

unsigned int
time_msec(void)
{
	return ticks * 10;
}
