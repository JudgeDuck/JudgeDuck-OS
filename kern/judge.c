#include <inc/x86.h>
#include <inc/trap.h>
#include <kern/pmap.h>
#include <kern/judge.h>
#include <kern/env.h>
#include <kern/timer.h>
#include <kern/sched.h>
#include <kern/pmap.h>

void
finish_judge(int verdict)
{
	struct Trapframe tf = curenv->env_tf;
	curenv->env_judging = 0;
	curenv->env_tf = curenv->env_judge_tf;
	
	pgdir_reperm(curenv->env_pgdir, PTE_TDW, PTE_W, NULL, (void *) UTOP);
	int pgs = pgdir_reperm(curenv->env_pgdir, PTE_D, 0, NULL, (void *) UTOP);
	
	lcr3(PADDR(judger_env->env_pgdir));
	judger_env->env_judge_res->time_cycles += tsc_when_trap_begin;
	judger_env->env_judge_res->time_ns -= lapic_tccr_when_trap_begin;
	judger_env->env_judge_res->verdict = verdict;
	judger_env->env_judge_res->tf = tf;
	judger_env->env_judge_res->mem_kb = pgs * PGSIZE / 1024;
	lcr3(PADDR(curenv->env_pgdir));
	
	sched_yield(); // won't return
}
