/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_SYSTEM_H 
#define _ASM_TRIMEDIA_SYSTEM_H

#ifdef __KERNEL__

#include <tmInterrupts.h>
#include <ops/custom_ops.h>

#define prepare_to_switch() do{ } while(0)

extern struct thread_info* __switch_to(struct thread_info* prev,struct thread_info* next);

#define switch_to(prev,next,last)   					\
do{ 									\
	last=__switch_to((prev)->thread_info,(next)->thread_info)->task;\
}while(0)

#define local_irq_enable()   		writepcsw(tmPCSW_IEN,tmPCSW_IEN)
#define local_irq_disable()  		writepcsw(0,tmPCSW_IEN)
#define local_irq_save(x)    		do{ x=readpcsw(); writepcsw(0,tmPCSW_IEN); }while(0)
#define local_save_flags(x)  		x=readpcsw()
#define local_irq_restore(x) 		writepcsw(x,tmPCSW_IEN)
#define irqs_disabled() 		(readpcsw()&tmPCSW_IEN)==0	

/*
 * Force strict CPU ordering.
 * FIXME: I DO NOT know how to implement this for TriMedia
 */
#define nop()  
#define mb()  
#define rmb()
#define wmb() 
#define set_rmb(var, value)    do { xchg(&var, value); } while (0)
#define set_mb(var, value)     set_rmb(var, value)
#define set_wmb(var, value)    do { var = value; wmb(); } while (0)

#define smp_mb()	barrier()
#define smp_rmb()	barrier()
#define smp_wmb()	barrier()
#define smp_read_barrier_depends()	do { } while(0)

#define xchg(ptr, with) \
  ((__typeof__ (*(ptr)))__xchg ((unsigned long)(with), (ptr), sizeof (*(ptr))))
#define tas(ptr) (xchg ((ptr), 1))

static inline unsigned long __xchg (unsigned long with,
				    __volatile__ void *ptr, int size)
{
	unsigned long tmp, flags;

	local_irq_save (flags);

	switch (size) {
	case 1:
		tmp = *(unsigned char *)ptr;
		*(unsigned char *)ptr = with;
		break;
	case 2:
		tmp = *(unsigned short *)ptr;
		*(unsigned short *)ptr = with;
		break;
	case 4:
		tmp = *(unsigned long *)ptr;
		*(unsigned long *)ptr = with;
		break;
	}

	local_irq_restore (flags);

	return tmp;
}

/*
 *FIXME: I realy do not know how to implement it
 */
#define HARD_RESET_NOW()  do{ local_irq_disable(); while(1);}while(0)	

/*
 *FIXME: I realy do not know how to implement it.
 */
#define arch_align_stack(x) (x)

#endif /*__KERNEL__*/ 

#endif /* _ASM_TRIMEDIA_SYSTEM_H */

