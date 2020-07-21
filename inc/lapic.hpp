#ifndef DUCK_LAPIC_H
#define DUCK_LAPIC_H

#include <stdint.h>

namespace LAPIC {
	void init();
	
	void timer_disable();
	void timer_single_shot_ns(uint64_t ns);
	void timer_periodic_ns(uint64_t ns, uint64_t cnt);
	void eoi();
}

#endif
