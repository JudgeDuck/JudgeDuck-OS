#include <inc/lib.h>

const char *child_argv[32];

void
umain(int argc, char **argv)
{
	if(argc != 2)
	{
		cprintf("ARBITER: Usage: %s [program to judge]\n", argv[0]);
		return;
	}

	cprintf("ARBITER: Spawning %s...\n", argv[1]);
	
	envid_t env = spawn(argv[1], child_argv);
	if(env < 0)
	{
		cprintf("ARBITER: Spawning failed...\n");
		sys_env_destroy(0);
	}

	while(1)
	{
		cprintf("ARBITER: before sys_accept_enter_judge......\n");
		struct JudgeParams prm;
		struct JudgeResult res;
		prm.ms = 5000;
		int ret = sys_accept_enter_judge(env, &prm, &res);
		cprintf("ARBITER: sys_accept_enter_judge returned %d\n", ret);
		if(!ret)
		{
			static const char *verdict_str[] = {"OK", "Time Limit Exceeded", "Runtime Error", "Illegal Syscall", "System Error"};
			cprintf("ARBITER: result verdict = %s\n", verdict_str[(int) res.verdict]);
			cprintf("ARBITER: result time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
			cprintf("ARBITER: result time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
			break;
		}
		sys_yield();
	}
	sys_env_destroy(0);
}
