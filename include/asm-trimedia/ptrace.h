/* 
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_PTRACE_H
#define _ASM_TRIMEDIA_PTRACE_H

#include <linux/thread_info.h>

/*
*
*/
struct pt_regs {
	unsigned long sp; /*stack pointer r4*/
	unsigned long fp; /*frame pointer r3*/
	unsigned long rp; /*return pointer r2*/
	unsigned long spc;
	unsigned long dpc;
	unsigned long pcsw;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	/*used to save caller saved registers, from r33 to r63 */
	unsigned long caller_save_regs[63-33+1]; 
	/*any extension below*/
};

struct switch_stack
{
	unsigned long rp; /* r2 */
	unsigned long fp; /* r3 */
	unsigned long sp; /* r4 */
	unsigned long dpc;
	unsigned long spc;
	unsigned long pcsw;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	/*used to save callee saved registers, from r9 to r32*/
	unsigned long callee_save_regs[32-9+1];
};

#ifdef __KERNEL__

static inline int user_mode(struct pt_regs* regs)
{
	return ((regs->sp-1)&(~(THREAD_SIZE-1)))!=(unsigned long)current_thread_info();
}

#define instruction_pointer(regs)  ((regs)->dpc)
#define profile_pc(regs)  instruction_pointer(regs)

extern void show_regs(struct pt_regs* regs);

#endif

#endif /*_ASM_TRIMEDIA_PTRACE_H*/
