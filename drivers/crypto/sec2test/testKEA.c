
/****************************************************************************
 * testKEA.c - Kasumi known-answer test for SEC2 device driver
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
 *       Jul 14,2005 sec - first spin for 8548
 */



#include "sec2drvTest.h"
#include "Sec2.h"



typedef struct _f8_testset
{
    const unsigned char *f8_iv;
    const unsigned char *f8_key;
    const unsigned char *f8_in;
    const unsigned char *f8_expout;
    unsigned long        iv_len;
    unsigned long        key_len;
    unsigned long        data_len;
} f8_testset;


static const unsigned char f8_iv_0[] =
{
    0x72, 0xa4, 0xf2, 0x0f,
    0x64, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c,
};

static const unsigned char f8_key_0[] =
{
    0x2b, 0xd6, 0x45, 0x9f,
    0x82, 0xc5, 0xb3, 0x00,
    0x95, 0x2c, 0x49, 0x10,
    0x48, 0x81, 0xff, 0x48,
};

static const unsigned char f8_in_0[] =
{
    0x7e, 0xc6, 0x12, 0x72,
    0x74, 0x3b, 0xf1, 0x61,
    0x47, 0x26, 0x44, 0x6a,
    0x6c, 0x38, 0xce, 0xd1,
    0x66, 0xf6, 0xca, 0x76,
    0xeb, 0x54, 0x30, 0x04,
    0x42, 0x86, 0x34, 0x6c,
    0xef, 0x13, 0x0f, 0x92,
    0x92, 0x2b, 0x03, 0x45,
    0x0d, 0x3a, 0x99, 0x75,
    0xe5, 0xbd, 0x2e, 0xa0,
    0xeb, 0x55, 0xad, 0x8e,
    0x1b, 0x19, 0x9e, 0x3e,
    0xc4, 0x31, 0x60, 0x20,
    0xe9, 0xa1, 0xb2, 0x85,
    0xe7, 0x62, 0x79, 0x53,
    0x59, 0xb7, 0xbd, 0xfd,
    0x39, 0xbe, 0xf4, 0xb2,
    0x48, 0x45, 0x83, 0xd5,
    0xaf, 0xe0, 0x82, 0xae,
    0xe6, 0x38, 0xbf, 0x5f,
    0xd5, 0xa6, 0x06, 0x19,
    0x39, 0x01, 0xa0, 0x8f,
    0x4a, 0xb4, 0x1a, 0xab,
    0x9b, 0x13, 0x48, 0x80,
    0x00, 0x00, 0x00, 0x00,
};


static const unsigned char f8_expout_0[] =
{
    0xd1, 0xe2, 0xde, 0x70,
    0xee, 0xf8, 0x6c, 0x69,
    0x64, 0xfb, 0x54, 0x2b,
    0xc2, 0xd4, 0x60, 0xaa,
    0xbf, 0xaa, 0x10, 0xa4,
    0xa0, 0x93, 0x26, 0x2b,
    0x7d, 0x19, 0x9e, 0x70,
    0x6f, 0xc2, 0xd4, 0x89,
    0x15, 0x53, 0x29, 0x69,
    0x10, 0xf3, 0xa9, 0x73,
    0x01, 0x26, 0x82, 0xe4,
    0x1c, 0x4e, 0x2b, 0x02,
    0xbe, 0x20, 0x17, 0xb7,
    0x25, 0x3b, 0xbf, 0x93,
    0x09, 0xde, 0x58, 0x19,
    0xcb, 0x42, 0xe8, 0x19,
    0x56, 0xf4, 0xc9, 0x9b,
    0xc9, 0x76, 0x5c, 0xaf,
    0x53, 0xb1, 0xd0, 0xbb,
    0x82, 0x79, 0x82, 0x6a,
    0xdb, 0xbc, 0x55, 0x22,
    0xe9, 0x15, 0xc1, 0x20,
    0xa6, 0x18, 0xa5, 0xa7,
    0xf5, 0xe8, 0x97, 0x08,
    0x93, 0x39, 0x65, 0x0f,
    0x00, 0x00, 0x00, 0x00,
};



static const unsigned char f8_iv_1[] =
{
    0xe2, 0x8b, 0xcf, 0x7b,
    0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18,
};


static const unsigned char f8_key_1[] =
{
    0xef, 0xa8, 0xb2, 0x22,
    0x9e, 0x72, 0x0c, 0x2a,
    0x7c, 0x36, 0xea, 0x55,
    0xe9, 0x60, 0x56, 0x95,
};


static const unsigned char f8_in_1[] =
{
    0x10, 0x11, 0x12, 0x31,
    0xe0, 0x60, 0x25, 0x3a,
    0x43, 0xfd, 0x3f, 0x57,
    0xe3, 0x76, 0x07, 0xab,
    0x28, 0x27, 0xb5, 0x99,
    0xb6, 0xb1, 0xbb, 0xda,
    0x37, 0xa8, 0xab, 0xcc,
    0x5a, 0x8c, 0x55, 0x0d,
    0x1b, 0xfb, 0x2f, 0x49,
    0x46, 0x24, 0xfb, 0x50,
    0x36, 0x7f, 0xa3, 0x6c,
    0xe3, 0xbc, 0x68, 0xf1,
    0x1c, 0xf9, 0x3b, 0x15,
    0x10, 0x37, 0x6b, 0x02,
    0x13, 0x0f, 0x81, 0x2a,
    0x9f, 0xa1, 0x69, 0xd8,
};


static const unsigned char f8_expout_1[] =
{
    0x3d, 0xea, 0xcc, 0x7c,
    0x15, 0x82, 0x1c, 0xaa,
    0x89, 0xee, 0xca, 0xde,
    0x9b, 0x5b, 0xd3, 0x61,
    0x4b, 0xd0, 0xc8, 0x41,
    0x9d, 0x71, 0x03, 0x85,
    0xdd, 0xbe, 0x58, 0x49,
    0xef, 0x1b, 0xac, 0x5a,
    0xe8, 0xb1, 0x4a, 0x5b,
    0x0a, 0x67, 0x41, 0x52,
    0x1e, 0xb4, 0xe0, 0x0b,
    0xb9, 0xec, 0xf3, 0xe9,
    0xf7, 0xcc, 0xb9, 0xca,
    0xe7, 0x41, 0x52, 0xd7,
    0xf4, 0xe2, 0xa0, 0x34,
    0xb6, 0xea, 0x00, 0xec,
};




static const unsigned char f8_iv_2[] =
{
    0xfa, 0x55, 0x6b, 0x26,
    0x1c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x03,
};

static const unsigned char f8_key_2[] =
{
    0x5a, 0xcb, 0x1d, 0x64,
    0x4c, 0x0d, 0x51, 0x20,
    0x4e, 0xa5, 0xf1, 0x45,
    0x10, 0x10, 0xd8, 0x52,
};

static const unsigned char f8_in_2[] =
{
    0xad, 0x9c, 0x44, 0x1f,
    0x89, 0x0b, 0x38, 0xc4,
    0x57, 0xa4, 0x9d, 0x42,
    0x14, 0x07, 0xe8, 0x00,
};

static const unsigned char f8_expout_2[] =
{
    0x9b, 0xc9, 0x2c, 0xa8,
    0x03, 0xc6, 0x7b, 0x28,
    0xa1, 0x1a, 0x4b, 0xee,
    0x5a, 0x0c, 0x25, 0x00,
};



static const unsigned char f8_iv_3[] =
{
    0x39, 0x8a, 0x59, 0xb4,
    0x2c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05,
};

static const unsigned char f8_key_3[] =
{
    0xd3, 0xc5, 0xd5, 0x92,
    0x32, 0x7f, 0xb1, 0x1c,
    0x40, 0x35, 0xc6, 0x68,
    0x0a, 0xf8, 0xc6, 0xd1,
};

static const unsigned char f8_in_3[] =
{
    0x98, 0x1b, 0xa6, 0x82,
    0x4c, 0x1b, 0xfb, 0x1a,
    0xb4, 0x85, 0x47, 0x20,
    0x29, 0xb7, 0x1d, 0x80,
    0x8c, 0xe3, 0x3e, 0x2c,
    0xc3, 0xc0, 0xb5, 0xfc,
    0x1f, 0x3d, 0xe8, 0xa6,
    0xdc, 0x66, 0xb1, 0xf0,
};

static const unsigned char f8_expout_3[] =
{
    0x5b, 0xb9, 0x43, 0x1b,
    0xb1, 0xe9, 0x8b, 0xd1,
    0x1b, 0x93, 0xdb, 0x7c,
    0x3d, 0x45, 0x13, 0x65,
    0x59, 0xbb, 0x86, 0xa2,
    0x95, 0xaa, 0x20, 0x4e,
    0xcb, 0xeb, 0xf6, 0xf7,
    0xa5, 0x10, 0x15, 0x12,
};




/* 16-byte context */
static const unsigned char f8_iv_4[] =
{
    0x72, 0xa4, 0xf2, 0x0f,
    0x64, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c
};

/* 16-byte key */
static const unsigned char f8_key_4[] = 
{
    0x2b, 0xd6, 0x45, 0x9f,
    0x82, 0xc5, 0xb3, 0x00,
    0x95, 0x2c, 0x49, 0x10,
    0x48, 0x81, 0xff, 0x48
};

/* 100 byte payload */
static const unsigned char f8_in_4[] =
{
    0x7e, 0xc6, 0x12, 0x72,
    0x74, 0x3b, 0xf1, 0x61,
    0x47, 0x26, 0x44, 0x6a,
    0x6c, 0x38, 0xce, 0xd1,
    0x66, 0xf6, 0xca, 0x76,
    0xeb, 0x54, 0x30, 0x04,
    0x42, 0x86, 0x34, 0x6c,
    0xef, 0x13, 0x0f, 0x92,
    0x92, 0x2b, 0x03, 0x45,
    0x0d, 0x3a, 0x99, 0x75,
    0xe5, 0xbd, 0x2e, 0xa0,
    0xeb, 0x55, 0xad, 0x8e,
    0x1b, 0x19, 0x9e, 0x3e,
    0xc4, 0x31, 0x60, 0x20,
    0xe9, 0xa1, 0xb2, 0x85,
    0xe7, 0x62, 0x79, 0x53,
    0x59, 0xb7, 0xbd, 0xfd,
    0x39, 0xbe, 0xf4, 0xb2,
    0x48, 0x45, 0x83, 0xd5,
    0xaf, 0xe0, 0x82, 0xae,
    0xe6, 0x38, 0xbf, 0x5f,
    0xd5, 0xa6, 0x06, 0x19,
    0x39, 0x01, 0xa0, 0x8f,
    0x4a, 0xb4, 0x1a, 0xab,
    0x9b, 0x13, 0x48, 0x80
};

/* 100-byte expected */
static const unsigned char f8_expout_4[] =
{
    0xd1, 0xe2, 0xde, 0x70,
    0xee, 0xf8, 0x6c, 0x69,
    0x64, 0xfb, 0x54, 0x2b,
    0xc2, 0xd4, 0x60, 0xaa,
    0xbf, 0xaa, 0x10, 0xa4,
    0xa0, 0x93, 0x26, 0x2b,
    0x7d, 0x19, 0x9e, 0x70,
    0x6f, 0xc2, 0xd4, 0x89,
    0x15, 0x53, 0x29, 0x69,
    0x10, 0xf3, 0xa9, 0x73,
    0x01, 0x26, 0x82, 0xe4,
    0x1c, 0x4e, 0x2b, 0x02,
    0xbe, 0x20, 0x17, 0xb7,
    0x25, 0x3b, 0xbf, 0x93,
    0x09, 0xde, 0x58, 0x19,
    0xcb, 0x42, 0xe8, 0x19,
    0x56, 0xf4, 0xc9, 0x9b,
    0xc9, 0x76, 0x5c, 0xaf,
    0x53, 0xb1, 0xd0, 0xbb,
    0x82, 0x79, 0x82, 0x6a,
    0xdb, 0xbc, 0x55, 0x22,
    0xe9, 0x15, 0xc1, 0x20,
    0xa6, 0x18, 0xa5, 0xa7,
    0xf5, 0xe8, 0x97, 0x08,
    0x93, 0x39, 0x65, 0x0f
};

static const unsigned char f8_iv_5[] =
{
    0x42, 0xd8, 0xa5, 0x4b,
    0x0c, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};


static const unsigned char f8_key_5[] =
{
    0x5d, 0xb9, 0x83, 0x28,
    0x25, 0x3a, 0xe2, 0x22,
    0x5e, 0x50, 0xf7, 0xf7,
    0xf4, 0x06, 0x54, 0x78,
};

static const unsigned char f8_in_5[] =
{
    0xb3
};

static const unsigned char f8_expout_5[] =
{
    0x01
};


static f8_testset f8_testdata[] =
{
    {
        f8_iv_0, f8_key_0, f8_in_0, f8_expout_0,
        16,      16,       100
    },

    {
        f8_iv_1, f8_key_1, f8_in_1, f8_expout_1,
        16,      16,       64
    },

    {
        f8_iv_2, f8_key_2, f8_in_2, f8_expout_2,
        16,      16,       15
    },

    {
        f8_iv_3, f8_key_3, f8_in_3, f8_expout_3,
        16,      16,       32
    },

    {
        f8_iv_4, f8_key_4, f8_in_4, f8_expout_4,
        16,      16,       100
    },

    {
        f8_iv_5, f8_key_5, f8_in_5, f8_expout_5,
        16,      16,       1
    },
};

static unsigned char encryptBuf[104];


int testKEAf8(int fd)
{
    KEA_CRYPT_REQ keaRq;
    int           i, status, testfail;

    testfail = 0;
    for (i = 0; i < 6; i++)
    {
        printf("\n*** Test KEA f8 case %d***\n", i);
    
        memset(encryptBuf, 0, 104);
        memset(&keaRq, 0, sizeof(keaRq));


        /* encrypt cycle */
        keaRq.opId     = DPD_KEA_f8_CIPHER_INIT;
        keaRq.ivBytes  = f8_testdata[i].iv_len;
        keaRq.ivData   = (unsigned char *)f8_testdata[i].f8_iv;
        keaRq.keyBytes = f8_testdata[i].key_len;
        keaRq.keyData  = (unsigned char *)f8_testdata[i].f8_key;
        keaRq.inBytes  = f8_testdata[i].data_len;
        keaRq.inData   = (unsigned char *)f8_testdata[i].f8_in;
        keaRq.outBytes = f8_testdata[i].data_len;
        keaRq.outData  = encryptBuf;


        status = putKmem(fd, (void *)f8_testdata[i].f8_iv, (void **)&keaRq.ivData, keaRq.ivBytes);
        if (status)
            return status;

        status = putKmem(fd, (void *)f8_testdata[i].f8_key, (void **)&keaRq.keyData, keaRq.keyBytes);
        if (status) {
            freeKmem(fd, (void **)&keaRq.ivData);
            return status;
        }
    
        status = putKmem(fd, (void *)f8_testdata[i].f8_in, (void **)&keaRq.inData, keaRq.inBytes);
        if (status) {
            freeKmem(fd, (void **)&keaRq.ivData);
            freeKmem(fd, (void **)&keaRq.keyData);
            return status;
        }
    
        status = putKmem(fd, NULL, (void **)&keaRq.outData, keaRq.outBytes);
        if (status) {
            freeKmem(fd, (void **)&keaRq.ivData);
            freeKmem(fd, (void **)&keaRq.keyData);
            freeKmem(fd, (void **)&keaRq.inData);
            return status;
        }

    
        armCompletion(&keaRq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&keaRq);

        if ((status = waitCompletion("testKEA(): f8 test", status, &keaRq)))
        {
            freeKmem(fd, (void **)&keaRq.ivData);
            freeKmem(fd, (void **)&keaRq.keyData);
            freeKmem(fd, (void **)&keaRq.inData);
            freeKmem(fd, (void **)&keaRq.outData);
            return status;
        }
    
        getKmem(fd, encryptBuf, (void **)&keaRq.outData, keaRq.inBytes);

        freeKmem(fd, (void **)&keaRq.ivData);
        freeKmem(fd, (void **)&keaRq.keyData);
        freeKmem(fd, (void **)&keaRq.inData);
        freeKmem(fd, (void **)&keaRq.outData);



        /* run results comparison */
        if ((memcmp(f8_testdata[i].f8_expout, encryptBuf, keaRq.outBytes)) == 0)
            printf("*** Test KEA f8 case %d Passed ***\n", i);
        else
        {
            printf("*** Test KEA f8 case %d Failed ***\n", i);
            printf("expected result:\n");
            dumpm((unsigned char *)f8_testdata[i].f8_expout, keaRq.outBytes);
            printf("actual result:\n");
            dumpm(encryptBuf, keaRq.outBytes);
            testfail++;
        }

    }

    if (testfail)
        return -1;

    return status;
}






typedef struct _f9_testset
{
    const unsigned char *f9_iv;
    const unsigned char *f9_key;
    const unsigned char *f9_in;
    const unsigned char *f9_expout;
    unsigned long        iv_len;
    unsigned long        key_len;
    unsigned long        data_len;
    unsigned long        digest_len;
} f9_testset;


static const unsigned char f9_iv_0[] =
{
    0x14, 0x79, 0x3e, 0x41,
    0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x03, 0x97, 0xe8, 0xfd,
    0x00, 0x00, 0x00, 0x00,
};

static const unsigned char f9_key_0[] =
{
    0xc7, 0x36, 0xc6, 0xaa,
    0xb2, 0x2b, 0xff, 0xf9, 
    0x1e, 0x26, 0x98, 0xd2,
    0xe2, 0x2a, 0xd5, 0x7e, 
};

static const unsigned char f9_in_0[] =
{
    0xd0, 0xa7, 0xd4, 0x63,
    0xdf, 0x9f, 0xb2, 0xb2,
    0x78, 0x83, 0x3f, 0xa0,
    0x2e, 0x23, 0x5a, 0xa1,
    0x72, 0xbd, 0x97, 0x0c,
    0x14, 0x73, 0xe1, 0x29,
    0x07, 0xfb, 0x64, 0x8b,
    0x65, 0x99, 0xaa, 0xa0,
    0xb2, 0x4a, 0x03, 0x86,
    0x65, 0x42, 0x2b, 0x20,
    0xa4, 0x99, 0x27, 0x6a,
    0x50, 0x42, 0x70, 0x09,
};

static const unsigned char f9_expout_0[] =
{
    0xdd, 0x7d, 0xfa, 0xdd,
    0xd6, 0x8d, 0x1e, 0xc1,
};




static const unsigned char f9_iv_1[] =
{
    0xD8, 0x13, 0x8C, 0x3A,
    0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0xCA, 0x23, 0xA5, 0x25,
    0x00, 0x00, 0x00, 0x00,
};


static const unsigned char f9_key_1[] =
{
    0xCE, 0x0F, 0x61, 0x8E,
    0x38, 0xD8, 0x05, 0xD6,
    0xCE, 0x56, 0x6B, 0xB7,
    0x4B, 0x7E, 0xB8, 0x97,
};


static const unsigned char f9_in_1[] =
{
    0x38, 0xEE, 0xC5, 0x6F,
    0x7B, 0x96, 0xC4, 0x10,
    0x57, 0xA3, 0xC2, 0x44,
    0x91, 0xD6, 0xF6, 0x2B,
    0x04, 0xA7, 0x6D, 0x84,
    0x8F, 0x29, 0x13, 0xAF,
    0x5C, 0x69, 0x64, 0x22,
    0x45, 0x89, 0x08, 0x9F,
    0x4D, 0xCC, 0xB2, 0xF2,
    0xBC, 0xFA, 0xA5, 0xE2,
    0x2E, 0x4E, 0xA7, 0xB4,
    0x7A, 0xC8, 0x5C, 0x98,
    0x3A, 0xB9, 0x44, 0x13,
    0x40, 0xB0, 0x38, 0xFB,
    0x8B, 0xED, 0x2A, 0xC9,
    0x7F, 0x4C, 0xED, 0x51,
    0x57, 0xAC, 0x8D, 0xE6,
    0x5E, 0xF9, 0xF5, 0x64,
    0x88, 0x64, 0x69, 0x6F,
    0x55, 0xF3, 0x8D, 0x53,
    0x79, 0x46, 0x67, 0x94,
    0x80, 0x72, 0xC0, 0x35,
    0x3A, 0x5B, 0x0B, 0x75,
    0x54, 0x22, 0xA1, 0xA2,
    0xB3, 0x44, 0x88, 0xA1,
    0x5D, 0x2F, 0x17, 0x13,
    0x79, 0xAA, 0x6C, 0x6F,
    0x87, 0x54, 0x82, 0x75,
    0x76, 0xF6, 0x25, 0x69,
    0x0E, 0x48, 0x48, 0x71,
    0x0F, 0x0A, 0x62, 0x0D,
    0x2D, 0x40, 0x8C, 0x2D,
    0xC9, 0x2D, 0x8A, 0x6D,
    0x7E, 0x5F, 0x54, 0x99,
    0x27, 0xCB, 0xE2, 0x89,
    0xE4, 0x27, 0xDF, 0x1B,
    0x09, 0xA8, 0x7D, 0x73,
    0x6E, 0x48, 0x43, 0x39,
    0x38, 0x84, 0xF8, 0x44,
    0x66, 0x4C, 0x18, 0x5C,
    0x90, 0xAB, 0x44, 0x19,
    0x9F, 0xF1, 0xB8, 0x1C,
    0xAA, 0x34, 0x5D, 0xC4,
    0x1E, 0xD6, 0xD2, 0xF7,
    0xCD, 0xFF, 0x44, 0xF7,
    0x6A, 0xB0, 0x6E, 0x30,
    0x97, 0xC4, 0x3F, 0xEB,
    0xC7, 0x99, 0x53, 0xBA,
    0x76, 0x10, 0x17, 0x91,
    0x8D, 0x7E, 0xD5, 0x98,
    0x4E, 0x84, 0xB4, 0xFC,
    0x2D, 0x28, 0xB9, 0xFC,
    0xF2, 0x23, 0x65, 0x23,
    0x7B, 0x3D, 0x71, 0x87,
    0x0E, 0x1B, 0xC9, 0x8C,
    0x58, 0x85, 0x33, 0x0D,
    0x2F, 0x9B, 0xAD, 0x50,
    0x0D, 0x24, 0x56, 0xC6,
    0x9B, 0x81, 0x42, 0x5F,
    0x3A, 0x44, 0xB2, 0x92,
    0x2B, 0x96, 0x7B, 0x70,
    0xF7, 0x0C, 0xC9, 0xDD,
    0x9B, 0xE2, 0xA3, 0x23,
    0xD7, 0x88, 0x81, 0x44,
    0x68, 0xA3, 0x48, 0xF9,
    0xDE, 0x13, 0x8F, 0x78,
    0x8E, 0x40, 0x9F, 0x1A,
    0x91, 0xDC, 0x61, 0x7C,
    0xDE, 0xCD, 0x62, 0x37,
    0x30, 0x86, 0x10, 0x1C,
    0xB5, 0x77, 0xB8, 0x41,
    0x7D, 0x77, 0xB4, 0x33,
    0x0A, 0x60, 0x42, 0x9C,
    0x74, 0x50, 0xC6, 0xA2,
    0xD8, 0x7F, 0xA9, 0x4E,
    0x1D, 0xAC, 0x24, 0x96,
    0x11, 0x6E, 0x75, 0xD0,
    0xA6, 0x6C, 0x66, 0xFE,
    0xCD, 0xEB, 0xB5, 0x6E,
    0xCE, 0x37, 0x33, 0xBE,
    0x34, 0xDA, 0x82, 0xE7,
    0x87, 0x7C, 0x39, 0x07,
    0x8F, 0x0A, 0x67, 0xA2,
    0x86, 0x73, 0x4F, 0x9D,
    0xA0, 0x40, 0xBE, 0x0D,
    0xF2, 0x59, 0xEC, 0x95,
    0xAD, 0x52, 0x30, 0x57,
    0xBB, 0x00, 0xEC, 0xC0,
    0x49, 0x8C, 0x8D, 0xB4,
    0x14, 0x53, 0x60, 0x4A,
    0xF8, 0x47, 0x52, 0x60,
    0x5A, 0x00, 0x0E, 0x0B,
    0xC3, 0x52, 0x3C, 0xE2,
    0xF0, 0x9A, 0xD2, 0xF7,
    0x3A, 0x7C, 0xF7, 0xE2,
    0x32, 0xDF, 0xDD, 0xB6,
    0x13, 0x1D, 0x47, 0xB1,
    0x91, 0xC7, 0x5E, 0xC6,
    0xDF, 0x9A, 0xB9, 0x42,
    0x09, 0x54, 0xB3, 0x34,
    0x1A, 0x20, 0x36, 0x6B,
    0x42, 0xC4, 0xD1, 0x7C,
    0xE7, 0x6E, 0x82, 0x66,
    0xAE, 0xD8, 0x4C, 0x81,
    0xB5, 0xD9, 0x02, 0x98,
    0x7F, 0x5A, 0xE7, 0xE7,
    0x95, 0x34, 0x61, 0xFB,
    0xFC, 0x36, 0xC3, 0x4B,
    0xEA, 0xC7, 0xCA, 0xB4,
    0x6D, 0x45, 0xFD, 0xD4,
    0xDC, 0xBE, 0x24, 0xD9,
    0xFD, 0x0C, 0xE2, 0x5B,
    0x08, 0x83, 0xA9, 0x4D,
    0xFB, 0x6D, 0x59, 0x29,
    0x26, 0xBC, 0x99, 0x9F,
    0x6A, 0x0A, 0x5D, 0x89,
    0xA7, 0x8E, 0x4C, 0x9C,
    0x2B, 0xB2, 0xE6, 0xE0,
    0x18, 0x68, 0xA5, 0xA0,
    0xB7, 0xE2, 0x46, 0x70,
    0x07, 0x43, 0xA2, 0x61,
    0xED, 0xD3, 0x1F, 0x55,
    0xA9, 0xE3, 0x8B, 0x9F,
    0xC3, 0xFE, 0x5D, 0x84,
    0x52, 0x33, 0xEC, 0x19,
    0x3E, 0x40, 0x64, 0x4A,
    0x3A, 0xD4, 0x01, 0xE1,
    0x70, 0x3E, 0x9E, 0x7B,
    0xFF, 0x52, 0x25, 0x33,
    0x15, 0x5D, 0x9C, 0xEF,
    0x93, 0xF5, 0x25, 0xF8,
    0x31, 0xE0, 0x0C, 0x44,
    0xC0, 0xCD, 0x38, 0x3C,
    0xF6, 0x2E, 0xD1, 0x97,
    0xCC, 0x1E, 0xA7, 0x30,
    0x70, 0xB5, 0xC8, 0x67,
    0xE8, 0x15, 0xB5, 0x6E,
    0x8A, 0x62, 0x29, 0xE8,
    0x5E, 0xCE, 0xBF, 0x60,
    0x56, 0x78, 0x67, 0x10,
    0xBD, 0x90, 0x9E, 0x90,
    0xB1, 0x7D, 0x6A, 0x7D,
    0xC7, 0x47, 0xBE, 0x66,
    0xA2, 0x54, 0x8E, 0xC2,
    0xCB, 0x58, 0x5D, 0x93,
    0x74, 0xDC, 0xD9, 0x73,
    0xA1, 0xA4, 0x10, 0xC6,
    0x4B, 0x7D, 0x5D, 0xA9,
    0x2B, 0xE6, 0xF8, 0x26,
    0xE7, 0x68, 0xE3, 0x97,
    0x83, 0xB3, 0x9C, 0x37,
    0x96, 0xBB, 0x07, 0x0A,
    0x33, 0x25, 0xF4, 0xBC,
    0x34, 0x93, 0xCB, 0x6F,
    0x81, 0x4D, 0x3A, 0x71,
    0x22, 0xE5, 0x7D, 0xC1,
    0x11, 0xAF, 0xE1, 0x9A,
    0xA3, 0x98, 0x71, 0xF4,
    0x43, 0x65, 0xB6, 0x00,
    0x22, 0xCB, 0x58, 0xD5,
    0xDB, 0x4A, 0x40, 0xE3,
    0x53, 0x80, 0xE9, 0x76,
    0x81, 0xA8, 0x9A, 0xEB,
    0x2F, 0x93, 0x7B, 0xE1,
    0x9C, 0x1E, 0xED, 0x7D,
    0xD2, 0xAB, 0x11, 0x74,
    0x01, 0xA4, 0xEF, 0xA7,
    0x0F, 0xBA, 0xE5, 0xA5,
    0x06, 0x94, 0x21, 0x77,
    0x33, 0x06, 0x75, 0x0C,
    0xDE, 0x98, 0xD7, 0x34,
    0xDD, 0x8F, 0x11, 0x62,
    0xCB, 0x61, 0x95, 0x0A,
    0xFB, 0x15, 0xCF, 0xBF,
    0x8D, 0x8B, 0x5E, 0xEF,
    0xA5, 0x26, 0x52, 0xEC,
    0xA7, 0x2D, 0x7F, 0x1F,
    0x93, 0x18, 0x68, 0x1A,
    0x30, 0x65, 0xCB, 0xCA,
    0xE8, 0x78, 0x84, 0x9C,
    0x66, 0xC9, 0x25, 0x2C,
    0xB5, 0x6F, 0x57, 0x0B,
    0x63, 0x9B, 0x65, 0x50,
    0x5A, 0x85, 0x19, 0x56,
    0x1F, 0x03, 0xBC, 0x85,
    0xD0, 0x28, 0x6D, 0x33,
    0xA5, 0xC0, 0xBE, 0xF2,
    0x74, 0x4A, 0xE9, 0x3E,
    0x9F, 0x76, 0xA2, 0x5D,
    0x41, 0x5F, 0x3D, 0xD8,
    0xD3, 0xC3, 0x5C, 0x57,
    0xD9, 0xBF, 0x2F, 0xC3,
    0x70, 0xD9, 0x6A, 0x32,
    0xFB, 0x52, 0x79, 0x74,
    0x97, 0xBB, 0x96, 0x67,
    0x9C, 0x45, 0x62, 0x4B,
    0x98, 0x6D, 0xD0, 0x41,
    0x8E, 0xB0, 0xC8, 0x30,
    0x2C, 0xE6, 0x99, 0xEF,
    0xCC, 0xF5, 0x18, 0xBD,
    0x90, 0x08, 0x53, 0x62,
    0x0E, 0x34, 0xE8, 0x75,
    0x5B, 0x33, 0x15, 0x1C,
    0x5A, 0x5C, 0x1D, 0x21,
    0x16, 0x96, 0x1E, 0x6E,
    0xEA, 0x1F, 0xA7, 0xAC,
    0x00, 0x0D, 0xBC, 0xCE,
    0xA5, 0x51, 0x52, 0xD5,
    0x35, 0x4C, 0x27, 0x84,
    0x16, 0xDF, 0x22, 0xF1,
    0xB6, 0xD5, 0x26, 0x12,
    0x48, 0x21, 0x75, 0x8C,
    0xA2, 0x8D, 0x08, 0xDC,
    0xBB, 0xE0, 0x81, 0x1B,
    0x86, 0x58, 0x8F, 0x66,
    0x35, 0xA8, 0x8F, 0xD4,
    0x9F, 0xA4, 0x81, 0x6C,
    0x16, 0xFD, 0x3F, 0x7D,
    0xA4, 0x5D, 0xC4, 0x94,
    0x7F, 0x1F, 0xFE, 0x32,
    0x83, 0xF4, 0x88, 0x89,
    0xFB, 0xDF, 0xEE, 0x7C,
    0xC1, 0xD3, 0x4D, 0xB7,
    0x79, 0x75, 0x4A, 0xB5,
    0x8E, 0xF0, 0x19, 0x56,
    0xF9, 0x65, 0x42, 0x09,
    0xDE, 0x11, 0xB8, 0x83,
    0x33, 0x70, 0x19, 0xC5,
    0x45, 0x94, 0xFE, 0x84,
    0xF7, 0x8D, 0x63, 0xB2,
    0x81, 0x12, 0x78, 0x9B,
    0xAC, 0x0B, 0x5B, 0x02,
    0x24, 0x95, 0xB0, 0x01,
    0x36, 0x08, 0x59, 0xCE,
    0x8B, 0x7E, 0xFE, 0x6B,
    0xAB, 0x08, 0xFD, 0x61,
    0x5F, 0xA1, 0xDE, 0x01,
    0xA7, 0x3D, 0x26, 0x74,
    0x71, 0x4C, 0x57, 0xDF,
    0x0C, 0x2D, 0x8C, 0x8F,
    0x51, 0x91, 0x7B, 0xEE,
    0xA1, 0x44, 0x3D, 0xF7,
    0xB7, 0x35, 0x0C, 0x98,
    0xFE, 0xFA, 0x60, 0x46,
    0x1F, 0xE2, 0x5F, 0xCE,
    0xBE, 0x25, 0x8E, 0x18,
    0x19, 0x52, 0x69, 0x85,
    0x23, 0x1A, 0xAF, 0x0F,
    0x4D, 0xBC, 0x49, 0x7C,
    0x58, 0x11, 0x27, 0xFC,
    0x7F, 0xA2, 0xCB, 0x2F,
    0x7E, 0xC8, 0xD7, 0x11,
    0xEB, 0xE2, 0xCB, 0x85,
    0x61, 0x95, 0xEA, 0xED,
    0x9C, 0xB1, 0x65, 0x69,
    0x13, 0xA6, 0xB6, 0x73,
    0x8F, 0xEF, 0x46, 0x65,
    0xE0, 0xAF, 0x2F, 0xDA,
    0x98, 0xA1, 0x46, 0x2F,
    0x84, 0x90, 0xEB, 0xEF,
    0xE2, 0x5B, 0x4C, 0x81,
    0x2C, 0xAD, 0xF4, 0xF9,
    0x50, 0xDE, 0xE6, 0x98,
    0x17, 0xA6, 0x5F, 0x99,
    0x7C, 0x80, 0xC4, 0x2F,
    0xBA, 0xAC, 0xB7, 0x2C,
    0x83, 0x00, 0x1E, 0xC4,
    0xB4, 0xB4, 0xBC, 0xB9,
    0x04, 0xD4, 0xD7, 0xB0,
    0xDF, 0x20, 0x2B, 0x93,
    0xEF, 0x43, 0x8B, 0xAD,
    0xEB, 0xBC, 0xDB, 0x02,
    0x57, 0xA2, 0x9B, 0xB2,
    0xF3, 0x0A, 0x1B, 0x9D,
    0x76, 0x27, 0x90, 0x27,
    0x98, 0x80, 0x01, 0x21,
    0xEE, 0xBF, 0xB7, 0x48,
    0x3E, 0x90, 0x58, 0xC4,
    0x80, 0x03, 0x9C, 0x92,
    0x4E, 0xDC, 0xCA, 0xAA,
    0x68, 0x86, 0x3A, 0xB1,
    0x75, 0x4D, 0x7F, 0x60,
    0xB0, 0xF3, 0xD0, 0xF3,
    0x54, 0x57, 0x64, 0xEA,
    0xFC, 0x71, 0xB6, 0x25,
    0x34, 0x9A, 0xAB, 0x19,
    0x5A, 0xED, 0x90, 0x71,
    0xFB, 0x1F, 0x8E, 0x77,
    0xB5, 0xA2, 0x8C, 0x77,
    0xE0, 0xB4, 0x7F, 0xC4,
    0x33, 0x1A, 0x98, 0xA8,
    0xCD, 0xD6, 0x29, 0x28,
    0xD7, 0x0B, 0x24, 0xFA,
    0x20, 0x79, 0xA2, 0xAC,
    0x72, 0x7A, 0xD6, 0x33,
    0x1C, 0x54, 0x7E, 0x18,
    0x6F, 0xA0, 0xD2, 0xDB,
    0x5D, 0xC8, 0x06, 0x41,
    0xD4, 0xEB, 0x39, 0xA7,
    0xA1, 0x98, 0x6E, 0xF2,
    0x73, 0x75, 0xD0, 0x5E,
    0x79, 0x49, 0x0B, 0x9F,
    0x8C, 0x0A, 0x4A, 0x57,
    0xF9, 0xB8, 0x13, 0x3A,
    0x03, 0x78, 0xC2, 0xCF,
    0xD5, 0x27, 0x9C, 0x5E,
    0xDE, 0xBA, 0x54, 0x20,
    0x6D, 0xD9, 0xB1, 0x4A,
    0x43, 0x01, 0x59, 0xDA,
    0x44, 0x95, 0xD9, 0x40,
    0x83, 0x93, 0x75, 0x62,
    0x9D, 0xDD, 0x93, 0xB1,
    0xA4, 0x77, 0xD8, 0x73,
    0x7C, 0x0B, 0xFC, 0xDE,
    0x05, 0x46, 0xB4, 0xF7,
    0x98, 0x6D, 0x53, 0x85,
    0x20, 0x41, 0x65, 0x52,
    0x14, 0x0E, 0xCC, 0x94,
    0xAA, 0xEA, 0x20, 0x49,
    0xC4, 0x8D, 0x81, 0x76,
    0x7B, 0x84, 0x1A, 0x67,
    0xBC, 0x60, 0x72, 0xDA,
    0x75, 0xF1, 0xCC, 0x44,
    0x11, 0x46, 0x57, 0x50,
    0x59, 0x8C, 0xF3, 0x43,
    0x59, 0xD6, 0x3C, 0xAA,
    0xBE, 0xBF, 0x8C, 0x03,
    0xEC, 0xB9, 0xC5, 0x17,
    0x01, 0xEC, 0xF4, 0x5A,
    0xA9, 0x00, 0x33, 0x77,
    0x7E, 0x4D, 0x3C, 0x54,
    0x94, 0xE1, 0x91, 0x7B,
    0x89, 0xB0, 0xCC, 0x37,
    0xD2, 0xFB, 0xCA, 0x2D,
    0xB8, 0x15, 0x35, 0xA8,
    0xDA, 0x85, 0xF3, 0x3A,
    0x44, 0xC1, 0x47, 0x6F,
    0xF8, 0x52, 0x77, 0x48,
    0x96, 0x5C, 0x54, 0xF5,
    0xCD, 0x09, 0x44, 0x46,
    0x65, 0x00, 0x2E, 0xE1,
    0xA6, 0xBA, 0xA7, 0x2C,
    0x1B, 0x9D, 0x99, 0xA4,
    0x00, 0xEA, 0xCA, 0x8A,
    0x88, 0x7E, 0x16, 0xF7,
    0xA6, 0x90, 0xE7, 0x0A,
    0xF2, 0x02, 0xA3, 0x56,
    0x3C, 0xF9, 0x23, 0xE9,
    0x3B, 0x5C, 0x24, 0x6F,
    0x50, 0x87, 0xE8, 0x96,
    0xA1, 0xB2, 0x94, 0x26,
    0x22, 0x3D, 0xA4, 0xE2,
    0xC7, 0xDB, 0xA2, 0x21,
    0x69, 0x6B, 0x0E, 0xB3,
    0x2E, 0xC0, 0xD3, 0xF3,
    0x95, 0x84, 0xF1, 0x35,
    0xE5, 0xB5, 0x6E, 0x54,
    0x31, 0x42, 0x93, 0x7B,
    0xB9, 0xDB, 0xD7, 0xDD,
    0x6E, 0xEE, 0x0B, 0xDC,
    0x54, 0xAE, 0x0C, 0xD6,
    0x0F, 0x79, 0x05, 0x72,
    0xCD, 0x4D, 0x3C, 0x59,
    0xC4, 0xC5, 0x0A, 0xE3,
    0x02, 0xC2, 0x36, 0xC0,
    0xC0, 0x66, 0xF2, 0x4D,
    0x93, 0xF6, 0x14, 0xC1,
    0xA6, 0x2F, 0x35, 0xD8,
    0x99, 0xA2, 0xAB, 0x34,
    0x48, 0xA6, 0x03, 0x73,
    0xEA, 0xCB, 0xEB, 0x39,
    0x77, 0x00, 0x00, 0x00,
};

static const unsigned char f9_expout_1[] =
{
    0x14, 0x3F, 0xD7, 0xD2,
    0xBB, 0xA8, 0xC8, 0xAF,
};


/* case 9 */
static const unsigned char f9_iv_2[] =
{
    0x29, 0x6f, 0x39, 0x3c,
    0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x6b, 0x22, 0x77, 0x37,
    0x00, 0x00, 0x00, 0x00,
};

static const unsigned char f9_key_2[] =
{
    0xf4, 0xeb, 0xec, 0x69,
    0xe7, 0x3e, 0xaf, 0x2e,
    0xb2, 0xcf, 0x6a, 0xf4,
    0xb3, 0x12, 0x0f, 0xfd,
};

/* 125 bytes */
static const unsigned char f9_in_2[] =
{
    0x10, 0xbf, 0xff, 0x83,
    0x9e, 0x0c, 0x71, 0x65,
    0x8d, 0xbb, 0x2d, 0x17,
    0x07, 0xe1, 0x45, 0x72,
    0x4f, 0x41, 0xc1, 0x6f,
    0x48, 0xbf, 0x40, 0x3c,
    0x3b, 0x18, 0xe3, 0x8f,
    0xd5, 0xd1, 0x66, 0x3b,
    0x6f, 0x6d, 0x90, 0x01,
    0x93, 0xe3, 0xce, 0xa8,
    0xbb, 0x4f, 0x1b, 0x4f,
    0x5b, 0xe8, 0x22, 0x03,
    0x22, 0x32, 0xa7, 0x8d,
    0x7d, 0x75, 0x23, 0x8d,
    0x5e, 0x6d, 0xae, 0xcd,
    0x3b, 0x43, 0x22, 0xcf,
    0x59, 0xbc, 0x7e, 0xa8,
    0x4a, 0xb1, 0x88, 0x11,
    0xb5, 0xbf, 0xb7, 0xbc,
    0x55, 0x3f, 0x4f, 0xe4,
    0x44, 0x78, 0xce, 0x28,
    0x7a, 0x14, 0x87, 0x99,
    0x90, 0xd1, 0x8d, 0x12,
    0xca, 0x79, 0xd2, 0xc8,
    0x55, 0x14, 0x90, 0x21,
    0xcd, 0x5c, 0xe8, 0xca,
    0x03, 0x71, 0xca, 0x04,
    0xfc, 0xce, 0x14, 0x3e,
    0x3d, 0x7c, 0xfe, 0xe9,
    0x45, 0x85, 0xb5, 0x88,
    0x5c, 0xac, 0x46, 0x06,
    0x8b
};

static const unsigned char f9_expout_2[] =
{
    0xc3, 0x83, 0x83, 0x9d,
    0x93, 0xff, 0xc6, 0xd1,
};


/* case 10 */
static const unsigned char f9_iv_3[] =
{
    0x3e, 0xdc, 0x87, 0xe2,
    0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0xa4, 0xf2, 0xd8, 0xe2,
    0x00, 0x00, 0x00, 0x00,
};

static const unsigned char f9_key_3[] =
{
    0xd4, 0x2f, 0x68, 0x24,
    0x28, 0x20, 0x1c, 0xaf,
    0xcd, 0x9f, 0x97, 0x94,
    0x5e, 0x6d, 0xe7, 0xb7,
};

/* 16 bytes */
static const unsigned char f9_in_3[] =
{
    0xb5, 0x92, 0x43, 0x84,
    0x32, 0x8a, 0x4a, 0xe0,
    0x0b, 0x73, 0x71, 0x09,
    0xf8, 0xb6, 0xc8, 0xdd,
};

static const unsigned char f9_expout_3[] =
{
    0xf5, 0x3d, 0x0f, 0x66,
    0x9e, 0xf0, 0x4d, 0x59,
};


/* case 11 */
static const unsigned char f9_iv_4[] =
{
    0xbb, 0xa5, 0x18, 0x81,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x95, 0x7c, 0x43, 0x91,
    0x00, 0x00, 0x00, 0x00,
};

static const unsigned char f9_key_4[] =
{
    0xde, 0xa8, 0x8b, 0xba,
    0x37, 0x40, 0x1b, 0xa3,
    0xe3, 0xc4, 0x8e, 0xeb,
    0xae, 0x82, 0xc2, 0x8e,
};

/* 8 bytes */
static const unsigned char f9_in_4[] =
{
    0x70, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

static const unsigned char f9_expout_4[] =
{
    0x7b, 0xe3, 0xf1, 0x46,
    0xe1, 0x9d, 0xbe, 0x9a,
};


/* case 18 */
static const unsigned char f9_iv_5[] =
{
    0x22, 0x35, 0xcd, 0x14,
    0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x55, 0x2e, 0xfe, 0x16,
    0x00, 0x00, 0x00, 0x00,	
};

static const unsigned char f9_key_5[] =
{
    0xaa, 0x20, 0x11, 0xfe,
    0x9c, 0x8a, 0x63, 0x6e,
    0x31, 0xb7, 0x12, 0x23,
    0x2f, 0xf3, 0xbc, 0x94,
};

/* 1 bytes */
static const unsigned char f9_in_5[] =
{
    0xf3
};

static const unsigned char f9_expout_5[] =
{
    0x99, 0xf3, 0xa3, 0xcf,
    0x01, 0x96, 0x36, 0x36,
};


static f9_testset f9_testdata[] =
{
    {
        f9_iv_0, f9_key_0, f9_in_0, f9_expout_0,
        24,      16,       48,      8
    },

    {
        f9_iv_1, f9_key_1, f9_in_1, f9_expout_1,
        24,      16,       1485,    8
    },

    {
        f9_iv_2, f9_key_2, f9_in_2, f9_expout_2,
        24,      16,       125 ,    8
    },

    {
        f9_iv_3, f9_key_3, f9_in_3, f9_expout_3,
        24,      16,       16,      8
    },

    {
        f9_iv_4, f9_key_4, f9_in_4, f9_expout_4,
        24,      16,       8,       8
    },

    {
        f9_iv_5, f9_key_5, f9_in_5, f9_expout_5,
        24,      16,       1,       8
    },
};

static unsigned char ctxBuf[8];
static unsigned char ivOut[24];

int testKEAf9(int fd)
{
    KEA_CRYPT_REQ keaRq;
    int           i, status, testfail;

    testfail = 0;
    for (i = 0; i < 6; i++)
    {
        printf("\n*** Test KEA f9 case %d ***\n", i);
    
        memset(ctxBuf, 0, 8);
        memset(&keaRq, 0, sizeof(keaRq));


        /* encrypt cycle */
        keaRq.opId        = DPD_KEA_f9_CIPHER_INIT_FINAL;
        keaRq.ivBytes     = f9_testdata[i].iv_len;
        keaRq.ivData      = (unsigned char *)f9_testdata[i].f9_iv;
        keaRq.keyBytes    = f9_testdata[i].key_len;
        keaRq.keyData     = (unsigned char *)f9_testdata[i].f9_key;
        keaRq.inBytes     = f9_testdata[i].data_len;
        keaRq.inData      = (unsigned char *)f9_testdata[i].f9_in;
        keaRq.ctxBytes    = f9_testdata[i].digest_len;
        keaRq.ctxData     = ctxBuf;

        keaRq.outIvBytes  = f9_testdata[i].iv_len;
        keaRq.outIvData   = ivOut;

        status = putKmem(fd, (void *)f9_testdata[i].f9_iv, (void **)&keaRq.ivData, keaRq.ivBytes);
        if (status)
            return status;

        status = putKmem(fd, (void *)f9_testdata[i].f9_key, (void **)&keaRq.keyData, keaRq.keyBytes);
        if (status) {
            freeKmem(fd, (void **)&keaRq.ivData);
            return status;
        }
    
        status = putKmem(fd, (void *)f9_testdata[i].f9_in, (void **)&keaRq.inData, keaRq.inBytes);
        if (status) {
            freeKmem(fd, (void **)&keaRq.ivData);
            freeKmem(fd, (void **)&keaRq.keyData);
            return status;
        }

        status = putKmem(fd, NULL, (void **)&keaRq.ctxData, keaRq.ctxBytes);
        if (status) {
            freeKmem(fd, (void **)&keaRq.ivData);
            freeKmem(fd, (void **)&keaRq.keyData);
            freeKmem(fd, (void **)&keaRq.inData);
            return status;
        }

        status = putKmem(fd, NULL, (void **)&keaRq.outIvData, keaRq.outIvBytes);
        if (status) {
            freeKmem(fd, (void **)&keaRq.ivData);
            freeKmem(fd, (void **)&keaRq.keyData);
            freeKmem(fd, (void **)&keaRq.inData);
            freeKmem(fd, (void **)&keaRq.ctxData);
            return status;
        }
    
        armCompletion(&keaRq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&keaRq);

        if ((status = waitCompletion("testKEA(): f9 test", status, &keaRq)))
        {
            freeKmem(fd, (void **)&keaRq.ivData);
            freeKmem(fd, (void **)&keaRq.keyData);
            freeKmem(fd, (void **)&keaRq.inData);
            freeKmem(fd, (void **)&keaRq.ctxData);
            freeKmem(fd, (void **)&keaRq.outIvData);
            return status;
        }
    
        getKmem(fd, ctxBuf, (void **)&keaRq.ctxData, keaRq.ctxBytes);
        getKmem(fd, ivOut, (void **)&keaRq.outIvData, keaRq.outIvBytes);
    
        freeKmem(fd, (void **)&keaRq.ivData);
        freeKmem(fd, (void **)&keaRq.keyData);
        freeKmem(fd, (void **)&keaRq.inData);
        freeKmem(fd, (void **)&keaRq.ctxData);
        freeKmem(fd, (void **)&keaRq.outIvData);

        /* run results comparison */
        if ((memcmp(f9_testdata[i].f9_expout, ctxBuf, keaRq.ctxBytes)) == 0)       
            printf("*** Test KEA f9 case %d Passed ***\n", i);
        else
        {
            printf("*** Test KEA f9 case %d Failed ***\n", i);
            printf("expected result:\n");
            dumpm((unsigned char *)f9_testdata[i].f9_expout, keaRq.ctxBytes);
            printf("actual result:\n");
            dumpm(ctxBuf, keaRq.ctxBytes);
            testfail++;
        }
    }

    if (testfail)
        return -1;
        
    return 0;
}





int testKEA(int fd)
{
    int stat;
    
    stat = 0;

    if (testKEAf8(fd) == -1)
        stat++;

    if (testKEAf9(fd) == -1)
        stat++;

    if (stat)
        return(-1);
    else
        return(0);
}





