
/****************************************************************************
 * testAll.c - Top-level known-answer test manager for SEC2 device driver
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
 *  1.1.0 Dec 05,2004 sec - prep for linux-compatible driver release
 *  1.1.1 Dec 15,2004 sec - bring DES and PKHA cases in
 *  1.2   02-Feb-2005 sec - clean up notifier handling
 *  1.3   Aug 11,2005 sec - add KFHA, RAID, and TLS cases for 8548
 *  1.5   Oct 13,2005 sec - add testAux module
 */


#include "sec2drvTest.h"
#include "Sec2.h"

#ifdef __KERNEL__
#include <linux/init.h>
#define MODVERSIONS
#endif

#define TESTALL_PKHA
#define TESTALL_AES
#define TESTALL_HASH
#define TESTALL_HMAC
#define TESTALL_IPSEC
#define TESTALL_RNG
#define TESTALL_RC4
#define TESTALL_DES
#define TESTALL_CCMP

#ifdef FEATURE_EXISTS_SEC2_1
#define TESTALL_KEA
#define TESTALL_ICV
#define TESTALL_TLS
#define TESTALL_XOR
#endif

#ifndef _LINUX_USERMODE_
#define TESTALL_SCATTER
#define TESTALL_AUX
#endif

int testPKHA(int);
int testIPSEC(int);
int testAesa(int);
int testAfha(int);
int testAFHActx(int);
int testDes(int, int, int);
int testDesECB(int, int, int);
int testHmac(int);
int testKEA(int);
int testRng(int);
int testSG(int);
int testccmp(int);
int testccmpdec(int);
int testXOR(int);
int hash_test(int);
int testHmac(int);
int testTLS(int);
int testICV(int);
int testAux(int);

/* declare completion handlers for all modules */
static void notifyDone(void);
static void notifyError(void);

/*
 * This next block of code comprises some common synchronization, memory service,
 * and event handing functionality common to the entire test series
 */
 
/*
 * If it's Linux in kernel mode, we'll use a semaphore as a request
 * interlock. If it's a Linux user-mode test, we'll use a signal instead
 */



#ifdef __KERNEL__
static DECLARE_MUTEX(notifySemId);
#endif

#ifdef LINUX_USERMODE
static int fd;
static volatile char proceed;
#endif



/* Normal and error completion handlers */
static void notifyDone(void)
{
#ifndef LINUX_USERMODE
    semGive(notifySemId);
#else
    proceed++;
#endif
    return;
}

static void notifyError(void)
{

#ifndef LINUX_USERMODE
    semGive(notifySemId);
#else
    proceed++;
#endif
    return;
}


static const char errstrMemAlloc[]    = { "MEMORY_ALLOCATION" };
static const char errstrInvChn[]      = { "INVALID_CHANNEL" };
static const char errstrInvCha[]      = { "INVALID_CHA_TYPE" };
static const char errstrInvOpId[]     = { "INVALID_OPERATION_ID" };
static const char errstrChnNotAvail[] = { "CHANNEL_NOT_AVAILABLE" };
static const char errstrChaNotAvail[] = { "CHA_NOT_AVAILABLE" };
static const char errstrInvLen[]      = { "INVALID_LENGTH" };
static const char errstrAlign[]       = { "OUTPUT_BUFFER_ALIGNMENT" };
static const char errstrAddr[]        = { "ADDRESS_PROBLEM" };
static const char errstrNoReqs[]      = { "INSUFFICIENT_REQS" };
static const char errstrChaErr[]      = { "CHA_ERROR" };
static const char errstrNullRq[]      = { "NULL_REQUEST" };
static const char errstrTimeOut[]     = { "REQUEST_TIMED_OUT" };
static const char errstrMalloc[]      = { "MALLOC_FAILED" };
static const char errstrFree[]        = { "FREE_FAILED" };
static const char errstrParity[]      = { "PARITY_SYSTEM_ERROR" };
static const char errstrIncomp[]      = { "INCOMPLETE_POINTER" };
static const char errstrTEA[]         = { "TEA_ERROR" };
static const char errstrFrag[]        = { "FRAGMENT_POOL_EXHAUSTED" };
static const char errstrFIFO[]        = { "FETCH_FIFO_OVERFLOW" };
static const char errstrBus[]         = { "BUS_MASTER_ERROR" };
static const char errstrScatter[]     = { "SCATTER_LIST_ERROR" };
static const char errstrUnknown[]     = { "UNKNOWN_ERROR" };
static const char errstrNotFound[]    = { "IO_CARD_NOT_FOUND" };
static const char errstrIOalloc[]     = { "IO_MEMORY_ALLOCATE_ERROR" };
static const char errstrIOIO[]        = { "IO_IO_ERROR" };
static const char errstrIntAlloc[]    = { "IO_INTERRUPT_ALLOCATE_ERROR" };
static const char errstrBAR0[]        = { "CANNOT_SETUP_BAR0_ERROR" };
static const char errstrCancel[]      = { "CANCELLED_REQUEST" };
static const char errstrInvAddr[]     = { "INVALID_ADDRESS" };

static const char *mapErrStr(int status)
{
    switch(status)
    {
        case SEC2_MEMORY_ALLOCATION:
            return(errstrMemAlloc);

        case SEC2_INVALID_CHANNEL:
            return(errstrInvChn);

        case SEC2_INVALID_CHA_TYPE:
            return(errstrInvCha);

        case SEC2_INVALID_OPERATION_ID:
            return(errstrInvOpId);

        case SEC2_CHANNEL_NOT_AVAILABLE:
            return(errstrChnNotAvail);

        case SEC2_CHA_NOT_AVAILABLE:
            return(errstrChaNotAvail);

        case SEC2_INVALID_LENGTH:
            return(errstrInvLen);

        case SEC2_OUTPUT_BUFFER_ALIGNMENT:
            return(errstrAlign);

        case SEC2_ADDRESS_PROBLEM:
            return(errstrAddr);

        case SEC2_INSUFFICIENT_REQS:
            return(errstrNoReqs);

        case SEC2_CHA_ERROR:
            return(errstrChaErr);

        case SEC2_NULL_REQUEST:
            return(errstrNullRq);

        case SEC2_REQUEST_TIMED_OUT:
            return(errstrTimeOut);

        case SEC2_MALLOC_FAILED:
            return(errstrMalloc);

        case SEC2_FREE_FAILED:
            return(errstrFree);

        case SEC2_PARITY_SYSTEM_ERROR:
            return(errstrParity);

        case SEC2_INCOMPLETE_POINTER:
            return(errstrIncomp);

        case SEC2_TEA_ERROR:
            return(errstrTEA);

        case SEC2_FRAGMENT_POOL_EXHAUSTED:
            return(errstrFrag);

        case SEC2_FETCH_FIFO_OVERFLOW:
            return(errstrFIFO);

        case SEC2_BUS_MASTER_ERROR:
            return(errstrBus);

        case SEC2_SCATTER_LIST_ERROR:
            return(errstrScatter);

        case SEC2_IO_CARD_NOT_FOUND:
            return(errstrNotFound);

        case SEC2_IO_MEMORY_ALLOCATE_ERROR:
            return(errstrIOalloc);

        case SEC2_IO_IO_ERROR:
            return(errstrIOIO);

        case SEC2_IO_INTERRUPT_ALLOCATE_ERROR:
            return(errstrIntAlloc);

        case SEC2_CANNOT_SETUP_BAR0_ERROR:
            return(errstrBAR0);

        case SEC2_CANCELLED_REQUEST:
            return(errstrCancel);

        case SEC2_INVALID_ADDRESS:
            return(errstrInvAddr);

        case SEC2_UNKNOWN_ERROR:
        default:
            return(errstrUnknown);
    }
}



/* Basic "completion init" request. The idea is to set up handers in the user's request */
/* block as appropriate, either signal handlers for user mode, or a semaphore release */
/* otherwise */

void armCompletion(void *req)
{
    GENERIC_REQ *rq;
    
    rq = req;

#ifndef LINUX_USERMODE
    rq->notify          = (PSEC2_NOTIFY_ROUTINE)notifyDone;
    rq->notify_on_error = (PSEC2_NOTIFY_ON_ERROR_ROUTINE)notifyError;
    rq->notifyFlags     = 0;
#else
    rq->notify          = (void *)getpid();
    rq->notify_on_error = (void *)getpid();
    rq->notifyFlags     = (NOTIFY_IS_PID | NOTIFY_ERROR_IS_PID);
    signal(SIGUSR1, (void (*)())notifyDone);
    signal(SIGUSR2, (void (*)())notifyError);
    proceed = 0;
#endif
}


#ifdef __KERNEL__
/* substitute the 3-arg form for a direct call to the 4-arg form */
int subIoctl(int fd, unsigned long code, unsigned long param)
{
    return SEC2_ioctl(NULL, NULL, code, param);
}
#endif




/*
 * Basic "wait" block, assuming that armCompletion() was already called to
 * set things up before the ioctl() call to the driver to process the
 * request block. This gets called with 3 arguments:
 *
 * testname - points to a string to be printed with the error information
 *            in the event of a late error having been posted to the 
 *            user's request block
 *
 * iostat   - error code returned from prior ioctl call
 * 
 * req      - points to the user's request block, the status field will
 *            be checked for any error following the driver's interrupt
 *            service.
 */

int waitCompletion(char *testname, int iostat, void *req)
{
    GENERIC_REQ *rq;
#ifndef __KERNEL__
    int          timestat;
    unsigned int timeout = 0;
#endif    
    
    rq = req;
    
    /* firstly, check for obvious processing errors either from the OS or */
    /* the driver, which would have precluded kicking off any processing  */

    if (iostat != SEC2_SUCCESS) {
        if (iostat == -1)
            printf("%s ioctl failed\n", testname);
        else
            printf ("%s, driver error 0x%08x processing request\n", testname, iostat);

        return iostat;
    }


    /* If we got this far, then the driver will have triggered some kind of */
    /* processing. Wait for the ioctl() call completion with whatever the   */
    /* right mechanism is for the mode we're using                          */


#ifndef LINUX_USERMODE
    if ((iostat = semTake(notifySemId, 300))) {
        printf("Request Timeout\n");
        return iostat;
    }
#else

    while(!proceed)  
    {
        usleep(100);
        if (++timeout >= 30000) {
            printf("Request Timeout\n");
            return -1;
        }
    } 
#endif
    

    /* If an error occurred as a consequence of a the request (i.e. something a */
    /* problem for the hardware), then the status in the request header will    */
    /* be updated with it, so show this                                         */

    if (rq->status != SEC2_SUCCESS)
    {
        printf("%s, error 0x%08x (%s) reported post-request\n", testname, rq->status, mapErrStr(rq->status));
        return rq->status;
    }

    return 0;
}






/*
 * These functions handle memory buffer differences for each environment
 *
 * putKmem() allocates kernel memory for data to be placed into, and if the
 * data pointer is not NULL, will copy the data block.
 *
 * getKmem() copies post-operation data back to the user's buffer
 *
 * freeKmem() frees a kernel memory block after use
 */
#ifdef _LINUX_USERMODE_
int putKmem(int fd, void *data, void **block, unsigned int size)
{
    MALLOC_REQ  kmem;
    int iostat;
    
    kmem.pid = getpid();
    kmem.sz  = size;
    iostat = ioctl(fd, IOCTL_MALLOC, &kmem);
    if (iostat == SEC2_MALLOC_FAILED)
        return iostat;
        
    *block = kmem.ptr;
    
    if (data != NULL) 
    {
        kmem.from = data;
        kmem.to = kmem.ptr;
        iostat = ioctl(fd, IOCTL_COPYFROM, &kmem);
    }
    
    return 0;
}



int getKmem(int fd, void *data, void **block, unsigned int size)
{
    MALLOC_REQ kmem;
    int iostat;
    
    kmem.pid  = getpid();
    kmem.sz   = size;
    kmem.to   = data;
    kmem.from = *block;
    iostat = ioctl(fd, IOCTL_COPYTO, &kmem);
    
    return 0;
}


int freeKmem(int fd, void **block)
{
    int iostat;
    iostat = ioctl(fd, IOCTL_FREE, *block);
    
    return 0;
}

#endif

#ifdef __KERNEL__  /* then this is a Linux kernel-mode test module */
int putKmem(int fd, void *data, void **block, unsigned int size)
{
    *block = (void *)kmalloc(size, GFP_KERNEL | __GFP_DMA);
    if (*block == NULL)
        return -1;

    if (data != NULL)
        memcpy(*block, data, size);
    
    return 0;
}


int getKmem(int fd, void *data, void **block, unsigned int size)
{
    memcpy(data, *block, size);
    return 0;
}

int freeKmem(int fd, void **block)
{
    kfree(*block);
    return 0;
}

#endif



/* Top level of test */
int testAll(void)
{
    int status=0, nFails=0, nTests=0;
    int fd;
    
/*
 * First, if we're going to use a semaphore as the deferred-service
 * interlock, go create it
 *  
 */
#ifdef __KERNEL__
    init_MUTEX_LOCKED(&notifySemId);
#endif 

#ifdef __KERNEL__    
    fd = 0;
#else
    fd = open(SEC2_DEVNAME, O_RDWR, 0);
    if (fd == -1)
    {
        printf("testAll(): can't open %s, error = %d: ", SEC2_DEVNAME, errno);
        return -1;
    }
#endif

#ifdef TESTALL_HASH
    if ((status = hash_test(fd)) == 0) 
        nTests++;
    else
        printf("hash_test(%d) Failed! %04x\n", ++nFails, status);
#endif


#ifdef TESTALL_IPSEC
    if ((status = testIPSEC(fd)) == 0) /* IPSEC with DES */
        nTests++;
    else
        printf("testIPSEC(%d) Failed! %04x\n", ++nFails, status);
#endif



#ifdef TESTALL_RC4
    if ((status = testAfha(fd)) == 0) /* arc4 (0x1.) */
        nTests++;
    else
        printf("testAfha(%d) Failed! %04x\n", ++nFails, status);

    if ((status = testAFHActx(fd)) == 0)
        nTests++;
    else
        printf("testAFHActx(%d) Failed! %04x\n", ++nFails, status);
#if 0

    if ((status = testAfhaLong(fd)) == 0)
        nTests++;
    else
        printf("testAfhaLong(%d) Failed! %04x\n", ++nFails, status);
#endif
#endif



#ifdef TESTALL_DES
    if ((status = testDes(fd, 512, 4)) == 0)  /* DES (0x2.) */
        nTests++;
    else
        printf("testDes(%d) Failed! %04x\n", ++nFails, status);

    if ((status = testDesECB(fd, 512, 4)) == 0)   /* DES ECB */
        nTests++;
    else
        printf("testDesECB(%d) Failed! %04x\n", ++nFails, status);
#endif


#ifdef TESTALL_HMAC
    if ((status = testHmac(fd)))  /* Hmac (0x3.) */
        printf("testHmac(%d) Failed! %04x\n", ++nFails, status);
    else 
        nTests++;
#endif


#ifdef TESTALL_RNG
    if ((status = testRng(fd)) == 0)  /* RNG (0x4.) */
        nTests++;
    else
        printf("testRng(%d) Failed! %04x\n", ++nFails, status);
#endif


#ifdef TESTALL_AES
    if ((status = testAesa(fd)) == 0) /* AESA_SEL (0x6.) */
        nTests++;
    else
        printf("testAesa(%d) Failed! %04x\n", ++nFails, status);
#endif

#ifdef TESTALL_KEA
    if ((status = testKEA(fd)) == 0)
        nTests++;
    else
        printf("testKEA(%d) Failed! %04x\n", ++nFails, status);
#endif

#ifdef TESTALL_PKHA
  if ((status = testPKHA(fd)) == 0)
    nTests++;
  else
    printf("testPKHA(%d) failed, status = %04x\n", ++nFails, status);
#endif



#ifdef TESTALL_CCMP
  if ((status = testccmp(fd)) == 0)
    nTests++;
  else
    printf("testccmp(%d) failed, status = %04x\n", ++nFails, status);

  if ((status = testccmpdec(fd)) == 0)
    nTests++;
  else
    printf("testccmpdec(%d) failed, status = %04x\n", ++nFails, status);
#endif

#ifdef TESTALL_XOR
  if ((status = testXOR(fd)) == 0)
    nTests++;
  else
    printf("testXOR(%d) failed, status = %04x\n", ++nFails, status);
#endif

#ifdef TESTALL_TLS
  if ((status = testTLS(fd)) == 0)
    nTests++;
  else
    printf("testTLS(%d) failed, status = %04x\n", ++nFails, status);
#endif

#ifdef TESTALL_ICV
  if ((status = testICV(fd)) == 0)
    nTests++;
  else
    printf("testICV(%d) failed, status = %04x\n", ++nFails, status);
#endif

#ifdef TESTALL_SCATTER
  if ((status = testSG(fd)) == 0)
    nTests++;
  else
    printf("testSG(%d) failed, status = %04x\n", ++nFails, status);
#endif

#ifdef TESTALL_AUX
  if ((status = testAux(fd)) == 0)
    nTests++;
  else
    printf("testAux(%d) failed, status = %04x\n", ++nFails, status);
#endif 

#ifdef TESTALL_FIFO_OVERFLOW
  if ((status = testFF()) == 0)
    nTests++;
  else
    printf("testFF(%d) failed, status = %04x\n", ++nFails, status);
#endif



  /* All tests complete, show status and exit */

  if (nFails == 0)
    printf("testAll(): All %d Tests Passed\n", nTests);
  else
    printf("Tests Passed %d, Failed %d!\n", nTests, nFails);

#ifdef __KERNEL__
    semDelete(notifySemId);
#else
    close(fd);
#endif

    if (nFails)
        return -1;
      
    return 0;
}




#ifdef _LINUX_USERMODE_

int main(int argc, char **argv)
{
    return testAll();
}
#endif


#ifdef __KERNEL__


static inline int __sem_update_count(struct semaphore *sem, int incr)
{
        int old_count, tmp;

        __asm__ __volatile__("\n"
"1:     lwarx   %0,0,%3\n"
"       srawi   %1,%0,31\n"
"       andc    %1,%0,%1\n"
"       add     %1,%1,%4\n"
        PPC405_ERR77(0,%3)
"       stwcx.  %1,0,%3\n"
"       bne     1b"
        : "=&r" (old_count), "=&r" (tmp), "=m" (sem->count)
        : "r" (&sem->count), "r" (incr), "m" (sem->count)
        : "cc");

        return old_count;
}

long __down_interruptible_timeout(struct semaphore * sem, long timeout)
{
        int retval = 0;
        struct task_struct *tsk = current;
        DECLARE_WAITQUEUE(wait, tsk);

        tsk->state = TASK_INTERRUPTIBLE;
        add_wait_queue_exclusive(&sem->wait, &wait);
        smp_wmb();

        while (__sem_update_count(sem, -1) <= 0) {
                if (signal_pending(current)) {
                        /*
                         * A signal is pending - give up trying.
                         * Set sem->count to 0 if it is negative,
                         * since we are no longer sleeping.
                         */
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
        return timeout>=0? 0: -1;
}

static int __init sec2test_init(void)
{
        testAll();
        return 0;
}

static void __exit sec2test_exit(void)
{
        return;
}

module_init(sec2test_init);
module_exit(sec2test_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SEC2.x driver test module");
MODULE_AUTHOR("Freescale Semiconductor Inc.");

#endif
