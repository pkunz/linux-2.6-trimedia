
/****************************************************************************
 * testipsec.c - IPSec known-answer test for SEC2 device driver
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
 * 1.1.1 Jan 08,2005 sec - Todd's comprehensive test cases added and code
 *                         cleaned up.
 * 1.2   02-Feb-2005 sec - fix up type warnings
 * 1.3   Aug 11,2005
 */


#include "Sec2.h"
#include "sec2drvTest.h"

/* NOTE: (ENC_DATA_LENGTH - 8) must be a multiple of 16 or the AES tests will fail! */
#define ENC_DATA_LENGTH  (3000)


typedef struct
{
    unsigned long   encryptopId;
    unsigned long   hmacopId;
    unsigned long   ipsecencopId;
    unsigned long   ipsecdecopId;
    unsigned long   keyBytes;
    unsigned long   hashOutBytes;
    char            testDesc[30];
} IPSECTESTTYPE;

static unsigned char PlainText[ENC_DATA_LENGTH] __attribute__ ((aligned (16)));
static unsigned char ipsecDecryptOut[ENC_DATA_LENGTH + 32] __attribute__ ((aligned (16)));
static unsigned char encryptOnlyOutput[ENC_DATA_LENGTH]  __attribute__ ((aligned (8)));
static unsigned char ipsecoutput[ENC_DATA_LENGTH + 32]  __attribute__ ((aligned (8)));
static unsigned char outivdata[16]  __attribute__ ((aligned (8)));
static unsigned char hmacdigest[32]  __attribute__ ((aligned (8)));
static unsigned char ipsecdigest[32]  __attribute__ ((aligned (8)));

/* encrypt key - 24 bytes */
static const unsigned char EncKey[] __attribute__ ((aligned (16))) = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e
};

/* auth key - 16 bytes */
static const unsigned char authKey[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
};

/* IV - 8 bytes */
/* static unsigned char in_iv[] __attribute__ ((aligned (16))) = "abcdef01"; */
static const unsigned char in_iv[] __attribute__ ((aligned (16))) = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**********************************************************************
 **********************************************************************
 *                  D E S   C B C   T E S T S                         *
 **********************************************************************
 **********************************************************************/

#define NUM_DES_CBC_TESTS   6

static IPSECTESTTYPE descbctest[NUM_DES_CBC_TESTS] = 
{
    {
        /* IPSEC TDES CBC MD5 PAD */
        DPD_TDES_CBC_CTX_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_CBC_TDES_ENCRYPT_MD5_PAD,
        DPD_IPSEC_CBC_TDES_DECRYPT_MD5_PAD,
        24,
        16,
        "IPSEC TDES CBC MD5 PAD",
    },
    
    {
        /* IPSEC TDES CBC SHA-1 PAD */
        DPD_TDES_CBC_CTX_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_CBC_TDES_ENCRYPT_SHA_PAD,
        DPD_IPSEC_CBC_TDES_DECRYPT_SHA_PAD,
        24,
        20,
        "IPSEC TDES CBC SHA-1 PAD",
    },
    
    {
        /* IPSEC TDES CBC SHA-256 PAD */
        DPD_TDES_CBC_CTX_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_CBC_TDES_ENCRYPT_SHA256_PAD,
        DPD_IPSEC_CBC_TDES_DECRYPT_SHA256_PAD,
        24,
        32,
        "IPSEC TDES CBC SHA-256 PAD",
    },
    
    {
        /* IPSEC SDES CBC MD5 PAD */
        DPD_SDES_CBC_CTX_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_CBC_SDES_ENCRYPT_MD5_PAD,
        DPD_IPSEC_CBC_SDES_DECRYPT_MD5_PAD,
        8,
        16,
        "IPSEC SDES CBC MD5 PAD",
    },
    
    {
        /*  IPSEC SDES CBC SHA-1 PAD */
        DPD_SDES_CBC_CTX_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_CBC_SDES_ENCRYPT_SHA_PAD,
        DPD_IPSEC_CBC_SDES_DECRYPT_SHA_PAD,
        8,
        20,
        "IPSEC SDES CBC SHA-1 PAD",
    },
    
    {
        /* IPSEC SDES CBC SHA-256 PAD */
        DPD_SDES_CBC_CTX_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_CBC_SDES_ENCRYPT_SHA256_PAD,
        DPD_IPSEC_CBC_SDES_DECRYPT_SHA256_PAD,
        8,
        32,
        "IPSEC SDES CBC SHA-256 PAD"
    }
};


static int testIPSECdescbcreq
(
    int fd
)
{
    IPSEC_CBC_REQ         ipsecReq;
    DES_LOADCTX_CRYPT_REQ desReq;
    HMAC_PAD_REQ          dyHmacReq;
    int                   status, iTestIndex;
    
    for (iTestIndex = 0; iTestIndex < NUM_DES_CBC_TESTS; iTestIndex++) 
    {
        printf("\n*** Test %s ***\n", descbctest[iTestIndex].testDesc);
        printf("Testing encryption\n");

        /********************************************************************
         * First step of test is to perform encryption and HMAC separately. *
         * This result will compared with the IPSEC operation result later. *
         ********************************************************************/
        
        /* Clear buffers */
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(&desReq,   0, sizeof(desReq));  
        memset(&dyHmacReq, 0, sizeof(dyHmacReq));
        memset(ipsecoutput,0, ENC_DATA_LENGTH);
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memset(encryptOnlyOutput,  0, ENC_DATA_LENGTH);
        
        /* Copy the first 8 bytes to leave unencrypted because this is the ESP header
           in IPSec which is authenticated but not encrypted. */
        memcpy(encryptOnlyOutput, PlainText, 8);       
        
        /* Set up encryption request */
        desReq.opId       = descbctest[iTestIndex].encryptopId;
        desReq.inIvBytes  = 8;
        desReq.keyBytes   = descbctest[iTestIndex].keyBytes;
        desReq.inBytes    = ENC_DATA_LENGTH - 8; 
        desReq.outIvBytes = 8;
        desReq.keyData    = (unsigned char *)EncKey;
        desReq.inData     = &PlainText[8];
        desReq.inIvData   = (unsigned char *)in_iv;
        desReq.outData    = &encryptOnlyOutput[8];
        desReq.outIvData  = outivdata;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)EncKey,  (void **)&desReq.keyData, desReq.keyBytes)))
            return status;
        if ((status = putKmem(fd, &PlainText[8], (void **)&desReq.inData, ENC_DATA_LENGTH - 8))) 
            goto free_descbc_enc_data_1;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&desReq.inIvData, 8))) 
            goto free_descbc_enc_data_2;
        if ((status = putKmem(fd, NULL, (void **)&desReq.outData, ENC_DATA_LENGTH - 8))) 
            goto free_descbc_enc_data_3;
        if ((status = putKmem(fd, NULL, (void **)&desReq.outIvData, 8))) 
            goto free_descbc_enc_data_4;
        
        /* Issue IOCTL */
        armCompletion(&desReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&desReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: cipher only encryption", status, &desReq)))
            goto free_descbc_enc_data_all;

        /* If we got to here then there was no error.  Get the encrypted data
           and IV. */
        getKmem(fd, &encryptOnlyOutput[8], (void **)&desReq.outData,   ENC_DATA_LENGTH - 8);
        getKmem(fd, outivdata,             (void **)&desReq.outIvData, 8);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_descbc_enc_data_all: 
            freeKmem(fd, (void **)&desReq.outIvData);
        free_descbc_enc_data_4:   
            freeKmem(fd, (void **)&desReq.outData);
        free_descbc_enc_data_3:   
            freeKmem(fd, (void **)&desReq.inIvData);
        free_descbc_enc_data_2:   
            freeKmem(fd, (void **)&desReq.inData);
        free_descbc_enc_data_1:
            freeKmem(fd, (void **)&desReq.keyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /* Set up HMAC request */
        dyHmacReq.opId      = descbctest[iTestIndex].hmacopId;
        dyHmacReq.keyBytes  = 16;
        dyHmacReq.inBytes   = ENC_DATA_LENGTH;
        dyHmacReq.outBytes  = descbctest[iTestIndex].hashOutBytes;
        dyHmacReq.keyData   = (unsigned char *)authKey;
        dyHmacReq.inData    = encryptOnlyOutput;
        dyHmacReq.outData   = hmacdigest;

        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&dyHmacReq.keyData, 16))) return status;
        if ((status = putKmem(fd, encryptOnlyOutput, (void **)&dyHmacReq.inData, ENC_DATA_LENGTH))) 
            goto free_descbc_hmac_data_2;        
        if ((status = putKmem(fd, NULL, (void **)&dyHmacReq.outData, dyHmacReq.outBytes))) 
            goto free_descbc_hmac_data_2;        
        
        /* Issue IOCTL */
        armCompletion(&dyHmacReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&dyHmacReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: authentication only", status, &dyHmacReq)))
            goto free_descbc_hmac_data_all;
        
        getKmem(fd, hmacdigest, (void **)&dyHmacReq.outData, dyHmacReq.outBytes);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_descbc_hmac_data_all: 
            freeKmem(fd, (void **)&dyHmacReq.outData);
        free_descbc_hmac_data_2: 
            freeKmem(fd, (void **)&dyHmacReq.inData);
        /* free_descbc_hmac_data_1: */
            freeKmem(fd, (void **)&dyHmacReq.keyData);
        
        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /********************************************************************
         * Second step of test is to perform encryption and HMAC using the  *
         * single IPSEC operation ID.  Compare this to the result obtained  *
         * in the first step.                                               *
         ********************************************************************/
        
        /* Set up IPSec request */
        ipsecReq.opId             = descbctest[iTestIndex].ipsecencopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptCtxInBytes  = 8;
        ipsecReq.cryptKeyBytes    = descbctest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = descbctest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptCtxInData   = (unsigned char *)in_iv;;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = PlainText; 
        ipsecReq.inData           = &PlainText[8];
        ipsecReq.cryptDataOut     = ipsecoutput;
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&ipsecReq.cryptCtxInData, ipsecReq.cryptCtxInBytes))) 
            goto free_descbc_ipsec_data_1;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_descbc_ipsec_data_2;
        if ((status = putKmem(fd, (void *)PlainText, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_descbc_ipsec_data_3;
        if ((status = putKmem(fd, &PlainText[8], (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_descbc_ipsec_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_descbc_ipsec_data_5;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_descbc_ipsec_data_6;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: combined cipher/authentication", status, &ipsecReq)))
            goto free_descbc_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, ipsecoutput,   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        free_descbc_ipsec_data_all:
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_descbc_ipsec_data_6:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_descbc_ipsec_data_5:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_descbc_ipsec_data_4:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_descbc_ipsec_data_3:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_descbc_ipsec_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptCtxInData);
        free_descbc_ipsec_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(&encryptOnlyOutput[8], ipsecoutput, ENC_DATA_LENGTH-8)) != 0)
        {
            printf ("separate and combined descriptors disagree on cypher\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, descbctest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("separate and combined descriptors disagree on digests\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("Testing decryption\n");
        
        
        /********************************************************************
         * Third step of test is to perform decryption and HMAC.  Compare   * 
         * this to the original plain text and HMAC digest.                 *
         ********************************************************************/
        
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memcpy(ipsecDecryptOut, PlainText, 8);       
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(ipsecdigest,0, 20);
 
        /* Set up IPSec request */
        ipsecReq.opId             = descbctest[iTestIndex].ipsecdecopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptCtxInBytes  = 8;
        ipsecReq.cryptKeyBytes    = descbctest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = descbctest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptCtxInData   = (unsigned char *)in_iv;;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = ipsecDecryptOut; 
        ipsecReq.inData           = ipsecoutput;
        ipsecReq.cryptDataOut     = &ipsecDecryptOut[8];
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&ipsecReq.cryptCtxInData, ipsecReq.cryptCtxInBytes))) 
            goto free_descbc_ipsec_decr_data_1;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_descbc_ipsec_decr_data_2;
        if ((status = putKmem(fd, ipsecDecryptOut, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_descbc_ipsec_decr_data_3;
        if ((status = putKmem(fd, ipsecoutput, (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_descbc_ipsec_decr_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_descbc_ipsec_decr_data_5;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_descbc_ipsec_decr_data_6;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: decryption", status, &ipsecReq)))
            goto free_descbc_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, &ipsecDecryptOut[8],   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        /* free_descbc_ipsec_decr_data_all: */
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_descbc_ipsec_decr_data_6:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_descbc_ipsec_decr_data_5:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_descbc_ipsec_decr_data_4:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_descbc_ipsec_decr_data_3:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_descbc_ipsec_decr_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptCtxInData);
        free_descbc_ipsec_decr_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(PlainText, ipsecDecryptOut, ENC_DATA_LENGTH)) != 0)
        {
            printf ("IPSec decryption doesn't match plaintext.\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, descbctest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("IPSec decryption digest doesn't match HMAC digest\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("*** Test %s complete ***\n", descbctest[iTestIndex].testDesc);
    }

    return 0;
} /* testIPSECdescbcreq */


/**********************************************************************
 **********************************************************************
 *                  D E S   E C B   T E S T S                         *
 **********************************************************************
 **********************************************************************/

#define NUM_DES_ECB_TESTS   6

static IPSECTESTTYPE desecbtest[NUM_DES_ECB_TESTS] = 
{
    {
        /* IPSEC TDES ECB MD5 PAD */
        DPD_TDES_ECB_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ECB_TDES_ENCRYPT_MD5_PAD,
        DPD_IPSEC_ECB_TDES_DECRYPT_MD5_PAD,
        24,
        16,
        "IPSEC TDES ECB MD5 PAD",
    },
    
    {
        /* IPSEC TDES ECB SHA-1 PAD */
        DPD_TDES_ECB_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ECB_TDES_ENCRYPT_SHA_PAD,
        DPD_IPSEC_ECB_TDES_DECRYPT_SHA_PAD,
        24,
        20,
        "IPSEC TDES ECB SHA-1 PAD",
    },
    
    {
        /* IPSEC TDES ECB SHA-256 PAD */
        DPD_TDES_ECB_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ECB_TDES_ENCRYPT_SHA256_PAD,
        DPD_IPSEC_ECB_TDES_DECRYPT_SHA256_PAD,
        24,
        32,
        "IPSEC TDES ECB SHA-256 PAD",
    },
    
    {
        /* IPSEC SDES CBC MD5 PAD */
        DPD_SDES_ECB_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ECB_SDES_ENCRYPT_MD5_PAD,
        DPD_IPSEC_ECB_SDES_DECRYPT_MD5_PAD,
        8,
        16,
        "IPSEC SDES ECB MD5 PAD",
    },
    
    {
        /*  IPSEC SDES ECB SHA-1 PAD */
        DPD_SDES_ECB_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ECB_SDES_ENCRYPT_SHA_PAD,
        DPD_IPSEC_ECB_SDES_DECRYPT_SHA_PAD,
        8,
        20,
        "IPSEC SDES ECB SHA-1 PAD",
    },
    
    {
        /* IPSEC SDES ECB SHA-256 PAD */
        DPD_SDES_ECB_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ECB_SDES_ENCRYPT_SHA256_PAD,
        DPD_IPSEC_ECB_SDES_DECRYPT_SHA256_PAD,
        8,
        32,
        "IPSEC SDES ECB SHA-256 PAD"
    }
};


static int testIPSECdesecbreq
(
    int fd
)
{
    IPSEC_ECB_REQ         ipsecReq;
    DES_CRYPT_REQ         desReq;
    HMAC_PAD_REQ          dyHmacReq;
    int                   status, iTestIndex;
    
    unsigned char hmacdigest[40]  __attribute__ ((aligned (8)));
    unsigned char ipsecdigest[40]  __attribute__ ((aligned (8)));

    for (iTestIndex = 0; iTestIndex < NUM_DES_ECB_TESTS; iTestIndex++) 
    {
        printf("\n*** Test %s ***\n", desecbtest[iTestIndex].testDesc);
        printf("Testing encryption\n");

        /********************************************************************
         * First step of test is to perform encryption and HMAC separately. *
         * This result will compared with the IPSEC operation result later. *
         ********************************************************************/
        
        /* Clear buffers */
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(&desReq,   0, sizeof(desReq));  
        memset(&dyHmacReq, 0, sizeof(dyHmacReq));
        memset(ipsecoutput,0, ENC_DATA_LENGTH);
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memset(encryptOnlyOutput,  0, ENC_DATA_LENGTH);
        
        /* Copy the first 8 bytes to leave unencrypted because this is the ESP header
           in IPSec which is authenticated but not encrypted. */
        memcpy(encryptOnlyOutput, PlainText, 8);       
        
        /* Set up encryption request */
        desReq.opId       = desecbtest[iTestIndex].encryptopId;
        desReq.keyBytes   = desecbtest[iTestIndex].keyBytes;
        desReq.inBytes    = ENC_DATA_LENGTH - 8; 
        desReq.keyData    = (unsigned char *)EncKey;
        desReq.inData     = &PlainText[8];
        desReq.outData    = &encryptOnlyOutput[8];
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)EncKey,  (void **)&desReq.keyData, desReq.keyBytes)))
            return status;
        if ((status = putKmem(fd, &PlainText[8], (void **)&desReq.inData, ENC_DATA_LENGTH - 8))) 
            goto free_desecb_enc_data_1;
        if ((status = putKmem(fd, NULL, (void **)&desReq.outData, ENC_DATA_LENGTH - 8))) 
            goto free_desecb_enc_data_3;
        
        /* Issue IOCTL */
        armCompletion(&desReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&desReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: cipher only encryption", status, &desReq)))
            goto free_desecb_enc_data_all;

        /* If we got to here then there was no error.  Get the encrypted data
           and IV. */
        getKmem(fd, &encryptOnlyOutput[8], (void **)&desReq.outData,   ENC_DATA_LENGTH - 8);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_desecb_enc_data_all: 
        /* free_desecb_enc_data_4: */
            freeKmem(fd, (void **)&desReq.outData);
        free_desecb_enc_data_3:   
            freeKmem(fd, (void **)&desReq.inData);
        free_desecb_enc_data_1:
            freeKmem(fd, (void **)&desReq.keyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /* Set up HMAC request */
        dyHmacReq.opId      = desecbtest[iTestIndex].hmacopId;
        dyHmacReq.keyBytes  = 16;
        dyHmacReq.inBytes   = ENC_DATA_LENGTH;
        dyHmacReq.outBytes  = desecbtest[iTestIndex].hashOutBytes;
        dyHmacReq.keyData   = (unsigned char *)authKey;
        dyHmacReq.inData    = encryptOnlyOutput;
        dyHmacReq.outData   = hmacdigest;

        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&dyHmacReq.keyData, 16))) return status;
        if ((status = putKmem(fd, encryptOnlyOutput, (void **)&dyHmacReq.inData, ENC_DATA_LENGTH))) 
            goto free_desecb_hmac_data_1;        
        if ((status = putKmem(fd, NULL, (void **)&dyHmacReq.outData, dyHmacReq.outBytes))) 
            goto free_desecb_hmac_data_2;        
        
        /* Issue IOCTL */
        armCompletion(&dyHmacReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&dyHmacReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: authentication only", status, &dyHmacReq)))
            goto free_desecb_hmac_data_all;
        
        getKmem(fd, hmacdigest, (void **)&dyHmacReq.outData, dyHmacReq.outBytes);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_desecb_hmac_data_all: 
            freeKmem(fd, (void **)&dyHmacReq.outData);
        free_desecb_hmac_data_2: 
            freeKmem(fd, (void **)&dyHmacReq.inData);
        free_desecb_hmac_data_1: 
            freeKmem(fd, (void **)&dyHmacReq.keyData);
        
        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /********************************************************************
         * Second step of test is to perform encryption and HMAC using the  *
         * single IPSEC operation ID.  Compare this to the result obtained  *
         * in the first step.                                               *
         ********************************************************************/
        
        /* Set up IPSec request */
        ipsecReq.opId             = desecbtest[iTestIndex].ipsecencopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptKeyBytes    = desecbtest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH - 8; 
        ipsecReq.hashDataOutBytes = desecbtest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = PlainText; 
        ipsecReq.inData           = &PlainText[8];
        ipsecReq.cryptDataOut     = ipsecoutput;
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_desecb_ipsec_data_1;
        if ((status = putKmem(fd, PlainText, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_desecb_ipsec_data_2;
        if ((status = putKmem(fd, &PlainText[8], (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_desecb_ipsec_data_3;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_desecb_ipsec_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_desecb_ipsec_data_5;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: combined cipher/authentication", status, &ipsecReq)))
            goto free_desecb_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, ipsecoutput,   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        free_desecb_ipsec_data_all:
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_desecb_ipsec_data_5:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_desecb_ipsec_data_4:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_desecb_ipsec_data_3:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_desecb_ipsec_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_desecb_ipsec_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(&encryptOnlyOutput[8], ipsecoutput, ENC_DATA_LENGTH-8)) != 0)
        {
            printf ("separate and combined descriptors disagree on cypher\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, desecbtest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("separate and combined descriptors disagree on digests\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("Testing decryption\n");
        
        
        /********************************************************************
         * Third step of test is to perform decryption and HMAC.  Compare   * 
         * this to the original plain text and HMAC digest.                 *
         ********************************************************************/
        
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memcpy(ipsecDecryptOut, PlainText, 8);       
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(ipsecdigest,0, 20);
 
        /* Set up IPSec request */
        ipsecReq.opId             = desecbtest[iTestIndex].ipsecdecopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptKeyBytes    = desecbtest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = desecbtest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = ipsecDecryptOut; 
        ipsecReq.inData           = ipsecoutput;
        ipsecReq.cryptDataOut     = &ipsecDecryptOut[8];
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_desecb_ipsec_decr_data_1;
        if ((status = putKmem(fd, ipsecDecryptOut, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_desecb_ipsec_decr_data_2;
        if ((status = putKmem(fd, ipsecoutput, (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_desecb_ipsec_decr_data_3;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_desecb_ipsec_decr_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_desecb_ipsec_decr_data_5;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: decryption", status, &ipsecReq)))
            goto free_desecb_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, &ipsecDecryptOut[8],   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        /* free_desecb_ipsec_decr_data_all: */
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_desecb_ipsec_decr_data_5:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_desecb_ipsec_decr_data_4:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_desecb_ipsec_decr_data_3:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_desecb_ipsec_decr_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_desecb_ipsec_decr_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(PlainText, ipsecDecryptOut, ENC_DATA_LENGTH)) != 0)
        {
            printf ("IPSec decryption doesn't match plaintext.\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, desecbtest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("IPSec decryption digest doesn't match HMAC digest\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("*** Test %s complete ***\n", desecbtest[iTestIndex].testDesc);
    }

    return 0;
} /* testIPSECdesecbreq */


/**********************************************************************
 **********************************************************************
 *                  A E S   C B C   T E S T S                         *
 **********************************************************************
 **********************************************************************/

#define NUM_AES_CBC_TESTS   3

static IPSECTESTTYPE aescbctest[NUM_AES_CBC_TESTS] = 
{
    {
        /* IPSEC AES CBC MD5 PAD */
        DPD_AESA_CBC_ENCRYPT_CRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_AES_CBC_ENCRYPT_MD5_APAD,
        DPD_IPSEC_AES_CBC_DECRYPT_MD5_APAD,
        16,
        16,
        "IPSEC AES CBC MD5 PAD",
    },
    
    {
        /* IPSEC AES CBC SHA-1 PAD */
        DPD_AESA_CBC_ENCRYPT_CRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_AES_CBC_ENCRYPT_SHA_APAD,
        DPD_IPSEC_AES_CBC_DECRYPT_SHA_APAD,
        24,
        20,
        "IPSEC AES CBC SHA-1 PAD",
    },
    
    {
        /* IPSEC AES CBC SHA-256 PAD */
        DPD_AESA_CBC_ENCRYPT_CRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_AES_CBC_ENCRYPT_SHA256_APAD,
        DPD_IPSEC_AES_CBC_DECRYPT_SHA256_APAD,
        24,
        32,
        "IPSEC AES CBC SHA-256 PAD"
    }
};

static int testIPSECaescbcreq
(
    int fd
)
{
    IPSEC_AES_CBC_REQ     ipsecReq;
    AESA_CRYPT_REQ        aesReq;
    HMAC_PAD_REQ          dyHmacReq;
    int                   status, iTestIndex;
    
    for (iTestIndex = 0; iTestIndex < NUM_AES_CBC_TESTS; iTestIndex++) 
    {
        printf("\n*** Test %s ***\n", aescbctest[iTestIndex].testDesc);
        printf("Testing encryption\n");

        /********************************************************************
         * First step of test is to perform encryption and HMAC separately. *
         * This result will compared with the IPSEC operation result later. *
         ********************************************************************/
        
        /* Clear buffers */
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(&aesReq,   0, sizeof(aesReq));  
        memset(&dyHmacReq, 0, sizeof(dyHmacReq));
        memset(ipsecoutput,0, ENC_DATA_LENGTH);
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memset(encryptOnlyOutput,  0, ENC_DATA_LENGTH);
        
        /* Copy the first 8 bytes to leave unencrypted because this is the ESP header
           in IPSec which is authenticated but not encrypted. */
        memcpy(encryptOnlyOutput, PlainText, 8);       
        
        /* Set up encryption request */
        aesReq.opId       = aescbctest[iTestIndex].encryptopId;
        aesReq.inIvBytes  = 16;
        aesReq.keyBytes   = aescbctest[iTestIndex].keyBytes;
        aesReq.inBytes    = ENC_DATA_LENGTH - 8; 
        aesReq.outCtxBytes= 16;
        aesReq.keyData    = (unsigned char *)EncKey;
        aesReq.inData     = &PlainText[8];
        aesReq.inIvData   = (unsigned char *)in_iv;
        aesReq.outData    = &encryptOnlyOutput[8];
        aesReq.outCtxData = outivdata;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)EncKey,  (void **)&aesReq.keyData, aesReq.keyBytes)))
            return status;
        if ((status = putKmem(fd, &PlainText[8], (void **)&aesReq.inData, ENC_DATA_LENGTH - 8))) 
            goto free_aescbc_enc_data_1;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&aesReq.inIvData, 16))) 
            goto free_aescbc_enc_data_2;
        if ((status = putKmem(fd, NULL, (void **)&aesReq.outData, ENC_DATA_LENGTH - 8)))
            goto free_aescbc_enc_data_3;
        if ((status = putKmem(fd, NULL, (void **)&aesReq.outCtxData, 16))) 
            goto free_aescbc_enc_data_4;
        
        /* Issue IOCTL */
        armCompletion(&aesReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: cipher only encryption", status, &aesReq)))
            goto free_aescbc_enc_data_all;

        /* If we got to here then there was no error.  Get the encrypted data
           and IV. */
        getKmem(fd, &encryptOnlyOutput[8], (void **)&aesReq.outData,   ENC_DATA_LENGTH - 8);
        getKmem(fd, outivdata,     (void **)&aesReq.outCtxData, 16);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_aescbc_enc_data_all: 
            freeKmem(fd, (void **)&aesReq.outCtxData);
        free_aescbc_enc_data_4:   
            freeKmem(fd, (void **)&aesReq.outData);
        free_aescbc_enc_data_3:   
            freeKmem(fd, (void **)&aesReq.inIvData);
        free_aescbc_enc_data_2:   
            freeKmem(fd, (void **)&aesReq.inData);
        free_aescbc_enc_data_1:
            freeKmem(fd, (void **)&aesReq.keyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /* Set up HMAC request */
        dyHmacReq.opId      = aescbctest[iTestIndex].hmacopId;
        dyHmacReq.keyBytes  = 16;
        dyHmacReq.inBytes   = ENC_DATA_LENGTH;
        dyHmacReq.outBytes  = aescbctest[iTestIndex].hashOutBytes;
        dyHmacReq.keyData   = (unsigned char *)authKey;
        dyHmacReq.inData    = encryptOnlyOutput;
        dyHmacReq.outData   = hmacdigest;

        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&dyHmacReq.keyData, 16))) return status;
        if ((status = putKmem(fd, encryptOnlyOutput, (void **)&dyHmacReq.inData, ENC_DATA_LENGTH))) 
            goto free_aescbc_hmac_data_2;        
        if ((status = putKmem(fd, NULL, (void **)&dyHmacReq.outData, dyHmacReq.outBytes))) 
            goto free_aescbc_hmac_data_2;        
        
        /* Issue IOCTL */
        armCompletion(&dyHmacReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&dyHmacReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: authentication only", status, &dyHmacReq)))
            goto free_aescbc_hmac_data_all;
        
        getKmem(fd, hmacdigest, (void **)&dyHmacReq.outData, dyHmacReq.outBytes);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_aescbc_hmac_data_all: 
            freeKmem(fd, (void **)&dyHmacReq.outData);
        free_aescbc_hmac_data_2: 
            freeKmem(fd, (void **)&dyHmacReq.inData);
        /* free_aescbc_hmac_data_1: */
            freeKmem(fd, (void **)&dyHmacReq.keyData);
        
        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /********************************************************************
         * Second step of test is to perform encryption and HMAC using the  *
         * single IPSEC operation ID.  Compare this to the result obtained  *
         * in the first step.                                               *
         ********************************************************************/
        
        /* Set up IPSec request */
        ipsecReq.opId             = aescbctest[iTestIndex].ipsecencopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptCtxInBytes  = 16;
        ipsecReq.cryptKeyBytes    = aescbctest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = aescbctest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptCtxInData   = (unsigned char *)in_iv;;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = PlainText; 
        ipsecReq.inData           = &PlainText[8];
        ipsecReq.cryptDataOut     = ipsecoutput;
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&ipsecReq.cryptCtxInData, ipsecReq.cryptCtxInBytes))) 
            goto free_aescbc_ipsec_data_1;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_aescbc_ipsec_data_2;
        if ((status = putKmem(fd, PlainText, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_aescbc_ipsec_data_3;
        if ((status = putKmem(fd, &PlainText[8], (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_aescbc_ipsec_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_aescbc_ipsec_data_5;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_aescbc_ipsec_data_6;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: combined cipher/authentication", status, &ipsecReq)))
            goto free_aescbc_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, ipsecoutput, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        free_aescbc_ipsec_data_all:
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_aescbc_ipsec_data_6:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_aescbc_ipsec_data_5:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_aescbc_ipsec_data_4:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_aescbc_ipsec_data_3:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_aescbc_ipsec_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptCtxInData);
        free_aescbc_ipsec_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(&encryptOnlyOutput[8], ipsecoutput, ENC_DATA_LENGTH-8)) != 0)
        {
            printf ("separate and combined descriptors disagree on cypher\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, aescbctest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("separate and combined descriptors disagree on digests\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("Testing decryption\n");
        
        
        /********************************************************************
         * Third step of test is to perform decryption and HMAC.  Compare   * 
         * this to the original plain text and HMAC digest.                 *
         ********************************************************************/
        
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memcpy(ipsecDecryptOut, PlainText, 8);       
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(ipsecdigest,0, 20);
 
        /* Set up IPSec request */
        ipsecReq.opId             = aescbctest[iTestIndex].ipsecdecopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptCtxInBytes  = 16;
        ipsecReq.cryptKeyBytes    = aescbctest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = aescbctest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptCtxInData   = (unsigned char *)in_iv;;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = ipsecDecryptOut; 
        ipsecReq.inData           = ipsecoutput;
        ipsecReq.cryptDataOut     = &ipsecDecryptOut[8];
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&ipsecReq.cryptCtxInData, ipsecReq.cryptCtxInBytes))) 
            goto free_aescbc_ipsec_decr_data_1;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_aescbc_ipsec_decr_data_2;
        if ((status = putKmem(fd, ipsecDecryptOut, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_aescbc_ipsec_decr_data_3;
        if ((status = putKmem(fd, ipsecoutput, (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_aescbc_ipsec_decr_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_aescbc_ipsec_decr_data_5;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_aescbc_ipsec_decr_data_6;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: decryption", status, &ipsecReq)))
            goto free_aescbc_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, &ipsecDecryptOut[8],   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        /* free_aescbc_ipsec_decr_data_all: */
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_aescbc_ipsec_decr_data_6:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_aescbc_ipsec_decr_data_5:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_aescbc_ipsec_decr_data_4:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_aescbc_ipsec_decr_data_3:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_aescbc_ipsec_decr_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptCtxInData);
        free_aescbc_ipsec_decr_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(PlainText, ipsecDecryptOut, ENC_DATA_LENGTH)) != 0)
        {
            printf ("IPSec decryption doesn't match plaintext.\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, aescbctest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("IPSec decryption digest doesn't match HMAC digest\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("*** Test %s complete ***\n", aescbctest[iTestIndex].testDesc);
    }

    return 0;
} /* testIPSECaescbcreq */


/**********************************************************************
 **********************************************************************
 *                  A E S   E C B   T E S T S                         *
 **********************************************************************
 **********************************************************************/

#define NUM_AES_ECB_TESTS   3

static IPSECTESTTYPE aesecbtest[NUM_AES_ECB_TESTS] = 
{
    {
        /* IPSEC AES ECB MD5 PAD */
        DPD_AESA_ECB_ENCRYPT_CRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_AES_ECB_ENCRYPT_MD5_APAD,
        DPD_IPSEC_AES_ECB_DECRYPT_MD5_APAD,
        16,
        16,
        "IPSEC AES ECB MD5 PAD",
    },
    
    {
        /* IPSEC AES ECB SHA-1 PAD */
        DPD_AESA_ECB_ENCRYPT_CRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_AES_ECB_ENCRYPT_SHA_APAD,
        DPD_IPSEC_AES_ECB_DECRYPT_SHA_APAD,
        24,
        20,
        "IPSEC AES ECB SHA-1 PAD",
    },
    
    {
        /* IPSEC AES ECB SHA-256 PAD */
        DPD_AESA_ECB_ENCRYPT_CRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_AES_ECB_ENCRYPT_SHA256_APAD,
        DPD_IPSEC_AES_ECB_DECRYPT_SHA256_APAD,
        24,
        32,
        "IPSEC AES ECB SHA-256 PAD"
    }
};

static int testIPSECaesecbreq
(
    int fd
)
{
    IPSEC_AES_ECB_REQ     ipsecReq;
    AESA_CRYPT_REQ        aesReq;
    HMAC_PAD_REQ          dyHmacReq;
    int                   status, iTestIndex;
    
    unsigned char hmacdigest[40]  __attribute__ ((aligned (8)));
    unsigned char ipsecdigest[40]  __attribute__ ((aligned (8)));

    for (iTestIndex = 0; iTestIndex < NUM_AES_ECB_TESTS; iTestIndex++) 
    {
        printf("\n*** Test %s ***\n", aesecbtest[iTestIndex].testDesc);
        printf("Testing encryption\n");

        /********************************************************************
         * First step of test is to perform encryption and HMAC separately. *
         * This result will compared with the IPSEC operation result later. *
         ********************************************************************/
        
        /* Clear buffers */
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(&aesReq,   0, sizeof(aesReq));  
        memset(&dyHmacReq, 0, sizeof(dyHmacReq));
        memset(ipsecoutput,0, ENC_DATA_LENGTH);
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memset(encryptOnlyOutput,  0, ENC_DATA_LENGTH);
        
        /* Copy the first 8 bytes to leave unencrypted because this is the ESP header
           in IPSec which is authenticated but not encrypted. */
        memcpy(encryptOnlyOutput, PlainText, 8);       
        
        /* Set up encryption request */
        aesReq.opId       = aesecbtest[iTestIndex].encryptopId;
        aesReq.keyBytes   = aesecbtest[iTestIndex].keyBytes;
        aesReq.inBytes    = ENC_DATA_LENGTH - 8; 
        aesReq.keyData    = (unsigned char *)EncKey;
        aesReq.inData     = &PlainText[8];
        aesReq.outData    = &encryptOnlyOutput[8];
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)EncKey,  (void **)&aesReq.keyData, aesReq.keyBytes)))
            return status;
        if ((status = putKmem(fd, &PlainText[8], (void **)&aesReq.inData, ENC_DATA_LENGTH - 8))) 
            goto free_aesecb_enc_data_1;
        if ((status = putKmem(fd, NULL, (void **)&aesReq.outData, ENC_DATA_LENGTH - 8))) 
            goto free_aesecb_enc_data_2;
        
        /* Issue IOCTL */
        armCompletion(&aesReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&aesReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: cipher only encryption", status, &aesReq)))
            goto free_aesecb_enc_data_all;

        /* If we got to here then there was no error.  Get the encrypted data
           and IV. */
        getKmem(fd, &encryptOnlyOutput[8], (void **)&aesReq.outData,   ENC_DATA_LENGTH - 8);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_aesecb_enc_data_all: 
            freeKmem(fd, (void **)&aesReq.outData);
        free_aesecb_enc_data_2:   
            freeKmem(fd, (void **)&aesReq.inData);
        free_aesecb_enc_data_1:
            freeKmem(fd, (void **)&aesReq.keyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /* Set up HMAC request */
        dyHmacReq.opId      = aesecbtest[iTestIndex].hmacopId;
        dyHmacReq.keyBytes  = 16;
        dyHmacReq.inBytes   = ENC_DATA_LENGTH;
        dyHmacReq.outBytes  = aesecbtest[iTestIndex].hashOutBytes;
        dyHmacReq.keyData   = (unsigned char *)authKey;
        dyHmacReq.inData    = encryptOnlyOutput;
        dyHmacReq.outData   = hmacdigest;

        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&dyHmacReq.keyData, 16))) return status;
        if ((status = putKmem(fd, encryptOnlyOutput, (void **)&dyHmacReq.inData, ENC_DATA_LENGTH))) 
            goto free_aesecb_hmac_data_2;        
        if ((status = putKmem(fd, NULL, (void **)&dyHmacReq.outData, dyHmacReq.outBytes))) 
            goto free_aesecb_hmac_data_2;        
        
        /* Issue IOCTL */
        armCompletion(&dyHmacReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&dyHmacReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: authentication only", status, &dyHmacReq)))
            goto free_aesecb_hmac_data_all;
        
        getKmem(fd, hmacdigest, (void **)&dyHmacReq.outData, dyHmacReq.outBytes);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_aesecb_hmac_data_all: 
            freeKmem(fd, (void **)&dyHmacReq.outData);
        free_aesecb_hmac_data_2: 
            freeKmem(fd, (void **)&dyHmacReq.inData);
        /* free_aesecb_hmac_data_1: */
            freeKmem(fd, (void **)&dyHmacReq.keyData);
        
        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /********************************************************************
         * Second step of test is to perform encryption and HMAC using the  *
         * single IPSEC operation ID.  Compare this to the result obtained  *
         * in the first step.                                               *
         ********************************************************************/
        
        /* Set up IPSec request */
        ipsecReq.opId             = aesecbtest[iTestIndex].ipsecencopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptKeyBytes    = aesecbtest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = aesecbtest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = PlainText; 
        ipsecReq.inData           = &PlainText[8];
        ipsecReq.cryptDataOut     = ipsecoutput;
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_aesecb_ipsec_data_1;
        if ((status = putKmem(fd, PlainText, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_aesecb_ipsec_data_2;
        if ((status = putKmem(fd, &PlainText[8], (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_aesecb_ipsec_data_3;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_aesecb_ipsec_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_aesecb_ipsec_data_5;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: combined cipher/authentication", status, &ipsecReq)))
            goto free_aesecb_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, ipsecoutput,   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        free_aesecb_ipsec_data_all:
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_aesecb_ipsec_data_5:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_aesecb_ipsec_data_4:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_aesecb_ipsec_data_3:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_aesecb_ipsec_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_aesecb_ipsec_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(&encryptOnlyOutput[8], ipsecoutput, ENC_DATA_LENGTH-8)) != 0)
        {
            printf ("separate and combined descriptors disagree on cypher\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, aesecbtest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("separate and combined descriptors disagree on digests\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("Testing decryption\n");
        
        
        /********************************************************************
         * Third step of test is to perform decryption and HMAC.  Compare   * 
         * this to the original plain text and HMAC digest.                 *
         ********************************************************************/
        
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memcpy(ipsecDecryptOut, PlainText, 8);       
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(ipsecdigest,0, 20);
 
        /* Set up IPSec request */
        ipsecReq.opId             = aesecbtest[iTestIndex].ipsecdecopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptKeyBytes    = aesecbtest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = aesecbtest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = ipsecDecryptOut; 
        ipsecReq.inData           = ipsecoutput;
        ipsecReq.cryptDataOut     = &ipsecDecryptOut[8];
        ipsecReq.hashDataOut      = ipsecdigest;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_aesecb_ipsec_decr_data_1;
        if ((status = putKmem(fd, ipsecDecryptOut, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_aesecb_ipsec_decr_data_2;
        if ((status = putKmem(fd, ipsecoutput, (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_aesecb_ipsec_decr_data_3;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes))) 
            goto free_aesecb_ipsec_decr_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.hashDataOut, ipsecReq.hashDataOutBytes)))
            goto free_aesecb_ipsec_decr_data_5;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: decryption", status, &ipsecReq)))
            goto free_aesecb_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, &ipsecDecryptOut[8],   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes);
        getKmem(fd, ipsecdigest, (void **)&ipsecReq.hashDataOut,  ipsecReq.hashDataOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        /* free_aesecb_ipsec_decr_data_all: */
            freeKmem(fd, (void **)&ipsecReq.hashDataOut);
        free_aesecb_ipsec_decr_data_5:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_aesecb_ipsec_decr_data_4:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_aesecb_ipsec_decr_data_3:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_aesecb_ipsec_decr_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_aesecb_ipsec_decr_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(PlainText, ipsecDecryptOut, ENC_DATA_LENGTH)) != 0)
        {
            printf ("IPSec decryption doesn't match plaintext.\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(ipsecdigest, hmacdigest, aesecbtest[iTestIndex].hashOutBytes)) !=0)
        {
            printf ("IPSec decryption digest doesn't match HMAC digest\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("*** Test %s complete ***\n", aesecbtest[iTestIndex].testDesc);
    }

    return 0;
} /* testIPSECaesecbreq */


/**********************************************************************
 **********************************************************************
 *                  E S P   C B C   T E S T S                         *
 **********************************************************************
 **********************************************************************/

#define NUM_ESP_CBC_TESTS   6

static IPSECTESTTYPE espcbctest[NUM_ESP_CBC_TESTS] = 
{
    {
        /* IPSEC TDES ESP CBC MD5 PAD */
        DPD_TDES_CBC_CTX_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_TDES_CBC_CRPT_MD5_PAD,
        DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_MD5_PAD,
        24,
        16,
        "IPSEC TDES ESP CBC MD5 PAD",
    },
    
    {
        /* IPSEC TDES ESP SHA-1 PAD */
        DPD_TDES_CBC_CTX_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_TDES_CBC_CRPT_SHA_PAD,
        DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_SHA_PAD,
        24,
        20,
        "IPSEC TDES ESP CBC SHA-1 PAD",
    },
    
    {
        /* IPSEC TDES ESP SHA-256 PAD */
        DPD_TDES_CBC_CTX_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_TDES_CBC_CRPT_SHA256_PAD,
        DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_SHA256_PAD,
        24,
        32,
        "IPSEC TDES ESP CBC SHA-256 PAD",
    },
    
    {
        /* IPSEC SDES ESP MD5 PAD */
        DPD_SDES_CBC_CTX_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_SDES_CBC_CRPT_MD5_PAD,
        DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_MD5_PAD,
        8,
        16,
        "IPSEC SDES ESP CBC MD5 PAD",
    },
    
    {
        /*  IPSEC SDES ESP SHA-1 PAD */
        DPD_SDES_CBC_CTX_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_SDES_CBC_CRPT_SHA_PAD,
        DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_SHA_PAD,
        8,
        20,
        "IPSEC SDES ESP CBC SHA-1 PAD",
    },
    
    {
        /* IPSEC SDES ESP SHA-256 PAD */
        DPD_SDES_CBC_CTX_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_SDES_CBC_CRPT_SHA256_PAD,
        DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_SHA256_PAD,
        8,
        32,
        "IPSEC SDES ESP CBC SHA-256 PAD"
    }
};


static int testIPSECespcbcreq
(
    int fd
)
{
    IPSEC_ESP_REQ         ipsecReq;
    DES_LOADCTX_CRYPT_REQ desReq;
    HMAC_PAD_REQ          dyHmacReq;
    int                   status, iTestIndex;
    
    for (iTestIndex = 0; iTestIndex < NUM_ESP_CBC_TESTS; iTestIndex++) 
    {
        printf("\n*** Test %s ***\n", espcbctest[iTestIndex].testDesc);
        printf("Testing encryption\n");

        /********************************************************************
         * First step of test is to perform encryption and HMAC separately. *
         * This result will compared with the IPSEC operation result later. *
         ********************************************************************/
        
        /* Clear buffers */
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(&desReq,   0, sizeof(desReq));  
        memset(&dyHmacReq, 0, sizeof(dyHmacReq));
        memset(ipsecoutput,0, ENC_DATA_LENGTH + 32);
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memset(encryptOnlyOutput,  0, ENC_DATA_LENGTH);
        
        /* Copy the first 8 bytes to leave unencrypted because this is the ESP header
           in IPSec which is authenticated but not encrypted. */
        memcpy(encryptOnlyOutput, PlainText, 8);       
        
        /* Set up encryption request */
        desReq.opId       = espcbctest[iTestIndex].encryptopId;
        desReq.inIvBytes  = 8;
        desReq.keyBytes   = espcbctest[iTestIndex].keyBytes;
        desReq.inBytes    = ENC_DATA_LENGTH - 8; 
        desReq.outIvBytes = 8;
        desReq.keyData    = (unsigned char *)EncKey;
        desReq.inData     = &PlainText[8];
        desReq.inIvData   = (unsigned char *)in_iv;
        desReq.outData    = &encryptOnlyOutput[8];
        desReq.outIvData  = outivdata;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)EncKey,  (void **)&desReq.keyData, desReq.keyBytes)))
            return status;
        if ((status = putKmem(fd, &PlainText[8], (void **)&desReq.inData, ENC_DATA_LENGTH - 8))) 
            goto free_espcbc_enc_data_1;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&desReq.inIvData, 8))) 
            goto free_espcbc_enc_data_2;
        if ((status = putKmem(fd, NULL, (void **)&desReq.outData, ENC_DATA_LENGTH - 8))) 
            goto free_espcbc_enc_data_3;
        if ((status = putKmem(fd, NULL, (void **)&desReq.outIvData, 8))) 
            goto free_espcbc_enc_data_4;
        
        /* Issue IOCTL */
        armCompletion(&desReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&desReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: cipher only encryption", status, &desReq)))
            goto free_espcbc_enc_data_all;

        /* If we got to here then there was no error.  Get the encrypted data
           and IV. */
        getKmem(fd, &encryptOnlyOutput[8], (void **)&desReq.outData,   ENC_DATA_LENGTH - 8);
        getKmem(fd, outivdata,          (void **)&desReq.outIvData, 8);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_espcbc_enc_data_all: 
            freeKmem(fd, (void **)&desReq.outIvData);
        free_espcbc_enc_data_4:   
            freeKmem(fd, (void **)&desReq.outData);
        free_espcbc_enc_data_3:   
            freeKmem(fd, (void **)&desReq.inIvData);
        free_espcbc_enc_data_2:   
            freeKmem(fd, (void **)&desReq.inData);
        free_espcbc_enc_data_1:
            freeKmem(fd, (void **)&desReq.keyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /* Set up HMAC request */
        dyHmacReq.opId      = espcbctest[iTestIndex].hmacopId;
        dyHmacReq.keyBytes  = 16;
        dyHmacReq.inBytes   = ENC_DATA_LENGTH;
        dyHmacReq.outBytes  = espcbctest[iTestIndex].hashOutBytes;
        dyHmacReq.keyData   = (unsigned char *)authKey;
        dyHmacReq.inData    = encryptOnlyOutput;
        dyHmacReq.outData   = hmacdigest;

        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&dyHmacReq.keyData, 16))) return status;
        if ((status = putKmem(fd, encryptOnlyOutput, (void **)&dyHmacReq.inData, ENC_DATA_LENGTH))) 
            goto free_espcbc_hmac_data_2;        
        if ((status = putKmem(fd, NULL, (void **)&dyHmacReq.outData, dyHmacReq.outBytes))) 
            goto free_espcbc_hmac_data_2;        
        
        /* Issue IOCTL */
        armCompletion(&dyHmacReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&dyHmacReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: authentication only", status, &dyHmacReq)))
            goto free_espcbc_hmac_data_all;
        
        getKmem(fd, hmacdigest, (void **)&dyHmacReq.outData, dyHmacReq.outBytes);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_espcbc_hmac_data_all: 
            freeKmem(fd, (void **)&dyHmacReq.outData);
        free_espcbc_hmac_data_2: 
            freeKmem(fd, (void **)&dyHmacReq.inData);
        /* free_espcbc_hmac_data_1: */
            freeKmem(fd, (void **)&dyHmacReq.keyData);
        
        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /********************************************************************
         * Second step of test is to perform encryption and HMAC using the  *
         * single IPSEC operation ID.  Compare this to the result obtained  *
         * in the first step.                                               *
         ********************************************************************/
        
        /* Set up IPSec request */
        ipsecReq.opId             = espcbctest[iTestIndex].ipsecencopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptCtxInBytes  = 8;
        ipsecReq.cryptCtxOutBytes = 8;
        ipsecReq.cryptKeyBytes    = espcbctest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = espcbctest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptCtxInData   = (unsigned char *)in_iv;;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = PlainText; 
        ipsecReq.inData           = &PlainText[8];
        ipsecReq.cryptDataOut     = ipsecoutput;
        ipsecReq.cryptCtxOutData  = outivdata;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&ipsecReq.cryptCtxInData, ipsecReq.cryptCtxInBytes))) 
            goto free_espcbc_ipsec_data_1;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_espcbc_ipsec_data_2;
        if ((status = putKmem(fd, PlainText, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_espcbc_ipsec_data_3;
        if ((status = putKmem(fd, &PlainText[8], (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_espcbc_ipsec_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes + espcbctest[iTestIndex].hashOutBytes))) 
            goto free_espcbc_ipsec_data_5;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptCtxOutData, 8))) 
            goto free_espcbc_ipsec_data_6;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: combined cipher/authentication", status, &ipsecReq)))
            goto free_espcbc_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, ipsecoutput,   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes + espcbctest[iTestIndex].hashOutBytes);
        getKmem(fd, outivdata, (void **)&ipsecReq.cryptCtxOutData,  8);

        /* Free buffers.  Labels are for error-handling only! */
        free_espcbc_ipsec_data_all:
            freeKmem(fd, (void **)&ipsecReq.cryptCtxOutData);
        free_espcbc_ipsec_data_6:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_espcbc_ipsec_data_5:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_espcbc_ipsec_data_4:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_espcbc_ipsec_data_3:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_espcbc_ipsec_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptCtxInData);
        free_espcbc_ipsec_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(&encryptOnlyOutput[8], ipsecoutput, ENC_DATA_LENGTH-8)) != 0)
        {
            printf ("separate and combined descriptors disagree on cypher\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(&ipsecoutput[ENC_DATA_LENGTH-8], hmacdigest, 12)) !=0)
        {
            printf ("separate and combined descriptors disagree on digests\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("Testing decryption\n");
        
        
        /********************************************************************
         * Third step of test is to perform decryption and HMAC.  Compare   * 
         * this to the original plain text and HMAC digest.                 *
         ********************************************************************/
        
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memcpy(ipsecDecryptOut, PlainText, 8);       
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(ipsecdigest,0, 20);
 
        /* Set up IPSec request */
        ipsecReq.opId             = espcbctest[iTestIndex].ipsecdecopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptCtxInBytes  = 8;
        ipsecReq.cryptCtxOutBytes = 8;
        ipsecReq.cryptKeyBytes    = espcbctest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = espcbctest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptCtxInData   = (unsigned char *)in_iv;;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = ipsecDecryptOut; 
        ipsecReq.inData           = ipsecoutput;
        ipsecReq.cryptDataOut     = &ipsecDecryptOut[8];
        ipsecReq.cryptCtxOutData  = outivdata;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)in_iv, (void **)&ipsecReq.cryptCtxInData, ipsecReq.cryptCtxInBytes))) 
            goto free_espcbc_ipsec_decr_data_1;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_espcbc_ipsec_decr_data_2;
        if ((status = putKmem(fd, ipsecDecryptOut, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_espcbc_ipsec_decr_data_3;
        if ((status = putKmem(fd, ipsecoutput, (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_espcbc_ipsec_decr_data_4;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes + espcbctest[iTestIndex].hashOutBytes))) 
            goto free_espcbc_ipsec_decr_data_5;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptCtxOutData, 8))) 
            goto free_espcbc_ipsec_decr_data_6;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: decryption", status, &ipsecReq)))
            goto free_espcbc_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, &ipsecDecryptOut[8], (void *)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes  + espcbctest[iTestIndex].hashOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        /* free_espcbc_ipsec_decr_data_all: */
            freeKmem(fd, (void **)&ipsecReq.cryptCtxOutData);
        free_espcbc_ipsec_decr_data_6:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_espcbc_ipsec_decr_data_5:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_espcbc_ipsec_decr_data_4:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_espcbc_ipsec_decr_data_3:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_espcbc_ipsec_decr_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptCtxInData);
        free_espcbc_ipsec_decr_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(PlainText, ipsecDecryptOut, ENC_DATA_LENGTH)) != 0)
        {
            printf ("IPSec decryption doesn't match plaintext.\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(&ipsecDecryptOut[ENC_DATA_LENGTH], hmacdigest, 12)) !=0)
        {
            printf ("IPSec decryption digest doesn't match HMAC digest\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("*** Test %s complete ***\n", espcbctest[iTestIndex].testDesc);
    }

    return 0;
} /* testIPSECespcbcreq */


/**********************************************************************
 **********************************************************************
 *                  E S P   E C B   T E S T S                         *
 **********************************************************************
 **********************************************************************/

#define NUM_ESP_ECB_TESTS   6

static IPSECTESTTYPE especbtest[NUM_ESP_ECB_TESTS] = 
{
    {
        /* IPSEC TDES ESP ECB MD5 PAD */
        DPD_TDES_ECB_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_TDES_ECB_CRPT_MD5_PAD,
        DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_MD5_PAD,
        24,
        16,
        "IPSEC TDES ESP ECB MD5 PAD",
    },
    
    {
        /* IPSEC TDES ESP ECB SHA-1 PAD */
        DPD_TDES_ECB_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_TDES_ECB_CRPT_SHA_PAD,
        DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_SHA_PAD,
        24,
        20,
        "IPSEC TDES ESP ECB SHA-1 PAD",
    },
    
    {
        /* IPSEC TDES ESP ECB SHA-256 PAD */
        DPD_TDES_ECB_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_TDES_ECB_CRPT_SHA256_PAD,
        DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_SHA256_PAD,
        24,
        32,
        "IPSEC TDES ESP ECB SHA-256 PAD",
    },
    
    {
        /* IPSEC SDES ESP ECB MD5 PAD */
        DPD_SDES_ECB_ENCRYPT,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_SDES_ECB_CRPT_MD5_PAD,
        DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_MD5_PAD,
        8,
        16,
        "IPSEC SDES ESP ECB MD5 PAD",
    },
    
    {
        /*  IPSEC SDES ESP ECB SHA-1 PAD */
        DPD_SDES_ECB_ENCRYPT,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_SDES_ECB_CRPT_SHA_PAD,
        DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_SHA_PAD,
        8,
        20,
        "IPSEC SDES ESP ECB SHA-1 PAD",
    },
    
    {
        /* IPSEC SDES ESP SHA-256 PAD */
        DPD_SDES_ECB_ENCRYPT,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_IPSEC_ESP_OUT_SDES_ECB_CRPT_SHA256_PAD,
        DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_SHA256_PAD,
        8,
        32,
        "IPSEC SDES ESP ECB SHA-256 PAD"
    }
};


static int testIPSECespecbreq
(
    int fd
)
{
    IPSEC_ESP_REQ         ipsecReq;
    DES_CRYPT_REQ         desReq;
    HMAC_PAD_REQ          dyHmacReq;
    int                   status, iTestIndex;
    
    for (iTestIndex = 0; iTestIndex < NUM_ESP_ECB_TESTS; iTestIndex++) 
    {
        printf("\n*** Test %s ***\n", especbtest[iTestIndex].testDesc);
        printf("Testing encryption\n");

        /********************************************************************
         * First step of test is to perform encryption and HMAC separately. *
         * This result will compared with the IPSEC operation result later. *
         ********************************************************************/
        /* Clear buffers */
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(&desReq,   0, sizeof(desReq));  
        memset(&dyHmacReq, 0, sizeof(dyHmacReq));
        memset(ipsecoutput,0, ENC_DATA_LENGTH + 32);
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memset(encryptOnlyOutput,  0, ENC_DATA_LENGTH);
        
        /* Copy the first 8 bytes to leave unencrypted because this is the ESP header
           in IPSec which is authenticated but not encrypted. */
        memcpy(encryptOnlyOutput, PlainText, 8);       
        
        /* Set up encryption request */
        desReq.opId       = especbtest[iTestIndex].encryptopId;
        desReq.keyBytes   = especbtest[iTestIndex].keyBytes;
        desReq.inBytes    = ENC_DATA_LENGTH - 8; 
        desReq.keyData    = (unsigned char *)EncKey;
        desReq.inData     = &PlainText[8];
        desReq.outData    = &encryptOnlyOutput[8];
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)EncKey,  (void **)&desReq.keyData, desReq.keyBytes)))
            return status;
        if ((status = putKmem(fd, &PlainText[8], (void **)&desReq.inData, ENC_DATA_LENGTH - 8))) 
            goto free_especb_enc_data_1;
        if ((status = putKmem(fd, NULL, (void **)&desReq.outData, ENC_DATA_LENGTH - 8))) 
            goto free_especb_enc_data_2;
        
        /* Issue IOCTL */
        armCompletion(&desReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&desReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: cipher only encryption", status, &desReq)))
            goto free_especb_enc_data_all;

        /* If we got to here then there was no error.  Get the encrypted data
           and IV. */
        getKmem(fd, &encryptOnlyOutput[8], (void **)&desReq.outData,   ENC_DATA_LENGTH - 8);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_especb_enc_data_all: 
            freeKmem(fd, (void **)&desReq.outData);
        free_especb_enc_data_2:   
            freeKmem(fd, (void **)&desReq.inData);
        free_especb_enc_data_1:
            freeKmem(fd, (void **)&desReq.keyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /* Set up HMAC request */
        dyHmacReq.opId      = especbtest[iTestIndex].hmacopId;
        dyHmacReq.keyBytes  = 16;
        dyHmacReq.inBytes   = ENC_DATA_LENGTH;
        dyHmacReq.outBytes  = especbtest[iTestIndex].hashOutBytes;
        dyHmacReq.keyData   = (unsigned char *)authKey;
        dyHmacReq.inData    = encryptOnlyOutput;
        dyHmacReq.outData   = hmacdigest;

        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&dyHmacReq.keyData, 16))) return status;
        if ((status = putKmem(fd, encryptOnlyOutput, (void **)&dyHmacReq.inData, ENC_DATA_LENGTH))) 
            goto free_especb_hmac_data_1;        
        if ((status = putKmem(fd, NULL, (void **)&dyHmacReq.outData, dyHmacReq.outBytes))) 
            goto free_especb_hmac_data_2;        
        
        /* Issue IOCTL */
        armCompletion(&dyHmacReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&dyHmacReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: authentication only", status, &dyHmacReq)))
            goto free_especb_hmac_data_all;
        
        getKmem(fd, hmacdigest, (void **)&dyHmacReq.outData, dyHmacReq.outBytes);
        
        /* Free buffers.  Labels are for error-handling only! */
        free_especb_hmac_data_all: 
            freeKmem(fd, (void **)&dyHmacReq.outData);
        free_especb_hmac_data_2: 
            freeKmem(fd, (void **)&dyHmacReq.inData);
        free_especb_hmac_data_1: 
            freeKmem(fd, (void **)&dyHmacReq.keyData);
        
        /* If we encountered an error along the way then return. */
        if (status) return status;
        
        /********************************************************************
         * Second step of test is to perform encryption and HMAC using the  *
         * single IPSEC operation ID.  Compare this to the result obtained  *
         * in the first step.                                               *
         ********************************************************************/
        
        /* Set up IPSec request */
        ipsecReq.opId             = especbtest[iTestIndex].ipsecencopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptKeyBytes    = especbtest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = especbtest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = PlainText; 
        ipsecReq.inData           = &PlainText[8];
        ipsecReq.cryptDataOut     = ipsecoutput;
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_especb_ipsec_data_1;
        if ((status = putKmem(fd, PlainText, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_especb_ipsec_data_2;
        if ((status = putKmem(fd, &PlainText[8], (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_especb_ipsec_data_3;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes + especbtest[iTestIndex].hashOutBytes))) 
            goto free_especb_ipsec_data_4;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: combined cipher/authentication", status, &ipsecReq)))
            goto free_especb_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, ipsecoutput,   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes + especbtest[iTestIndex].hashOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        free_especb_ipsec_data_all:
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_especb_ipsec_data_4:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_especb_ipsec_data_3:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_especb_ipsec_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_especb_ipsec_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(&encryptOnlyOutput[8], ipsecoutput, ENC_DATA_LENGTH-8)) != 0)
        {
            printf ("separate and combined descriptors disagree on cypher\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(&ipsecoutput[ENC_DATA_LENGTH-8], hmacdigest, 12)) !=0)
        {
            printf ("separate and combined descriptors disagree on digests\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("Testing decryption\n");
        
        
        /********************************************************************
         * Third step of test is to perform decryption and HMAC.  Compare   * 
         * this to the original plain text and HMAC digest.                 *
         ********************************************************************/
        
        memset(ipsecDecryptOut,0, ENC_DATA_LENGTH);
        memcpy(ipsecDecryptOut, PlainText, 8);       
        memset(&ipsecReq,  0, sizeof(ipsecReq));  
        memset(ipsecdigest,0, 20);
 
        /* Set up IPSec request */
        ipsecReq.opId             = especbtest[iTestIndex].ipsecdecopId;
        ipsecReq.hashKeyBytes     = 16;
        ipsecReq.cryptKeyBytes    = especbtest[iTestIndex].keyBytes;
        ipsecReq.hashInDataBytes  = 8;
        ipsecReq.inDataBytes      = ENC_DATA_LENGTH-8; 
        ipsecReq.hashDataOutBytes = especbtest[iTestIndex].hashOutBytes;
        ipsecReq.hashKeyData      = (unsigned char *)authKey;
        ipsecReq.cryptKeyData     = (unsigned char *)EncKey;
        ipsecReq.hashInData       = ipsecDecryptOut; 
        ipsecReq.inData           = ipsecoutput;
        ipsecReq.cryptDataOut     = &ipsecDecryptOut[8];
        
        /* Allocate buffers and copy data */
        if ((status = putKmem(fd, (void *)authKey, (void **)&ipsecReq.hashKeyData, ipsecReq.hashKeyBytes)))
            return status;
        if ((status = putKmem(fd, (void *)EncKey, (void **)&ipsecReq.cryptKeyData, ipsecReq.cryptKeyBytes))) 
            goto free_especb_ipsec_decr_data_1;
        if ((status = putKmem(fd, ipsecDecryptOut, (void **)&ipsecReq.hashInData, ipsecReq.hashInDataBytes))) 
            goto free_especb_ipsec_decr_data_2;
        if ((status = putKmem(fd, ipsecoutput, (void **)&ipsecReq.inData, ipsecReq.inDataBytes))) 
            goto free_especb_ipsec_decr_data_3;
        if ((status = putKmem(fd, NULL, (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes + especbtest[iTestIndex].hashOutBytes))) 
            goto free_especb_ipsec_decr_data_4;
        
        /* Issue IOCTL */
        armCompletion(&ipsecReq);
        status = ioctl(fd, IOCTL_PROC_REQ, (int)&ipsecReq);
        
        /* Check for completion error */
        if ((status = waitCompletion("testIPSEC: decryption", status, &ipsecReq)))
            goto free_especb_ipsec_data_all;

        /* Get output buffers */
        getKmem(fd, &ipsecDecryptOut[8],   (void **)&ipsecReq.cryptDataOut, ipsecReq.inDataBytes  + especbtest[iTestIndex].hashOutBytes);

        /* Free buffers.  Labels are for error-handling only! */
        /* free_especb_ipsec_decr_data_all: */
            freeKmem(fd, (void **)&ipsecReq.cryptDataOut);
        free_especb_ipsec_decr_data_4:
            freeKmem(fd, (void **)&ipsecReq.inData);
        free_especb_ipsec_decr_data_3:
            freeKmem(fd, (void **)&ipsecReq.hashInData);
        free_especb_ipsec_decr_data_2:
            freeKmem(fd, (void **)&ipsecReq.cryptKeyData);
        free_especb_ipsec_decr_data_1:
            freeKmem(fd, (void **)&ipsecReq.hashKeyData);

        /* If we encountered an error along the way then return. */
        if (status) return status;
       
        /* Verify that separate and combined descriptor operations agree. */
        if ((memcmp(PlainText, ipsecDecryptOut, ENC_DATA_LENGTH)) != 0)
        {
            printf ("IPSec decryption doesn't match plaintext.\n");
            return SEC2_UNKNOWN_ERROR;
        }
        if ((memcmp(&ipsecDecryptOut[ENC_DATA_LENGTH], hmacdigest, 12)) !=0)
        {
            printf ("IPSec decryption digest doesn't match HMAC digest\n");
            return SEC2_UNKNOWN_ERROR;
        }

        printf("*** Test %s complete ***\n", especbtest[iTestIndex].testDesc);
    }

    return 0;
} /* testIPSECespecbreq */



int testIPSEC(int fd)
{
    int status, testfail, i;
        
    /* Initialize PlainText with repeating pattern of 1..50 */
    for (i = 0; i < 3000; i++) PlainText[i] = (i%50) + 1;
    
    testfail = 0;
    
    if ((status = testIPSECdescbcreq(fd))) testfail++;
    if ((status = testIPSECdesecbreq(fd))) testfail++;
    if ((status = testIPSECaescbcreq(fd))) testfail++;
    if ((status = testIPSECaesecbreq(fd))) testfail++;
    if ((status = testIPSECespcbcreq(fd))) testfail++;
    if ((status = testIPSECespecbreq(fd))) testfail++;

    if (testfail)
        return -1;
    else
        return 0;
}



