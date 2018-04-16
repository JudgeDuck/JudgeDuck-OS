// Called from entry.S to get us going.
// entry.S already took care of defining envs, pages, uvpd, and uvpt.

#include <inc/lib.h>

extern void umain(int argc, char **argv);

const volatile struct Env *thisenv;
const char *binaryname = "<unknown>";

void
libmain(int argc, char **argv)
{
	// set thisenv to point at our Env structure in envs[].
	// LAB 3: Your code here.
	//extern struct Env envs[];
	thisenv = NULL;
	envid_t cid = sys_getenvid();
	for(int i = 0; i < NENV; i++)
		if(envs[i].env_id == cid)
		{
			thisenv = envs + i;
			break;
		}
	cprintf("envs %p, thisenv %p\n", envs, thisenv);

	// save the name of the program so that panic() can use it
	if (argc > 0)
		binaryname = argv[0];

	// call user main routine
	umain(argc, argv);

	// exit gracefully
	exit();
}

