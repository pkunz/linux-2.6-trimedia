

/****************************************************************************
 * sec2_ioctl.c - Driver service hooks for the SEC2.x security core
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2002.  All rights reserved
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
***************************************************************************/

/* Revision History:
 * 1.0   Aug 24,2003 dgs - adapted from the final version of mpc18x
 * 1.1.0 Nov 22,2003 sec - merge in changes from Linux fork
 * 1.1.1 Dev 15,2004 sec - remove old diagnostics
 * 1.2   Jan 27,2005 sec - fix minor compiler warnings
 * 1.3   Jul 21,2005 sec - update for 2.6 builds
 * 1.5   Oct 13,2005 sec - add support for static channel and aux channel handlers
 */

#include "Sec2Driver.h"


#define MOD_DEC_USE_COUNT   do { } while (0);
#define MOD_INC_USE_COUNT MOD_DEC_USE_COUNT


#ifdef __KERNEL__
int SEC2Open(struct inode *nd, struct file *fil)
{
#ifdef DBG
    SEC2Dump(DBGTXT_SETRQ, ("SEC2Open() called, inode 0x%08x, file 0x%08x\n", (unsigned int)nd, (unsigned int)fil));
#endif

    MOD_INC_USE_COUNT;

    return 0;
}
#endif


#ifdef __KERNEL__
int SEC2Close(struct inode *nd, struct file *fil)
#endif
{
    int     channel;
    QUEUE_ENTRY *QueueEntry;
    int     callingTaskId = (int)taskIdSelf();

#ifdef DBG
    SEC2Dump(DBGTXT_SETRQ, ("SEC2Close() called\n"));
#endif

    QueueEntry = ProcessQueueTop;
    while (QueueEntry->pReq != NULL)
    {
        if (QueueEntry->requestingTaskId == callingTaskId)
        {
            ((GENERIC_REQ*)QueueEntry->pReq)->status = SEC2_CANCELLED_REQUEST;
            QueueEntry->pReq = NULL;
        }
        QueueEntry = QueueEntry->next;
    }


    IOLockChannelAssignment();

    /* release any leftover reserved channels associated with this descriptor */
    for (channel=0; channel<NUM_CHANNELS; channel++)
        if (ChannelAssignments[channel].ownerTaskId == callingTaskId)
            /* release this channel */
            ReleaseChannel(channel+1, callingTaskId, TRUE);

    IOUnLockChannelAssignment();
    ScheduleNext();

#ifdef __KERNEL__
    MOD_DEC_USE_COUNT;
#endif

    return SEC2_SUCCESS;
}


#ifdef __KERNEL__
int SEC2_ioctl(struct inode  *nd,
               struct file   *fil,
               unsigned int   ioctlCode,
               unsigned long  param)
#else
int SEC2_ioctl(int devDesc, register int ioctlCode, register void *param)
#endif
{
    int status, chan;


#ifdef __KERNEL__
    MALLOC_REQ *mem;
#endif
    status = SEC2_SUCCESS;
    
    switch (ioctlCode)
    {
    case IOCTL_PROC_REQ:
        if (param == (int)NULL)
        {
            status = SEC2_INVALID_ADDRESS;
            break;
        }
        status = ProcessRequest((GENERIC_REQ *)param, (int)taskIdSelf());
        break;

    case IOCTL_GET_STATUS:
        GetStatus((STATUS_REQ *)param, (int)taskIdSelf());
        break;

    case IOCTL_RESERVE_CHANNEL_STATIC:
        status = ReserveChannelStatic((unsigned char *)param, (int)taskIdSelf());
        break;

    case IOCTL_RELEASE_CHANNEL:
        status = ReleaseChannel(*(unsigned char *)param, (int)taskIdSelf(), FALSE);
        break;


#ifdef __KERNEL__
    case IOCTL_MALLOC:
        if ((((MALLOC_REQ *)param)->ptr = malloc(((MALLOC_REQ *)param)->sz)) == 0) {
            status = SEC2_MALLOC_FAILED;
            break;
        }
        memset(((MALLOC_REQ *)param)->ptr, 0, ((MALLOC_REQ *)param)->sz);
        status = SEC2_SUCCESS;
        break;

    case IOCTL_COPYFROM:
        mem = (MALLOC_REQ *)param;
        mem->pid = current->pid;
        copy_from_user(mem->to, mem->from, mem->sz);
        status = SEC2_SUCCESS;
        break;

    case IOCTL_COPYTO:
        mem = (MALLOC_REQ *)param;
        mem->pid = current->pid;
        copy_to_user(mem->to, mem->from, mem->sz);
        status = SEC2_SUCCESS;
        break;

    case IOCTL_FREE:
        free((void *)param);
        break;
#endif

    case IOCTL_INSTALL_AUX_HANDLER:
        chan = ((AUX_HANDLER_SPEC *)param)->channel;
#ifdef DBG
        SEC2Dump(DBGTXT_SETRQ,
                 ("installAuxHandler(): requsted handler 0x%08x on channel %d\n",
                  (unsigned long)(((AUX_HANDLER_SPEC *)param)->auxHandler),
                  chan));
#endif
        /* see if requested channel is valid */
        if ((chan <= 0) || (chan > NUM_CHANNELS))
        {
            status = SEC2_INVALID_CHANNEL;
            break;
        }
        
        /* channel is valid, is it reserved (and not busy)? */
        if (ChannelAssignments[chan - 1].assignment != CHANNEL_STATIC_ASSIGNED)
        {
            status = SEC2_CHANNEL_NOT_AVAILABLE;
            break;
        }


        /* Channel spec is in range, and is reserved for use. Notice that */
        /* we really don't have any good means to identify the requestor  */
        /* for validity (could be the kernel itself), so will assume that */
        /* channel ownership is not an issue. Now register/remove the     */
        /* handler                                                        */
        
        ChannelAssignments[chan - 1].auxHandler = ((AUX_HANDLER_SPEC *)param)->auxHandler;

        break;



    default:
        /* invalide IOCTL code, return an error */
        /* need OS-specific error value here */
        return (S_ioLib_UNKNOWN_REQUEST);
    } /* end switch (ioctlCode) */

    return (status);
}
