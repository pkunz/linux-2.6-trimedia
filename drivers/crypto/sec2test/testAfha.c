
/****************************************************************************
 * testAfha.c - ARC4 known-answer test for SEC2 device driver
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
 * 1.2   02-Feb-2005 sec - fix some type warnings
 * 1.3   Aug 11,2005
 */



#include "sec2drvTest.h"
#include "Sec2.h"


#define RC4_KEYSIZE  (8)
#define RC4_TESTSIZE (40)
#define RC4_CTXSIZE  (259)

static const unsigned char AFKey[] =
{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

static unsigned char rc4TestData[RC4_TESTSIZE];
static unsigned char rc4EncResult[RC4_TESTSIZE];
static unsigned char rc4Result[RC4_TESTSIZE];
static unsigned char rc4Ctx[RC4_CTXSIZE];


int testAfha(int fd)
{
    int status, i, teststat;
    ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ afReq;


    printf("\n*** Test AFHA ***\n");


    /* AFHA_SEL (0x10200050) test */
    /* try an arcfour operation on a dynamic channel */

    memset(&afReq,       0, sizeof(afReq));
    memset(rc4EncResult, 0, RC4_TESTSIZE);
    memset(rc4Result,    0, RC4_TESTSIZE);

    /* precoat test array */
    for (i = 0; i < RC4_TESTSIZE; i++)
        rc4TestData[i] = 0x03;


    /* encryption (first) pass */
    afReq.opId            = DPD_RC4_LDKEY_CRYPT_ULCTX;
    afReq.keyBytes        = RC4_KEYSIZE;
    afReq.keyData         = (unsigned char *)AFKey;
    afReq.inBytes         = RC4_TESTSIZE;
    afReq.inData          = rc4TestData;
    afReq.outData         = rc4EncResult;
    afReq.outCtxBytes     = RC4_CTXSIZE;
    afReq.outCtxData      = rc4Ctx;

    status = putKmem(fd, (void *)AFKey, (void **)&afReq.keyData, RC4_KEYSIZE);
    if (status)
        return status;
    
    status = putKmem(fd, rc4TestData, (void **)&afReq.inData,  RC4_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&afReq.keyData);
        return status;
    }
    
    status = putKmem(fd, NULL, (void **)&afReq.outData, RC4_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&afReq.keyData);
        freeKmem(fd, (void **)&afReq.inData);
        return status;
    }

    status = putKmem(fd, NULL, (void **)&afReq.outCtxData, RC4_CTXSIZE);
    if (status) {
        freeKmem(fd, (void **)&afReq.keyData);
        freeKmem(fd, (void **)&afReq.inData);
        freeKmem(fd, (void **)&afReq.outData);
        return status;
    }

    
    armCompletion(&afReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&afReq);

    if ((status = waitCompletion("testAfha(): encryption test", status, &afReq)))
    {
        freeKmem(fd, (void **)&afReq.keyData);
        freeKmem(fd, (void **)&afReq.inData);
        freeKmem(fd, (void **)&afReq.outData);
        freeKmem(fd, (void **)&afReq.outCtxData);
        return status;
    }
    
    getKmem(fd, rc4EncResult, (void **)&afReq.outData,    RC4_TESTSIZE);
    getKmem(fd, rc4Ctx,       (void **)&afReq.outCtxData, RC4_TESTSIZE);

    freeKmem(fd, (void **)&afReq.keyData);
    freeKmem(fd, (void **)&afReq.inData);
    freeKmem(fd, (void **)&afReq.outData);
    freeKmem(fd, (void **)&afReq.outCtxData);




    /* decryption (second) pass */
    afReq.opId            = DPD_RC4_LDKEY_CRYPT_ULCTX;
    afReq.keyBytes        = RC4_KEYSIZE;
    afReq.keyData         = (unsigned char *)AFKey;
    afReq.inBytes         = RC4_TESTSIZE;
    afReq.inData          = rc4EncResult;
    afReq.outData         = rc4Result;
    afReq.outCtxBytes     = RC4_CTXSIZE;
    afReq.outCtxData      = rc4Ctx;

    status = putKmem(fd, (void *)AFKey, (void **)&afReq.keyData, RC4_KEYSIZE);
    if (status)
        return status;
    
    status = putKmem(fd, rc4EncResult, (void **)&afReq.inData,  RC4_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&afReq.keyData);
        return status;
    }
    
    status = putKmem(fd, NULL, (void **)&afReq.outData, RC4_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&afReq.keyData);
        freeKmem(fd, (void **)&afReq.inData);
        return status;
    }

    status = putKmem(fd, NULL, (void **)&afReq.outCtxData, RC4_CTXSIZE);
    if (status) {
        freeKmem(fd, (void **)&afReq.keyData);
        freeKmem(fd, (void **)&afReq.inData);
        freeKmem(fd, (void **)&afReq.outData);
        return status;
    }

    
    armCompletion(&afReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&afReq);

    if ((status = waitCompletion("testAfha(): decryption test", status, &afReq)))
    {
        freeKmem(fd, (void **)&afReq.keyData);
        freeKmem(fd, (void **)&afReq.inData);
        freeKmem(fd, (void **)&afReq.outData);
        freeKmem(fd, (void **)&afReq.outCtxData);
        return status;
    }
    
    getKmem(fd, rc4Result, (void **)&afReq.outData,    RC4_TESTSIZE);
    getKmem(fd, rc4Ctx,    (void **)&afReq.outCtxData, RC4_TESTSIZE);

    freeKmem(fd, (void **)&afReq.keyData);
    freeKmem(fd, (void **)&afReq.inData);
    freeKmem(fd, (void **)&afReq.outData);
    freeKmem(fd, (void **)&afReq.outCtxData);


    /* compare */
    if(memcmp(rc4TestData, rc4Result, RC4_TESTSIZE) == 0) {
        printf("*** Test AFHA Passed ***\n");
        teststat = 0;
    } else {
        printf("*** Test AFHA Failed ***\n");
        printf("received:\n");
        dumpm(rc4Result, RC4_TESTSIZE);
        printf("\nshould be:\n");
        dumpm(rc4TestData, RC4_TESTSIZE);
        printf("\n");
        teststat = -1;
    }

    return teststat;
}




