
/****************************************************************************
 * testAux.c - auxiliary handler check for SEC2 driver
 ****************************************************************************
 * Copyright (c) 2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/

/* Revision History:
 * 1.5   Oct 13,2005 sec - new
 */


#include "sec2drvTest.h"
#include "Sec2.h"

static const unsigned char sha1plaintext[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
static const unsigned char sha1digest[] = 
{ 
    0x84, 0x98, 0x3e, 0x44, 0x1c, 
    0x3b, 0xd2, 0x6e, 0xba, 0xae,
    0x4a, 0xa1, 0xf9, 0x51, 0x29, 
    0xe5, 0xe5, 0x46, 0x70, 0xf1 
};


static unsigned long auxFlag;

static int sha1Static(int fd, unsigned char chan)
{
    HASH_REQ hashRQ;
    int      stat, testfail;
    char    *digestBuffer;

    testfail = 0;
    
    digestBuffer = (unsigned char *)malloc(20);
    
    memset(digestBuffer, 0, 20);
    memset(&hashRQ, 0, sizeof(hashRQ));
    
    /* Set up request block */
    hashRQ.opId     = DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX;
    hashRQ.channel  = chan;
    
    hashRQ.inBytes  = 56;
    hashRQ.inData   = (unsigned char *)sha1plaintext; 
    hashRQ.outBytes = 20;
    hashRQ.outData  = digestBuffer;
    
    /* push buffers */
    stat = putKmem(fd, (void *)sha1plaintext, (void **)&hashRQ.inData, hashRQ.inBytes);
    if (stat)
        return stat;
        
    stat = putKmem(fd, NULL, (void **)&hashRQ.outData, hashRQ.outBytes);
    if (stat)
    {
        freeKmem(fd, (void **)&hashRQ.inData);
        return stat;
    }

    armCompletion(&hashRQ);
    stat = ioctl(fd, IOCTL_PROC_REQ, (int)&hashRQ);

    if ((stat = waitCompletion("sha1Static(): hash test", stat, &hashRQ)))
    {
        freeKmem(fd, (void **)&hashRQ.inData);
        freeKmem(fd, (void **)&hashRQ.outData);
        return stat;
    }
        
    getKmem(fd, digestBuffer, (void **)&hashRQ.outData, hashRQ.outBytes);
    freeKmem(fd, (void **)&hashRQ.inData);
    freeKmem(fd, (void **)&hashRQ.outData);

    if (memcmp(digestBuffer, sha1digest, 20)) 
    {
        printf("sha1Static() output miscompare\n");
        printf("expected digest:\n");
        dumpm((void *)sha1digest, 20);
        printf("actual digest:\n");
        dumpm(digestBuffer, 20);
        testfail++;
    }
        
    free(digestBuffer);

    if (testfail)
        return -1;
    else
        return 0;
}



/* This is an auxiliary handler that will run in the driver's */
/* deferred-service context                                   */

static int dummyAuxHandler(int ch)
{
    printf("testAux(): auxHandler active on channel %d\n", ch);

    /* bump the flag as a clue that we ran... */
    auxFlag++;
    
    return 0;
}


static int local_ioctl(int fd, unsigned int code, unsigned long param)
{
#ifdef __KERNEL__
    return SEC2_ioctl(NULL, NULL, code, param);
#else
    return SEC2_ioctl(fd, code, param);
#endif
}


/* Test the use of an auxiliary channel with a static handler */

int testAux(int fd)
{
    int              testfail, stat;
    AUX_HANDLER_SPEC handlerspec;
    
    testfail = 0;
    auxFlag  = 0;
    
    printf("\n*** testAux() ***\n");

    /* First, reserve a channel */
    stat = local_ioctl(fd, IOCTL_RESERVE_CHANNEL_STATIC, (unsigned long)&(handlerspec.channel));
    if (stat)
    {
        printf("testAux(): can't reserve static channel, error 0x%08x\n", stat);
        return -1;
    }
    
    /* Now install the aux handler in that channel */
    handlerspec.auxHandler = dummyAuxHandler;
    stat = local_ioctl(fd, IOCTL_INSTALL_AUX_HANDLER, (unsigned long)&handlerspec);
    if (stat)
    {
        printf("testAux(): can't install aux handler, error 0x%08x\n", stat);
        local_ioctl(fd, IOCTL_RELEASE_CHANNEL, (unsigned long)&(handlerspec.channel));
        return -1;
    }
    
    

    /* Call a common crypto-op against that static channel */
    stat = sha1Static(fd, handlerspec.channel);
    
    if (stat)
        printf("testAux(): problem with crypto call\n");
    
    /* remove handler */
    handlerspec.auxHandler = NULL;
    stat = local_ioctl(fd, IOCTL_INSTALL_AUX_HANDLER, (unsigned long)&handlerspec);
    if (stat)
    {
        printf("testAux(): can't release aux handler, error 0x%08x\n", stat);
        return -1;
    }

    /* release channel */
    stat = local_ioctl(fd, IOCTL_RELEASE_CHANNEL, (unsigned long)&(handlerspec.channel));
    if (stat)
    {
        printf("testAux(): can't release channel, error 0x%08x\n", stat);
        return -1;
    }


    if (auxFlag == 0)
    {
        printf("testAux(): auxiliary handler never ran\n");
        testfail++;
    }
    
    printf("*** testAux() Complete ***\n");

    if (testfail)
        return -1;
    else
        return 0;
    
}

