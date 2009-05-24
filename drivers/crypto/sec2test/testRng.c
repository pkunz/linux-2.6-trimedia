
/****************************************************************************
 * testRng.c - random number generation test for SEC2 device driver
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
 * 1.2   02-Feb-2005 sec - fix warnings
 * 1.3   Aug 11,2005
 */

#include "sec2drvTest.h"
#include "Sec2.h"


#define RNG_TESTSIZE (160)

static unsigned char rngBuf[RNG_TESTSIZE];



int testRng(int fd)
{
    RNG_REQ rngReq;
    int     status;


    /* RNG test */
    printf("\n*** Test RNG ***\n");

    memset(&rngReq, 0, sizeof(rngReq));
    memset(rngBuf, 0, RNG_TESTSIZE);

    rngReq.opId     = DPD_RNG_GETRN;
    rngReq.rngBytes = RNG_TESTSIZE;
    rngReq.rngData  = rngBuf;


    status = putKmem(fd, NULL, (void **)&rngReq.rngData, RNG_TESTSIZE);
    if (status)
        return status;
        
    armCompletion(&rngReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&rngReq);

    if ((status = waitCompletion("testRng(): data extraction test", status, &rngReq)))
    {
        freeKmem(fd, (void **)&rngReq.rngData);
        return status;
    }
    
    getKmem(fd, rngBuf, (void **)&rngReq.rngData, RNG_TESTSIZE);

    freeKmem(fd, (void **)&rngReq.rngData);

    /* check random buffer content for nonzero */
    if (rngBuf[0]) {
        printf("*** Test RNG Data ***\n");
        status = 0;
    } else {
        printf("*** Test RNG Failed ***\n");
        status = -1;
    }

    return status;
}

