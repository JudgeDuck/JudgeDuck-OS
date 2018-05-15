// Test preemption by forking off a child process that just spins forever.
// Let it run for a couple time slices, then kill it.

#include <inc/lib.h>
#include <inc/x86.h>

char TMPSTK[1048576];

void
sort_wrapper(void)
{
	cprintf("CHILD:  I am sort_wrapper!\n");
	// while(1);
	unsigned sum = 0;
	unsigned n = 20000;
	for(unsigned i = 0; i < n; i++)
		for(unsigned j = 0; j < n; j++)
			sum += i + j;
	cprintf("CHILD:  sum = %u\n", sum);
	sys_quit_judge();
}

void
umain(int argc, char **argv)
{
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
	
	envid_t env;

	cprintf("PARENT: I am the parent.  Forking the child...\n");
	if ((env = fork()) == 0) {
		cprintf("CHILD:  I am the child.\n");
		cprintf("CHILD:  before sys_enter_judge......\n");
		int ret = sys_enter_judge(sort_wrapper);
		cprintf("CHILD:  sys_enter_judge returned %d!\n", ret);
		sys_env_destroy(0);
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
		if(!ret)
		{
			static const char *verdict_str[] = {"OK", "Time Limit Exceeded", "Runtime Error", "Illegal Syscall", "System Error"};
			cprintf("PARENT: result verdict = %s\n", verdict_str[(int) res.verdict]);
			cprintf("PARENT: result time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
			cprintf("PARENT: result time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
			break;
		}
		sys_yield();
	}
	sys_env_destroy(0);
}
