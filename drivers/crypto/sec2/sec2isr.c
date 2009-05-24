/****************************************************************************
 * sec2isr.c  -- The Interrupt Service code for SEC2 Driver
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2002.  All rights reserved
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ****************************************************************************/

/*
 * Revision History:
 * 1.0   Aug 24,2003 dgs  - adapted from the final version of mpc18x
 *       May 12,2004 sec - turned on some windview events for debug
 *       Jun 06,2004 sec - corrected channel reset logic
 *       Jun 10,2004 sec - remove superfluous I/O calls
 *       Jul 22,2004 sec - added double CHA reset to handle errata
 * 1.1.0 Nov 16,2004 sec - updated for linux change merge from prior revs
 * 1.1.1 Dec 16,2004 sec - cleanup, remove unused diagnostics
 * 1.2   Jan 27,2005
 * 1.3   Jul 26,2005 sec - updates for 2.6 and MPC8548
 */


#include "Sec2Driver.h"

/**************************************************************************
 * Function Name: InterruptServiceRoutine
 * Purpose: To handle all interrupt generated by Channel or CHA (Done or Error)
 *          indicated by the Interrupt Status Register 
 *          If Error, clean the InterruptStatusRegister by writing InterruptClearRegister
 *          If Done, call ProcessRequestDone routine 
 * Input: Interrupt - PKINTERRUPT
 *        Context - PVOID the request Context
 *

 *************************************************************************/
#ifdef __KERNEL__
DEVICE_REGS pRegs_queue[PREGS_QUEUE_DEPTH];
unsigned PQ_head = 0;
unsigned PQ_tail = 0;
unsigned PQ_entries = 0;

extern void ProcessingComplete(unsigned long not_used);
DECLARE_TASKLET(isr_tasklet,ProcessingComplete,0);

irqreturn_t InterruptServiceRoutine(int irq, void *dev_id, struct pt_regs *pt_regs)

#endif
{
  register int        cha, cha_res;
  register int        channel;
  unsigned long       temp[2];
  unsigned long       intStatus0, intStatus1;
  DEVICE_REGS        *pRegs;
  static DEVICE_REGS  regs;



  /* Save a copy of the ISR for deferred service routine to use */
  /* after the fact */
  intStatus0 = *(InterruptStatusRegister); 
  intStatus1 = *(InterruptStatusRegister + 1); 

  pRegs = &regs;
  pRegs->IntStatus[0] = intStatus0;
  pRegs->IntStatus[1] = intStatus1;



  /* First, is this interrupt a consequence of an error? */
  if ((intStatus0 & ALL_INT_ERRORS_MASK_LOW) ||
      (intStatus1 & ALL_INT_ERRORS_MASK_HIGH)) 
  {
    /* save assigment statii for background servicing to use */
    pRegs->ChaAssignmentStatus[0] = *(ChaAssignmentStatusRegister);
    pRegs->ChaAssignmentStatus[1] = *(ChaAssignmentStatusRegister + 1);

    /* service CHA error interrupts first */
    for (cha_res = 0; cha_res < NUM_CHAS; cha_res++)
    {
      /* save the channel this CHA is assigned to if any */
      if (cha_res < 4)
        channel = (pRegs->ChaAssignmentStatus[0] >> (cha_res * 8)) & 0xF;
      else
        channel = (pRegs->ChaAssignmentStatus[1] >> ((cha_res - 4) * 8)) & 0xF;
      
      /* Now go reset relevant CHA. This will loop from 0 -> NUM_CHAS */
      /* Obtain CHA ID from the map in cha_idx                        */
      cha = cha_idx[cha_res];
      if ((intStatus1 & ChaErrorInterruptMasks[cha]) ||
          (channel > 0 && channel < (NUM_CHANNELS+1) &&
          (intStatus0 & ChannelErrorInterruptMasks[channel-1])))
      {
        /* found a CHA error interrupt, read the CHA status register */
        pRegs->ChaError[cha][0] = *(unsigned long *)(ChaInterruptStatusRegister[cha]);
        pRegs->ChaError[cha][1] = *(unsigned long *)(ChaInterruptStatusRegister[cha] + 1);

        /* ensure that the associated channel is reset */
        if (channel > 0 && channel < (NUM_CHANNELS+1))
          intStatus0 |= ChannelErrorInterruptMasks[channel-1];
        
        pRegs->ChannelError[channel][0] = *(unsigned long *)(ChannelPointerStatusRegister[channel]);
        pRegs->ChannelError[channel][1] = *(unsigned long *)(ChannelPointerStatusRegister[channel] + 1);



        /* Done extracting info, reset the CHA                                       */
        /* This will be done twice to work around an errata that has to do with CHA */
        /* reset hold time                                                          */

        *(ChaResetControlRegister[cha])     = CHA_SOFTWARE_RESET;
        *(ChaResetControlRegister[cha] + 1) = 0;

        *(ChaResetControlRegister[cha])     = CHA_SOFTWARE_RESET;
        *(ChaResetControlRegister[cha] + 1) = 0;

        if (cha == CHA_DES)  /* is this cha des? */
        {
           *(ChaInterruptControlRegister[cha])     = CHA_DESA_KEY_PARITY_ERROR_DISABLE;
           *(ChaInterruptControlRegister[cha] + 1) = 0;
        }

        if (cha == CHA_PKHA) /* is this cha the pkha? */
        {
          *(ChaInterruptControlRegister[cha])     = CHA_PKHA_ILLEGAL_ADDR_ERROR_DISABLE;
          *(ChaInterruptControlRegister[cha] + 1) = 0;
        }
      } /* if (cha causes error) */
    } /* for (all chas) */


    /* done servicing CHA interrupts, now go look after channels */
    for (channel=0; channel<NUM_CHANNELS; channel++)
    {
      if (intStatus0 & ChannelErrorInterruptMasks[channel])
      {
        /* fetch a copy of the CPSR for out-of-ISR processing */
        pRegs->ChannelError[channel][0] = *(unsigned long *)(ChannelPointerStatusRegister[channel]);
        pRegs->ChannelError[channel][1] = *(unsigned long *)(ChannelPointerStatusRegister[channel] + 1);

        /* reset the channel */
        *(ChannelConfigRegister[channel])     = (unsigned long)CHANNEL_RESET;
        *(ChannelConfigRegister[channel] + 1) = (unsigned long)0;

        /* give the hardware a chance to clock... */
        temp[0] = *(unsigned long *)(ChannelConfigRegister[channel]);

        /* re-enable channel interrupts */                
        *(ChannelConfigRegister[channel])     = (unsigned long)0;
        *(ChannelConfigRegister[channel] + 1) = (unsigned long)CHANNEL_INTEN;
      } /* if (channel error) */
    } /* for (all channels) */
  } /* if (any error interrupt) */

  /* clear latched interrupts at the control register level */
  *(InterruptClearRegister)     = intStatus0;
  *(InterruptClearRegister + 1) = intStatus1;

  /* If there were no CHA or channel errors, then the deferred service */
  /* routine will take care of requestor notifications and the like,   */
  /* based on information in the saved copy of the ISR. So, all we     */
  /* need to do is make sure that it gets scheduled so that            */
  /* ProcessingComplete() can run (see sec2_request.c)                 */
    
#ifdef __KERNEL__
  QmsgSend();
  return IRQ_HANDLED;
#else
  msgQSend(IsrMsgQId, (char*)pRegs, sizeof(DEVICE_REGS), NO_WAIT, MSG_PRI_NORMAL);
#endif
} /* end of SEC2InterruptService */ 
