/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/judge.h>
#include <inc/ns.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>
#include <kern/sched.h>
#include <kern/timer.h>
#include <kern/judge.h>
#include <kern/time.h>
#include <kern/e1000.h>

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void
sys_cputs(const char *s, size_t len)
{
	// Check that the user has permission to read memory [s, s+len).
	// Destroy the environment if not.

	// LAB 3: Your code here.
	user_mem_assert(curenv, s, len, PTE_U);

	// Print the string supplied by the user.
	cprintf("%.*s", len, s);
}

// Read a character from the system console without blocking.
// Returns the character, or 0 if there is no input waiting.
static int
sys_cgetc(void)
{
	return cons_getc();
}

// Returns the current environment's envid.
static envid_t
sys_getenvid(void)
{
	return curenv->env_id;
}

// Destroy a given environment (possibly the currently running environment).
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_destroy(envid_t envid)
{
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0)
		return r;
	env_destroy(e);
	return 0;
}

// Deschedule current environment and pick a different one to run.
static void
sys_yield(void)
{
	sched_yield();
	// sched_halt();
}
static void
sys_halt(void)
{
	sched_halt();
}

// Allocate a new environment.
// Returns envid of new environment, or < 0 on error.  Errors are:
//	-E_NO_FREE_ENV if no free environment is available.
//	-E_NO_MEM on memory exhaustion.
static envid_t
sys_exofork(int is_contestant)
{
	// Create the new environment with env_alloc(), from kern/env.c.
	// It should be left as env_alloc created it, except that
	// status is set to ENV_NOT_RUNNABLE, and the register set is copied
	// from the current environment -- but tweaked so sys_exofork
	// will appear to return 0.
	
	// LAB 4: Your code here.
	struct Env *e;
	int ret = env_alloc(&e, curenv->env_id);
	if(ret < 0)
		return ret;
	if (is_contestant) {
		contestant_env = e;
	}
	e->env_status = ENV_NOT_RUNNABLE;
	e->env_tf = curenv->env_tf;
	e->env_tf.tf_regs.reg_eax = 0;
	return e->env_id;
	//panic("sys_exofork not implemented");
}

// Set envid's env_status to status, which must be ENV_RUNNABLE
// or ENV_NOT_RUNNABLE.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if status is not a valid status for an environment.
static int
sys_env_set_status(envid_t envid, int status)
{
	// Hint: Use the 'envid2env' function from kern/env.c to translate an
	// envid to a struct Env.
	// You should set envid2env's third argument to 1, which will
	// check whether the current environment has permission to set
	// envid's status.

	// LAB 4: Your code here.
	struct Env *e;
	int ret = envid2env(envid, &e, 1);
	if(ret < 0)
		return ret;
	if(status > ENV_NOT_RUNNABLE)
		return -E_INVAL;
	e->env_status = status;
	return 0;
	//panic("sys_env_set_status not implemented");
}

// Set envid's trap frame to 'tf'.
// tf is modified to make sure that user environments always run at code
// protection level 3 (CPL 3) with interrupts enabled.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_trapframe(envid_t envid, struct Trapframe *tf)
{
	// LAB 5: Your code here.
	// Remember to check whether the user has supplied us with a good
	// address!
	//panic("sys_env_set_trapframe not implemented");
	struct Env *e;
	int ret = envid2env(envid, &e, 1);
	if(ret < 0)
		return ret;
	user_mem_assert(curenv, tf, sizeof(struct Trapframe), PTE_U);
	e->env_tf = *tf;
	e->env_tf.tf_ds = GD_UD | 3;
	e->env_tf.tf_es = GD_UD | 3;
	e->env_tf.tf_ss = GD_UD | 3;
	e->env_tf.tf_cs = GD_UT | 3;
	e->env_tf.tf_eflags |= FL_IF;
	return 0;
}

static int sys_net_try_transmit(void *buf, int size)
{
	if(size > PGSIZE)
		return -E_INVAL;
	user_mem_assert(curenv, buf, size, PTE_U);
	pte_t *pte;
	struct PageInfo *pp = page_lookup(curenv->env_pgdir, buf, &pte);
	if(!pp)
		return -E_INVAL;
	return try_transmit(page2pa(pp) + ((uint32_t) buf & 0xfff), size);
}

static int sys_net_try_receive(struct jif_pkt *jp)
{
	user_mem_assert(curenv, jp, PGSIZE, PTE_U | PTE_W);
	return try_receive(jp);
}

// Set the page fault upcall for 'envid' by modifying the corresponding struct
// Env's 'env_pgfault_upcall' field.  When 'envid' causes a page fault, the
// kernel will push a fault record onto the exception stack, then branch to
// 'func'.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_pgfault_upcall(envid_t envid, void *func)
{
	// LAB 4: Your code here.
	struct Env *e;
	int ret = envid2env(envid, &e, 1);
	if(ret < 0)
		return ret;
	e->env_pgfault_upcall = func;
	return 0;
	//panic("sys_env_set_pgfault_upcall not implemented");
}

// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
// The page's contents are set to 0.
// If a page is already mapped at 'va', that page is unmapped as a
// side effect.
//
// perm -- PTE_U | PTE_P must be set, PTE_AVAIL | PTE_W may or may not be set,
//         but no other bits may be set.  See PTE_SYSCALL in inc/mmu.h.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
//	-E_INVAL if perm is inappropriate (see above).
//	-E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int
sys_page_alloc(envid_t envid, void *va, int perm)
{
	// Hint: This function is a wrapper around page_alloc() and
	//   page_insert() from kern/pmap.c.
	//   Most of the new code you write should be to check the
	//   parameters for correctness.
	//   If page_insert() fails, remember to free the page you
	//   allocated!

	// LAB 4: Your code here.
	struct Env *e;
	int ret = envid2env(envid, &e, 1);
	//cprintf("alloc envid %x curenvid %x\n", envid, curenv->env_id);
	if(ret < 0)
		return ret;
	//cprintf("alloc\n");
	if(va >= (void *) UTOP || (size_t) va % PGSIZE != 0)
		return -E_INVAL;
	//cprintf("alloc\n");
	if(!(perm & PTE_U) || !(perm & PTE_P) || (perm & ~PTE_SYSCALL))
		return -E_INVAL;
	//cprintf("alloc\n");
	struct PageInfo *pp = page_alloc(ALLOC_ZERO);
	if(!pp)
		return -E_NO_MEM;
	//cprintf("alloc\n");
	ret = page_insert(e->env_pgdir, pp, va, perm);
	if(ret < 0)
	{
		page_free(pp);
		return -E_NO_MEM;
	}
	//cprintf("alloc\n");
	return 0;
	//panic("sys_page_alloc not implemented");
}

// Map the page of memory at 'srcva' in srcenvid's address space
// at 'dstva' in dstenvid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_page_alloc, except
// that it also must not grant write access to a read-only
// page.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if srcenvid and/or dstenvid doesn't currently exist,
//		or the caller doesn't have permission to change one of them.
//	-E_INVAL if srcva >= UTOP or srcva is not page-aligned,
//		or dstva >= UTOP or dstva is not page-aligned.
//	-E_INVAL is srcva is not mapped in srcenvid's address space.
//	-E_INVAL if perm is inappropriate (see sys_page_alloc).
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in srcenvid's
//		address space.
//	-E_NO_MEM if there's no memory to allocate any necessary page tables.
static int
sys_page_map(envid_t srcenvid, void *srcva,
	     envid_t dstenvid, void *dstva, int perm)
{
	// Hint: This function is a wrapper around page_lookup() and
	//   page_insert() from kern/pmap.c.
	//   Again, most of the new code you write should be to check the
	//   parameters for correctness.
	//   Use the third argument to page_lookup() to
	//   check the current permissions on the page.

	// LAB 4: Your code here.
	int ret;
	//cprintf("map begin\n");
	struct Env *se;
	ret = envid2env(srcenvid, &se, 1);
	if(ret < 0)
		return ret;
	struct Env *de;
	ret = envid2env(dstenvid, &de, 1);
	if(ret < 0)
		return ret;
	//cprintf("map middle\n");
	if(srcva >= (void *) UTOP || (size_t) srcva % PGSIZE != 0)
		return -E_INVAL;
	if(dstva >= (void *) UTOP || (size_t) dstva % PGSIZE != 0)
		return -E_INVAL;
	pte_t *pte;
	//cprintf("map ha\n");
	struct PageInfo *pp = page_lookup(se->env_pgdir, srcva, &pte);
	if(!pp)
		return -E_INVAL;
	//cprintf("map asdf %p\n", perm);
	if(!(perm & PTE_U) || !(perm & PTE_P) || (perm & ~PTE_SYSCALL))
		return -E_INVAL;
	//cprintf("map qwer\n");
	if((perm & PTE_W) && !(*pte & PTE_W))
		return -E_INVAL;
	//cprintf("map haha\n");
	ret = page_insert(de->env_pgdir, pp, dstva, perm);
	if(ret < 0)
		return ret;
	return 0;
	//panic("sys_page_map not implemented");
}

// Unmap the page of memory at 'va' in the address space of 'envid'.
// If no page is mapped, the function silently succeeds.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
static int
sys_page_unmap(envid_t envid, void *va)
{
	// Hint: This function is a wrapper around page_remove().

	// LAB 4: Your code here.
	struct Env *e;
	int ret = envid2env(envid, &e, 1);
	if(ret < 0)
		return ret;
	if(va >= (void *) UTOP || (size_t) va % PGSIZE != 0)
		return -E_INVAL;
	page_remove(e->env_pgdir, va);
	return 0;
	//panic("sys_page_unmap not implemented");
}

// Try to send 'value' to the target env 'envid'.
// If srcva < UTOP, then also send page currently mapped at 'srcva',
// so that receiver gets a duplicate mapping of the same page.
//
// The send fails with a return value of -E_IPC_NOT_RECV if the
// target is not blocked, waiting for an IPC.
//
// The send also can fail for the other reasons listed below.
//
// Otherwise, the send succeeds, and the target's ipc fields are
// updated as follows:
//    env_ipc_recving is set to 0 to block future sends;
//    env_ipc_from is set to the sending envid;
//    env_ipc_value is set to the 'value' parameter;
//    env_ipc_perm is set to 'perm' if a page was transferred, 0 otherwise.
// The target environment is marked runnable again, returning 0
// from the paused sys_ipc_recv system call.  (Hint: does the
// sys_ipc_recv function ever actually return?)
//
// If the sender wants to send a page but the receiver isn't asking for one,
// then no page mapping is transferred, but no error occurs.
// The ipc only happens when no errors occur.
//
// Returns 0 on success, < 0 on error.
// Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist.
//		(No need to check permissions.)
//	-E_IPC_NOT_RECV if envid is not currently blocked in sys_ipc_recv,
//		or another environment managed to send first.
//	-E_INVAL if srcva < UTOP but srcva is not page-aligned.
//	-E_INVAL if srcva < UTOP and perm is inappropriate
//		(see sys_page_alloc).
//	-E_INVAL if srcva < UTOP but srcva is not mapped in the caller's
//		address space.
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in the
//		current environment's address space.
//	-E_NO_MEM if there's not enough memory to map srcva in envid's
//		address space.
static int
sys_ipc_try_send(envid_t envid, uint32_t value, void *srcva, unsigned perm)
{
	// LAB 4: Your code here.
	struct Env *e;
	int ret = envid2env(envid, &e, 0);
	if(ret < 0)
		return ret;
	if(!e->env_ipc_recving)
		return -E_IPC_NOT_RECV;
	bool sent = 0;
	if(srcva < (void *) UTOP)
	{
		if((uint32_t) srcva % PGSIZE)
			return -E_INVAL;
		if(!(perm & PTE_U) || !(perm & PTE_P) || (perm & ~PTE_SYSCALL))
			return -E_INVAL;
		pte_t *pte;
		struct PageInfo *pp = page_lookup(curenv->env_pgdir, srcva, &pte);
		if(!pp)
			return -E_INVAL;
		if((perm & PTE_W) && !(*pte & PTE_W))
			return -E_INVAL;
		if(e->env_ipc_dstva < (void *) UTOP)
		{
			ret = page_insert(e->env_pgdir, pp, e->env_ipc_dstva, perm);
			if(ret < 0)
				return ret;
		}
		e->env_ipc_perm = perm;
		sent = 1;
	}
	e->env_ipc_recving = 0;
	e->env_ipc_from = curenv->env_id;
	e->env_ipc_value = value;
	if(!sent)
		e->env_ipc_perm = 0;
	//cprintf("mark runnable %x\n", e->env_id);
	e->env_status = ENV_RUNNABLE;
	e->env_tf.tf_regs.reg_eax = 0;
	return 0;
	//panic("sys_ipc_try_send not implemented");
}

// Block until a value is ready.  Record that you want to receive
// using the env_ipc_recving and env_ipc_dstva fields of struct Env,
// mark yourself not runnable, and then give up the CPU.
//
// If 'dstva' is < UTOP, then you are willing to receive a page of data.
// 'dstva' is the virtual address at which the sent page should be mapped.
//
// This function only returns on error, but the system call will eventually
// return 0 on success.
// Return < 0 on error.  Errors are:
//	-E_INVAL if dstva < UTOP but dstva is not page-aligned.
static int
sys_ipc_recv(void *dstva)
{
	// LAB 4: Your code here.
	if(dstva < (void *) UTOP && (uint32_t) dstva % PGSIZE != 0)
		return -E_INVAL;
	curenv->env_ipc_dstva = dstva;
	curenv->env_ipc_recving = 1;
	//cprintf("mark NOTrunnable %x\n", curenv->env_id);
	curenv->env_status = ENV_NOT_RUNNABLE;
	sched_yield();
	//panic("sys_ipc_recv not implemented");
	//return 0;
}

static int
sys_enter_judge(void *eip, struct JudgeParams *prm)
{
	uint64_t ns = prm->ns;
	if(ns < 1 || ns > 40000000000ll) return -E_INVAL;
	// TODO: validate
	curenv->env_status = ENV_NOT_RUNNABLE;
	curenv->env_judge_waiting = 1;
	curenv->env_judge_tf = curenv->env_tf;
	curenv->env_judge_tf.tf_eip = (uint32_t) eip;
	prm->data_begin = (void *) 0x10000000;
	curenv->env_judge_prm = *prm;
	// curenv->env_judge_tf.tf_esp = (uint32_t) esp;
	sched_yield();
	return 0;
}

static int
sys_accept_enter_judge(envid_t envid, struct JudgeResult *res)
{
	// TODO: validate
	struct Env *env;
	int ret = envid2env(envid, &env, 0);
	if(ret < 0) return -E_INVAL;
	if(!env->env_judge_waiting) return -E_INVAL;
	
	lapic_timer_disable();
	
	extern int reboot_cnt;
	reboot_cnt = 0;
	
	struct Trapframe tmp = env->env_judge_tf;
	struct JudgeParams prm = env->env_judge_prm;
	
	if(prm.defrag_mem) pmem_defrag();
	env->env_judge_tf = env->env_tf; env->env_tf = tmp;
	env->env_tf.tf_esp = (uintptr_t) prm.esp;
	env->env_judge_tf.tf_regs.reg_eax = 0;
	
	pgdir_reperm(env->env_pgdir, PTE_D, 0, NULL, (void *) UTOP);
	pgdir_reperm(env->env_pgdir, PTE_W, PTE_TDW, NULL, (void *) UTOP);
	pgdir_reperm(env->env_pgdir, PTE_TDW, PTE_W, env->env_judge_prm.data_begin, (void *) env->env_tf.tf_esp);
	
	curenv->env_tf.tf_regs.reg_eax = 0; // return 0
	curenv->env_judge_res = res;
	curenv->env_judge_prm = env->env_judge_prm;  // Why I need this ???
	res->verdict = VERDICT_SE;
	judger_env = curenv;
	
	// cprintf("here we go!\n");
	
	env->env_judge_waiting = 0;
	env->env_judging = 1;
	
	unsigned sum = 0;
	for(unsigned *i = (unsigned *) 0xf0000000; i < (unsigned *) 0xf1000000; i++)
		sum ^= *i;
	cprintf("magic checksum %08x\n", sum);
	
	judger_env->env_judge_res->time_ns = prm.ns;
	res->time_cycles = -read_tsc();
	
	asm volatile("wbinvd\n");
	for (register int i = 0; i < 100000000; i++) asm volatile("");
	asm volatile("invd\n");
	
	timer_single_shot_ns(prm.ns);
	env_run(env);
	
	// won't reach here
	return 233;
}

static int
sys_quit_judge()
{
	// cprintf("quit judge!\n");
	if(!curenv->env_judging) return -E_INVAL;
	
	finish_judge(VERDICT_OK);
	// won't reach here
	return 233;
}

static int
sys_map_judge_pages(void *dst, unsigned offset, unsigned len)
{
	if (offset % PGSIZE != 0 || len % PGSIZE != 0) {
		return -E_INVAL;
	}
	if (offset >= JUDGE_PAGES_SIZE || JUDGE_PAGES_SIZE - offset < len) {
		return -E_INVAL;
	}
	if ((unsigned) dst % PGSIZE != 0) {
		return -E_INVAL;
	}
	if (len == 0) {
		return 0;
	}
	
	struct Env *e;
	int ret = envid2env(0, &e, 1);
	if (ret < 0) {
		return ret;
	}
	
	struct PageInfo *pp = get_first_judge_page() + offset / PGSIZE;
	int n = len / PGSIZE;
	for (int i = 0; i < n; i++) {
		ret = page_insert(e->env_pgdir, pp + i, dst + i * PGSIZE, PTE_P | PTE_U | PTE_W);
		if (ret < 0) {
			return -E_NO_MEM;
		}
	}
	return n;
}

// Return the current time.
static int
sys_time_msec(void)
{
	// LAB 6: Your code here.
	//panic("sys_time_msec not implemented");
	return time_msec();
}

// Dispatches to the correct kernel function, passing the arguments.
int32_t
syscall(uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	// Call the function corresponding to the 'syscallno' parameter.
	// Return any appropriate return value.
	// LAB 3: Your code here.

	//panic("syscall not implemented");
	//cprintf("got syscall %d\n", syscallno);
	switch (syscallno) {
	case SYS_cputs:
		sys_cputs((const char *) a1, a2);
		return 0;
	case SYS_cgetc:
		return sys_cgetc();
	case SYS_getenvid:
		return sys_getenvid();
	case SYS_env_destroy:
		return sys_env_destroy(a1);
	case SYS_yield:
		sys_yield();
		return 0;
	case SYS_page_alloc:
		return sys_page_alloc(a1, (void *) a2, a3);
	case SYS_page_map:
		return sys_page_map(a1, (void *) a2, a3, (void *) a4, a5);
	case SYS_page_unmap:
		return sys_page_unmap(a1, (void *) a2);
	case SYS_exofork:
		return sys_exofork((int) a1);
	case SYS_env_set_status:
		return sys_env_set_status(a1, a2);
	case SYS_env_set_pgfault_upcall:
		return sys_env_set_pgfault_upcall(a1, (void *) a2);
	case SYS_ipc_try_send:
		return sys_ipc_try_send(a1, a2, (void *) a3, a4);
	case SYS_ipc_recv:
		return sys_ipc_recv((void *) a1);
	case SYS_enter_judge:
		return sys_enter_judge((void *) a1, (void *) a2);
	case SYS_accept_enter_judge:
		return sys_accept_enter_judge(a1, (void *) a2);
	case SYS_quit_judge:
		return sys_quit_judge();
	case SYS_map_judge_pages:
		return sys_map_judge_pages((void *) a1, a2, a3);
	case SYS_env_set_trapframe:
		return sys_env_set_trapframe(a1, (struct Trapframe *) a2);
	case SYS_time_msec:
		return sys_time_msec();
	case SYS_net_try_transmit:
		return sys_net_try_transmit((void *) a1, a2);
	case SYS_net_try_receive:
		return sys_net_try_receive((struct jif_pkt *) a1);
	case SYS_halt:
		sys_halt();
		return 0;
	default:
		return -E_INVAL;
	}
}

