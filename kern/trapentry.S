/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

	TRAPHANDLER_NOEC(trap_00,  0)
	TRAPHANDLER_NOEC(trap_01,  1)
	TRAPHANDLER_NOEC(trap_02,  2)
	TRAPHANDLER_NOEC(trap_03,  3)
	TRAPHANDLER_NOEC(trap_04,  4)
	TRAPHANDLER_NOEC(trap_05,  5)
	TRAPHANDLER_NOEC(trap_06,  6)
	TRAPHANDLER_NOEC(trap_07,  7)
	TRAPHANDLER     (trap_08,  8)
	TRAPHANDLER_NOEC(trap_09,  9)
	TRAPHANDLER     (trap_10, 10)
	TRAPHANDLER     (trap_11, 11)
	TRAPHANDLER     (trap_12, 12)
	TRAPHANDLER     (trap_13, 13)
	TRAPHANDLER     (trap_14, 14)
	TRAPHANDLER     (trap_15, 15)
	TRAPHANDLER_NOEC(trap_16, 16)
	TRAPHANDLER_NOEC(trap_17, 17)
	TRAPHANDLER_NOEC(trap_18, 18)
	TRAPHANDLER_NOEC(trap_19, 19)
	
	TRAPHANDLER_NOEC(trap_32, 32)
	TRAPHANDLER_NOEC(trap_33, 33)
	TRAPHANDLER_NOEC(trap_34, 34)
	TRAPHANDLER_NOEC(trap_35, 35)
	TRAPHANDLER_NOEC(trap_36, 36)
	TRAPHANDLER_NOEC(trap_37, 37)
	TRAPHANDLER_NOEC(trap_38, 38)
	TRAPHANDLER_NOEC(trap_39, 39)
	TRAPHANDLER_NOEC(trap_40, 40)
	TRAPHANDLER_NOEC(trap_41, 41)
	TRAPHANDLER_NOEC(trap_42, 42)
	TRAPHANDLER_NOEC(trap_43, 43)
	TRAPHANDLER_NOEC(trap_44, 44)
	TRAPHANDLER_NOEC(trap_45, 45)
	TRAPHANDLER_NOEC(trap_46, 46)
	TRAPHANDLER_NOEC(trap_47, 47)

	TRAPHANDLER_NOEC(trap_48, 48)

/*
 * Lab 3: Your code here for _alltraps
 */
# TODO: save %fs, %gs into trapframe
_alltraps:
	pushl %es
	pushl %ds
	pushal
    movw $(GD_KD), %ax
    movw %ax, %ds
    movw %ax, %es
	pushl %esp
	call trap

