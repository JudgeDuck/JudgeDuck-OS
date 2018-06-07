#include <inc/lib.h>
void
umain(int argc, char **argv)
{
	for(;;)
	{
		int r = spawnl("judged", "judged");
		if(r < 0)
		{
			cprintf("spawn error\n");
			return;
		}
		wait(r);
		for(int i = 0; i < 50; i++) sys_yield();
	}
}
