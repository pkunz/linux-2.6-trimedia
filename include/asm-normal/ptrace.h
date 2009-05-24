#ifndef _ASM_NORMAL_PTRACE_H
#define _ASM_NORMAL_PTRACE_H

#include <linux/config.h>

/* processor status and miscellaneous context registers.  */

/* this struct defines the way the registers are stored on the
   stack during a system call. */
struct pt_regs {
	/* Saved main processor registers. */
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r0;
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r7;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	long syscall_nr;
	/* Saved main processor status and miscellaneous context registers. */
};


#ifdef __KERNEL__
#define user_mode(regs) (0)

#define instruction_pointer(regs) (1)
#define profile_pc(regs) instruction_pointer(regs)

extern void show_regs(struct pt_regs *);

#endif /* __KERNEL */

#endif /* _ASM_NORMAL_PTRACE_H */

