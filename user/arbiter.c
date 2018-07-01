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
	if(argc < 2)
	{
		cprintf("ARBITER: Usage: %s [program] [extra params]\n", argv[0]);
		return;
	}

	cprintf("ARBITER: spawning %s...\n", argv[1]);
	
	envid_t env = spawn(argv[1], (const char **) (argv + 1));
	if(env < 0)
	{
		panic("ARBITER: Spawning failed...\n");
		exit();
	}

	for(int i = 0; i < 5; i++)
	{
		struct JudgeResult res;
		cprintf("ARBITER: before sys_accept_enter_judge......\n");
		int ret = sys_accept_enter_judge(env, &res);
		cprintf("ARBITER: sys_accept_enter_judge returned %d\n", ret);
		if(!ret)
		{
			static const char *verdict_str[] = {"Run Finished", "Time Limit Exceeded", "Runtime Error", "Illegal Syscall", "System Error", "Memory Limit Exceeded"};
			cprintf("ARBITER: verdict = %s\n", verdict_str[(int) res.verdict]);
			printf("verdict = %s\n", verdict_str[(int) res.verdict]);
			if(res.verdict == VERDICT_IS)
			{
				cprintf("ARBITER: syscall_id = %d\n", res.tf.tf_regs.reg_eax);
				printf("syscall_id = %d\n", res.tf.tf_regs.reg_eax);
			}
			if(res.verdict == VERDICT_RE)
			{
				cprintf("ARBITER: runtime error %d (%s)\n", res.tf.tf_trapno, trapname(res.tf.tf_trapno));
				printf("runtime error %d (%s)\n", res.tf.tf_trapno, trapname(res.tf.tf_trapno));
			}
			cprintf("ARBITER: time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
			cprintf("ARBITER: time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
			cprintf("ARBITER: mem_kb = %d\n", res.mem_kb);
			printf("time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
			printf("time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
			printf("mem_kb = %d\n", res.mem_kb);
			break;
		}
		for(int i = 0; i < 50; i++) sys_yield();
	}
	wait(env);
	int fd = open("arbiter.in", O_RDWR);
	char buf[256];
	int r = read(fd, buf, sizeof(buf));
	if(r > 0) write(1, buf, r);
	close(fd);
	exit();
}
