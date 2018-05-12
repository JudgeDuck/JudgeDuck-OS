// Test preemption by forking off a child process that just spins forever.
// Let it run for a couple time slices, then kill it.

#include <inc/lib.h>
#include <inc/x86.h>

char TMPSTK[1048576];

void
sort_wrapper(void)
{
	cprintf("CHILD:  I am sort_wrapper!\n");
	unsigned sum = 0;
	for(unsigned i = 0; i < 100000000; i++) sum += i;
	cprintf("CHILD:  sum = %u\n", sum);
	sys_quit_judge();
}

void
umain(int argc, char **argv)
{
	envid_t env;

	cprintf("PARENT: I am the parent.  Forking the child...\n");
	if ((env = fork()) == 0) {
		cprintf("CHILD:  I am the child.\n");
		cprintf("CHILD:  before sys_enter_judge......\n");
		int ret = sys_enter_judge(sort_wrapper, TMPSTK + sizeof(TMPSTK));
		cprintf("CHILD:  sys_enter_judge returned %d!\n", ret);
		sys_env_destroy(0);
	}
	
	if(fork() == 0)
	{
		while(1)
		{
			cprintf("HAHA:   test\n");
			sys_yield();
		}
	}
	if(fork() == 0)
	{
		while(1);
	}

	cprintf("PARENT: I am the parent.\n");
	while(1)
	{
		cprintf("PARENT: before sys_accept_enter_judge......\n");
		struct JudgeParams prm;
		struct JudgeResult res;
		prm.ms = 5000;
		int ret = sys_accept_enter_judge(env, &prm, &res);
		cprintf("PARENT: sys_accept_enter_judge returned %d\n", ret);
		cprintf("PARENT: result verdict = %d\n", (int) res.verdict);
		cprintf("PARENT: result time_cycles = %lld\n", res.time_cycles);
		if(!ret && res.verdict == VERDICT_OK) break;
		sys_yield();
	}
	sys_env_destroy(0);
}
