/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_THREAD_INFO_H
#define _ASM_TRIMEDIA_THREAD_INFO_H

#include <asm/page.h>

#ifdef __KERNEL__

#ifndef __ASSEMBLY__

/*
 * Size of kernel stack for each process. This must be a power of 2...
 */
#ifdef CONFIG_4KSTACKS
#define THREAD_SIZE_ORDER (0)
#else
#define THREAD_SIZE_ORDER (1)
#endif
                                                                                
/*
 * for asm files, THREAD_SIZE is now generated by asm-offsets.c
 */
#define THREAD_SIZE (PAGE_SIZE<<THREAD_SIZE_ORDER)

/*
 * low level task data.
 */
struct thread_info {
	struct task_struct *task;		/* main task structure */
	struct exec_domain *exec_domain;	/* execution domain */
	unsigned long	   flags;		/* low level flags */
	int		   cpu;			/* cpu we're on */
	int		   preempt_count;	/* 0 => preemptable, <0 => BUG */
	struct restart_block restart_block;
	unsigned long ksp; 			/* kernel stack pointer*/
};

/*
 * macros/functions for gaining access to the thread information structure
 * FIXME: I do not know what the preempt_count should be? 1 or 0.
 */
#define INIT_THREAD_INFO(tsk)			\
{						\
	.task		= &tsk,			\
	.exec_domain	= &default_exec_domain,	\
	.flags		= 0,			\
	.cpu		= 0,			\
	.restart_block	= {			\
		.fn = do_no_restart_syscall,	\
	},					\
	.ksp 		= 0, 			\
}

extern union thread_union* __init_thread_union_ptr;
#define init_thread_union  (*__init_thread_union_ptr)

#define init_thread_info	(init_thread_union.thread_info)
#define init_stack		(init_thread_union.stack)

extern struct thread_info* __current_thread_info;
/* how to get the thread information struct from C */
static inline struct thread_info* current_thread_info(void)
{
	return __current_thread_info;
}

/* thread information allocation */
#define alloc_thread_info(tsk) ((struct thread_info *) \
				__get_free_pages(GFP_KERNEL, THREAD_SIZE_ORDER))
#define free_thread_info(ti)	free_pages((unsigned long) (ti), THREAD_SIZE_ORDER)
#define get_thread_info(ti)	get_task_struct((ti)->task)
#define put_thread_info(ti)	put_task_struct((ti)->task)
#endif /* __ASSEMBLY__ */

#define	PREEMPT_ACTIVE	0x4000000

/*
 * thread information flag bit numbers
 */
#define TIF_SYSCALL_TRACE	0	/* syscall trace active */
#define TIF_NOTIFY_RESUME	1	/* resumption notification requested */
#define TIF_SIGPENDING		2	/* signal pending */
#define TIF_NEED_RESCHED	3	/* rescheduling necessary */
#define TIF_POLLING_NRFLAG	4	/* true if poll_idle() is polling
					   TIF_NEED_RESCHED */
#define TIF_MEMDIE		5

/* as above, but as bit values */
#define _TIF_SYSCALL_TRACE	(1<<TIF_SYSCALL_TRACE)
#define _TIF_NOTIFY_RESUME	(1<<TIF_NOTIFY_RESUME)
#define _TIF_SIGPENDING		(1<<TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1<<TIF_NEED_RESCHED)
#define _TIF_POLLING_NRFLAG	(1<<TIF_POLLING_NRFLAG)

#define _TIF_WORK_MASK		0x0000FFFE	/* work to do on interrupt/exception return */

#endif /* __KERNEL__ */

#endif /* _ASM_TRIMEDIA_THREAD_INFO_H*/

