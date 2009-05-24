
/****************************************************************************
 * sec2_request.c - request/response handler
 ****************************************************************************
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/


/* Revision History:
 * 1.0   Aug 24,2003 dgs - adapted from the final version of mpc18x
 *       Jun 04,2004 sec - add completion cleanup for scatter pointers
 * 1.1.0 Nov 16,2004 sec - integrate linux changes from prior versions, eliminate
 *                         StartDescriptor() function
 * 1.1.1 Dec 16,2004 sec - remove lock variable, remove old diagnostics, add
 *                         minor optimizations
 * 1.2   Jan 27,2005 sec - add CHA details to error dump
 * 1.3   Jul 28,2005 sec - fix KFHA references, and add desc buffer viewer
 * 1.5   Oct 13,2005 sec - add support for static channels, and for per-channel
 *                         auxiliary deferred-service handlers
 */

#include "Sec2Driver.h"




/********************************************************************************/
/* Function Name: ProcessRequest                                                */
/* Purpose: To handle the IO request routine                                    */
/* Input: req - the pointer of the IO request                                   */
/*        callingTaskId - the task Id                                           */
/* Output: req - the pointer of the IO request buffer                           */
/* Return: SEC2_Success if success, otherwise error code                        */
/********************************************************************************/

int ProcessRequest(GENERIC_REQ *req, int callingTaskId)
{
    register int    freeChannel;
    unsigned char   channel;
    int             status;
    QUEUE_ENTRY     *newEntry;
    BOOLEAN         bAvailableCHA;

    
    channel = (((GENERIC_REQ *)req)->channel);
    
    /* if requested channel is out of range, flag status and return error */
    if (channel > NUM_CHANNELS)
    {
        ((GENERIC_REQ *)req)->status = SEC2_INVALID_CHANNEL;
        return SEC2_INVALID_CHANNEL;
    }

    /* if static channel specified, use that channel, else use dynamic */
    if (channel > 0)
    {   /* process a static request */
        IOLockChannelAssignment();
        
        /* is channel (a) static, and (b) assigned to this requestor? */
        if ((ChannelAssignments[channel - 1].assignment != CHANNEL_STATIC_ASSIGNED) ||
            (ChannelAssignments[channel - 1].ownerTaskId != callingTaskId))
        {
            IOUnLockChannelAssignment();
            ((GENERIC_REQ *)req)->status = SEC2_INVALID_CHANNEL;
            return SEC2_INVALID_CHANNEL;
        } /* no channel, or not assigned */
        
        /* OK, so this channel is ours to use, proceed */
        /* Translate request to DPD */
        ChannelAssignments[channel - 1].pReq       = req;
        ChannelAssignments[channel - 1].assignment = CHANNEL_STATIC_BUSY;
        IOUnLockChannelAssignment();

#ifdef DBG
        SEC2Dump(DBGTXT_SETRQ,
                 ("ProcessStaticRequest->staticChannel = %d, calling task = 0x%x\n",
                 channel,
                 callingTaskId));
                                
#endif
        status = RequestToDpd(req, channel - 1);
        
#ifdef DBG
        SEC2Dump(DBGTXT_SETRQ,
                 ("ProcessStaticRequest->RequestToDpd() status = 0x%x\n", status));
#endif

        /* if DPD translation failed, flag error and clean up */
        if (status != SEC2_SUCCESS)
            return CompleteRequest(channel - 1, status, FALSE, NULL);

        /* All OK, so initiate the request */        
        *(ChannelNextDescriptorRegister[channel - 1] + 1) = __pa((unsigned long)(ChannelAssignments[channel - 1].Dpds[0]));

#ifdef __KERNEL__
                    schedule();
#endif
        
    }
    else  /* process a dynamic channel request */
    {
        /* check to see if the required CHA is available */
        bAvailableCHA = FALSE;
        if (CheckChas(req->opId) == SEC2_SUCCESS)
            bAvailableCHA = TRUE;

        IOLockChannelAssignment();
        
        if (bAvailableCHA && FreeChannels > 0)
        {
            /* dynamic request, see if there are any free channels */
            for (freeChannel = 0;
                 freeChannel < NUM_CHANNELS;
                 freeChannel++)
            {

                if (ChannelAssignments[freeChannel].assignment == CHANNEL_FREE)
                {
                    SEC2Dump(DBGTXT_SETRQ, ("ProcessRequest(): free channel = %d\n", freeChannel));
                    /* reserve this free channel for this operation */
                    ChannelAssignments[freeChannel].assignment = CHANNEL_DYNAMIC;
                    ChannelAssignments[freeChannel].ownerTaskId = callingTaskId;
                    ChannelAssignments[freeChannel].pReq = req;
                    FreeChannels--;

                    IOUnLockChannelAssignment();

                    /* translate this request into DPD's */
                    status = RequestToDpd(req, freeChannel);
                    if (status != SEC2_SUCCESS)
                        return CompleteRequest(freeChannel, status, FALSE, NULL);

                    /* write the address of the first DPD to the channel */
                    /* this starts the request */
                    *(ChannelNextDescriptorRegister[freeChannel]+1) = __pa((unsigned long)(ChannelAssignments[freeChannel].Dpds[0]));

#ifdef __KERNEL__
                    schedule();
#endif
                    return SEC2_SUCCESS;
                } /* if (there is free channel */
            } /* for (free channels */
        } /* if (cha_available AND channel_available) */
        else /* no resources */
        {
            IOUnLockChannelAssignment(); 

            /* if request queue full, return error */
            if (QueueEntryDepth >= QUEUE_ENTRY_DEPTH)
                return (SEC2_INSUFFICIENT_REQS);

            /* else queue request */
#ifndef __KERNEL__
            semTake (QueueSemId, 100);
#endif
            newEntry = ProcessQueueBottom;
            ProcessQueueBottom = ProcessQueueBottom->next;
            /* fill in the queue structure */
            newEntry->pReq = req;
            newEntry->requestingTaskId = callingTaskId;
            QueueEntryDepth++;
#ifndef __KERNEL__
            semGive (QueueSemId);
#endif
            SEC2Dump(DBGTXT_SETRQ, ("Queueing Up->newEntry=0x%lx\n", (unsigned long)newEntry));
        } /* no resources, error or queue request */


    } /* else process dynamic channel request */
    return SEC2_SUCCESS;
} 




#ifdef __KERNEL__
void ProcessingComplete(unsigned long not_used)
#else
void ProcessingComplete(void)
#endif
{
    register int          channel;
    register int          cha;
    int                   status[NUM_CHANNELS];       /* The status for the channel */
    BOOLEAN               bIsComplete[NUM_CHANNELS];  /* Indicates that the channel is done */
    DEVICE_REGS          *pRegs; /* register? */
    static   DEVICE_REGS  regs;
#ifdef DBG
    int                   i, j;
    unsigned long        *descword;
#endif

    pRegs = &regs;

    /* we sit here forever, waiting for register content messages from the ISR */
    while (1)
    {
        QmsgWait();


#ifdef DBG

        SEC2Dump(DBGTXT_SVCRQ, ("ProcessingComplete(): IntStatus - "));
        if ((pRegs->IntStatus[0] & ALL_CHANNEL_INT_ERROR_MASK) ||
            (pRegs->IntStatus[1] & ALL_CHA_INT_ERROR_MASK))
        {
            SEC2Dump(DBGTXT_SVCRQ, ("Error "));

            if (pRegs->IntStatus[0] & CHANNEL_INT_ERROR_1) SEC2Dump(DBGTXT_SVCRQ, ("ch1 "));
            if (pRegs->IntStatus[0] & CHANNEL_INT_ERROR_2) SEC2Dump(DBGTXT_SVCRQ, ("ch2 "));
            if (pRegs->IntStatus[0] & CHANNEL_INT_ERROR_3) SEC2Dump(DBGTXT_SVCRQ, ("ch3 "));
            if (pRegs->IntStatus[0] & CHANNEL_INT_ERROR_4) SEC2Dump(DBGTXT_SVCRQ, ("ch4 "));
            if (pRegs->IntStatus[1] & CHA_INT_ERROR_DEU) SEC2Dump(DBGTXT_SVCRQ, ("DES "));
            if (pRegs->IntStatus[1] & CHA_INT_ERROR_AESU) SEC2Dump(DBGTXT_SVCRQ, ("AES "));
            if (pRegs->IntStatus[1] & CHA_INT_ERROR_MDEU) SEC2Dump(DBGTXT_SVCRQ, ("MD "));
            if (pRegs->IntStatus[1] & CHA_INT_ERROR_AFEU) SEC2Dump(DBGTXT_SVCRQ, ("AF "));
            if (pRegs->IntStatus[1] & CHA_INT_ERROR_RNG) SEC2Dump(DBGTXT_SVCRQ, ("RNG "));
            if (pRegs->IntStatus[1] & CHA_INT_ERROR_PKEU) SEC2Dump(DBGTXT_SVCRQ, ("PK "));
            if (pRegs->IntStatus[1] & CHA_INT_ERROR_KEU) SEC2Dump(DBGTXT_SVCRQ, ("KEU "));

            SEC2Dump(DBGTXT_SVCRQ, ("\n"));

            for (i = 0; i < NUM_CHANNELS; i++)
                SEC2Dump(DBGTXT_SVCRQ, ("Ch %d ptr status: 0x%08lx%08lx\n",
                                        i + 1,
                                        pRegs->ChannelError[i][0],
                                        pRegs->ChannelError[i][1]));

            SEC2Dump(DBGTXT_SVCRQ, ("DES int status:  0x%08lx\n", pRegs->ChaError[cha_idx[0]][1])); 
            SEC2Dump(DBGTXT_SVCRQ, ("AES int status:  0x%08lx\n", pRegs->ChaError[cha_idx[1]][1])); 
            SEC2Dump(DBGTXT_SVCRQ, ("MDEU int status: 0x%08lx\n", pRegs->ChaError[cha_idx[2]][1])); 
            SEC2Dump(DBGTXT_SVCRQ, ("AFEU int status: 0x%08lx\n", pRegs->ChaError[cha_idx[3]][1])); 
            SEC2Dump(DBGTXT_SVCRQ, ("RNG int status:  0x%08lx\n", pRegs->ChaError[cha_idx[4]][1])); 
            SEC2Dump(DBGTXT_SVCRQ, ("PKEU int status: 0x%08lx\n", pRegs->ChaError[cha_idx[5]][1]));
            SEC2Dump(DBGTXT_SVCRQ, ("KEU int status:  0x%08lx\n", pRegs->ChaError[cha_idx[6]][1]));
        }
        else
            if ((pRegs->IntStatus[0] & ALL_CHANNEL_INT_DONE_MASK) ||
                (pRegs->IntStatus[1] & ALL_CHA_INT_DONE_MASK))
            {
                SEC2Dump(DBGTXT_SVCRQ, ("Done "));
                if (pRegs->IntStatus[0] & CHANNEL_INT_DONE_1) SEC2Dump(DBGTXT_SVCRQ, ("ch1 "));
                if (pRegs->IntStatus[0] & CHANNEL_INT_DONE_2) SEC2Dump(DBGTXT_SVCRQ, ("ch2 "));
                if (pRegs->IntStatus[0] & CHANNEL_INT_DONE_3) SEC2Dump(DBGTXT_SVCRQ, ("ch3 "));
                if (pRegs->IntStatus[0] & CHANNEL_INT_DONE_4) SEC2Dump(DBGTXT_SVCRQ, ("ch4 "));
                if (pRegs->IntStatus[1] & CHA_INT_DONE_DEU)  SEC2Dump(DBGTXT_SVCRQ, ("DES "));
                if (pRegs->IntStatus[1] & CHA_INT_DONE_AESU) SEC2Dump(DBGTXT_SVCRQ, ("AES "));
                if (pRegs->IntStatus[1] & CHA_INT_DONE_MDEU) SEC2Dump(DBGTXT_SVCRQ, ("MD "));
                if (pRegs->IntStatus[1] & CHA_INT_DONE_AFEU) SEC2Dump(DBGTXT_SVCRQ, ("AF "));
                if (pRegs->IntStatus[1] & CHA_INT_DONE_RNG)  SEC2Dump(DBGTXT_SVCRQ, ("RNG "));
                if (pRegs->IntStatus[1] & CHA_INT_DONE_PKEU) SEC2Dump(DBGTXT_SVCRQ, ("PK "));
                if (pRegs->IntStatus[1] & CHA_INT_ERROR_KEU) SEC2Dump(DBGTXT_SVCRQ, ("KEU "));
            }
            else
                SEC2Dump(DBGTXT_SVCRQ, ("Unknown"));
        
        SEC2Dump(DBGTXT_SVCRQ, ("\n"));
        
        for (i = 0; i < NUM_CHANNELS; i++)
        {
            descword = (unsigned long *)ChannelDataBufferDescriptorRegister[i];
            SEC2Dump(DBGTXT_DESCBUF, ("Ch %d descriptor buffer: header = 0x%08lx\n", i, *descword));
            descword += 2;
            for (j = 0; j < 7; j++)
            {
                SEC2Dump(DBGTXT_DESCBUF, ("p%d: length/extent 0x%08lx", j, *(descword++)));
                SEC2Dump(DBGTXT_DESCBUF, (", pointer 0x%08lx\n", *(descword++)));
            }
        }
#endif

        for (channel = 0; channel < NUM_CHANNELS; channel++)
        {
            bIsComplete[channel] = FALSE;
            status[channel]      = SEC2_SUCCESS;
        }

        /* deal with any channel done interrupts */
        if ((pRegs->IntStatus[0] & ALL_CHANNEL_INT_DONE_MASK) != 0)
            /* found at least one channel done interrupt */
            for (channel = 0; channel < NUM_CHANNELS; channel++)
                if ((pRegs->IntStatus[0] & ChannelDoneInterruptMasks[channel]) != 0)
                {   /* found a channel done interrupt */
                    bIsComplete[channel] = TRUE; 
                    if (ChannelAssignments[channel].auxHandler != NULL)
                    {
#ifdef DBG
                        SEC2Dump(DBGTXT_SVCRQ,
                                 ("ProcessingComplete(): calling aux handler channel %d\n", channel + 1));
#endif
                        ChannelAssignments[channel].auxHandler(channel + 1);            
                    }
                } /* service channel interrupt */
                
                
        /* deal with any CHA done (manual/debug/target mode) interrupts */
        if ((pRegs->IntStatus[1] & ALL_CHA_INT_DONE_MASK) != 0)
            /* found at least one CHA done interrupt */
            for (cha = 0; cha < NUM_CHAS; cha++)
                if ((pRegs->IntStatus[1] & ChaDoneInterruptMasks[cha]) != 0)
                {
                    /* found a CHA done interrupt, notify application */                             
                    if (cha < 4)
                        channel = (pRegs->ChaAssignmentStatus[0] >> (cha * 8)) & 0xF;
                    else
                        channel = (pRegs->ChaAssignmentStatus[1] >> ((cha - 4) * 8)) & 0xF;

                    if (channel > 0 && channel < (NUM_CHANNELS+1))
                        bIsComplete[channel-1] = TRUE;
            
                    SEC2Dump(DBGTXT_SVCRQ,
                             ("ProcessingComplete(): Channel %d Assigned\n",
                             channel));
                } /* if (CHA int status) */


        /* deal with any error interrupts */
        if (((pRegs->IntStatus[0] & ALL_INT_ERRORS_MASK_LOW) != 0) ||
            ((pRegs->IntStatus[1] & ALL_INT_ERRORS_MASK_HIGH) != 0))
        {
            /* need to find the channel */
            /* deal with CHA error interrupts */

            for (cha=0; cha<NUM_CHAS; cha++)
            {
                if (cha < 4)
                    channel = (pRegs->ChaAssignmentStatus[0] >> (cha * 8)) & 0xF;
                else
                    channel = (pRegs->ChaAssignmentStatus[1] >> ((cha - 4) * 8)) & 0xF;

                if (channel > 0 && channel < (NUM_CHANNELS+1) &&
                    (pRegs->IntStatus[0] & ChannelErrorInterruptMasks[channel-1]) != 0)
                {
                    status[channel-1]      = SEC2_CHA_ERROR;
                    bIsComplete[channel-1] = TRUE;
                }
            } /* end for cha loop */

            for (channel = 0; channel < NUM_CHANNELS; channel++)
            {
                if ((pRegs->IntStatus[0] & ChannelErrorInterruptMasks[channel]) != 0)
                {
                    bIsComplete[channel] = TRUE;

                    /* this was a channel error then                                 */
                    /* return the error code with some sort of quasi-priority, since */
                    /* multiples could occur                                         */

                    status[channel] = SEC2_UNKNOWN_ERROR;
                    
                    if ((pRegs->ChannelError[channel][1] & CHANNEL_CHA_ERROR) ||
                        (pRegs->ChannelError[channel][1] & CHANNEL_CHA_ASSIGN_ERROR))
                        status[channel] = SEC2_CHA_ERROR;

                    if (pRegs->ChannelError[channel][1] & CHANNEL_UNRECOGNIZED_HEADER_ERROR)
                        status[channel] = SEC2_INVALID_OPERATION_ID;
           
                    if (pRegs->ChannelError[channel][1] & CHA_PARITY_SYSTEM_ERROR)
                        status[channel] = SEC2_PARITY_SYSTEM_ERROR;

                    if (pRegs->ChannelError[channel][1] & CHANNEL_NULL_FETCH_POINTER)
                        status[channel] = SEC2_INCOMPLETE_POINTER;

                    if ((pRegs->ChannelError[channel][1] & CHANNEL_SCATTER_LENGTH_ERROR) ||
                        (pRegs->ChannelError[channel][1] & CHANNEL_SCATTER_BOUND_ERROR) ||
                        (pRegs->ChannelError[channel][1] & CHANNEL_GATHER_LENGTH_ERROR) ||
                        (pRegs->ChannelError[channel][1] & CHANNEL_GATHER_BOUND_ERROR) ||
                        (pRegs->ChannelError[channel][1] & CHANNEL_ZERO_SCATTER_LEN_ERROR))
                        status[channel] = SEC2_SCATTER_LIST_ERROR;                
                
                    if (pRegs->ChannelError[channel][1] & CHANNEL_BUS_MASTER_ERROR)
                        status[channel] = SEC2_BUS_MASTER_ERROR;

                    if ((pRegs->ChannelError[channel][1] & CHANNEL_FETCH_FIFO_NEARMISS_ERROR) ||
                        (pRegs->ChannelError[channel][1] & CHANNEL_FETCH_FIFO_OVERFLOW_ERROR))
                        status[channel] = SEC2_FETCH_FIFO_OVERFLOW;

                } /* if (valid interrupt) */
            } /* for (channels) */
        } /* if (cha int done) */
 
        for (channel = 0; channel < NUM_CHANNELS; channel++)
            if (bIsComplete[channel])
                CompleteRequest(channel, status[channel], TRUE, pRegs);

        /* schedule queued requests on any freed channels */
        ScheduleNext();

    } /* if (message from queue) */

    SEC2Dump(DBGTXT_SVCRQ, ("ProcessingComplete Done\n"));
}  /* End of ProcessingComplete */





void RemoveQueueEntry(QUEUE_ENTRY *entry)
{
    SEC2Dump(DBGTXT_SVCRQ, ("RemoveQueueEntry->Entry=0x%lx\n", (unsigned long)entry));
    semTake (QueueSemId, 100);
    if (QueueEntryDepth > 0) {
        QueueEntryDepth--;
        entry->requestingTaskId = 0;
        entry->pReq = NULL;
        SEC2Dump(DBGTXT_SVCRQ, ("RemoveQueueEntry->QueueEntryDepth=0x%lx\n", QueueEntryDepth));
        ProcessQueueTop = ProcessQueueTop->next;
        SEC2Dump(DBGTXT_SVCRQ, ("RemoveQueueEntry->ProcessQueueTop=0x%lx\n", (unsigned long)ProcessQueueTop));
    }
    semGive (QueueSemId);
    return;

} /* end of RemoveQueueEntry */




/* does not return a status value -- all errors are returned to the requesting
application through the request structure status value */
/* this should only be called with channel assignments locked */
void ScheduleNext()
{
    register unsigned int channel;
    int status;
    
    IOLockChannelAssignment();

    channel = 0;
    while (channel < NUM_CHANNELS)
    {
        if (ChannelAssignments[channel].assignment != CHANNEL_FREE)
        {
            channel++;
            continue;
        }

        /* Is there anything queued up? */
        if (QueueEntryDepth > 0) {
            if (CheckChas(ProcessQueueTop->pReq->opId) != SEC2_SUCCESS) {
                SEC2Dump(DBGTXT_SETRQ, ("ScheduleNext->Breaking Out CheckChas fails 0x%lx\n", ProcessQueueTop->pReq->opId));
                break;
            }
            ChannelAssignments[channel].assignment = CHANNEL_DYNAMIC;
            ChannelAssignments[channel].pReq = ProcessQueueTop->pReq;
            ChannelAssignments[channel].ownerTaskId = ProcessQueueTop->requestingTaskId;
            RemoveQueueEntry(ProcessQueueTop);
            FreeChannels--;

            SEC2Dump(DBGTXT_SETRQ, ("ScheduleNext->FreeChannels 0x%x\n", FreeChannels));

            IOUnLockChannelAssignment();
            status = RequestToDpd(ChannelAssignments[channel].pReq, channel); 

            if (status != SEC2_SUCCESS)
            {
            /* Problem with request, return error to the application */
                CompleteRequest(channel, status, TRUE, NULL);
                IOLockChannelAssignment();
                continue;
            }
            else
            {
                /* write the address of the first DPD to the channel */
                /* this starts hardware processing the request */

                *(ChannelNextDescriptorRegister[channel]+1) = __pa((unsigned long)(ChannelAssignments[channel].Dpds[0]));

#ifdef __KERNEL__
                schedule();
#endif

                IOLockChannelAssignment();
            }
        }
        /* finished with this channel */
        channel++;
    }

    IOUnLockChannelAssignment();

}


/* As T2.0 is designed, output from PKHA will be "front loaded" in the output */
/* buffer such that data starts at the zeroth element in the output buffer    */
/* As long as the output and the modulus size match, this is fine. If the     */
/* output is larger than the modulus, then we would normally expect the       */
/* output to be at the far end of the buffer (leading zeros at the start)     */
/* However, it puts the modulated output "up front". Normally, there's no     */
/* reason for the output > modulus, but if it exists, this corrects for it    */

#ifdef PKHA_OUTPUT_SHIFT

static void fixPKHAresult(DPD *pkDesc)
{
  int i;
  unsigned long modlen, outlen, shift, *outbuf;
  unsigned long *src, *dest;
  
  modlen = (pkDesc->fld[0].len & 0xffff0000) >> 16;
  outlen = (pkDesc->fld[4].len & 0xffff0000) >> 16;
  outbuf = (unsigned long *)pkDesc->fld[4].ptr;
  

  /* Check difference and save, if not different, exit */
  if (outlen > modlen)
    shift = (outlen - modlen)/4;
  else
    return;
    
  dest = &outbuf[(outlen / 4) - 1];
  src  = &outbuf[(modlen / 4) - 1];

  for (i = 0; i < shift; i++)
  {
    *(dest--) = *(src);
    *(src--)  = 0;
  }
}

#endif




int CompleteRequest(int channel, int status, BOOLEAN bDoNotify, DEVICE_REGS *pRegs)
{
    register GENERIC_REQ    *req = NULL;
    int                      i, j;


    IOLockChannelAssignment();

    if (ChannelAssignments[channel].pReq != NULL)
      req = ChannelAssignments[channel].pReq;
    else
      return -1;

    SEC2Dump(DBGTXT_SVCRQ, ("CompleteRequest->channel %d, request=0x%lx\n", channel, (unsigned long)req));
    ((GENERIC_REQ*)req)->status = status;

    ChannelAssignments[channel].pReq = NULL;

#ifdef PKHA_OUTPUT_SHIFT
    /* This is here as a workaround for completing PKHA */
    for (i = 0; i < MAX_DPDS; i++)
      if (ChannelAssignments[channel].Dpds[i] != NULL)
        if ((ChannelAssignments[channel].Dpds[i]->header & 0xf8) == 0x00000080) /* PKHA? */
          fixPKHAresult(ChannelAssignments[channel].Dpds[i]);
#endif        
        
    /* Walk through the request's descriptor list. For any DPD that was in use, */
    /* if they used scatter lists, we have to return the free elements to the   */
    /* fragment pool. So check for scattered pointers, and unmap them           */

    for (i = 0; i < MAX_DPDS; i++)
      if (ChannelAssignments[channel].Dpds[i] != NULL)
        for (j = 0; j < NUM_DPD_FLDS; j++)
          if (ChannelAssignments[channel].Dpds[i]->fld[j].len & PTRTYPE_JBIT)
            UnmapScatterFragments((void *)ChannelAssignments[channel].Dpds[i]->fld[j].ptr);

    /* Change the assignment state.  This is the last thing to be done
       since the channel becomes immediately available when you set it
       to FREE (dynamic) or ASSIGNED (static).
    */
    if (ChannelAssignments[channel].assignment == CHANNEL_DYNAMIC)
    {
        FreeChannels++;
        ChannelAssignments[channel].assignment = CHANNEL_FREE;
    }
    
    if (ChannelAssignments[channel].assignment == CHANNEL_STATIC_BUSY)
        ChannelAssignments[channel].assignment = CHANNEL_STATIC_ASSIGNED;
    
    IOUnLockChannelAssignment();

    if (bDoNotify)
    {
        if (status == SEC2_SUCCESS)
        {
            /* Everythink OK, so this is a normal completion notifier */
            SEC2Dump(DBGTXT_SVCRQ, ("CompleteReqest(): success notifier in process\n"));

            /* Go check the requestors preferred notification mechanism
             * If NOTIFY_IS_PID is true, then the "notify" member of the
             * request block is actually a process ID, and the caller needs
             * us to send a signal */
            if (((GENERIC_REQ *)req)->notifyFlags & NOTIFY_IS_PID)
            {
#ifdef __KERNEL__
                send_sig(SIGUSR1, find_task_by_pid((long)((GENERIC_REQ*)req)->notify), 0);
#endif
            }
            else
                if ((((GENERIC_REQ*)req)->notify) != NULL)
                    (((GENERIC_REQ*)req)->notify)(((GENERIC_REQ*)req)->pNotifyCtx);
        }
        else
        {

            /* else this is an error notifier... */
            SEC2Dump(DBGTXT_SVCRQ, ("CompleteReqest(): error notifier in process\n"));

            if (((GENERIC_REQ *)req)->notifyFlags & NOTIFY_ERROR_IS_PID)
            {
#ifdef __KERNEL__
                send_sig(SIGUSR2, find_task_by_pid((long)((GENERIC_REQ *)req)->notify_on_error), 0);
#endif
            } 
            else
            {
                if ((((GENERIC_REQ*)req)->notify_on_error) != NULL)
                {
                    /* first, do the error context save */
                    ((GENERIC_REQ*)req)->ctxNotifyOnErr.errorcode = ((GENERIC_REQ*)req)->status;

                    if (pRegs == NULL)
                        memset(&(((GENERIC_REQ*)req)->ctxNotifyOnErr.driverstatus),0,sizeof(STATUS_REQ));
                    else
                        CopyRegsToStatus(&(((GENERIC_REQ*)req)->ctxNotifyOnErr.driverstatus), pRegs);

                    ((GENERIC_REQ*)req)->ctxNotifyOnErr.request = req;

                    /* now call the handler */
                    SEC2Dump(DBGTXT_SVCRQ, ("CompleteRequest(): calling error notifier with context\n"));   
                    (((GENERIC_REQ*)req)->notify_on_error)(&(((GENERIC_REQ*)req)->ctxNotifyOnErr));
                }
            }
        }
    }

    return status;
}



int GetStatus(STATUS_REQ *pStatus, int callingTaskId)
{
    int i;

    pStatus->ChaAssignmentStatusRegister[0] = *(unsigned long *)(ChaAssignmentStatusRegister); 
    pStatus->ChaAssignmentStatusRegister[1] = *(unsigned long *)(ChaAssignmentStatusRegister + 1); 

    pStatus->InterruptControlRegister[0]    = *(unsigned long *)(InterruptControlRegister);
    pStatus->InterruptControlRegister[1]    = *(unsigned long *)(InterruptControlRegister + 1);
    
    pStatus->InterruptStatusRegister[0]     = *(unsigned long *)(InterruptStatusRegister); 
    pStatus->InterruptStatusRegister[1]     = *(unsigned long *)(InterruptStatusRegister + 1); 

    pStatus->IdRegister                     = *(unsigned long *)(IdRegister); 
     
    for (i = 0; i < NUM_CHANNELS; i++)
    {
      pStatus->ChannelStatusRegister[i][0] = *(unsigned long *)(ChannelPointerStatusRegister[i]);  
      pStatus->ChannelStatusRegister[i][1] = *(unsigned long *)(ChannelPointerStatusRegister[i] + 1);  
    }
    
    for (i = 0; i < NUM_CHANNELS; i++)
    {
      pStatus->ChannelConfigurationRegister[i][0] = *(unsigned long *)(ChannelConfigRegister[i]); 
      pStatus->ChannelConfigurationRegister[i][1] = *(unsigned long *)(ChannelConfigRegister[i] + 1); 
    }
                    
    for (i = 0; i < NUM_CHAS; i++)
    {
      pStatus->CHAInterruptStatusRegister[i][0] = *(unsigned long *)(ChaInterruptStatusRegister[i]); 
      pStatus->CHAInterruptStatusRegister[i][1] = *(unsigned long *)(ChaInterruptStatusRegister[i] + 1); 
    }
    
    
    pStatus->QueueEntryDepth = QueueEntryDepth;             
    pStatus->FreeChannels    = FreeChannels;
    pStatus->FreeRngas       = FreeRngas;     
    pStatus->FreeAfhas       = FreeAfhas;         
    pStatus->FreeDesas       = FreeDesas;         
    pStatus->FreeMdhas       = FreeMdhas;         
    pStatus->FreePkhas       = FreePkhas;
    pStatus->FreeAesas       = FreeAesas;     
    pStatus->FreeKeas        = FreeKeas;       
    pStatus->BlockSize       = BlockSize;

    return SEC2_SUCCESS;
}


void CopyRegsToStatus(STATUS_REQ *pStatus, DEVICE_REGS *pRegs)
{
    int     channel, cha;

    /* Build up the driver status structure.  It will be passed back by
       the notify on error routine
    */

    pStatus->InterruptStatusRegister[0] = pRegs->IntStatus[0];
    pStatus->InterruptStatusRegister[1] = pRegs->IntStatus[1];
    pStatus->ChaAssignmentStatusRegister[0] = pRegs->ChaAssignmentStatus[0];
    pStatus->ChaAssignmentStatusRegister[1] = pRegs->ChaAssignmentStatus[1];

    for (channel=0; channel<NUM_CHANNELS; channel++)
    {
        pStatus->ChannelStatusRegister[channel][0] = pRegs->ChannelError[channel][0];
        pStatus->ChannelStatusRegister[channel][1] = pRegs->ChannelError[channel][1];
    }
    for (cha=0; cha<NUM_CHAS; cha++)
    {
        pStatus->CHAInterruptStatusRegister[cha][0] = pRegs->ChaError[cha][0];
        pStatus->CHAInterruptStatusRegister[cha][1] = pRegs->ChaError[cha][1];
    }
}

