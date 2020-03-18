#ifndef DUCK_PIC_H
#define DUCK_PIC_H

namespace PIC {
	const int IRQ_OFFSET = 32;      // IRQ 0 corresponds to int IRQ_OFFSET
	
	// Hardware IRQ numbers. We receive these as (IRQ_OFFSET+IRQ_WHATEVER)
	const int IRQ_TIMER     =  0;
	const int IRQ_KBD       =  1;
	const int IRQ_SERIAL    =  4;
	const int IRQ_SPURIOUS  =  7;
	const int IRQ_IDE       = 14;
	const int IRQ_ERROR     = 19;
	
	void init();
	void eoi();
	void enable(int irq);
	void disable(int irq);
}

#endif
