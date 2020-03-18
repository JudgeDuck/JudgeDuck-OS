#include <stdio.h>
#include <stdint.h>

#include <inc/trap.hpp>
#include <inc/x86_64.hpp>
#include <inc/lapic.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>

extern void *__traps[256];
extern void *kernel_stack_top;

namespace Trap {
	struct InterruptDescriptor {
		uint16_t ptr_low;
		uint16_t cs;
		uint16_t options;
		uint16_t ptr_mid;
		uint32_t ptr_high;
		uint32_t reserved;
	} __attribute__((packed, aligned(16)));
	
	struct PushRegs {
		uint64_t rax, rcx, rdx, rbx, rbp, rsi, rdi;
		uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
	} __attribute__((packed));
	
	struct Trapframe {
		PushRegs tf_regs;
		uint64_t tf_num;
		uint64_t tf_errorcode;
		uint64_t tf_rip;
		uint64_t tf_cs;
		uint64_t tf_rflags;
		uint64_t tf_rsp;
		uint64_t tf_ss;
	} __attribute__((packed));
	
	struct TaskState {
		uint32_t reserved1;
		uint64_t rsp0;
		uint64_t rsp1;
		uint64_t rsp2;
		uint64_t reserved2;
		uint64_t ist1;
		uint64_t ist2;
		uint64_t ist3;
		uint64_t ist4;
		uint64_t ist5;
		uint64_t ist6;
		uint64_t ist7;
		uint64_t reserved3;
		uint16_t reserved4;
		uint16_t IOPB_offset;
	} __attribute__((packed, aligned(16))) task_state;
	
	const int GDT_KERNEL_CS = 8;
	const int GDT_TSS = 16;
	static uint64_t gdt[] = {
		0,
		[GDT_KERNEL_CS >> 3] = (1ul << 43) | (1ul << 44) | (1ul << 47) | (1ul << 53),
		[GDT_TSS >> 3] = (sizeof(TaskState) - 1) | (((uint64_t) &task_state & 0xffffff) << 16) | (9ul << 40) | (3ul << 45) | (1ul << 47) | (1ul << 53) | (((uint64_t) &task_state >> 24) << 56),
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
	
	extern "C"
	void trap_return(Trapframe *tf);
	
	extern "C"
	void trap_handler(Trapframe *tf) {
		int num = (int) tf->tf_num;
		LINFO() << "trap " << num;
		if (num == 32) {
			LINFO() << "LAPIC works!";
			LAPIC::eoi();
		}
		trap_return(tf);
	}
	
	static void set_idt(int id, void *addr) {
		uint64_t a = (uint64_t) addr;
		
		idt[id] = (InterruptDescriptor) {
			.ptr_low = (uint16_t) a,
			.cs = GDT_KERNEL_CS,
			.options = (14 << 8) | (1 << 15),  // must-be-one bits | present
			.ptr_mid = (uint16_t) (a >> 16),
			.ptr_high = (uint32_t) (a >> 32),
			.reserved = 0
		};
	}
	
	void init() {
		LINFO_ENTER();
		
		for (int i = 0; i < 256; i++) {
			set_idt(i, __traps[i]);
		}
		
		x86_64::lidt(&idt_desc);
		
		task_state.rsp0 = (uint64_t) kernel_stack_top;
		task_state.IOPB_offset = sizeof(TaskState);
		x86_64::lgdt(&gdt_desc);
		x86_64::ltr(GDT_TSS);
	}
}
