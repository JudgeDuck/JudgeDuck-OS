#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/env.h>
#include <kern/syscall.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/picirq.h>
#include <kern/cpu.h>
#include <kern/timer.h>
#include <kern/judge.h>
#include <kern/spinlock.h>
#include <kern/time.h>

static struct Taskstate ts;

/* For debugging, so print_trapframe can distinguish between printing
 * a saved trapframe and printing the current trapframe and print some
 * additional information in the latter case.
 */
static struct Trapframe *last_tf;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[256] = { { 0 } };
struct Pseudodesc idt_pd = {
	sizeof(idt) - 1, (uint32_t) idt
};


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

void trap_00();
void trap_01();
void trap_02();
void trap_03();
void trap_04();
void trap_05();
void trap_06();
void trap_07();
void trap_08();
void trap_09();
void trap_10();
void trap_11();
void trap_12();
void trap_13();
void trap_14();
void trap_15();
void trap_16();
void trap_17();
void trap_18();
void trap_19();

void trap_32();
void trap_33();
void trap_34();
void trap_35();
void trap_36();
void trap_37();
void trap_38();
void trap_39();
void trap_40();
void trap_41();
void trap_42();
void trap_43();
void trap_44();
void trap_45();
void trap_46();
void trap_47();

void trap_48();

void trap_251();

void
trap_init(void)
{
	extern struct Segdesc gdt[];

	// LAB 3: Your code here.
	SETGATE(idt[0 ], 0, GD_KT, &trap_00, 0);
	SETGATE(idt[1 ], 0, GD_KT, &trap_01, 0);
	SETGATE(idt[2 ], 0, GD_KT, &trap_02, 0);
	SETGATE(idt[3 ], 0, GD_KT, &trap_03, 3);
	SETGATE(idt[4 ], 0, GD_KT, &trap_04, 0);
	SETGATE(idt[5 ], 0, GD_KT, &trap_05, 0);
	SETGATE(idt[6 ], 0, GD_KT, &trap_06, 0);
	SETGATE(idt[7 ], 0, GD_KT, &trap_07, 0);
	SETGATE(idt[8 ], 0, GD_KT, &trap_08, 0);
	SETGATE(idt[9 ], 0, GD_KT, &trap_09, 0);
	SETGATE(idt[10], 0, GD_KT, &trap_10, 0);
	SETGATE(idt[11], 0, GD_KT, &trap_11, 0);
	SETGATE(idt[12], 0, GD_KT, &trap_12, 0);
	SETGATE(idt[13], 0, GD_KT, &trap_13, 0);
	SETGATE(idt[14], 0, GD_KT, &trap_14, 0);
	SETGATE(idt[15], 0, GD_KT, &trap_15, 0);
	SETGATE(idt[16], 0, GD_KT, &trap_16, 0);
	SETGATE(idt[17], 0, GD_KT, &trap_17, 0);
	SETGATE(idt[18], 0, GD_KT, &trap_18, 0);
	SETGATE(idt[19], 0, GD_KT, &trap_19, 3);
	
	SETGATE(idt[32], 0, GD_KT, &trap_32, 0);
	SETGATE(idt[33], 0, GD_KT, &trap_33, 0);
	SETGATE(idt[34], 0, GD_KT, &trap_34, 0);
	SETGATE(idt[35], 0, GD_KT, &trap_35, 0);
	SETGATE(idt[36], 0, GD_KT, &trap_36, 0);
	SETGATE(idt[37], 0, GD_KT, &trap_37, 0);
	SETGATE(idt[38], 0, GD_KT, &trap_38, 0);
	SETGATE(idt[39], 0, GD_KT, &trap_39, 0);
	SETGATE(idt[40], 0, GD_KT, &trap_40, 0);
	SETGATE(idt[41], 0, GD_KT, &trap_41, 0);
	SETGATE(idt[42], 0, GD_KT, &trap_42, 0);
	SETGATE(idt[43], 0, GD_KT, &trap_43, 0);
	SETGATE(idt[44], 0, GD_KT, &trap_44, 0);
	SETGATE(idt[45], 0, GD_KT, &trap_45, 0);
	SETGATE(idt[46], 0, GD_KT, &trap_46, 0);
	SETGATE(idt[47], 0, GD_KT, &trap_47, 0);
	
	SETGATE(idt[48], 0, GD_KT, &trap_48, 3);
	
	SETGATE(idt[251], 0, GD_KT, &trap_251, 3);  // for IPI
	
	// Per-CPU setup
	trap_init_percpu();
}

// Initialize and load the per-CPU TSS and IDT
void
trap_init_percpu(void)
{
	// The example code here sets up the Task State Segment (TSS) and
	// the TSS descriptor for CPU 0. But it is incorrect if we are
	// running on other CPUs because each CPU has its own kernel stack.
	// Fix the code so that it works for all CPUs.
	//
	// Hints:
	//   - The macro "thiscpu" always refers to the current CPU's
	//     struct CpuInfo;
	//   - The ID of the current CPU is given by cpunum() or
	//     thiscpu->cpu_id;
	//   - Use "thiscpu->cpu_ts" as the TSS for the current CPU,
	//     rather than the global "ts" variable;
	//   - Use gdt[(GD_TSS0 >> 3) + i] for CPU i's TSS descriptor;
	//   - You mapped the per-CPU kernel stacks in mem_init_mp()
	//
	// ltr sets a 'busy' flag in the TSS selector, so if you
	// accidentally load the same TSS on more than one CPU, you'll
	// get a triple fault.  If you set up an individual CPU's TSS
	// wrong, you may not get a fault until you try to return from
	// user space on that CPU.
	//
	// LAB 4: Your code here:

	// Setup a TSS so that we get the right stack
	// when we trap to the kernel.
	int i = cpunum();
	thiscpu->cpu_ts.ts_esp0 = KSTACKTOP - i * (KSTKSIZE + KSTKGAP);
	thiscpu->cpu_ts.ts_ss0 = GD_KD;
	thiscpu->cpu_ts.ts_iomb = sizeof(struct Taskstate);

	// Initialize the TSS slot of the gdt.
	gdt[(GD_TSS0 >> 3) + i] = SEG16(STS_T32A, (uint32_t) (&thiscpu->cpu_ts),
							sizeof(struct Taskstate) - 1, 0);
	gdt[(GD_TSS0 >> 3) + i].sd_s = 0;

	// Load the TSS selector (like other segment selectors, the
	// bottom three bits are special; we leave them 0)
	ltr(GD_TSS0 + (i << 3));

	// Load the IDT
	lidt(&idt_pd);
}

void
print_trapframe(struct Trapframe *tf)
{for (unsigned i = 1; i; i++) {__asm__ volatile("");}
	cprintf("TRAP frame at %p from CPU %d\n", tf, cpunum());
	print_regs(&tf->tf_regs);
	cprintf("  es   0x----%04x\n", tf->tf_es);
	cprintf("  ds   0x----%04x\n", tf->tf_ds);
	cprintf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	// If this trap was a page fault that just happened
	// (so %cr2 is meaningful), print the faulting linear address.
	if (tf == last_tf && tf->tf_trapno == T_PGFLT)
		cprintf("  cr2  0x%08x\n", rcr2());
	cprintf("  err  0x%08x", tf->tf_err);
	// For page faults, print decoded fault error code:
	// U/K=fault occurred in user/kernel mode
	// W/R=a write/read caused the fault
	// PR=a protection violation caused the fault (NP=page not present).
	if (tf->tf_trapno == T_PGFLT)
		cprintf(" [%s, %s, %s]\n",
			tf->tf_err & 4 ? "user" : "kernel",
			tf->tf_err & 2 ? "write" : "read",
			tf->tf_err & 1 ? "protection" : "not-present");
	else
		cprintf("\n");
	cprintf("  eip  0x%08x\n", tf->tf_eip);
	cprintf("  cs   0x----%04x\n", tf->tf_cs);
	cprintf("  flag 0x%08x\n", tf->tf_eflags);
	if ((tf->tf_cs & 3) != 0) {
		cprintf("  esp  0x%08x\n", tf->tf_esp);
		cprintf("  ss   0x----%04x\n", tf->tf_ss);
	}
}

void
print_regs(struct PushRegs *regs)
{
	cprintf("  edi  0x%08x\n", regs->reg_edi);
	cprintf("  esi  0x%08x\n", regs->reg_esi);
	cprintf("  ebp  0x%08x\n", regs->reg_ebp);
	cprintf("  oesp 0x%08x\n", regs->reg_oesp);
	cprintf("  ebx  0x%08x\n", regs->reg_ebx);
	cprintf("  edx  0x%08x\n", regs->reg_edx);
	cprintf("  ecx  0x%08x\n", regs->reg_ecx);
	cprintf("  eax  0x%08x\n", regs->reg_eax);
}
int reboot_cnt = 0;

static void
trap_dispatch(struct Trapframe *tf)
{
	// Handle processor exceptions.
	// LAB 3: Your code here.
	//cprintf("got trap %d eip %p\n", tf->tf_trapno, tf->tf_eip);
	//print_trapframe(tf);
	if(tf->tf_trapno == T_PGFLT)
	{
		if(curenv && curenv->env_judging)
		{
			print_trapframe(tf);
			finish_judge(VERDICT_RE);
		}
		page_fault_handler(tf);
		return;
	}
	else if(tf->tf_trapno == T_BRKPT)
	{
		if(curenv && curenv->env_judging)
			finish_judge(VERDICT_RE);
		monitor(tf);
		return;
	}
	else if(tf->tf_trapno == T_SYSCALL)
	{
		if(curenv && curenv->env_judging && !curenv->env_judge_prm.syscall_enabled[tf->tf_regs.reg_eax])
			finish_judge(VERDICT_IS);
		tf->tf_regs.reg_eax = syscall(
			tf->tf_regs.reg_eax, tf->tf_regs.reg_edx, tf->tf_regs.reg_ecx,
			tf->tf_regs.reg_ebx, tf->tf_regs.reg_edi, tf->tf_regs.reg_esi
		);
		return;
	}
	else if(tf->tf_trapno == IRQ_OFFSET + IRQ_TIMER)
	{
		// cprintf("timer interrupt\n");
		if (tsc_measurement_running) {
			tsc_measurement_end();
			cprintf("%llu\n", get_tsc_frequency());
		}
		lapic_eoi();
		time_tick();
		if(curenv && curenv->env_judging)
			finish_judge(VERDICT_TLE);
		//if(++reboot_cnt >= 10000) outb(0x92, 0x3); // courtesy of Chris Frost
		sched_yield();
		return;
	}
	else if(tf->tf_trapno == IRQ_OFFSET + IRQ_KBD)
	{
		kbd_intr();
		return;
	}
	else if(tf->tf_trapno == IRQ_OFFSET + IRQ_SERIAL)
	{
		serial_intr();
		return;
	}

	// Handle spurious interrupts
	// The hardware sometimes raises these because of noise on the
	// IRQ line or other reasons. We don't care.
	if (tf->tf_trapno == IRQ_OFFSET + IRQ_SPURIOUS) {
		// cprintf("Spurious interrupt on irq 7\n");
		// print_trapframe(tf);
		return;
	}
	
	// Handle IPI
	// Currently it is used for kill contestant processes
	// TODO: extend its usage
	if (tf->tf_trapno == T_IPI) {
		lapic_eoi();
		if (tf->tf_cs == GD_KT) return;
		if (curenv && curenv->env_judging) {
			finish_judge(VERDICT_OK);
		}
		return;
	}

	// Handle clock interrupts. Don't forget to acknowledge the
	// interrupt using lapic_eoi() before calling the scheduler!
	// LAB 4: Your code here.

	// Add time tick increment to clock interrupts.
	// Be careful! In multiprocessors, clock interrupts are
	// triggered on every CPU.
	// LAB 6: Your code here.


	// Handle keyboard and serial interrupts.
	// LAB 5: Your code here.

	// Unexpected trap: The user process or the kernel has a bug.
	if (tf->tf_cs == GD_KT)
	{
		print_trapframe(tf);
		panic("unhandled trap in kernel");
	}
	else
	{
		if(curenv && curenv->env_judging)
			finish_judge(VERDICT_RE);
		print_trapframe(tf);
		env_destroy(curenv);
		return;
	}
}

void
trap(struct Trapframe *tf)
{
	// The environment may have set DF and some versions
	// of GCC rely on DF being clear
	asm volatile("cld" ::: "cc");
	
	lapic_tccr_when_trap_begin = lapic_timer_current_count();
	tsc_when_trap_begin = read_tsc();

	// Halt the CPU if some other CPU has called panic()
	extern char *panicstr;
	if (panicstr)
		asm volatile("hlt");
	//asm volatile("cli");
	
	//print_trapframe(tf);

	// Re-acqurie the big kernel lock if we were halted in
	// sched_yield()
	if (xchg(&thiscpu->cpu_status, CPU_STARTED) == CPU_HALTED)
		lock_kernel();
	// Check that interrupts are disabled.  If this assertion
	// fails, DO NOT be tempted to fix it by inserting a "cli" in
	// the interrupt path.
	assert(!(read_eflags() & FL_IF));

	if ((tf->tf_cs & 3) == 3) {
		// Trapped from user mode.
		// Acquire the big kernel lock before doing any
		// serious kernel work.
		// LAB 4: Your code here.
		lock_kernel();
		assert(curenv);

		// Garbage collect if current enviroment is a zombie
		if (curenv->env_status == ENV_DYING) {
			env_free(curenv);
			curenv = NULL;
			sched_yield();
		}

		// Copy trap frame (which is currently on the stack)
		// into 'curenv->env_tf', so that running the environment
		// will restart at the trap point.
		curenv->env_tf = *tf;
		// The trapframe on the stack should be ignored from here on.
		tf = &curenv->env_tf;
	}

	// Record that tf is the last real trapframe so
	// print_trapframe can print some additional information.
	last_tf = tf;

	// Dispatch based on what type of trap occurred
	trap_dispatch(tf);

	// If we made it to this point, then no other environment was
	// scheduled, so we should return to the current environment
	// if doing so makes sense.
	if (curenv && curenv->env_status == ENV_RUNNING)
		env_run(curenv);
	else
		sched_yield();
}


void
page_fault_handler(struct Trapframe *tf)
{
	uint32_t fault_va;

	// Read processor's CR2 register to find the faulting address
	fault_va = rcr2();

	// Handle kernel-mode page faults.

	// LAB 3: Your code here.
	if(!(tf->tf_err & 4))
	{
		print_trapframe(tf);
		panic("kernel pagefault\n");
	}

	// We've already handled kernel-mode exceptions, so if we get here,
	// the page fault happened in user mode.

	// Call the environment's page fault upcall, if one exists.  Set up a
	// page fault stack frame on the user exception stack (below
	// UXSTACKTOP), then branch to curenv->env_pgfault_upcall.
	//
	// The page fault upcall might cause another page fault, in which case
	// we branch to the page fault upcall recursively, pushing another
	// page fault stack frame on top of the user exception stack.
	//
	// It is convenient for our code which returns from a page fault
	// (lib/pfentry.S) to have one word of scratch space at the top of the
	// trap-time stack; it allows us to more easily restore the eip/esp. In
	// the non-recursive case, we don't have to worry about this because
	// the top of the regular user stack is free.  In the recursive case,
	// this means we have to leave an extra word between the current top of
	// the exception stack and the new stack frame because the exception
	// stack _is_ the trap-time stack.
	//
	// If there's no page fault upcall, the environment didn't allocate a
	// page for its exception stack or can't write to it, or the exception
	// stack overflows, then destroy the environment that caused the fault.
	// Note that the grade script assumes you will first check for the page
	// fault upcall and print the "user fault va" message below if there is
	// none.  The remaining three checks can be combined into a single test.
	//
	// Hints:
	//   user_mem_assert() and env_run() are useful here.
	//   To change what the user environment runs, modify 'curenv->env_tf'
	//   (the 'tf' variable points at 'curenv->env_tf').

	// LAB 4: Your code here.

	// Destroy the environment that caused the fault.
	if(!curenv->env_pgfault_upcall)
	{
		cprintf("no page fault upcall %x\n", curenv->env_id);
		goto trap_gg;
	}
	user_mem_assert(curenv, curenv->env_pgfault_upcall, 1, PTE_U);
	//user_mem_assert(curenv, (void *) (UXSTACKTOP - PGSIZE), PGSIZE, PTE_W);
	uint32_t *esp = (uint32_t *) tf->tf_esp;
	//cprintf("user exception esp %p va %p envid %x\n", esp, fault_va, curenv->env_id);
	//print_trapframe(tf);
	if(esp < (uint32_t *) UXSTACKTOP && esp >= (uint32_t *) (UXSTACKTOP - PGSIZE) && (uint32_t) esp - sizeof(struct UTrapframe) - sizeof(uint32_t) < UXSTACKTOP - PGSIZE + 8)
	{
		cprintf("user exception stack overflow\n");
		goto trap_gg;
	}
	lcr3(PADDR(curenv->env_pgdir));
	if(esp < (uint32_t *) UXSTACKTOP && esp >= (uint32_t *) (UXSTACKTOP - PGSIZE))
	{
		user_mem_assert(curenv, (void *) (esp - 14), 14 * 4, PTE_W);
		*--esp = 0;
	}
	else
	{
		esp = (uint32_t *) UXSTACKTOP;
		user_mem_assert(curenv, (void *) (esp - 13), 13 * 4, PTE_W);
	}
	*--esp = tf->tf_esp;
	*--esp = tf->tf_eflags;
	*--esp = tf->tf_eip;
	*((struct PushRegs *) (esp) - 1) = tf->tf_regs;
	esp -= 8;
	*--esp = tf->tf_err;
	*--esp = fault_va;
	lcr3(PADDR(kern_pgdir));
	tf->tf_esp = (uintptr_t) esp;
	//cprintf("last esp %p\n", esp);
	tf->tf_eip = (uintptr_t) curenv->env_pgfault_upcall;
	//cprintf("got upcall %p\n", tf->tf_eip);
	env_run(curenv);
trap_gg:
	cprintf("[%08x] user fault va %08x ip %08x\n",
		curenv->env_id, fault_va, tf->tf_eip);
	print_trapframe(tf);
	monitor(tf);
	env_destroy(curenv);
}

