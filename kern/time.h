#ifndef JOS_KERN_TIME_H
#define JOS_KERN_TIME_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/types.h>

void time_init(void);
void time_tick(void);
unsigned int time_msec(void);

uint64_t get_tsc_frequency();
void set_tsc_frequency(uint64_t freq);

extern volatile int tsc_measurement_running;
void tsc_measurement_start();
void tsc_measurement_end();

#endif /* JOS_KERN_TIME_H */
