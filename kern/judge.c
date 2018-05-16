#include <inc/x86.h>
#include <kern/pmap.h>
#include <kern/judge.h>
#include <kern/env.h>
#include <kern/timer.h>
#include <kern/sched.h>

void
finish_judge(int verdict)
{
	curenv->env_judging = 0;
	curenv->env_tf = curenv->env_judge_tf;
	
	lcr3(PADDR(judger_env->env_pgdir));
	judger_env->env_judge_res->time_cycles += tsc_when_trap_begin;
	judger_env->env_judge_res->time_ns -= lapic_tccr_when_trap_begin;
	judger_env->env_judge_res->verdict = verdict;
	lcr3(PADDR(curenv->env_pgdir));
	
	sched_yield(); // won't return
}
