/* 
 * arch/trimedia/kernel/traps.c: Sets up all exception vectors
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/config.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/a.out.h>
#include <linux/user.h>
#include <linux/string.h>
#include <linux/linkage.h>
#include <linux/init.h>
#include <linux/ptrace.h>

#include <asm/setup.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/siginfo.h>

#include <tmInterrupts.h>

extern void system_call(void);

extern void
show_registers(struct pt_regs* regs)
{
	printk("begin to show registers+++++++++++++++++++++++++++++++++++++++\n");
	printk("regs->sp=%x, regs->fp=%x, regs->rp=%x\n",regs->sp,regs->fp,regs->rp);
	printk("regs->spc=%x, regs->dpc=%x, regs->pcsw=%x\n",regs->spc,regs->dpc,regs->pcsw);
	printk("regs->r5=%x, regs->r6=%x\n",regs->r5,regs->r6);
	printk("regs->r7=%x, regs->r8=%x\n",regs->r7,regs->r8);
	printk("regs->r33=%x, regs->r34=%x, regs->r35=%x, regs->r36=%x\n",
			regs->caller_save_regs[0],regs->caller_save_regs[1],
			regs->caller_save_regs[2],regs->caller_save_regs[3]);
	printk("regs->r37=%x, regs->r38=%x, regs->r39=%x\n",
			regs->caller_save_regs[4],regs->caller_save_regs[5],
			regs->caller_save_regs[6]);
	printk("end to show registers+++++++++++++++++++++++++++++++++++++++++\n");
}

extern void register_exception_handlers(void);
extern void tmbreak_trapexecution(void *low, void *high, int inside);

/*
 * Register system call handler and exception handlers
 */
void __init trap_init(void)
{
	/*
	 * Register system call handler.
	 */
	tmLibdevErr_t ret;
	intInstanceSetup_t setup;

	setup.enabled=True;
	setup.handler=system_call;
	setup.level_triggered=False;
	setup.priority=intPRIO_6;

	ret=intOpen(intRTOS);
	if(ret!=TMLIBDEV_OK){
		panic("trap_init: intOpen failed!\n");
	}

	ret=intInstanceSetup(intRTOS,&setup);
	if(ret!=TMLIBDEV_OK){
		panic("trap_init: intInstanceSetup failed!\n");
	}

	/*Register exception handlers*/
	register_exception_handlers();

	{
		char* start=NULL;
		extern int __Rt_mbase;

		start=&__Rt_mbase;
		tmbreak_trapexecution(0,start,1);
	}

	return;
}

void die_if_kernel(char *str, struct pt_regs *fp, int nr)
{
}

void show_stack(struct task_struct *task, unsigned long *stack)
{
}

/*
 * The architecture-independent backtrace generator
 */
void dump_stack(void)
{
}

EXPORT_SYMBOL(dump_stack);

