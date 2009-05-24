
/****************************************************************************
 * sec2_io.c -- bus operations for SEC2 Driver 
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2002.  All rights reserved
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/



/* Revision History:
 * 1.0   Aug 24,2003 dgs adapted from the final version of mpc18x
 *       May 12,2004 sec - switched core interrupt for 8555 configuration
 * 1.1.0 Dec 05,2004 sec - merge in linux support from prior versions
 * 1.1.1 Dec 16,2004 sec - remove lock variable, remove old diagnostics
 * 1.3   Jan 27,2005 sec - updated for kernel 2.6 on 8548
 */

#include "Sec2Driver.h"


void IOInitSemaphores()
{
    /* initialize semaphores */

#ifdef __KERNEL__
    spin_lock_init(&ChannelAssignLock);
#endif
}





/*
 * IOConnectInterrupt()
 */
 
#ifdef __KERNEL__
IO_STATUS IOConnectInterrupt()
{
    if ((request_irq(INUM_SEC2,
                     InterruptServiceRoutine,
                     0,
                     "mpc-sec",
                     NULL)) != 0)
        return -1;
    
    return (SEC2_SUCCESS);
}

#endif




#ifdef __KERNEL__

#include <linux/sched.h>
#include <asm/atomic.h>
#include <asm/semaphore.h>

unsigned long irq_flag;  /* here for spin locks to use */

static inline int __sem_update_count(struct semaphore *sem, int incr)
{
    int old_count, tmp;
    
    __asm__ __volatile__("\n"
"1: lwarx   %0,0,%3\n"
"   srawi   %0,%0,31\n"
"   andc    %1,%0,%1\n"
"   add     %1,%1,%4\n"
    PPC405_ERR77(0,%3)
"   stwcx.  %1,0,%3\n"
"   bne     1b"
    : "=&r" (old_count), "=&r" (tmp), "=m" (sem->count)
    : "r" (&sem->count), "r" (incr), "m" (sem->count)
    : "cc");
    
    return old_count;
}

long __down_interruptible_timeout(struct semaphore *sem, long timeout)
{
    int retval = 0;
    struct task_struct *tsk = current;
    DECLARE_WAITQUEUE(wait, tsk);
    
    tsk->state = TASK_INTERRUPTIBLE;
    add_wait_queue_exclusive(&sem->wait, &wait);
    smp_wmb();
    
    while (__sem_update_count(sem, -1) <= 0) {
        if (signal_pending(current)) {
            __sem_update_count(sem, 0);
            retval = -EINTR;
            break;
        }
        timeout = schedule_timeout(timeout);
        tsk->state = TASK_INTERRUPTIBLE;
    }
    tsk->state = TASK_RUNNING;
    remove_wait_queue(&sem->wait, &wait);
    wake_up(&sem->wait);
    return timeout >= 0 ? 0 : -1;
}

#endif
