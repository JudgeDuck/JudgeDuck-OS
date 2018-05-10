#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int ret = fork();
	cprintf("asdfasdfklja;skldfjl;ajsl;dfj;alskjdf;klajs;dfj;alskdjfl;ja;sdlfjasdkl;fj fork returned %x\n", ret);
}

