
/****************************************************************************
 * testHmac.c - HMAC known-answer test for SEC2 device driver
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
 * 1.2   02-Feb-2005 sec - add in Todd's extra cases
 * 1.3   Aug 11,2005
 */



#include "sec2drvTest.h"
#include "Sec2.h"


typedef struct
{
    unsigned long   opId;
    unsigned long   keyLen;
    unsigned char   *pKeydata;
    unsigned long   plainTextLen;
    unsigned char   *pPlaintext;
    unsigned long   digestLen;
    unsigned char   *pDigest;
    char            testDesc[30];
} HMACTESTTYPE;


/*
 * HMAC-MD5 test vector from RFC2202
 */

static const unsigned char md5padkeydata1[] = "Jefe"; /* 4 */
/* Plaintext padded manually for no pad test */
static const unsigned char md5padplaintext1[] = /* 64 */
{
    'w','h','a','t',' ','d','o',' ',
    'y','a',' ','w','a','n','t',' ', 
    'f','o','r',' ','n','o','t','h',
    'i','n','g','?',0x80,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0xe0,0x02,0,0,0,0,0,0
};
static const unsigned char md5paddigest1[] = /* 16 */
{ 
    0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03,
    0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38 
};

/*
 * HMAC-SHA1 test vector from RFC2202
 */
static const unsigned char sha1padkeydata1[] = "Jefe"; /* 4 */
/* Plaintext padded manually for no pad test */
static const unsigned char sha1padplaintext1[] =  /* 64 */
{
    'w','h','a','t',' ','d','o',' ',
    'y','a',' ','w','a','n','t',' ', 
    'f','o','r',' ','n','o','t','h',
    'i','n','g','?',0x80,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0x02,0xe0
};
static const unsigned char sha1paddigest1[] = /* 20 */
{ 
    0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2, 0xd2, 0x74, 
    0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c, 0x25, 0x9a, 0x7c, 0x79 
};

/*
 * HMAC-SHA256 test vector from
 * draft-ietf-ipsec-ciph-sha-256-01.txt
 */
static const unsigned char sha256padkeydata1[] = /* 32 */
{   
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
};
/* Plaintext padded manually for no pad test */
static const unsigned char sha256padplaintext1[] = /* 64 */
{
    'a','b','c',0x80,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0x02,0x18
};
static const unsigned char sha256paddigest1[] = /* 32 */
{ 
    0xa2, 0x1b, 0x1f, 0x5d, 0x4c, 0xf4, 0xf7, 0x3a,
    0x4d, 0xd9, 0x39, 0x75, 0x0f, 0x7a, 0x06, 0x6a,
    0x7f, 0x98, 0xcc, 0x13, 0x1c, 0xb1, 0x6a, 0x66,
    0x92, 0x75, 0x90, 0x21, 0xcf, 0xab, 0x81, 0x81 
};


static unsigned char tag1[32];

#define NUM_HMACTESTS   6


HMACTESTTYPE hmacTest[NUM_HMACTESTS] =
{
    {
        /* MD5 HMAC PAD Test 1 */
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        4,
        (unsigned char *)md5padkeydata1,
        28,
        (unsigned char *)md5padplaintext1,
        16,
        (unsigned char *)md5paddigest1,
        "MD5 HMAC PAD TEST 1",
    },
    
    {
        /* SHA-1 HMAC PAD Test 1 */
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        4,
        (unsigned char *)sha1padkeydata1,
        28,
        (unsigned char *)sha1padplaintext1,
        20,
        (unsigned char *)sha1paddigest1,
        "SHA-1 HMAC PAD TEST 1",
    },
    
    {
        /* SHA-256 HMAC PAD Test 1 */
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        32,
        (unsigned char *)sha256padkeydata1,
        3,
        (unsigned char *)sha256padplaintext1,
        32,
        (unsigned char *)sha256paddigest1,
        "SHA-256 HMAC PAD TEST 1",
    },
    
    {
        /* MD5 HMAC No PAD Test 1 */
        DPD_MD5_LDCTX_HMAC_ULCTX,
        4,
        (unsigned char *)md5padkeydata1,
        64,
        (unsigned char *)md5padplaintext1,
        16,
        (unsigned char *)md5paddigest1,
        "MD5 HMAC NO PAD TEST 1",
    },
    
    {
        /* SHA-1 HMAC No PAD Test 1 */
        DPD_SHA_LDCTX_HMAC_ULCTX,
        4,
        (unsigned char *)sha1padkeydata1,
        64,
        (unsigned char *)sha1padplaintext1,
        20,
        (unsigned char *)sha1paddigest1,
        "SHA-1 HMAC NO PAD TEST 1",
    },
    
    {
        /* SHA-256 HMAC No PAD Test 1 */
        DPD_SHA256_LDCTX_HMAC_ULCTX,
        32,
        (unsigned char *)sha256padkeydata1,
        64,
        (unsigned char *)sha256padplaintext1,
        32,
        (unsigned char *)sha256paddigest1,
        "SHA-256 HMAC NO PAD TEST 1"
    }
};


int testHmac(int fd)
{
    HMAC_PAD_REQ hmacRQ;
    int          status, testfail;
    int          iTestIndex;
    
    testfail = 0;
    for (iTestIndex = 0; iTestIndex < 3; iTestIndex++) 
    {
        printf("*** %s ***\n", hmacTest[iTestIndex].testDesc);
        
        memset(tag1, 0, 32);
        memset(&hmacRQ, 0, sizeof(hmacRQ));
        
        
        hmacRQ.opId     = hmacTest[iTestIndex].opId;
        hmacRQ.keyBytes = hmacTest[iTestIndex].keyLen;
        hmacRQ.keyData  = hmacTest[iTestIndex].pKeydata;
        hmacRQ.inBytes  = hmacTest[iTestIndex].plainTextLen;
        hmacRQ.inData   = hmacTest[iTestIndex].pPlaintext; /* length-padded buffer */
        hmacRQ.outBytes = hmacTest[iTestIndex].digestLen;
        hmacRQ.outData  = tag1;
        
        
        status = putKmem(fd,
                         hmacTest[iTestIndex].pKeydata,
                         (void **)&hmacRQ.keyData, 
                         hmacTest[iTestIndex].keyLen);
        if (status)
            return status;
        
        status = putKmem(fd, 
                         hmacTest[iTestIndex].pPlaintext, 
                         (void **)&hmacRQ.inData,
                         hmacTest[iTestIndex].plainTextLen);
        if (status) {
            freeKmem(fd, (void **)&hmacRQ.keyData);
            return status;
        }
        
        status = putKmem(fd, NULL, (void **)&hmacRQ.outData, hmacTest[iTestIndex].digestLen);
        if (status) {
            freeKmem(fd, (void **)&hmacRQ.keyData);
            freeKmem(fd, (void **)&hmacRQ.inData);
            return status;
        }
        
        
        armCompletion(&hmacRQ);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&hmacRQ);
        
        if ((status = waitCompletion("testHmac(): message digest test", status, &hmacRQ)))
        {
            freeKmem(fd, (void **)&hmacRQ.keyData);
            freeKmem(fd, (void **)&hmacRQ.inData);
            freeKmem(fd, (void **)&hmacRQ.outData);
            return status;
        }
        
        getKmem(fd, tag1, (void **)&hmacRQ.outData, hmacTest[iTestIndex].digestLen);
        
        freeKmem(fd, (void **)&hmacRQ.keyData);
        freeKmem(fd, (void **)&hmacRQ.inData);
        freeKmem(fd, (void **)&hmacRQ.outData);
        
        
        /* compare result */
        if (memcmp(tag1, hmacTest[iTestIndex].pDigest, hmacTest[iTestIndex].digestLen) == 0) 
            status = 0;
        else 
        {
            printf("*** %s Failed ***\n", hmacTest[iTestIndex].testDesc);
            printf("expected digest:\n");
            dumpm(hmacTest[iTestIndex].pDigest, hmacTest[iTestIndex].digestLen);
            printf("actual digest:\n");
            dumpm(tag1, hmacTest[iTestIndex].digestLen);
            status = -1;
            testfail++;
        }
        printf("*** %s Complete ***\n", hmacTest[iTestIndex].testDesc);
    } /* for */
    if (testfail)
        return -1;
    else
        return status;
}

