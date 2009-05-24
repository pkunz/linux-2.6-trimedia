

/****************************************************************************
 * Sec2Notify.h - ISR register transfer definitions for the SEC2.x
 *                security core family
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2000.  All rights reserved
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/

/*
 * Revision History:
 * 1.0   Aug 24,2003 dgs - adapted from the final version of mpc18x
 * 1.1.0 Nov 22,2004 sec - move GENERIC_REQ to global header out of necessity
 * 1.2   Jan 27,2005
 * 1.3   Aug 11,2005
 */





#ifndef SEC2NOTIFY_H
#define SEC2NOTIFY_H

/*
 * STATUS_REQ
 * structure used to indicate the state of the SEC2 as well as the
 * river.  Returned as a pointer by GetStatus() and embedded in all
 * requests.
 */
typedef struct
{
    unsigned long ChaAssignmentStatusRegister[2];
    unsigned long InterruptControlRegister[2];
    unsigned long InterruptStatusRegister[2];
    unsigned long IdRegister;
    unsigned long ChannelStatusRegister[NUM_CHANNELS][2];
    unsigned long ChannelConfigurationRegister[NUM_CHANNELS][2];
    unsigned long CHAInterruptStatusRegister[NUM_CHAS][2];
    unsigned long QueueEntryDepth;
    unsigned long FreeChannels;
    unsigned long FreeRngas;
    unsigned long FreeAfhas;
    unsigned long FreeDesas;
    unsigned long FreeMdhas;
    unsigned long FreePkhas;
    unsigned long FreeAesas;
    unsigned long FreeKeas;
    unsigned long BlockSize;
} STATUS_REQ;

typedef STATUS_REQ SEC2_STATUS;

typedef void* PSEC2_NOTIFY_CTX;

/*
 * SEC2_NOTIFY_ON_ERROR_CTX
 * structure used in all requests to indicate the nature of errors.
 */
typedef struct
{
    unsigned long errorcode; /* Error that the request generated */
    void *request;           /* Pointer to original request block */
    STATUS_REQ driverstatus; /* Detailed device state block */
} SEC2_NOTIFY_ON_ERROR_CTX, *PSEC2_NOTIFY_ON_ERROR_CTX;


typedef void (*PSEC2_NOTIFY_ROUTINE) (PSEC2_NOTIFY_CTX pCtx);
typedef void (*PSEC2_NOTIFY_ON_ERROR_ROUTINE) (PSEC2_NOTIFY_ON_ERROR_CTX pErrCtx);

#endif
