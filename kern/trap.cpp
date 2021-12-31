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
#include <inc/memory.hpp>
#include <inc/abi.hpp>

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
	const uint16_t GDT_USER32_SS = 48;
	const uint16_t GDT_USER32_TLS = 56;
	const uint16_t GDT_ESPFIX_SS = 64;
	const uint16_t GDT_KERNEL32_CS = 72;
	const uint16_t GDT_TSS = 80;
	static uint64_t gdt[] = {
		0,
		[GDT_KERNEL_CS >> 3] = (1ul << 43) | (1ul << 44) | (1ul << 47) | (1ul << 53),
		[GDT_KERNEL_SS >> 3] = (1ul << 44) | (1ul << 47) | (1ul << 41),
		[GDT_USER32_CS >> 3] = 0x00cffa000000ffff,
		[GDT_USER_SS >> 3] = (1ul << 44) | (1ul << 47) | (1ul << 41) | (3ul << 45),
		[GDT_USER_CS >> 3] = (1ul << 43) | (1ul << 44) | (1ul << 47) | (1ul << 53) | (3ul << 45),
		[GDT_USER32_SS >> 3] = 0x00cff2000000ffff,
		[GDT_USER32_TLS >> 3] = 0,  // filled by set_thread_area
		[GDT_ESPFIX_SS >> 3] = 0,  // filled by prepare_espfix
		[GDT_KERNEL32_CS >> 3] = 0x00cf9a000000ffff,
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
	const uint8_t TRAP_RUN_USER32 = 234;
	const uint8_t TRAP_ABORT = 253;
	const uint8_t TRAP_INVALID_SYSCALL = 254;
	const uint8_t TRAP_SYSCALL = 255;
	const uint8_t TRAP_SYSCALL32 = 128;
	
	extern "C" { uint64_t __syscall_stack; }
	static Trapframe tf_run_user, tf_from_user, tf_to_user;
	static uint64_t user_time_limit_ns;
	
	extern "C"
	void trap_return(Trapframe *tf);
	
	extern "C"
	void syscall_return_record_tsc(Trapframe *tf);
	
	extern "C"
	void syscall_return_32_record_tsc(Trapframe *tf);
	
	extern "C" {
		void __trap_0x80_entry();
		void __trap_0x80_return(Trapframe *tf);
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
	
	struct user_desc {
		uint32_t entry_number;
		uint32_t base_addr;
		uint32_t limit;
		uint32_t seg_32bit:1;
		uint32_t contents:2;
		uint32_t read_exec_only:1;
		uint32_t limit_in_pages:1;
		uint32_t seg_not_present:1;
		uint32_t useable:1;
	} __attribute__((packed));
	
	extern "C" {
		uint64_t __trap_epc;
		uint64_t __trap_cr2;
	}
	
	extern "C"
	void trap_handler(Trapframe *tf) {
		// restore kernel tsc
		tf->tf_regs.tsc -= x86_64::rdmsr(x86_64::TSC_ADJUST);
		x86_64::wrmsr(x86_64::TSC_ADJUST, 0);
		
		int num = (int) tf->tf_num;
		LDEBUG("trap %d, CPL %d", (int) num, (int) (tf->tf_cs & 3));
		
		if (tf->tf_cs & 3) {  // trap from user
			LAPIC::eoi();
			LAPIC::timer_disable();
			tf_from_user = *tf;
			tf = &tf_run_user;
			tf->tf_regs.rax = num;
			uint64_t tsc_subtract = tf_to_user.tf_regs.tsc;
			tf->tf_regs.tsc = tf_from_user.tf_regs.tsc - tsc_subtract;
			
			uint64_t cr2;
			__asm__ volatile ("movq %%cr2, %0" : "=r" (cr2));
			LDEBUG("trap %d, rip %lx, cr2 %lx, ec %lu",
				num, tf_from_user.tf_rip, cr2, tf_from_user.tf_errorcode);
			LDEBUG("tf_cs %lu, tf_rflags %lx, tf_rsp %lx, tf_ss %lu",
				tf_from_user.tf_cs, tf_from_user.tf_rflags,
				tf_from_user.tf_rsp, tf_from_user.tf_ss);
			uint32_t ds, es, fs, gs;
			__asm__ volatile ("mov %%ds, %0" : "=r" (ds));
			__asm__ volatile ("mov %%es, %0" : "=r" (es));
			__asm__ volatile ("mov %%fs, %0" : "=r" (fs));
			__asm__ volatile ("mov %%gs, %0" : "=r" (gs));
			LDEBUG("ds %u, es %u, fs %u, gs %u", ds, es, fs, gs);
			
			__trap_epc = tf_from_user.tf_rip;
			__trap_cr2 = cr2;
			
			// Flush TLB to show A/D bits
			x86_64::lcr3(x86_64::rcr3());
		} else {
			if (num == TRAP_IRQ + PIC::IRQ_TIMER) {
				// timer interrupt caused by scheduler
				// just return with interrupts disabled
				LAPIC::eoi();
				LAPIC::timer_disable();
				tf->tf_rflags &= ~0x200;  // disable interrupts
			} else if (num == TRAP_RUN_USER || num == TRAP_RUN_USER32) {
				tf_run_user = *tf;
				
				// clear gdt tls
				gdt[GDT_USER32_TLS >> 3] = 0;
				x86_64::lgdt(&gdt_desc);
				
				using x86_64::rdmsr;
				using x86_64::wrmsr;
				if (num == TRAP_RUN_USER32) {
					// set up segment registers
					__asm__ volatile ("mov %0, %%ax" : : "i" (GDT_USER32_SS | 3));
					__asm__ volatile ("mov %ax, %ds");
					__asm__ volatile ("mov %ax, %es");
					__asm__ volatile ("mov %ax, %fs");
					__asm__ volatile ("mov %ax, %gs");
					__asm__ volatile ("" : : : "%ax");
					
					// enable int 0x80
					set_idt(0x80, (void *) &__trap_0x80_entry);
					idt[0x80].options |= 3 << 5;
					x86_64::lidt(&idt_desc);
				} else {  // TRAP_RUN_USER
					// clear segment registers
					__asm__ volatile ("mov $0, %ax");
					__asm__ volatile ("mov %ax, %ds");
					__asm__ volatile ("mov %ax, %es");
					__asm__ volatile ("mov %ax, %fs");
					__asm__ volatile ("mov %ax, %gs");
					__asm__ volatile ("" : : : "%ax");
					
					// disable int 0x80
					set_idt(0x80, __traps[0x80]);
					x86_64::lidt(&idt_desc);
				}
				
				if (user_time_limit_ns != 0) {
					// 0.5ms per interrupt, add 10ms + 0.1% for context switching
					uint64_t timer_cnt = user_time_limit_ns / 500000ul;
					LAPIC::timer_periodic_ns(500000, timer_cnt + timer_cnt / 1000ul + 20);
				}
				
				if (num == TRAP_RUN_USER) {
					syscall_return_record_tsc(&tf_to_user);
				} else {  // TRAP_RUN_USER_32
					syscall_return_32_record_tsc(&tf_to_user);
				}
			} else {
				uint64_t cr2;
				__asm__ volatile ("movq %%cr2, %0" : "=r" (cr2));
				LERROR("trap %d, rip %lx, cr2 %lx, errorcode %lu",
					num, tf->tf_rip, cr2, tf->tf_errorcode);
				LERROR("tf_cs %lu, tf_rflags %lx, tf_rsp %lx, tf_ss %lu",
					tf->tf_cs, tf->tf_rflags,
					tf->tf_rsp, tf->tf_ss);
				LERROR("Unhandled kernel trap %d", num);
				LFATAL("GG, reboot");
				Timer::microdelay((uint64_t) 10e6);  // 10s
				x86_64::reboot();
			}
		}
		
		trap_return(tf);
	}
	
	extern "C" {
		uint64_t __espfix_new_kernel_rsp;
	}
	
	// orig_rsp: rsp when iret
	// new_rsp: tf_rsp
	static void prepare_espfix(uint64_t orig_rsp, uint64_t new_rsp) {
		uint64_t base_addr = ((orig_rsp >> 16) - (new_rsp >> 16)) << 16;
		base_addr = (uint64_t) (uint32_t) base_addr;
		uint64_t tmp = 0xffff;
		tmp |= (base_addr & 0xffffff) << 16;
		tmp |= (base_addr >> 24) << 56;
		tmp |= 0x00cf920000000000;  // 32-bit, writable, 4k-granularity, kernel
		gdt[GDT_ESPFIX_SS >> 3] = tmp;
		__espfix_new_kernel_rsp = (uint32_t) (orig_rsp - base_addr);
	}
	
	extern "C"
	void trap_0x80_handler(Trapframe *tf) {
		// must from 32-bit userspace
		int syscall_num = (int) (uint32_t) tf->tf_regs.rax;
		
		if (syscall_num == ABI::DUCK_sys_set_thread_area) {
			uint64_t rbx = (uint64_t) (uint32_t) tf->tf_regs.rbx;
			uint64_t ret = -EINVAL;
			
			// prepare espfix
			prepare_espfix((uint64_t) &tf->tf_rip, tf->tf_rsp);
			
			using Memory::user_writable_check;
			if (!user_writable_check(rbx)) {
				ret = -EFAULT;
			} else if (!user_writable_check(rbx + sizeof(user_desc) - 1)) {
				ret = -EFAULT;
			} else {
				user_desc *desc = (user_desc *) rbx;
				if (desc->entry_number != -1u && desc->entry_number != GDT_USER32_TLS / 8) {
					ret = -EINVAL;
				} else {
					uint64_t tmp = 0;
					tmp = desc->limit & 0xffff;
					tmp |= (((uint64_t) desc->base_addr) & 0xffffff) << 16;
					tmp |= (uint64_t) (desc->base_addr >> 24) << 56;
					// TODO FIXME: Check other flags in user_desc
					tmp |= 0x00cff20000000000;  // 32-bit, writable, 4k-granularity
					gdt[GDT_USER32_TLS >> 3] = tmp;
					x86_64::lgdt(&gdt_desc);
					
					desc->entry_number = GDT_USER32_TLS >> 3;
					ret = 0;
				}
			}
			
			if (ret != 0) {
				// apply espfix
				x86_64::lgdt(&gdt_desc);
			}
			
			// return value
			tf->tf_regs.rax = ret;
			
			// fix userspace ss since it could be invalid
			tf->tf_ss = GDT_USER32_SS | 3;
			
			__trap_0x80_return(tf);
		} else {
			trap_handler(tf);
		}
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
		wrmsr(x86_64::CStar, (uint64_t) &__syscall_entry);  // for 32-bit
		wrmsr(x86_64::Star, ((uint64_t) GDT_KERNEL_CS << 32) | ((uint64_t) GDT_USER32_CS << 48));
		wrmsr(x86_64::SFMask, 0x200);  // mask interrupt
		__syscall_stack = task_state.rsp[0];  // used by syscall_entry
	}
	
	static Trapframe make_trapframe(uint64_t rip, uint64_t rsp) {
		Trapframe tf;
		memset(&tf, 0, sizeof(tf));
		tf.tf_rip = rip;
		tf.tf_cs = GDT_USER_CS | 3;
		tf.tf_rflags = x86_64::read_rflags() & ~0x8d5;  // clear all status bits
		tf.tf_rflags |= 0x200;  // user & NO IOPL
		tf.tf_rsp = rsp;
		tf.tf_ss = GDT_USER_SS | 3;
		return tf;
	}
	
	static Trapframe make_trapframe_32(uint64_t eip, uint64_t esp) {
		Trapframe tf;
		memset(&tf, 0, sizeof(tf));
		tf.tf_rip = eip;
		tf.tf_cs = GDT_USER32_CS | 3;
		tf.tf_rflags = x86_64::read_rflags() & ~0x8d5;  // clear all status bits
		tf.tf_rflags |= 0x200;  // user & NO IOPL
		tf.tf_rsp = esp;
		tf.tf_ss = GDT_USER32_SS | 3;
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
			if (syscall_num == SYS_tkill) {  // 64-bit abort()
				trap_num = TRAP_ABORT;
			} else if (syscall_num != ABI::DUCK_sys_exit) {
				trap_num = TRAP_INVALID_SYSCALL;
				return_code = 0;
			} else {
				return_code = (int32_t) (uint32_t) tf_from_user.tf_regs.rdi;
			}
		} else {
			return_code = 0;
		}
	}
	
	void run_user_32(uint64_t entry, uint64_t esp, uint64_t time_limit_ns,
		uint64_t &tsc, uint8_t &trap_num, int32_t &return_code) {
		LDEBUG_ENTER_RET();
		
		user_time_limit_ns = time_limit_ns;
		tf_to_user = make_trapframe_32(entry, esp);
		
		__asm__ volatile ("int %1" : "=a" (trap_num) : "i" (TRAP_RUN_USER32) : "memory");
		
		// export tsc
		tsc = tf_run_user.tf_regs.tsc;
		
		// export return-code
		if (trap_num == TRAP_SYSCALL32) {
			trap_num = TRAP_SYSCALL;   // for API compatibility
			int syscall_num = (int) (uint32_t) tf_from_user.tf_regs.rax;
			if (syscall_num != ABI::DUCK_sys_exit_32) {
				trap_num = TRAP_INVALID_SYSCALL;
				return_code = 0;
			} else {
				return_code = (int32_t) (uint32_t) tf_from_user.tf_regs.rbx;
			}
		} else if (trap_num == TRAP_SYSCALL) {
			trap_num = TRAP_INVALID_SYSCALL;
			return_code = 0;
		} else {
			return_code = 0;
		}
	}
}
