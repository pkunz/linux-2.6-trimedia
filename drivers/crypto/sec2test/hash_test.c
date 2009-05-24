
/****************************************************************************
 * hash_test.c - basic SHA test for SEC2 driver testing
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
 * 1.1.0 Dec 05,2004 sec - prep for release
 * 1.2   02-Feb-2005 sec - include new case input from Todd
 * 1.3   Aug 11,2005 sec - add SHA224 support for SEC2.1
 */


#include "sec2drvTest.h"
#include "Sec2.h"

typedef struct
{
    unsigned long   opId;
    unsigned long   plainTextLen;
    unsigned char   *pPlaintext;
    unsigned long   digestLen;
    unsigned char   *pDigest;
    char            testDesc[30];
} HASHTESTTYPE;

/*
 * SHA1 test vectors from FIPS PUB 180-1
 */
static const unsigned char sha1padplaintext1[] = "abc";
static const unsigned char sha1paddigest1[] = 
{ 
    0xa9, 0x99, 0x3e, 0x36, 0x47, 
    0x06, 0x81, 0x6a, 0xba, 0x3e,
    0x25, 0x71, 0x78, 0x50, 0xc2, 
    0x6c, 0x9c, 0xd0, 0xd8, 0x9d 
};

static const unsigned char sha1padplaintext2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
static const unsigned char sha1paddigest2[] = 
{ 
    0x84, 0x98, 0x3e, 0x44, 0x1c, 
    0x3b, 0xd2, 0x6e, 0xba, 0xae,
    0x4a, 0xa1, 0xf9, 0x51, 0x29, 
    0xe5, 0xe5, 0x46, 0x70, 0xf1 
};

/*
 * SHA256 test vectors from from NIST
 */
static const unsigned char sha256padplaintext1[] = "abc";
static const unsigned char sha256paddigest1[] = 
{ 
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad 
};

static const unsigned char sha256padplaintext2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
static const unsigned char sha256paddigest2[] = 
{ 
    0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
    0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
    0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
    0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1 
};

/*
 * MD5 test vectors from RFC1321
 */
static const unsigned char md5padplaintext1[] = "a";
static const unsigned char md5paddigest1[] = 
{
    0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
    0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61
}; 

static const unsigned char md5padplaintext2[] = "abc";
static const unsigned char md5paddigest2[] = 
{
    0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
    0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
}; 

static const unsigned char md5padplaintext3[] = "message digest";
static const unsigned char md5paddigest3[] = 
{
    0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
    0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0
}; 

static const unsigned char md5padplaintext4[] = "abcdefghijklmnopqrstuvwxyz";
static const unsigned char md5paddigest4[] = 
{
    0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
    0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b
}; 

static const unsigned char md5nopadplaintext[] = 
{
    'a', 0x80,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    8,0,0,0,0,0,0,0
};

static const unsigned char md5nopaddigest[] = 
{
    0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
    0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61
}; 


static const unsigned char sha1nopadplaintext[] = 
{
    'a','b','c',0x80,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,24
};
static const unsigned char sha1nopaddigest[] = 
{
    0xa9, 0x99, 0x3e, 0x36, 0x47, 
    0x06, 0x81, 0x6a, 0xba, 0x3e,
    0x25, 0x71, 0x78, 0x50, 0xc2, 
    0x6c, 0x9c, 0xd0, 0xd8, 0x9d 
}; 

static const unsigned char sha256nopadplaintext[] = 
{
    'a','b','c',0x80,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,24
};

static const unsigned char sha256nopaddigest[] = 
{
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad 
}; 

static const unsigned char sha224paddigest1[] =
{
    0x23, 0x09, 0x7d, 0x22,
    0x34, 0x05, 0xd8, 0x22,
    0x86, 0x42, 0xa4, 0x77,
    0xbd, 0xa2, 0x55, 0xb3,
    0x2a, 0xad, 0xbc, 0xe4,
    0xbd, 0xa0, 0xb3, 0xf7,
    0xe3, 0x6c, 0x9d, 0xa7
};

static const unsigned char sha224paddigest2[] =
{
    0x75, 0x38, 0x8b, 0x16,
    0x51, 0x27, 0x76, 0xcc,
    0x5d, 0xba, 0x5d, 0xa1,
    0xfd, 0x89, 0x01, 0x50,
    0xb0, 0xc6, 0x45, 0x5c,
    0xb4, 0xf5, 0x8b, 0x19,
    0x52, 0x52, 0x25, 0x25
};

#ifdef FEATURE_EXISTS_SEC2_1
#define NUM_HASHTESTS   13
#else
#define NUM_HASHTESTS   11
#endif

HASHTESTTYPE hashTest[NUM_HASHTESTS] =
{
    {
        /* SHA-1 PAD Test 1 */
        DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
        3,
        (unsigned char *)sha1padplaintext1,
        20,
        (unsigned char *)sha1paddigest1,
        "SHA-1 HASH PAD TEST 1",
    },
    
    {
        /* SHA-1 PAD Test 2 */
        DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
        56,
        (unsigned char *)sha1padplaintext2,
        20,
        (unsigned char *)sha1paddigest2,
        "SHA-1 HASH PAD TEST 2",
    },
    
    {
        /* SHA-256 PAD Test 1 */
        DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX,
        3,
        (unsigned char *)sha256padplaintext1,
        32,
        (unsigned char *)sha256paddigest1,
        "SHA-256 HASH PAD TEST 1",
    },
    
    {
        /* SHA-256 PAD Test 2 */
        DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX,
        56,
        (unsigned char *)sha256padplaintext2,
        32,
        (unsigned char *)sha256paddigest2,
        "SHA-256 HASH PAD TEST 2",
    },
    
    {
        /* MD5 PAD Test 1 */
        DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
        1,
        (unsigned char *)md5padplaintext1,
        16,
        (unsigned char *)md5paddigest1,
        "MD5 HASH PAD TEST 1",
    },
    
    {
        /* MD5 PAD Test 2 */
        DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
        3,
        (unsigned char *)md5padplaintext2,
        16,
        (unsigned char *)md5paddigest2,
        "MD5 HASH PAD TEST 2",
    },
    
    {
        /* MD5 PAD Test 3 */
        DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
        14,
        (unsigned char *)md5padplaintext3,
        16,
        (unsigned char *)md5paddigest3,
        "MD5 HASH PAD TEST 3",
    },
    
    {
        /* MD5 PAD Test 4 */
        DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
        26,
        (unsigned char *)md5padplaintext4,
        16,
        (unsigned char *)md5paddigest4,
        "MD5 HASH PAD TEST 4",
    },
    
    {
        /* MD5 No PAD Test */
        DPD_MD5_LDCTX_IDGS_HASH_ULCTX,
        64,
        (unsigned char *)md5nopadplaintext,
        16,
        (unsigned char *)md5nopaddigest,
        "MD5 HASH No PAD TEST",
    },
    
    {
        /* SHA-1 No PAD Test */
        DPD_SHA_LDCTX_IDGS_HASH_ULCTX,
        64,
        (unsigned char *)sha1nopadplaintext,
        20,
        (unsigned char *)sha1nopaddigest,
        "SHA-1 HASH No PAD TEST",
    },
    
    {
        /* SHA-256 No PAD Test */
        DPD_SHA256_LDCTX_IDGS_HASH_ULCTX,
        64,
        (unsigned char *)sha256nopadplaintext,
        32,
        (unsigned char *)sha256nopaddigest,
        "SHA-256 HASH No PAD TEST",
    },
#ifdef FEATURE_EXISTS_SEC2_1    
    {
        /* SHA-224 PAD Test #1 from RFC3874 */
        DPD_SHA224_LDCTX_HASH_PAD_ULCTX,
        3,
        (unsigned char *)sha1padplaintext1,
        28,
        (unsigned char *)sha224paddigest1,
        "SHA-224 HASH PAD TEST 1",
    },
    
    {
        /* SHA-224 PAD Test #2 from RFC3874 */
        DPD_SHA224_LDCTX_HASH_PAD_ULCTX,
        56,
        (unsigned char *)sha1padplaintext2,
        28,
        (unsigned char *)sha224paddigest2,
        "SHA-224 HASH PAD TEST 2"
    }
#endif    
};


int hash_test(int fd)
{
    HASH_REQ hashRQ;
    int      iTestIndex, stat, testfail;
    char    *digestBuffer;

    testfail = 0;
    
    digestBuffer = (unsigned char *)malloc(32);
    
    for (iTestIndex = 0; iTestIndex < NUM_HASHTESTS; iTestIndex++)
    {
        printf("*** %s ***\n", hashTest[iTestIndex].testDesc);
        memset(digestBuffer, 0, sizeof(digestBuffer));
        memset(&hashRQ, 0, sizeof(hashRQ));
        
        hashRQ.opId     = hashTest[iTestIndex].opId;
        hashRQ.inBytes  = hashTest[iTestIndex].plainTextLen;
        hashRQ.inData   = hashTest[iTestIndex].pPlaintext; 
        hashRQ.outBytes = hashTest[iTestIndex].digestLen;
        hashRQ.outData  = digestBuffer;
        
        stat = putKmem(fd, (void *)hashTest[iTestIndex].pPlaintext, (void **)&hashRQ.inData, hashRQ.inBytes);
        if (stat) return stat;
        
        stat = putKmem(fd, NULL, (void **)&hashRQ.outData, hashRQ.outBytes);
        if (stat)
        {
            freeKmem(fd, (void **)&hashRQ.inData);
            return stat;
        }

        armCompletion(&hashRQ);
        stat = ioctl(fd, IOCTL_PROC_REQ, (int)&hashRQ);

        if ((stat = waitCompletion("hash_test(): padded SHA test", stat, &hashRQ)))
        {
            freeKmem(fd, (void **)&hashRQ.inData);
            freeKmem(fd, (void **)&hashRQ.outData);
            return stat;
        }
        
        getKmem(fd, digestBuffer, (void **)&hashRQ.outData, hashRQ.outBytes);
        freeKmem(fd, (void **)&hashRQ.inData);
        freeKmem(fd, (void **)&hashRQ.outData);

        if (memcmp(digestBuffer, hashTest[iTestIndex].pDigest, hashTest[iTestIndex].digestLen)) 
        {
            printf("*** %s Failed ***\n", hashTest[iTestIndex].testDesc);
            printf("expected digest:\n");
            dumpm(hashTest[iTestIndex].pDigest, hashTest[iTestIndex].digestLen);
            printf("actual digest:\n");
            dumpm(digestBuffer, hashTest[iTestIndex].digestLen);
            testfail++;
        }
        
        printf("*** %s Complete ***\n", hashTest[iTestIndex].testDesc);

    }
    free(digestBuffer);
    if (testfail)
        return -1;
    else
        return 0;
}



