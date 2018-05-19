#include <inc/lib.h>

const char *child_argv[32];

int
atoi(const char *s)
{
	int ans = 0;
	while(*s) ans = ans * 10 + (*s++ - '0');
	return ans;
}

static const char *trapname(int trapno)
{
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < ARRAY_SIZE(excnames))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return "Hardware Interrupt";
	return "(unknown trap)";
}

void
umain(int argc, char **argv)
{
	if(argc != 5)
	{
		cprintf("ARBITER: Usage: %s [program] [time_ms] [memory_kb] [defrag_mem]\n", argv[0]);
		return;
	}
	int time_ms = atoi(argv[2]);
	int mem_kb = atoi(argv[3]);
	int defrag_mem = atoi(argv[4]);
	cprintf("ARBITER: got TL %d ms, ML %d KB, defrag\n", time_ms, mem_kb, defrag_mem);

	cprintf("ARBITER: spawning %s...\n", argv[1]);
	
	envid_t env = spawn(argv[1], child_argv);
	if(env < 0)
	{
		cprintf("ARBITER: Spawning failed...\n");
		sys_env_destroy(0);
	}

	while(1)
	{
		struct JudgeParams prm;
		prm.ms = time_ms;
		prm.kb = mem_kb;
		prm.defrag_mem = defrag_mem;
		memset(prm.syscall_enabled, 0, sizeof(prm.syscall_enabled));
		prm.syscall_enabled[SYS_quit_judge] = 1;
		prm.syscall_enabled[SYS_cputs] = 1;
		
		struct JudgeResult res;
		cprintf("ARBITER: before sys_accept_enter_judge......\n");
		int ret = sys_accept_enter_judge(env, &prm, &res);
		cprintf("ARBITER: sys_accept_enter_judge returned %d\n", ret);
		if(!ret)
		{
			static const char *verdict_str[] = {"OK", "Time Limit Exceeded", "Runtime Error", "Illegal Syscall", "System Error"};
			cprintf("ARBITER: verdict = %s\n", verdict_str[(int) res.verdict]);
			if(res.verdict == VERDICT_IS)
				cprintf("ARBITER: syscall_id = %d\n", res.tf.tf_regs.reg_eax);
			if(res.verdict == VERDICT_RE)
			{
				cprintf("ARBITER: runtime error %d (%s)\n", res.tf.tf_trapno, trapname(res.tf.tf_trapno));
			}
			cprintf("ARBITER: time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
			cprintf("ARBITER: time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
			cprintf("ARBITER: mem_kb = %d\n", res.mem_kb);
			break;
		}
		sys_yield();
	}
	wait(env);
	exit();
}
