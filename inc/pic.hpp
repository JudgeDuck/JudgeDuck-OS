#ifndef DUCK_PIC_H
#define DUCK_PIC_H

#include <stdint.h>

namespace PIC {
	const uint8_t IRQ_OFFSET = 32;      // IRQ 0 corresponds to uint8_t IRQ_OFFSET
	
	// Hardware IRQ numbers. We receive these as (IRQ_OFFSET+IRQ_WHATEVER)
	const uint8_t IRQ_TIMER     =  0;
	const uint8_t IRQ_KBD       =  1;
	const uint8_t IRQ_SERIAL    =  4;
	const uint8_t IRQ_SPURIOUS  =  7;
	const uint8_t IRQ_IDE       = 14;
	const uint8_t IRQ_ERROR     = 19;
	
	void init();
	void eoi();
	void enable(uint8_t irq);
	void disable(uint8_t irq);
}

#endif
