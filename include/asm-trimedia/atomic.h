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

#ifndef _ASM_TRIMEDIA_ATOMIC_H
#define _ASM_TRIMEDIA_ATOMIC_H

#include <linux/config.h>

#include <asm/system.h>

#ifdef CONFIG_SMP
#error SMP not supported
#endif

typedef struct { volatile int counter; } atomic_t;

#define ATOMIC_INIT(i)	{ (i) }

#ifdef __KERNEL__

#define atomic_read(v)		((v)->counter)
#define atomic_set(v,i)		(((v)->counter) = (i))

#if 0
extern int atomic_add_return (int i, volatile atomic_t *v);
extern int atomic_sub_return (int i, volatile atomic_t *v);
extern void atomic_set_mask(unsigned long mask, unsigned long *v);
extern void atomic_clear_mask (unsigned long mask, unsigned long *addr);
#else

static inline int
atomic_add_return(int i,atomic_t* v)
{
	unsigned long flags;
	signed int val;

	local_irq_save(flags);
	val=v->counter;
	val=val+i;
	v->counter=val;
	local_irq_restore(flags);

	return val;
}

static inline int
atomic_sub_return(int i,atomic_t* v)
{
	unsigned long flags;
	signed int val;

	local_irq_save(flags);
	val=v->counter;
	val=val-i;
	v->counter=val;
	local_irq_restore(flags);

	return val;
}

static inline void
atomic_set_mask(unsigned long mask,unsigned long* addr)
{
	unsigned long flags;

	local_irq_save(flags);
	*addr |= mask;
	local_irq_restore(flags);

	return;
}

static inline void
atomic_clear_mask(unsigned long mask,unsigned long* addr)
{
	unsigned long flags;

	local_irq_save(flags);
	*addr &= ~mask;
	local_irq_restore(flags);

	return;
}

#endif 

#endif

#define atomic_add(i, v)	atomic_add_return ((i), (v))
#define atomic_sub(i, v)	atomic_sub_return ((i), (v))

#define atomic_dec_return(v)	atomic_sub_return (1, (v))
#define atomic_inc_return(v)	atomic_add_return (1, (v))
#define atomic_inc(v) 		atomic_inc_return (v)
#define atomic_dec(v) 		atomic_dec_return (v)

/*
 * atomic_inc_and_test - increment and test
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
#define atomic_inc_and_test(v) (atomic_inc_return(v) == 0)

#define atomic_sub_and_test(i,v)	(atomic_sub_return ((i), (v)) == 0)
#define atomic_dec_and_test(v)		(atomic_sub_return (1, (v)) == 0)
#define atomic_add_negative(i,v)	(atomic_add_return ((i), (v)) < 0)

/*
 *FIXME: I do not know how to implement it for TriMedia
 */
#if 1
#define smp_mb__before_atomic_dec()	barrier()
#define smp_mb__after_atomic_dec()	barrier()
#define smp_mb__before_atomic_inc()	barrier()
#define smp_mb__after_atomic_inc()	barrier()
#endif 

#endif /* _ASM_TRIMEDIA_ATOMIC_H */

