/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/linkage.h>
#include <linux/ptrace.h>

#include <asm/ptrace.h>
#include <asm/system.h>
#include <asm/thread_info.h>

static inline unsigned long
atomic_get_thread_flags(void)
{
	unsigned long ti_flags;
	unsigned long flags;
	local_irq_save(flags);
	ti_flags=current_thread_info()->flags;
	local_irq_restore(flags);

	return ti_flags;
}

asmlinkage extern void
end_of_interrupt(int syscall,struct pt_regs* regs)
{
	unsigned int flags;

	if(user_mode(regs)){
		/*return to userpace*/
		flags=atomic_get_thread_flags();
		while(flags & _TIF_WORK_MASK){
			if(flags & _TIF_NEED_RESCHED){
				schedule();
				flags=atomic_get_thread_flags();
				continue;	
			}
			do_notify_resume(syscall,flags,regs);
			break;
		}
	}else{
		/*return to kernelspace*/
#ifdef CONFIG_PREEMPT
		local_irq_disable();
		if(preempt_count()){
			/*
			 *FIXME: if (exception path) return;
			 */
			if(current_thread_info()->flags & _TIF_NEED_RESCHED){
				preempt_schedule_irq();
			}
		}
#endif 	
	}

	return;
}

