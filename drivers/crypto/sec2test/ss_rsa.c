
/****************************************************************************
 * ss_rsa.c - Single-Stage RSA known answer test for SEC2 device driver
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
 * 1.2   02-Feb-2005 sec - fix buffer frees, convert test data to const
 * 1.3   Aug 11,2005
 */


#include "sec2drvTest.h"
#include "Sec2.h"



typedef struct motcr_PubKeyDiffieHellman
{
   unsigned long*   pDH_p;
   unsigned long*   pDH_g;
   unsigned long*   pDHPrivateKey;
   unsigned long*   pDHPublicKey;
   unsigned long*   pDHOtherPublicKey;
   unsigned long*   pDHSharedSecret;
   unsigned long    dhParamsWordLength;
   unsigned long    dhPrivateKeyBitLength;
} motcr_PubKeyDiffieHellman;

/*
 * Define the structure of the DH database.
 */
typedef struct DH_ {
    unsigned short state;            /*Bitfields: DH_STATE_VALID, DH_STATE_SH_SECRET */
    unsigned short keygroup_index;
    motcr_PubKeyDiffieHellman DiffieHellman;
} DH;


static const unsigned long force[] =
{
    0xe573d406, 0xb6ccd305,
    0x689338bb, 0xcd7a8c07,
    0x5e92c86d, 0xedf2c964,
    0x0e3eead6, 0x037fcfed,
    0xa19b6db0, 0x3e6abf33,
    0xefac6c24, 0x264b3ac7,
    0xe6265927, 0x8b035f2f,
    0xade89b82, 0xbdefa6a6,
    0xfcffb105, 0x3c830f5d,
    0x0b26f1ee, 0x7eb811aa,
    0x9361d9f6, 0x6166e48b,
    0x45a8e25c, 0x5fef2127,
    0x11c71e4d, 0x8ed10c31,
    0xa6a2ea1f, 0xb559113b,
    0xeac23421, 0x851ad619,
    0xd87a5650, 0xff44ef91
};

static const unsigned long g[] =
{
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000002
};

static const unsigned long p[] =
{
    0xffffffff, 0xffffffff,
    0xc90fdaa2, 0x2168c234,
    0xc4c6628b, 0x80dc1cd1,
    0x29024e08, 0x8a67cc74,
    0x020bbea6, 0x3b139b22,
    0x514a0879, 0x8e3404dd,
    0xef9519b3, 0xcd3a431b,
    0x302b0a6d, 0xf25f1437,
    0x4fe1356d, 0x6d51c245,
    0xe485b576, 0x625e7ec6,
    0xf44c42e9, 0xa637ed6b,
    0x0bff5cb6, 0xf406b7ed,
    0xee386bfb, 0x5a899fa5,
    0xae9f2411, 0x7c4b1fe6,
    0x49286651, 0xece65381,
    0xffffffff, 0xffffffff
}; 

static const unsigned long expected[] =
{
    0xc34a7c1a, 0x2969db30,
    0xfca525bf, 0x3b3f8994,
    0xdfce2114, 0xbebf368d,
    0xd30c45aa, 0x50bbd9a7,
    0x81d5c486, 0x2ccb024e,
    0x82a4c39c, 0x03e1588a,
    0x635c913f, 0x4ce73143,
    0x810e9bad, 0x344b041e,
    0xcfe24db2, 0xae3e6bbd,
    0xf57bbeff, 0x35ab72c7, 
    0x902cc635, 0x7c86fb05,
    0x2b647ab8, 0xc1aa2616,
    0xa129c229, 0x8031d520,
    0xeb810534, 0x7e4b7ec4, 
    0xc8475548, 0x42315665,
    0x934ce027, 0x58c96273
};
    


int SEC2_SS_modExpo(int             fd,
                    unsigned short *requestId,
                    int             srcByteSize,
                    unsigned char  *src,    /* src size must be less than that of modulus */
                    int             expByteSize,
                    unsigned char  *exp,
                    int             modNByteSize,
                    unsigned char  *modN,
                    unsigned char  *outArr, /* result size assumed be same as modulus size */
                    void           (*pfCallback)(unsigned short *requestId, unsigned status))
      
{   /* compute (out)=(src)^exp mod (modN) */

    int            status = 0;
    MOD_SS_EXP_REQ SsExpReq;

    memset(&SsExpReq, 0, sizeof(SsExpReq));

    SsExpReq.opId       = DPD_MM_SS_RSA_EXP;
    SsExpReq.aDataBytes = srcByteSize;
    SsExpReq.expBytes   = expByteSize;
    SsExpReq.modBytes   = modNByteSize;
    SsExpReq.bDataBytes = srcByteSize;
    SsExpReq.aData      = src;
    SsExpReq.expData    = exp;
    SsExpReq.modData    = modN;         
    SsExpReq.bData      = outArr;   /* result buffer will contain exponentiation for src */

    status = putKmem(fd, src, (void **)&SsExpReq.aData, SsExpReq.aDataBytes);
    if (status)
        return status;
 
    status = putKmem(fd, exp, (void **)&SsExpReq.expData, SsExpReq.expBytes);
    if (status) {
        freeKmem(fd, (void **)&SsExpReq.aData);
        return status;
    }
    
    status = putKmem(fd, modN, (void **)&SsExpReq.modData, SsExpReq.modBytes);
    if (status) {
        freeKmem(fd, (void **)&SsExpReq.aData);
        freeKmem(fd, (void **)&SsExpReq.expData);
        return status;
    }

    status = putKmem(fd, NULL, (void **)&SsExpReq.bData, SsExpReq.bDataBytes);
    if (status) {
        freeKmem(fd, (void **)&SsExpReq.aData);
        freeKmem(fd, (void **)&SsExpReq.expData);
        freeKmem(fd, (void **)&SsExpReq.modData);
        return status;
    }

    
    armCompletion(&SsExpReq);
    status = ioctl(fd, IOCTL_PROC_REQ, (int)&SsExpReq);

    if ((status = waitCompletion("testPKHA(): single-stage RSA test", status, &SsExpReq)))
    {
        freeKmem(fd, (void **)&SsExpReq.aData);
        freeKmem(fd, (void **)&SsExpReq.expData);
        freeKmem(fd, (void **)&SsExpReq.modData);
        freeKmem(fd, (void **)&SsExpReq.bData);
        return status;
    }
    
    getKmem(fd, outArr, (void **)&SsExpReq.bData, SsExpReq.bDataBytes);

    freeKmem(fd, (void **)&SsExpReq.aData);
    freeKmem(fd, (void **)&SsExpReq.expData);
    freeKmem(fd, (void **)&SsExpReq.modData);
    freeKmem(fd, (void **)&SsExpReq.bData);

    return status;
}


static unsigned char dhparams[sizeof(DH)];

int pkha_exp_ss(int fd)
{
 
    int pLen, key_len, tstat;
    short id;
    unsigned char ss_public_key[512] __attribute__ ((aligned (8)));
    DH *dh_params;

    id      = 1;
    pLen    = strlen((unsigned char *)force);
    key_len = pLen;

    dh_params = (DH *)dhparams;
    dh_params->DiffieHellman.pDH_g = (unsigned long *)g;
    dh_params->DiffieHellman.pDH_p = (unsigned long *)p;

    tstat = SEC2_SS_modExpo(fd,
                            &id,
                            pLen,
                            (unsigned char *)dh_params->DiffieHellman.pDH_g,
                            key_len,
                            (unsigned char *)force,
                            pLen,
                            (unsigned char *)dh_params->DiffieHellman.pDH_p,
                            (unsigned char *)ss_public_key,
                            (void *)NULL); /*SEC2_DHGenKeyPair_callback) */

    if (tstat)
    {
        printf("pkha_exp_ss(): expo request failed\n");
        return(tstat);
    }
    
    if (memcmp((unsigned char *)ss_public_key,
               (unsigned char *)expected,
               128) != 0)
    {
      dumpm(ss_public_key, pLen);
      tstat = -1;
    }
    else
    {
      printf ("Results from SingleStep RSA as expected\n");
      tstat = 0;
    }

    return(tstat);
}





