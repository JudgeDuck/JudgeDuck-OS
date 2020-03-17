#ifndef DUCK_LAPIC_H
#define DUCK_LAPIC_H

#include <stdint.h>

namespace LAPIC {
	void init();
	
	void timer_disable();
	void timer_single_shot_ns(uint64_t ns);
	void eoi();
}

#endif
