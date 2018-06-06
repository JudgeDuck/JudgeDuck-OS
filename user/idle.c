// idle loop

#include <inc/x86.h>
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	binaryname = "idle";

	// cprintf("idle!\n");
	// Loop forever, simply trying to yield to a different environment.
	// Instead of busy-waiting like this,
	// a better way would be to use the processor's HLT instruction
	// to cause the processor to stop executing until the next interrupt -
	// doing so allows the processor to conserve power more effectively.
	while(1)
	{
		// cprintf("idle!\n");
		for(int i = 0; i < 20; i++) sys_yield();
		// cprintf("idle!\n");
		sys_halt();
	}
}

