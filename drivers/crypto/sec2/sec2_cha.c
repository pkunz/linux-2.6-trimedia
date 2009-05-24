 
/****************************************************************************
 * sec2_cha.c - Channel/CHA operations code for SEC2 Driver 
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2002.  All rights reserved
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
**************************************************************************/


/* Revision History:
 * 1.0   Aug 24,2003 dgs adapted from the final version of mpc18x
 * 1.1.0 Nov 15,2004 sec - merge linux changes from prior versions
 * 1.2   Jan 27,2005
 * 1.3   Jul 20,2005 sec - add checks for KFHA presence
 * 1.5   Oct 13,2005 sec - add support for reserved channels and per-channel
 *                         auxiliary deferred-service handlers
 */



 
#include "Sec2Driver.h"


int ReleaseCha(unsigned long channelCha, int callingTaskId, BOOLEAN locked);



/********************************************************************/
/* Function Name: ReserveChannelStatic                              */
/* Purpose:  To allocate a channel for use by a single task.        */
/* Input: channel - the channel number (1-4)                        */
/*                callingTaskId - the task id                       */
/* Output: channel - the channel number that allocated              */
/* Return: SEC2_SUCCESS if success otherwise the error code         */
/********************************************************************/
int ReserveChannelStatic(unsigned char *channel,
                         int            callingTaskId)
{
    unsigned char chan;
                
    IOLockChannelAssignment();

    /* look for a free channel */
    for (chan = 0; chan < NUM_CHANNELS; chan++)
    {
#ifdef DBG
        SEC2Dump(DBGTXT_SETRQ, ("ReserveChannelStatic chan = %d\n", chan));
#endif

        if (ChannelAssignments[chan].assignment == CHANNEL_FREE)
        {
            /* found a free channel, make it static */
            ChannelAssignments[chan].assignment = CHANNEL_STATIC_ASSIGNED;
#ifdef DBG
            SEC2Dump(DBGTXT_SETRQ, ("ReserveChannelStatic found a free channel\n"));
#endif

            FreeChannels--;

            /* set the channel's owner task ID */
            ChannelAssignments[chan].ownerTaskId = callingTaskId;
            IOUnLockChannelAssignment();
            /* set return value */
            *channel = chan + 1;
#ifdef DBG
            SEC2Dump(DBGTXT_SETRQ, ("ReserveChannelStatic-> reserved channel %d\n", *channel));
#endif
            return SEC2_SUCCESS;
        }
    }

    IOUnLockChannelAssignment();
        
    /* we didn't find a free channel */
#ifdef DBG
    SEC2Dump(DBGTXT_SETRQ, ("ReserveChannelStatic->no free channel to reserve\n"));
#endif
    return SEC2_CHANNEL_NOT_AVAILABLE;
}



/****************************************************************/
/* Function Name: ChaNumToType                                  */
/* Purpose: To translate the the CHA number to the CHA chaType  */
/* Input: cha - CHA Number                                      */
/* Output: ChaType                                              */
/****************************************************************/
int ChaNumToType(int cha)
{
    switch (cha)
    {
    case CHA_AFHA:
        return (CHA_AFHA);
        
    case CHA_DES:
        return (CHA_DES);
        
    case CHA_MDHA:
        return (CHA_MDHA);
        
    case CHA_RNG:
        return (CHA_RNG);
        
    case CHA_PKHA:
        return (CHA_PKHA);
        
    case CHA_AESA:
        return (CHA_AESA);
        
    case CHA_KEA:
        return (CHA_KEA);
        
    default:
        return (SEC2_INVALID_CHA_TYPE);
    }
} /* End of ChaNumToType */



/* this should only be called with channel assignments locked */
/********************************************************************/
/* Function Name: CheckChas                                         */
/* Purpose: Check to see if the CHA is available (at least one of   */
/*          this type of CHA is available)                          */
/* Input: The opId requested                                        */
/* Output: SEC2_SUCCESS if this type of CHA is available            */
/*         otherwise the error code                                 */
/********************************************************************/
int CheckChas(unsigned long OpId)
{
    unsigned int chaType = 0;

    switch (OpId & 0xf000)
    {
    case 0x1000:                        /* RNG Group */
        chaType |= CHA_RNGA_BITMASK;
        break;
        
    case 0x2000:                        /* Des Groups */
        chaType |= CHA_DESA_BITMASK;
        break;
        
    case 0x3000:                        /* RC4 Groups */
        chaType |= CHA_AFHA_BITMASK;
        break;
        
    case 0x4000:                        /* MD Groups */
        chaType |= CHA_MDHA_BITMASK;
        break;
        
    case 0x5000:                        /* PK Groups */
        chaType |= CHA_PKHA_BITMASK;
        break;
        
    case 0x6000:                        /* AES Groups */
        chaType |= CHA_AESA_BITMASK;
        break;
        
    case 0x7000:                        /* ipsec Des + MD Groups */
    case 0x9000:                        /* SSL/TLS */
        chaType |= (CHA_MDHA_BITMASK | CHA_DESA_BITMASK);
        break;
        
    case 0x8000:                        /* ipsec aes + md groups */
        chaType |= (CHA_MDHA_BITMASK | CHA_AESA_BITMASK);
        break;
        
    case 0xa000:                        /* Kasumi */
        chaType |= CHA_KEA_BITMASK;
        break;
        
    default:
        /* invalid opId */
        return (SEC2_INVALID_OPERATION_ID);
    }
    
    
    if (((chaType & CHA_AFHA_BITMASK) != 0) && (FreeAfhas == 0)) 
        return (SEC2_CHA_NOT_AVAILABLE);

    else if (((chaType & CHA_DESA_BITMASK) != 0) && (FreeDesas == 0))
        return (SEC2_CHA_NOT_AVAILABLE);

    else if (((chaType & CHA_MDHA_BITMASK) != 0) && (FreeMdhas == 0)) 
        return (SEC2_CHA_NOT_AVAILABLE);

    else if (((chaType & CHA_RNGA_BITMASK) != 0) && (FreeRngas == 0)) 
        return (SEC2_CHA_NOT_AVAILABLE);

    else if (((chaType & CHA_PKHA_BITMASK) != 0) && (FreePkhas == 0)) 
        return (SEC2_CHA_NOT_AVAILABLE);

    else if (((chaType & CHA_AESA_BITMASK) != 0) && (FreeAesas == 0)) 
        return (SEC2_CHA_NOT_AVAILABLE);

    else if (((chaType & CHA_KEA_BITMASK) != 0) && (FreeKeas == 0)) 
        return (SEC2_CHA_NOT_AVAILABLE);

    /* CHA(s) are available, return OK */
    return (SEC2_SUCCESS);
}



/********************************************************************/
/* Function Name: ReleaseChannel                                    */
/* Purpose:  To free a reserved channel                             */
/*                                                                  */
/* Input: channel - the channel number (1-4)                        */  
/*        callingTaskId - the task id                               */
/*        locked - TRUE or FALSE                                    */
/* Return: SEC2_SUCCESS if success otherwise the error code         */
/********************************************************************/
int ReleaseChannel(unsigned char channel, int callingTaskId, BOOLEAN locked)
{
    int cha, chaType;

    SEC2Dump(DBGTXT_SVCRQ, ("ReleaseChannel(%d, taskID=0x%8x, lock=%d)\n", channel, callingTaskId, locked));
    /* check channel value */
    if ((channel == 0) || (channel > NUM_CHANNELS))
        return SEC2_INVALID_CHANNEL;

    if (!locked)
        IOLockChannelAssignment();

    /* check that calling task is the channel's owner */
    if (ChannelAssignments[channel - 1].ownerTaskId != callingTaskId)
    {
        /* error, caller is not the channel owner */
        if (!locked)
            IOUnLockChannelAssignment();
        return SEC2_INVALID_CHANNEL;
    }

    /* release all cha's assigned to this channel */
    for (cha = 0; cha < NUM_CHAS; cha++)
    {
        if (ChaAssignments[cha] == channel)
        {
            /* determine cha type from cha number */
            chaType = ChaNumToType(cha);
            /* release the cha */
            ReleaseCha((channel << 8) | chaType, callingTaskId, TRUE);
        }
    }

    FreeChannels++;

    /* release the channel */
    ChannelAssignments[channel - 1].assignment = CHANNEL_FREE;

    if (!locked)
    {
        IOUnLockChannelAssignment();

        /* schedule queued requests on any freed channels */
        ScheduleNext();
    }

    return (SEC2_SUCCESS);
} /* end of ReleaseChannel */




/**************************************************************************/
/* Function Name: ReleaseCha                                              */
/* Purpose:  To release a specific CHA                                       */
/*              or a manual channel.                                      */
/* Input: channelCha - the bottom eight bits is the CHA chaType           */
/*                     the next eight bits is the channel number (1-4)    */    
/*        callingTaskId - the task id                                     */
/*        locked - TRUE or FALSE                                          */
/* Return: SEC2_SUCCESS if success otherwise the error code           */
/**************************************************************************/
int ReleaseCha(unsigned long channelCha, int callingTaskId, BOOLEAN locked)
{
    unsigned char channel = (unsigned char)((channelCha >> 8) & 0xFF);
    unsigned char chaType = (unsigned char)(channelCha & 0xFF);
    unsigned long chaStatus[2] = {0,0};
    int           cha;
    int           numChas = 0;

    SEC2Dump(DBGTXT_SVCRQ, ("ReleaseCha(%ld, taskID=0x%8x, lock=%d)\n", channelCha, callingTaskId, locked));

    /* check chaType value */
    if ((chaType != CHA_AFHA) &&
        (chaType != CHA_DES)  &&
        (chaType != CHA_MDHA) &&
        (chaType != CHA_RNG)  &&
        (chaType != CHA_PKHA) &&
        (chaType != CHA_AESA) &&
        (chaType != CHA_KEA))
        return (SEC2_INVALID_CHA_TYPE);

    if (!locked)
        IOLockChannelAssignment();

    /* check that callingTaskId matches the channel owner */
    if (ChannelAssignments[channel - 1].ownerTaskId != callingTaskId)
    {
        /* error, cha owner is not current task */
        if (!locked)
            IOUnLockChannelAssignment();
        return (SEC2_INVALID_CHANNEL);
    }

    /* determine number of CHAs for the chaType */
    switch (chaType)
    {
        case CHA_AFHA:
            numChas = NUM_AFHAS;
            break;
        
        case CHA_DES:
            numChas = NUM_DESAS;
            break;
         
        case CHA_MDHA:
            numChas = NUM_MDHAS;
            break;
        
        case CHA_RNG:
            numChas = NUM_RNGAS;
            break;
        
        case CHA_PKHA:
            numChas = NUM_PKHAS;
            break;
        
        case CHA_AESA:
            numChas = NUM_AESAS;
            break;
        
        case CHA_KEA:
            numChas = NUM_KEAS;
            break;
    }

    /* find the CHA to release */
    for (cha=chaType; cha<(chaType+numChas); cha++)
    {
        /* see if the CHA is assigned to the given channel */
        if (ChaAssignments[cha] == channel)
        {
            /* found a CHA assigned to the given channel */
            /* release the CHA */
            chaStatus[0] = *(unsigned long *)(ChaAssignmentStatusRegister);
            chaStatus[1] = *(unsigned long *)(ChaAssignmentStatusRegister + 1);
            
            if (cha < 4)
            {
                chaStatus[0] &= (~(0xF << (cha * 8)));
                ChaAssignments[cha] = CHA_DYNAMIC;
            }
            else
            {
                chaStatus[1] &= (~(0xF << ((cha - 4) * 8)) << 16);
                ChaAssignments[cha] = CHA_DYNAMIC;
            }
            
            *(ChaAssignmentControlRegister)     = chaStatus[0];
            *(ChaAssignmentControlRegister + 1) = chaStatus[1];

            /* reset the CHA */
            *(ChaResetControlRegister[cha_idx[cha]])     = CHA_SOFTWARE_RESET;
            *(ChaResetControlRegister[cha_idx[cha]] + 1) = 0;

            break;
        }
    }

    /* see if we found a CHA to release */
    if (cha >= (chaType + numChas))
    {
        /* we didn't find a CHA to release, return an error */
        if (!locked)
            IOUnLockChannelAssignment();
        return (SEC2_INVALID_CHA_TYPE);
    }

    /* update free CHA counts */
    switch (chaType)
    {
        case CHA_AFHA:
            FreeAfhas++;
            break;
            
        case CHA_DES:
            FreeDesas++;
            break;
            
        case CHA_MDHA:
            FreeMdhas++;
            break;
            
        case CHA_RNG:
            FreeRngas++;
            break;
            
        case CHA_PKHA:
            FreePkhas++;
            break;
            
        case CHA_AESA:
            FreeAesas++;
            break;

        case CHA_KEA:
            FreeKeas++;
            break;
    }

    if (!locked)
    {
        IOUnLockChannelAssignment();

        /* schedule queued requests on any freed CHAs */
        ScheduleNext();
    }

    return (SEC2_SUCCESS);
} /* End of ReleaseCha */
