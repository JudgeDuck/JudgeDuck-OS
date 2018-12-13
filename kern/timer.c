#include <kern/timer.h>
#include <inc/assert.h>

uint64_t next_timer_shot = 1000000000;
bool need_timer_shot = 0;

uint64_t tsc_when_trap_begin = 0;
// uint64_t lapic_tccr_when_trap_begin = 0;

void timer_single_shot_ns(uint64_t ns)
{
	need_timer_shot = 1;
	// cprintf("next = %lld\n", ns);
	next_timer_shot = ns;
}
void timer_single_shot_us(uint64_t us)
{
	timer_single_shot_ns(us * 1000);
}
void timer_single_shot_ms(uint64_t ms)
{
	timer_single_shot_ns(ms * 1000000);
}
void timer_single_shot_s(uint64_t s)
{
	timer_single_shot_ns(s * 1000000000);
}
