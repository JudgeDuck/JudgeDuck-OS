#ifndef JOS_KERN_TIMER_H
#define JOS_KERN_TIMER_H

#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/types.h>

void lapic_timer_single_shot(uint64_t ns);
void lapic_timer_disable();
uint64_t lapic_timer_current_count();

void timer_single_shot_ns(uint64_t);
void timer_single_shot_us(uint64_t);
void timer_single_shot_ms(uint64_t);
void timer_single_shot_s(uint64_t);

extern uint64_t next_timer_shot;
extern bool need_timer_shot;
extern uint64_t tsc_when_trap_begin;
extern uint64_t lapic_tccr_when_trap_begin;

// #define DEFAULT_TIMER_INTERVAL ((uint64_t) 500000000000ll)
#define DEFAULT_TIMER_INTERVAL ((uint64_t) 1000000000)

#endif
