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
	// Tell arbiter I'm done
	void *judge_page = (void *) page2pa(get_first_judge_page());
	boot_map_region(curenv->env_pgdir, 0, PGSIZE, (uint32_t) judge_page, PTE_P | PTE_W);
	lcr3(PADDR(curenv->env_pgdir));
	invlpg(0);
	volatile unsigned *contestant_done = (volatile unsigned *) 0x108;
	*contestant_done = 1;
	
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
	judger_env->env_judge_res->mem_kb = pgs * (PGSIZE / 1024);
	if (verdict == VERDICT_OK && judger_env->env_judge_res->mem_kb > judger_env->env_judge_prm.kb) {
		judger_env->env_judge_res->verdict = VERDICT_MLE;
	}
	lcr3(PADDR(curenv->env_pgdir));
	
	lapic_timer_single_shot(DEFAULT_TIMER_INTERVAL);
	
	sched_yield(); // won't return
}
