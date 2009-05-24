

/****************************************************************************
 * ccmpdec.c - Simulated CCMP decrypt packet test for SEC2 driver testing
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
 * 1.2 12-May-2005 sec - module kindly contributed by dgs
 * 1.3 Aug 11,2005
 */


#include "Sec2.h"
#include "sec2drvTest.h"


/* 16 bytes */
static const unsigned char key[] =
{
	0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,
	0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf
};



/* 56 bytes */
static const unsigned char context[] = 
{
    /* IV: Mac Size 8 bytes, Nonce Size 13 bytes, Msg length 2 bytes */
    0x59,
    /* Nonce */
    0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00,
    0xa0, 0xa1, 0xa2, 0xA3, 0xa4, 0xa5, 
    /* Message length */
    0x00, 0x17,    
    
    /* MIC */ 
    0x17, 0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* CCMP Initial counter: Nonce (13 bytes) counter (2 byes)*/
    0x01,
    /* Nonce */
    0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00,
    0xa0, 0xa1, 0xa2, 0xA3, 0xa4, 0xa5, 
    /* Counter value */
    0x00, 0x00,
    
    /* counter modulus exponent */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

/* 16+23 bytes */
static unsigned const char inpacket[] =
{
    /* header  hash only part (16 bytes)*/
    /* Alen: 8*/
    0x00, 0x08,
    /* AAD */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    /* padding to 16 bytes */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* cipher */
    0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6, 0x63, 0xd2,
    0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80,
    0x6d, 0x5f, 0x6b, 0x61, 0xda, 0xc3, 0x84, 0x00
};

/* 23 bytes */
static const unsigned char expected_out[] = 
{
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x00
};



/* 32 bytes */
static const unsigned char expect_ctx_out[] =
{
    /*  computed mac out */
    0x2d, 0xc6, 0x97, 0xe4, 0x11, 0xca, 0x83, 0xa8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* mic out */
    0x2d, 0xc6, 0x97, 0xe4, 0x11, 0xca, 0x83, 0xa8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static unsigned char plain[100];
static unsigned char ctx_out[32];


int testccmpdec(int fd)
{

    CCMP_REQ CcmpReq;
    int status;
    

    printf("*** Test AESA CCM mode Decrypt ***\n");
    
    memset(&CcmpReq, 0, sizeof(CcmpReq));

    CcmpReq.opId           = DPD_802_11_CCMP_INBOUND;

    /* inputs */
    CcmpReq.ctxBytes       = 56;
    CcmpReq.context        = (unsigned char *)context;

    CcmpReq.keyBytes       = 16;
    CcmpReq.keyData        = (unsigned char *)key;

    CcmpReq.FrameDataBytes = 39;
    CcmpReq.FrameData      = (unsigned char *)inpacket;

    /* outputs */
    CcmpReq.cryptDataBytes = 24;
    CcmpReq.cryptDataOut   = plain;

    CcmpReq.MICBytes       = 32;
    CcmpReq.MICData        = ctx_out;

    status = putKmem(fd, (void *)context, (void **)&CcmpReq.context, CcmpReq.ctxBytes);
    if (status)
        return status;
    
    status = putKmem(fd, (void *)key, (void **)&CcmpReq.keyData,  CcmpReq.keyBytes);
    if (status) {
        freeKmem(fd, (void **)&CcmpReq.context);
        return status;
    }

    status = putKmem(fd, (void *)inpacket, (void **)&CcmpReq.FrameData,  CcmpReq.FrameDataBytes);
    if (status) {
        freeKmem(fd, (void **)&CcmpReq.context);
        freeKmem(fd, (void **)&CcmpReq.keyData);
        return status;
    }

    status = putKmem(fd, NULL, (void **)&CcmpReq.MICData,  CcmpReq.MICBytes);
    if (status) {
        freeKmem(fd, (void **)&CcmpReq.context);
        freeKmem(fd, (void **)&CcmpReq.keyData);
        freeKmem(fd, (void **)&CcmpReq.FrameData);
        return status;
    }

    status = putKmem(fd, NULL, (void **)&CcmpReq.cryptDataOut,  CcmpReq.cryptDataBytes);
    if (status) {
        freeKmem(fd, (void **)&CcmpReq.context);
        freeKmem(fd, (void **)&CcmpReq.keyData);
        freeKmem(fd, (void **)&CcmpReq.FrameData);
        freeKmem(fd, (void **)&CcmpReq.MICData);
        return status;
    }

    armCompletion(&CcmpReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&CcmpReq);
    if ((status = waitCompletion("testccmp(): CCMP packet test", status, &CcmpReq)))
    {
        freeKmem(fd, (void **)&CcmpReq.context);
        freeKmem(fd, (void **)&CcmpReq.keyData);
        freeKmem(fd, (void **)&CcmpReq.FrameData);
        freeKmem(fd, (void **)&CcmpReq.MICData);
        freeKmem(fd, (void **)&CcmpReq.cryptDataOut);
        return -1;
    }

    getKmem(fd, plain, (void **)&CcmpReq.cryptDataOut, CcmpReq.cryptDataBytes);
    getKmem(fd, ctx_out, (void **)&CcmpReq.MICData, CcmpReq.MICBytes);
    
    freeKmem(fd, (void **)&CcmpReq.context);
    freeKmem(fd, (void **)&CcmpReq.keyData);
    freeKmem(fd, (void **)&CcmpReq.FrameData);
    freeKmem(fd, (void **)&CcmpReq.MICData);
    freeKmem(fd, (void **)&CcmpReq.cryptDataOut);

    if (memcmp((unsigned char *)expected_out, plain, CcmpReq.cryptDataBytes) != 0)
    {
        printf("PAYLOAD OUT DOES NOT MATCH:\n");
        dumpm(plain, CcmpReq.cryptDataBytes);
        return -1;
    }

    printf("Payload Out Matches Expected\n");

    if (memcmp((unsigned char *)expect_ctx_out, ctx_out, 16) != 0)
    {
        printf("Computed MAC OUT DOES NOT MATCH:\n");
        dumpm(ctx_out, 16);
        return -1;
    }

    printf("Computed MAC Matches Expected\n");

    if (memcmp((unsigned char *) &expect_ctx_out[16], &ctx_out[16], 16) != 0)
    {           
        printf("MIC OUT DOES NOT MATCH:\n");
        dumpm(ctx_out, 16);
        return -1;
    }

    printf("*** Test AESA CCM mode Passed***\n");
    
    return 0;
}

