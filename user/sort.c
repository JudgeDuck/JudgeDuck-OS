#include <inc/lib.h>

char TMPSTK[1048576];

void
sort_wrapper(void)
{
	cprintf("CHILD:   I am sort_wrapper!\n");
	// while(1);
	unsigned sum = 0;
	unsigned n = 20000;
	for(unsigned i = 0; i < n; i++)
		for(unsigned j = 0; j < n; j++)
			sum += i + j;
	cprintf("CHILD:   sum = %u\n", sum);
	sys_quit_judge();
}

void
umain(int argc, char **argv)
{
	cprintf("CHILD:   I am the child.\n");
	cprintf("CHILD:   before sys_enter_judge......\n");
	int ret = sys_enter_judge(sort_wrapper, TMPSTK + sizeof(TMPSTK));
	cprintf("CHILD:   sys_enter_judge returned %d!\n", ret);
}
