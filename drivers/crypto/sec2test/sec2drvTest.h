
/****************************************************************************
 * sec2drvTest.h - master include file for for SEC2 device driver test suite
 ****************************************************************************
 * Copyright (c) 2004-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/

/* Revision History:
 * 1.1.0 Dec 05,2004 sec - prep for linux-compatible driver release
 * 1.2   02-Feb-2005
 * 1.3   Aug 11,2005 sec - correct types in memory-push interfaces
 */

#ifdef _LINUX_USERMODE_
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#endif

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/config.h>
#include <linux/sched.h>
#include <linux/slab.h> 
#include <linux/mm.h>
#include <asm/atomic.h>
#include <asm/semaphore.h>
#undef malloc
#undef free
#define free kfree
#define malloc(a) kmalloc(a, GFP_KERNEL)
#undef semGive
#undef semTake
#define semDelete(x)
#define semGive(a) up(&a)
#define semTake(a, b) down_interruptible_timeout(&a, b)
#undef printf
#define printf printk
#define ioctl subIoctl

/* MPC834x is only SEC 2.0 */
#ifdef CONFIG_MPC8548
#define FEATURE_EXISTS_SEC2_1
#endif

int SEC2_ioctl(struct inode  *nd,
               struct file   *fil,
               unsigned int   ioctlCode,
               unsigned long  param);

int subIoctl(int fd, unsigned long code, unsigned long param);

extern long __down_interruptible_timeout(struct semaphore *sem, long timeout);
extern inline int down_interruptible_timeout(struct semaphore *sem, long timeout)
{
    int ret = 0;
#ifdef WAITQUEUE_DEBUG
    CHECK_MAGIC(sem->__magic);
#endif
    
    if (atomic_dec_return(&sem->count) < 0)
        ret = __down_interruptible_timeout(sem, timeout);
    
    smp_wmb();
    return ret;
}

#define taskIdSelf() current->pid
#endif



void armCompletion(void *req);
int waitCompletion(char *testname, int iostat, void *req);
void dumpm(unsigned char *csData, int nBytes);
int putKmem(int fd, void *data, void **block, unsigned int size);
int getKmem(int fd, void *data, void **block, unsigned int size);
#ifdef __KERNEL__
int freeKmem(int fd, void **block);
#else
int freeKmem(int fd, void *block);
#endif

#define SEC2_DEVNAME "/dev/sec2"
