/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_SEMAPHORE_H
#define _ASM_TRIMEDIA_SEMAPHORE_H

#include <linux/linkage.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/rwsem.h>

#include <asm/atomic.h>

struct semaphore {
	atomic_t count;
	int sleepers;
	wait_queue_head_t wait;
};

#define __SEMAPHORE_INITIALIZER(name,count)				      \
	{ ATOMIC_INIT (count), 0,					      \
	  __WAIT_QUEUE_HEAD_INITIALIZER ((name).wait) }

#define __MUTEX_INITIALIZER(name)					      \
	__SEMAPHORE_INITIALIZER (name,1)

#define __DECLARE_SEMAPHORE_GENERIC(name,count)	\
	struct semaphore name = __SEMAPHORE_INITIALIZER (name,count)

#define DECLARE_MUTEX(name)		__DECLARE_SEMAPHORE_GENERIC (name,1)
#define DECLARE_MUTEX_LOCKED(name)	__DECLARE_SEMAPHORE_GENERIC (name,0)

static inline void sema_init (struct semaphore *sem, int val)
{
	atomic_set(&sem->count, val);
	sem->sleepers = 0;
	init_waitqueue_head(&sem->wait);
}

static inline void init_MUTEX (struct semaphore *sem)
{
	sema_init (sem, 1);
}

static inline void init_MUTEX_LOCKED (struct semaphore *sem)
{
	sema_init (sem, 0);
}

#if 0
/*
 *FIXME: I realy don't know the usage of the following routine, 
 *let alone implement them
 */
/*
 * special register calling convention
 */
asmlinkage void __down_failed (void);
asmlinkage int  __down_interruptible_failed (void);
asmlinkage int  __down_trylock_failed (void);
asmlinkage void __up_wakeup (void);
#endif 

/*
 * implemented at lib/semaphore-sleeper.c
 */
extern void __down (struct semaphore * sem);
extern int  __down_interruptible (struct semaphore * sem);
extern int  __down_trylock (struct semaphore * sem);
extern void __up (struct semaphore * sem);

static inline void down (struct semaphore * sem)
{
	might_sleep();
	if (atomic_dec_return (&sem->count) < 0)
		__down (sem);
}

static inline int down_interruptible (struct semaphore * sem)
{
	int ret = 0;
	might_sleep();
	if (atomic_dec_return (&sem->count) < 0)
		ret = __down_interruptible (sem);
	return ret;
}

static inline int down_trylock (struct semaphore *sem)
{
	int ret = 0;
	if (atomic_dec_return (&sem->count) < 0)
		ret = __down_trylock (sem);
	return ret;
}

static inline void up (struct semaphore * sem)
{
	if (atomic_inc_return (&sem->count) <= 0)
		__up (sem);
}

#endif /* _ASM_TRIMEDIA_SEMAPHORE_H*/

