#ifndef JOS_INC_TIMER_H
#define JOS_INC_TIMER_H

#include <inc/types.h>

void lapic_timer_single_shot(uint64_t ns);

void timer_single_shot_ns(uint64_t);
void timer_single_shot_us(uint64_t);
void timer_single_shot_ms(uint64_t);
void timer_single_shot_s(uint64_t);

extern uint64_t next_timer_shot;

#define DEFAULT_TIMER_INTERVAL 1000000000

#endif
