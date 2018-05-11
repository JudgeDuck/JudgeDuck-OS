// Test preemption by forking off a child process that just spins forever.
// Let it run for a couple time slices, then kill it.

#include <inc/lib.h>
#include <inc/x86.h>

void
sort_wrapper(void)
{
	cprintf("I am sort_wrapper!\n");
	while(1);
}

void
umain(int argc, char **argv)
{
	envid_t env;

	cprintf("I am the parent.  Forking the child...\n");
	if ((env = fork()) == 0) {
		cprintf("I am the child.\n");
		sys_enter_judge(sort_wrapper, (void *) USTACKTOP);
	}

	cprintf("I am the parent.\n");
	while(1)
	{
		cprintf("PARENT: before sys_accept_enter_judge......\n");
		int ret = sys_accept_enter_judge(env, 500000, NULL);
		cprintf("PARENT: sys_accept_enter_judge returned %d\n", ret);
		sys_yield();
	}
	sys_env_destroy(env);
}
