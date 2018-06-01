#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int a = argv[1][0] - '0';
	float c = 1.6 * a;
	cprintf("floor(1.6 * %d) = %d\n", a, (int) c);
}
