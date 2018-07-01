#include <inc/lib.h>
void
umain(int argc, char **argv)
{
	for(;;)
	{
		for(int i = 0; i < 50; i++) sys_yield();
		int r = spawnl("judged", "judged", 0);
		if(r < 0)
		{
			cprintf("spawn error\n");
			return;
		}
		wait(r);
	}
}
