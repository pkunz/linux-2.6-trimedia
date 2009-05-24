
/****************************************************************************
 * testAesa.c - AES known-answer test for SEC2 device driver
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
 * 1.2   02-Feb-2005 sec - convert test data to const, fix warnings
 * 1.3   Aug 11,2005
 */



#include "sec2drvTest.h"
#include "Sec2.h"


#define AES_TESTSIZE (32)
#define AES_KEYSIZE  (16)

static const unsigned char keyData[] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87
};

static const unsigned char AesaData[] =
{"Now is the time for all good men"}; /* 32 bytes */

static unsigned char decryptBuf[AES_TESTSIZE], encryptBuf[AES_TESTSIZE];


int testAesECB(int fd)
{
    AESA_CRYPT_REQ aesaencReq;
    AESA_CRYPT_REQ aesadecReq;
    int            status;

    printf("\n*** Test AESA ECB ***\n");
    

    
    memset(decryptBuf, 0, AES_TESTSIZE);
    memset(encryptBuf, 0, AES_TESTSIZE);

    memset(&aesaencReq, 0, sizeof(aesaencReq));
    memset(&aesadecReq, 0, sizeof(aesadecReq));


    /* encrypt cycle */
    aesaencReq.opId     = DPD_AESA_ECB_ENCRYPT_CRYPT;
    aesaencReq.keyBytes = AES_KEYSIZE;
    aesaencReq.keyData  = (unsigned char *)keyData;
    aesaencReq.inBytes  = AES_TESTSIZE;
    aesaencReq.inData   = (unsigned char *)AesaData;
    aesaencReq.outData  = encryptBuf;


    status = putKmem(fd, (void *)keyData,  (void **)&aesaencReq.keyData, AES_KEYSIZE);
    if (status)
        return status;
    
    status = putKmem(fd, (void *)AesaData, (void **)&aesaencReq.inData,  AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        return status;
    }
    
    status = putKmem(fd, NULL, (void **)&aesaencReq.outData, AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inData);
        return status;
    }

    
    armCompletion(&aesaencReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesaencReq);

    if ((status = waitCompletion("testAesa(): ECB encryption test", status, &aesaencReq)))
    {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inData);
        freeKmem(fd, (void **)&aesaencReq.outData);
        return status;
    }
    
    getKmem(fd, encryptBuf, (void **)&aesaencReq.outData, AES_TESTSIZE);

    freeKmem(fd, (void **)&aesaencReq.keyData);
    freeKmem(fd, (void **)&aesaencReq.inData);
    freeKmem(fd, (void **)&aesaencReq.outData);



    /* decrypt cycle */
    aesadecReq.opId     = DPD_AESA_ECB_DECRYPT_CRYPT;
    aesadecReq.keyBytes = AES_KEYSIZE;
    aesadecReq.keyData  = (unsigned char *)keyData;
    aesadecReq.inBytes  = AES_TESTSIZE;
    aesadecReq.inData   = encryptBuf;
    aesadecReq.outData  = decryptBuf;

    status = putKmem(fd, (void *)keyData, (void **)&aesadecReq.keyData, AES_KEYSIZE);
    if (status)
        return status;
        
    status = putKmem(fd, encryptBuf, (void **)&aesadecReq.inData,  AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        return status;
    }
    
    status = putKmem(fd, NULL,       (void **)&aesadecReq.outData, AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.outData);
        return status;
    }

    armCompletion(&aesadecReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesadecReq);

    if ((status = waitCompletion("testAesa(): ECB decryption test", status, &aesadecReq)))
    {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.inData);
        freeKmem(fd, (void **)&aesadecReq.outData);
        return status;
    }
    
    getKmem(fd, decryptBuf, (void **)&aesadecReq.outData, AES_TESTSIZE);

    freeKmem(fd, (void **)&aesadecReq.keyData);
    freeKmem(fd, (void **)&aesadecReq.inData);
    freeKmem(fd, (void **)&aesadecReq.outData);


    /* run results comparison */
    if ((memcmp(AesaData, decryptBuf, AES_TESTSIZE)) == 0) {
        printf("*** Test AES ECB Passed ***\n");
        status = 0;
    } else {
        printf("*** Test AES ECB Failed ***\n");
        status = -1;
    }

    return status;
}


int testAesCBC(int fd)
{
    AESA_CRYPT_REQ aesaencReq;
    AESA_CRYPT_REQ aesadecReq;
    unsigned char  aesCtxOut[AES_KEYSIZE];
    unsigned char  iv_in[AES_KEYSIZE] = {0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,
                                         0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
    int            status;

    printf("\n*** Test AESA CBC ***\n");
     
    memset(decryptBuf, 0, AES_TESTSIZE);
    memset(encryptBuf, 0, AES_TESTSIZE);

    memset(&aesaencReq, 0, sizeof(aesaencReq));
    memset(&aesadecReq, 0, sizeof(aesadecReq));


    /* encrypt cycle */
    aesaencReq.opId            = DPD_AESA_CBC_ENCRYPT_CRYPT;
    aesaencReq.inIvBytes       = AES_KEYSIZE;
    aesaencReq.inIvData        = (unsigned char *)iv_in;
    aesaencReq.keyBytes        = AES_KEYSIZE;
    aesaencReq.keyData         = (unsigned char *)keyData;
    aesaencReq.inBytes         = AES_TESTSIZE;
    aesaencReq.inData          = (unsigned char *)AesaData;
    aesaencReq.outData         = encryptBuf;
    aesaencReq.outCtxBytes     = AES_KEYSIZE;
    aesaencReq.outCtxData      = (unsigned char *)aesCtxOut;

    status = putKmem(fd, (void *)keyData,  (void **)&aesaencReq.keyData,    AES_KEYSIZE);
    if (status)
        return status;
    
    status = putKmem(fd, iv_in,    (void **)&aesaencReq.inIvData,   AES_KEYSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        return status;
    }

    status = putKmem(fd, (void *)AesaData, (void **)&aesaencReq.inData,     AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inIvData);
        return status;
    }

    status = putKmem(fd, NULL,     (void **)&aesaencReq.outData,    AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inIvData);
        freeKmem(fd, (void **)&aesaencReq.inData);
        return status;
    }

    status = putKmem(fd, NULL,     (void **)&aesaencReq.outCtxData, AES_KEYSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inIvData);
        freeKmem(fd, (void **)&aesaencReq.inData);
        freeKmem(fd, (void **)&aesaencReq.outData);
        return status;
    }

    armCompletion(&aesaencReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesaencReq);
    
    if ((status = waitCompletion("testAesa(): CBC encryption test", status, &aesaencReq)))
    {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inIvData);
        freeKmem(fd, (void **)&aesaencReq.inData);
        freeKmem(fd, (void **)&aesaencReq.outData);
        freeKmem(fd, (void **)&aesaencReq.outCtxData);
        return status;
    }
    
    getKmem(fd, encryptBuf, (void **)&aesaencReq.outData,    AES_TESTSIZE);
    getKmem(fd, aesCtxOut,  (void **)&aesaencReq.outCtxData, AES_KEYSIZE);
    
    freeKmem(fd, (void **)&aesaencReq.keyData);
    freeKmem(fd, (void **)&aesaencReq.inIvData);
    freeKmem(fd, (void **)&aesaencReq.inData);
    freeKmem(fd, (void **)&aesaencReq.outData);
    freeKmem(fd, (void **)&aesaencReq.outCtxData);
    
    
    
    /* decrypt cycle */
    aesadecReq.opId            = DPD_AESA_CBC_DECRYPT_CRYPT;
    aesadecReq.inIvBytes       = AES_KEYSIZE;
    aesadecReq.inIvData        = (unsigned char *)iv_in;
    aesadecReq.keyBytes        = AES_KEYSIZE;
    aesadecReq.keyData         = (unsigned char *)keyData;
    aesadecReq.inBytes         = AES_TESTSIZE;
    aesadecReq.inData          = encryptBuf;
    aesadecReq.outData         = decryptBuf;
    aesadecReq.outCtxBytes     = AES_KEYSIZE;
    aesadecReq.outCtxData      = (unsigned char *)aesCtxOut;

    status = putKmem(fd, (void *)keyData, (void **)&aesadecReq.keyData, AES_KEYSIZE);
    if (status)
        return status;

    status = putKmem(fd, iv_in,      (void **)&aesadecReq.inIvData,   AES_KEYSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        return status;
    }

    status = putKmem(fd, encryptBuf, (void **)&aesadecReq.inData,     AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.inIvData);
        return status;
    }

    status = putKmem(fd, NULL,       (void **)&aesadecReq.outData,    AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.inIvData);
        freeKmem(fd, (void **)&aesadecReq.inData);
        return status;
    }

    status = putKmem(fd, NULL,       (void **)&aesadecReq.outCtxData, AES_KEYSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.inIvData);
        freeKmem(fd, (void **)&aesadecReq.inData);
        freeKmem(fd, (void **)&aesadecReq.outData);
        return status;
    }

    armCompletion(&aesadecReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesadecReq);
    
    if ((status = waitCompletion("testAesa(): CBC decryption test", status, &aesadecReq)))
    {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.inIvData);
        freeKmem(fd, (void **)&aesadecReq.inData);
        freeKmem(fd, (void **)&aesadecReq.outData);
        freeKmem(fd, (void **)&aesadecReq.outCtxData);
        return status;
    }
    
    getKmem(fd, decryptBuf, (void **)&aesadecReq.outData,    AES_TESTSIZE);
    getKmem(fd, aesCtxOut,  (void **)&aesadecReq.outCtxData, AES_KEYSIZE);
    
    freeKmem(fd, (void **)&aesadecReq.keyData);
    freeKmem(fd, (void **)&aesadecReq.inIvData);
    freeKmem(fd, (void **)&aesadecReq.inData);
    freeKmem(fd, (void **)&aesadecReq.outData);
    freeKmem(fd, (void **)&aesadecReq.outCtxData);
    

    /* compare results */
    if ((memcmp(AesaData, decryptBuf, AES_TESTSIZE)) == 0) {
        printf("*** Test AES CBC Mode Passed ***\n");
        status = 0;
    } else {
        printf("*** Test AES CBC Mode Failed ***\n");
        status = -1;
    }

    return status;
}



int testAesCTR(int fd)
{
    AESA_CRYPT_REQ aesaencReq;
    AESA_CRYPT_REQ aesadecReq;
    int            status;

    printf("\n*** Test AESA Counter Mode***\n");
    
    memset(decryptBuf, 0, AES_TESTSIZE);
    memset(encryptBuf, 0, AES_TESTSIZE);

    memset(&aesaencReq, 0, sizeof(aesaencReq));
    memset(&aesadecReq, 0, sizeof(aesadecReq));


    aesaencReq.opId            = DPD_AESA_CTR_CRYPT;
    aesaencReq.keyBytes        = AES_KEYSIZE;
    aesaencReq.keyData         = (unsigned char *)keyData;
    aesaencReq.inBytes         = AES_TESTSIZE;
    aesaencReq.inData          = (unsigned char *)AesaData;
    aesaencReq.outData         = encryptBuf;

    status = putKmem(fd, (void *)keyData,  (void **)&aesaencReq.keyData, AES_KEYSIZE);
    if (status)
        return status;
        
    status = putKmem(fd, (void *)AesaData, (void **)&aesaencReq.inData,  AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        return status;
    }
    
    status = putKmem(fd, NULL,     (void **)&aesaencReq.outData, AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inData);
        return status;
    }
    
    armCompletion(&aesaencReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesaencReq);

    if ((status = waitCompletion("testAesa(): counter mode encryption test", status, &aesaencReq)))
    {
        freeKmem(fd, (void **)&aesaencReq.keyData);
        freeKmem(fd, (void **)&aesaencReq.inData);
        freeKmem(fd, (void **)&aesaencReq.outData);
        return status;
    }
    
    getKmem(fd, encryptBuf, (void **)&aesaencReq.outData, AES_TESTSIZE);

    freeKmem(fd, (void **)&aesaencReq.keyData);
    freeKmem(fd, (void **)&aesaencReq.inData);
    freeKmem(fd, (void **)&aesaencReq.outData);


    aesadecReq.opId            = DPD_AESA_CTR_CRYPT;
    aesadecReq.keyBytes        = AES_KEYSIZE;
    aesadecReq.keyData         = (unsigned char *)keyData;
    aesadecReq.inBytes         = AES_TESTSIZE;
    aesadecReq.inData          = encryptBuf;
    aesadecReq.outData         = decryptBuf;

    status = putKmem(fd, (void *)keyData, (void **)&aesadecReq.keyData, AES_KEYSIZE);
    if (status)
        return status;
        
    status = putKmem(fd, encryptBuf, (void **)&aesadecReq.inData,  AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        return status;
    }
    
    status = putKmem(fd, NULL,       (void **)&aesadecReq.outData, AES_TESTSIZE);
    if (status) {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.inData);
        return status;
    }

    armCompletion(&aesadecReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesadecReq);

    if ((status = waitCompletion("testAesa(): counter mode decryption test", status, &aesadecReq)))
    {
        freeKmem(fd, (void **)&aesadecReq.keyData);
        freeKmem(fd, (void **)&aesadecReq.inData);
        freeKmem(fd, (void **)&aesadecReq.outData);
        return status;
    }
    
    getKmem(fd, decryptBuf, (void **)&aesadecReq.outData, AES_TESTSIZE);

    freeKmem(fd, (void **)&aesadecReq.keyData);
    freeKmem(fd, (void **)&aesadecReq.inData);
    freeKmem(fd, (void **)&aesadecReq.outData);


    if ((memcmp(AesaData, decryptBuf, AES_TESTSIZE)) == 0) {
        printf("*** Test AESA Counter Mode Passed ***\n");
        status = 0;
    } else {
        printf("*** Test AESA Counter Mode Failed ***\n");
        status = -1;
    }

    return status;
}



int testAesa(int fd)
{
    int status;

    status = testAesECB(fd);
    if (status)
        return status;

    status = testAesCBC(fd);
    if (status)
        return status;
        
    status = testAesCTR(fd);

    return status;
}

