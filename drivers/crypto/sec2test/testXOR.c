
/****************************************************************************
 * testXOR.c - AESA/XOR known-answer test for SEC2 device driver
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
 * 1.3   14-Jul-2005 sec - first spin for 8548
 */



#include "sec2drvTest.h"
#include "Sec2.h"


#define XOR_TESTSIZE (32)


/* case #1, all 3 inputs */
static const unsigned char inA1[] =
{
    0xD4, 0x7B, 0x4E, 0xC5,
    0x2D, 0x9A, 0x6D, 0x6F,
    0xA0, 0xAC, 0xCC, 0x05,
    0x72, 0x88, 0xB8, 0xF9,
    0x3B, 0x4C, 0xA1, 0xFD,
    0xED, 0x90, 0xE2, 0x3D,
    0x2A, 0x51, 0xA4, 0x9D,
    0x98, 0xBE, 0xBD, 0x97,
};

static const unsigned char inB1[] =
{
    0x9A, 0xC9, 0x7B, 0x5C,
    0xCD, 0x1F, 0x4F, 0xF7,
    0x0E, 0xB4, 0xCB, 0x50,
    0x64, 0xC7, 0xFF, 0x65,
    0x8C, 0xB5, 0xFB, 0x71,
    0x82, 0xC2, 0x42, 0xEE,
    0xA7, 0xCB, 0x99, 0x24,
    0x34, 0x71, 0x79, 0xBC,
};

static const unsigned char inC1[] =
{
    0x4C, 0xF5, 0xDC, 0x1D,
    0xFC, 0xE5, 0x5C, 0xF8,
    0xF7, 0x94, 0x94, 0x4B,
    0xD3, 0x3B, 0xCD, 0x2B,
    0x24, 0xB6, 0x66, 0x96,
    0x9C, 0x6B, 0xED, 0xE1,
    0xE9, 0x04, 0xAA, 0x54,
    0x95, 0x17, 0x7C, 0xD9,
};

static const unsigned char exp1[] =
{
    0x02, 0x47, 0xE9, 0x84,
    0x1C, 0x60, 0x7E, 0x60,
    0x59, 0x8C, 0x93, 0x1E,
    0xC5, 0x74, 0x8A, 0xB7,
    0x93, 0x4F, 0x3C, 0x1A,
    0xF3, 0x39, 0x4D, 0x32,
    0x64, 0x9E, 0x97, 0xED,
    0x39, 0xD8, 0xB8, 0xF2,
};


/* Case #2, zeroed A input */
static const unsigned char inA2[] =
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

static const unsigned char inB2[] =
{
    0x9A, 0xC9, 0x7B, 0x5C,
    0xCD, 0x1F, 0x4F, 0xF7,
    0x0E, 0xB4, 0xCB, 0x50,
    0x64, 0xC7, 0xFF, 0x65,
    0x8C, 0xB5, 0xFB, 0x71,
    0x82, 0xC2, 0x42, 0xEE,
    0xA7, 0xCB, 0x99, 0x24,
    0x34, 0x71, 0x79, 0xBC,
};

static const unsigned char inC2[] =
{
    0x4C, 0xF5, 0xDC, 0x1D,
    0xFC, 0xE5, 0x5C, 0xF8,
    0xF7, 0x94, 0x94, 0x4B,
    0xD3, 0x3B, 0xCD, 0x2B,
    0x24, 0xB6, 0x66, 0x96,
    0x9C, 0x6B, 0xED, 0xE1,
    0xE9, 0x04, 0xAA, 0x54,
    0x95, 0x17, 0x7C, 0xD9,
};


static const unsigned char exp2[] =
{
    0xD6, 0x3C, 0xA7, 0x41,
    0x31, 0xfa, 0x13, 0x0f,
    0xf9, 0x20, 0x5f, 0x1b,
    0xb7, 0xfc, 0x32, 0x4e,
    0xa8, 0x03, 0x9d, 0xe7,
    0x1e, 0xa9, 0xaf, 0x0f,
    0x4e, 0xcf, 0x33, 0x70,
    0xa1, 0x66, 0x05, 0x65,
};



/* Case #3, 2 of 3 inputs */
static const unsigned char inB3[] =
{
    0x9A, 0xC9, 0x7B, 0x5C,
    0xCD, 0x1F, 0x4F, 0xF7,
    0x0E, 0xB4, 0xCB, 0x50,
    0x64, 0xC7, 0xFF, 0x65,
    0x8C, 0xB5, 0xFB, 0x71,
    0x82, 0xC2, 0x42, 0xEE,
    0xA7, 0xCB, 0x99, 0x24,
    0x34, 0x71, 0x79, 0xBC,
};

static const unsigned char inC3[] =
{
    0x4C, 0xF5, 0xDC, 0x1D,
    0xFC, 0xE5, 0x5C, 0xF8,
    0xF7, 0x94, 0x94, 0x4B,
    0xD3, 0x3B, 0xCD, 0x2B,
    0x24, 0xB6, 0x66, 0x96,
    0x9C, 0x6B, 0xED, 0xE1,
    0xE9, 0x04, 0xAA, 0x54,
    0x95, 0x17, 0x7C, 0xD9,
};

static const unsigned char exp3[] =
{
    0x02, 0x47, 0xE9, 0x84,
    0x1C, 0x60, 0x7E, 0x60,
    0x59, 0x8C, 0x93, 0x1E,
    0xC5, 0x74, 0x8A, 0xB7,
    0x93, 0x4F, 0x3C, 0x1A,
    0xF3, 0x39, 0x4D, 0x32,
    0x64, 0x9E, 0x97, 0xED,
    0x39, 0xD8, 0xB8, 0xF2,
};




static unsigned char outBuf[XOR_TESTSIZE];


int testXOR(int fd)
{
    RAID_XOR_REQ  xorRq;
    int           status, failct;
    

    failct = 0;

    printf("\n*** Test RAID/XOR (AESA) - 3-way ***\n");
        
    memset(outBuf, 0, XOR_TESTSIZE);
    memset(&xorRq, 0, sizeof(xorRq));
    
    xorRq.opId     = DPD_RAID_XOR;
    xorRq.opSize   = XOR_TESTSIZE;
    xorRq.outData  = outBuf;
    xorRq.inDataA  = (unsigned char *)inA1;
    xorRq.inDataB  = (unsigned char *)inB1;
    xorRq.inDataC  = (unsigned char *)inC1;


    status = putKmem(fd, (void *)inA1, (void **)&xorRq.inDataA, xorRq.opSize);
    if (status)
        return status;
    
    status = putKmem(fd, (void *)inB1, (void **)&xorRq.inDataB, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataA);
        return status;
    }
        
    status = putKmem(fd, (void *)inC1, (void **)&xorRq.inDataC, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataA);
        freeKmem(fd, (void **)&xorRq.inDataB);
        return status;
    }
        
    status = putKmem(fd, NULL, (void **)&xorRq.outData, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataA);
        freeKmem(fd, (void **)&xorRq.inDataB);
        freeKmem(fd, (void **)&xorRq.inDataC);
        return status;
    }
    
        
    armCompletion(&xorRq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&xorRq);
    
    if ((status = waitCompletion("testXOR(): RAID/XOR 3way test", status, &xorRq)))
    {
        freeKmem(fd, (void **)&xorRq.inDataA);
        freeKmem(fd, (void **)&xorRq.inDataB);
        freeKmem(fd, (void **)&xorRq.inDataC);
        freeKmem(fd, (void **)&xorRq.outData);
        return status;
    }
        
    getKmem(fd, outBuf, (void **)&xorRq.outData, xorRq.opSize);
    
    freeKmem(fd, (void **)&xorRq.inDataA);
    freeKmem(fd, (void **)&xorRq.inDataB);
    freeKmem(fd, (void **)&xorRq.inDataC);
    freeKmem(fd, (void **)&xorRq.outData);
    
    
    
    /* run results comparison */
    if ((memcmp(exp1, outBuf, xorRq.opSize)) == 0)
    {
        printf("*** Test RAID/XOR (AESA) - 3-way Passed ***\n");
        status = 0;
    } else
    {
        printf("*** Test RAID/XOR (AESA) - 3-way Failed ***\n");
        printf("expected result:\n");
        dumpm((unsigned char *)exp1, xorRq.opSize);
        printf("actual result:\n");
        dumpm(outBuf, xorRq.opSize);
        status = -1;
        failct++;
    }






    printf("\n*** Test RAID/XOR (AESA) - 2/3-way ***\n");
        
    memset(outBuf, 0, XOR_TESTSIZE);
    
    xorRq.inDataA  = (unsigned char *)inA2;
    xorRq.inDataB  = (unsigned char *)inB2;
    xorRq.inDataC  = (unsigned char *)inC2;


    status = putKmem(fd, (void *)inA2, (void **)&xorRq.inDataA, xorRq.opSize);
    if (status)
        return status;
    
    status = putKmem(fd, (void *)inB2, (void **)&xorRq.inDataB, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataA);
        return status;
    }
        
    status = putKmem(fd, (void *)inC2, (void **)&xorRq.inDataC, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataA);
        freeKmem(fd, (void **)&xorRq.inDataB);
        return status;
    }
        
    status = putKmem(fd, NULL, (void **)&xorRq.outData, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataA);
        freeKmem(fd, (void **)&xorRq.inDataB);
        freeKmem(fd, (void **)&xorRq.inDataC);
        return status;
    }
    
        
    armCompletion(&xorRq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&xorRq);
    
    if ((status = waitCompletion("testXOR(): RAID/XOR 2/3way test", status, &xorRq)))
    {
        freeKmem(fd, (void **)&xorRq.inDataA);
        freeKmem(fd, (void **)&xorRq.inDataB);
        freeKmem(fd, (void **)&xorRq.inDataC);
        freeKmem(fd, (void **)&xorRq.outData);
        return status;
    }
        
    getKmem(fd, outBuf, (void **)&xorRq.outData, xorRq.opSize);
    
    freeKmem(fd, (void **)&xorRq.inDataA);
    freeKmem(fd, (void **)&xorRq.inDataB);
    freeKmem(fd, (void **)&xorRq.inDataC);
    freeKmem(fd, (void **)&xorRq.outData);
    
    
    
    if ((memcmp(exp2, outBuf, xorRq.opSize)) == 0)
    {
        printf("*** Test RAID/XOR (AESA) - 2/3-way Passed ***\n");
        status = 0;
    } else
    {
        printf("*** Test RAID/XOR (AESA) - 2/3-way Failed ***\n");
        printf("expected result:\n");
        dumpm((unsigned char *)exp2, xorRq.opSize);
        printf("actual result:\n");
        dumpm(outBuf, xorRq.opSize);
        status = -1;
        failct++;
    }


#if 0
    printf("\n*** Test RAID/XOR (AESA) - 2-way ***\n");
        
    memset(outBuf, 0, XOR_TESTSIZE);
    
    xorRq.inDataA  = NULL;
    xorRq.inDataB  = (unsigned char *)inB3;
    xorRq.inDataC  = (unsigned char *)inC3;


    status = putKmem(fd, (void *)inB3, (void **)&xorRq.inDataB, xorRq.opSize);
    if (status)
        return status;
        
    status = putKmem(fd, (void *)inC3, (void **)&xorRq.inDataC, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataB);
        return status;
    }
        
    status = putKmem(fd, NULL, (void **)&xorRq.outData, xorRq.opSize);
    if (status) {
        freeKmem(fd, (void **)&xorRq.inDataB);
        freeKmem(fd, (void **)&xorRq.inDataC);
        return status;
    }
    
        
    armCompletion(&xorRq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&xorRq);
    
    if ((status = waitCompletion("testXOR(): RAID/XOR 2-way test", status, &xorRq)))
    {
        freeKmem(fd, (void **)&xorRq.inDataB);
        freeKmem(fd, (void **)&xorRq.inDataC);
        freeKmem(fd, (void **)&xorRq.outData);
        return status;
    }
        
    getKmem(fd, outBuf, (void **)&xorRq.outData, xorRq.opSize);
    
    freeKmem(fd, (void **)&xorRq.inDataB);
    freeKmem(fd, (void **)&xorRq.inDataC);
    freeKmem(fd, (void **)&xorRq.outData);
    
    
    
    if ((memcmp(exp3, outBuf, xorRq.opSize)) == 0)
    {
        printf("*** Test RAID/XOR (AESA) - 2-way Passed ***\n");
        status = 0;
    } else
    {
        printf("*** Test RAID/XOR (AESA) - 2-way Failed ***\n");
        printf("expected result:\n");
        dumpm((unsigned char *)exp3, xorRq.opSize);
        printf("actual result:\n");
        dumpm(outBuf, xorRq.opSize);
        status = -1;
        failct++;
    }
#endif

    if (failct)
        return -1;
    
    return 0;
}

