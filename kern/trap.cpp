#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iomanip>
#include <sys/syscall.h>

#include <inc/trap.hpp>
#include <inc/x86_64.hpp>
#include <inc/lapic.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/utils.hpp>
#include <inc/pic.hpp>

extern void *__traps[256];
extern void *kernel_stack_top;

namespace Trap {
	struct InterruptDescriptor {
		uint16_t ptr_low;
		uint16_t selector;
		uint8_t ist;
		uint8_t options;
		uint16_t ptr_mid;
		uint32_t ptr_high;
		uint32_t reserved;
	} __attribute__((packed, aligned(16)));
	
	struct PushRegs {
		uint64_t rcx, rbx, rbp, rsi, rdi;
		uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
		uint64_t tsc;
		uint64_t rax, rdx;
	} __attribute__((packed));
	
	struct Trapframe {
		char fxstate[512];
		uint64_t tf_fsbase;
		PushRegs tf_regs;
		uint64_t tf_num;
		uint64_t tf_errorcode;
		uint64_t tf_rip;
		uint64_t tf_cs;
		uint64_t tf_rflags;
		uint64_t tf_rsp;
		uint64_t tf_ss;
	} __attribute__((packed, aligned(16)));
	
	struct TaskState {
		uint32_t reserved1;
		uint64_t rsp[3];
		uint64_t ist[8];
		uint64_t reserved3;
		uint16_t reserved4;
		uint16_t IOPB_offset;
	} __attribute__((packed, aligned(16)));
	
	static TaskState task_state;
	
	const uint16_t GDT_KERNEL_CS = 8;
	const uint16_t GDT_KERNEL_SS = 16;
	const uint16_t GDT_USER32_CS = 24;
	const uint16_t GDT_USER_SS = 32;
	const uint16_t GDT_USER_CS = 40;
	const uint16_t GDT_TSS = 48;
	static uint64_t gdt[] = {
		0,
		[GDT_KERNEL_CS >> 3] = (1ul << 43) | (1ul << 44) | (1ul << 47) | (1ul << 53),
		[GDT_KERNEL_SS >> 3] = (1ul << 44) | (1ul << 47) | (1ul << 41),
		[GDT_USER32_CS >> 3] = 0,  // TODO: fill it
		[GDT_USER_SS >> 3] = (1ul << 44) | (1ul << 47) | (1ul << 41) | (3ul << 45),
		[GDT_USER_CS >> 3] = (1ul << 43) | (1ul << 44) | (1ul << 47) | (1ul << 53) | (3ul << 45),
		[GDT_TSS >> 3] = (sizeof(TaskState) - 1) | (((uint64_t) &task_state & 0xffffff) << 16) | (9ul << 40) | (0ul << 45) | (1ul << 47) | (1ul << 53) | (((uint64_t) &task_state >> 24) << 56),
		[(GDT_TSS >> 3) + 1] = ((uint64_t) &task_state) >> 32
	};
	
	static InterruptDescriptor idt[256];
	static struct {
		uint16_t lim;
		uint64_t addr;
	} __attribute__((packed, aligned(16))) idt_desc = {
		.lim = sizeof(idt) - 1,
		.addr = (uint64_t) idt
	}, gdt_desc = {
		.lim = sizeof(gdt) - 1,
		.addr = (uint64_t) gdt
	};
	
	const uint32_t STACK_SIZE = 4096;
	static char interrupt_stack[8][STACK_SIZE] __attribute__((aligned(4096)));
	
	const uint8_t TRAP_IRQ = 32;
	const uint8_t TRAP_RUN_USER = 233;
	const uint8_t TRAP_INVALID_SYSCALL = 254;
	const uint8_t TRAP_SYSCALL = 255;
	
	extern "C" { uint64_t __syscall_stack; }
	static Trapframe tf_run_user, tf_from_user, tf_to_user;
	static uint64_t user_time_limit_ns;
	
	extern "C"
	void trap_return(Trapframe *tf);
	
	extern "C"
	void syscall_return_record_tsc(Trapframe *tf);
	
	extern "C"
	void trap_handler(Trapframe *tf) {
		int num = (int) tf->tf_num;
		LDEBUG() << "trap " << num << ", CPL " << (tf->tf_cs & 3);
		
		if (tf->tf_cs & 3) {  // trap from user
			LAPIC::eoi();
			LAPIC::timer_disable();
			tf_from_user = *tf;
			tf = &tf_run_user;
			tf->tf_regs.rax = num;
			tf->tf_regs.tsc = tf_from_user.tf_regs.tsc - tf_to_user.tf_regs.tsc;
		} else {
			if (num == TRAP_IRQ + PIC::IRQ_TIMER) {
				// timer interrupt caused by scheduler
				// just return with interrupts disabled
				LAPIC::eoi();
				LAPIC::timer_disable();
				tf->tf_rflags &= ~0x200;  // disable interrupts
			} else if (num == TRAP_RUN_USER) {
				tf_run_user = *tf;
				tf = &tf_to_user;
				
				if (user_time_limit_ns != 0) {
					// add 10ms to provide a hard limit
					LAPIC::timer_single_shot_ns(user_time_limit_ns + 10000000);
				}
				
				syscall_return_record_tsc(tf);
			} else {
				LERROR("tf_rip = %lx", tf->tf_rip);
				LERROR("Unhandled kernel trap %d", num);
				LFATAL("GG, reboot");
				Timer::microdelay((uint64_t) 10e6);  // 10s
				x86_64::reboot();
			}
		}
		
		trap_return(tf);
	}
	
	static void set_idt(int id, void *addr) {
		uint64_t a = (uint64_t) addr;
		
		idt[id] = (InterruptDescriptor) {
			.ptr_low = (uint16_t) a,
			.selector = GDT_KERNEL_CS,
			.ist = 0,
			.options = (14 << 0) | (1 << 7),  // must-be-one bits | present
			.ptr_mid = (uint16_t) (a >> 16),
			.ptr_high = (uint32_t) (a >> 32),
			.reserved = 0
		};
	}
	
	extern "C" void __syscall_entry();
	
	void init() {
		LDEBUG_ENTER_RET();
		
		for (uint32_t i = 0; i < 256; i++) {
			set_idt(i, __traps[i]);
		}
		idt[3].options |= 3 << 5;  // user can invoke int3
		idt[TRAP_RUN_USER].ist = 7;  // 7-th stack
		
		x86_64::lidt(&idt_desc);
		
		task_state.rsp[0] = (uint64_t) interrupt_stack[0] + STACK_SIZE;
		for (int i = 1; i <= 7; i++) {
			task_state.ist[i] = (uint64_t) interrupt_stack[i] + STACK_SIZE;
		}
		task_state.IOPB_offset = sizeof(TaskState);
		x86_64::lgdt(&gdt_desc);
		x86_64::ltr(GDT_TSS);
		
		// set up syscall [only used as exit]
		using x86_64::rdmsr;
		using x86_64::wrmsr;
		wrmsr(x86_64::Efer, rdmsr(x86_64::Efer) | 1);  // enable syscall
		wrmsr(x86_64::LStar, (uint64_t) &__syscall_entry);
		wrmsr(x86_64::Star, ((uint64_t) GDT_KERNEL_CS << 32) | ((uint64_t) GDT_USER32_CS << 48));
		wrmsr(x86_64::SFMask, 0x200);  // mask interrupt
		__syscall_stack = task_state.rsp[0];  // used by syscall_entry
	}
	
	static Trapframe make_trapframe(uint64_t rip, uint64_t rsp) {
		Trapframe tf;
		memset(&tf, 0, sizeof(tf));
		tf.tf_rip = rip;
		tf.tf_cs = GDT_USER_CS | 3;
		tf.tf_rflags = x86_64::read_rflags() | 0x3000 | 0x200;  // user | interrupt
		tf.tf_rsp = rsp;
		tf.tf_ss = GDT_USER_SS | 3;
		return tf;
	}
	
	void run_user_64(uint64_t entry, uint64_t rsp, uint64_t time_limit_ns,
		uint64_t &tsc, uint8_t &trap_num, int32_t &return_code) {
		LDEBUG_ENTER_RET();
		
		user_time_limit_ns = time_limit_ns;
		tf_to_user = make_trapframe(entry, rsp);
		LDEBUG("tf %p   entry %lx   rsp %lx", &tf_to_user, entry, rsp);
		
		__asm__ volatile ("int %1" : "=a" (trap_num) : "i" (TRAP_RUN_USER) : "memory");
		
		LDEBUG("tsc1 = %lu tsc2 = %lu tscdiff = %lu",
			tf_from_user.tf_regs.tsc, tf_to_user.tf_regs.tsc, tf_run_user.tf_regs.tsc);
		
		// export tsc
		tsc = tf_run_user.tf_regs.tsc;
		
		// export return-code
		if (trap_num == TRAP_SYSCALL) {
			int syscall_num = (int) tf_from_user.tf_regs.rax;
			if (syscall_num != SYS_exit) {
				trap_num = TRAP_INVALID_SYSCALL;
				return_code = 0;
			} else {
				return_code = (int32_t) tf_from_user.tf_regs.rdi;
			}
		} else {
			return_code = 0;
		}
	}
}
