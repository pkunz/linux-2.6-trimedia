
/****************************************************************************
 * Sec2Descriptors.h - Descriptor construction definitions for the SEC2.x
 *                     security processing core family
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2000.  All rights reserved
 * Copyright (c) 2003, 2004 Motorola Inc., Freescale Semiconductor
 * Copyright (c) 2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/

/*
 * Revision History:
 * 1.0 Aug 24,2003 dgs - adapted from the final version of mpc18x
 *     May 24,2003 sec - update types for ECC operations
 *     Aug 23,2003 sec - add TLS support
 *     Nov 22,2004 sec - merge in Linux changes
 *     Jan 17,2004 sec - change header for outbound CCMP requests
 * 1.2 Jan 27,2005 sec - delete and typedef some obsolete names, remove
 *                       nonpad IPSec types
 * 1.3 Aug 03,2005 sec - update KEA and corrected TLS types
 */




/*
 * Header constants for AESA_CRYPT_REQ / DPD_AESA_CRYPT_GROUP
 * Note that CCM and SRT modes are not handled by this request type,
 * they are deployed in the SRTP and CCMP request types.
 * Basic AES ECB, CBC, and counter mode are handled here.
 */
 
const unsigned long AesaDesc[1*NUM_AESA_CRYPT_DESC] = {
    /* DPD_AESA_CBC_ENCRYPT_CRYPT     OpId 0x6000 */
    /* CM = 1 (cbc), encrypt on                   */
    /* desc type = 2 (common, nosnoop)            */
    0x60300010,
    
    /* DPD_AESA_CBC_DECRYPT_CRYPT     OpId 0x6001 */
    /* CM = 1 (cbc), encrypt off                  */
    /* desc type = 2 (common, nosnoop)            */
    0x60200010,
    
    /* DPD_AESA_CBC_DECRYPT_CRYPT_RDK OpId 0x6002 */
    /* RDK = noexpand, CM = 1 (cbc), encrypt off  */
    /* desc type = 2 (common, nosnoop)            */
    0x60a00010, 

    /* DPD_AESA_ECB_ENCRYPT_CRYPT     OpId 0x6003 */
    /* CM = 0 (ecb), encrypt on                   */
    /* desc type = 2 (common, nosnoop)            */
    0x60100010, 

    /* DPD_AESA_ECB_DECRYPT_CRYPT_RDK OpId 0x6004 */
    /* CM = 0 (ecb), encrypt off                  */
    /* desc type = 2 (common, nosnoop)            */
    0x60000010,
    
    /* DPD_AESA_ECB_DECRYPT_CRYPT     OpId 0x6005 */
    /* RDK = noexpand, CM = 1 (ecb), encrypt off  */
    /* desc type = 2 (common, nosnoop)            */
    0x60800010, 

    /* DPD_AESA_CTR_CRYPT             OpId 0x6006 */
    /* CM = 2 (ctr), encrypt off                  */
    /* desc type = 2 (common, nosnoop)            */
    0x60600010, 

    /* DPD_AESA_CTR_HMAC              OpId 0x6007 */
    /* CM = 2 (ctr), encrypt off                  */
    /* desc type = 24 (HMAC-snoop)                */
    0x606000c0, 
    };



/*
 * Header constants for RNG_REQ / DPD_RNG_GROUP
 */
 
const unsigned long RngDesc[1*NUM_RNGA_DESC] = {
    /* DPD_RNG_GETRN Req OpId 0x1000 */
    0x40000010
    };



/*
 * Header constants for DES_CBC_CRYPT_REQ / DPD_DES_CBC_GROUP
 */
 
const unsigned long DesCbcReq[1 * NUM_DES_CBC_DESC] = {
    /* DPD_SDES_CBC_CTX_ENCRYPT Req OpId 0x2500 */
    0x20500010,
    /* DPD_SDES_CBC_CTX_DECRYPT Req OpId 0x2501 */
    0x20400010,
    /* DPD_TDES_CBC_CTX_ENCRYPT Req OpId 0x2502 */
    0x20700010,
    /* DPD_TDES_CBC_CTX_DECRYPT Req OpId 0x2503 */
    0x20600010
    };




/*
 * Header constants for DES_CRYPT_REQ / DPD_DES_ECB_GROUP
 */

const unsigned long DesReq[1*NUM_DES_DESC] = {
    /* DPD_SDES_ECB_ENCRYPT Req OpId 0x2600 */
    0x20100010,
    /* DPD_SDES_ECB_DECRYPT Req OpId 0x2601 */
    0x20000010,
    /* DPD_TDES_ECB_ENCRYPT Req OpId 0x2602 */
    0x20300010,
    /* DPD_TDES_ECB_DECRYPT Req OpId 0x2603 */
    0x20200010
    };




/*
 * Header constants for
 * ARC4_LOADCTX_CRYPT_REQ / DPD_RC4_LDCTX_CRYPT_ULCTX_GROUP
 */
 
const unsigned long Rc4LoadCtxUnloadCtxReq[1*NUM_RC4_LOADCTX_UNLOADCTX_DESC] = {
    /* DPD_RC4_LDCTX_CRYPT_ULCTX_GROUP OpId 0x3400 */
    0x10700050
    };



/*
 * Header constants for
 * ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ / DPD_RC4_LDKEY_CRYPT_ULCTX_GROUP
 */
const unsigned long Rc4LoadKeyUnloadCtxReq[1*NUM_RC4_LOADKEY_UNLOADCTX_DESC] = {
    /* DPD_RC4_LDKEY_CRYPT_ULCTX Req OpId 0x3500 */
    0x10200050
    };



/*
 * Header constants for HASH_REQ / DPD_HASH_LDCTX_HASH_ULCTX_GROUP
 */
const unsigned long MdhaReq[1 * NUM_MDHA_DESC] = {
    /* DPD_SHA256_LDCTX_HASH_ULCTX          OpId 0x4400 */
    /* SHA-256                                          */
    0x30100010,

    /* DPD_MD5_LDCTX_HASH_ULCTX             OpId 0x4401 */
    /* MD-5                                             */
    0x30200010,

    /* DPD_SHA_LDCTX_HASH_ULCTX             OpId 0x4402 */
    /* SHA-1                                            */
    0x30000010,

    /* DPD_SHA256_LDCTX_IDGS_HASH_ULCTX     OpId 0x4403 */
    /* SHA-256, CONT, INIT                              */
    0x39100010,

    /* DPD_MD5_LDCTX_IDGS_HASH_ULCTX        OpId 0x4404 */
    /* MD-5, CONT, INIT                                 */
    0x39200010,

    /* DPD_SHA_LDCTX_IDGS_HASH_ULCTX        OpId 0x4405 */
    /* SHA-1, CONT, INIT                                */
    0x39000010,

    /* DPD_SHA256_CONT_HASH_ULCTX           OpId 0x4406 */
    /* SHA256, CONT                                     */
    0x38100010,

    /* DPD_MD5_CONT_HASH_ULCTX              OpId 0x4407 */
    /* MD-5, CONT                                       */
    0x38200010,

    /* DPD_SHA_CONT_HASH_ULCTX              OpId 0x4408 */
    /* SHA-1, CONT                                      */
    0x38000010,

    /* DPD_SHA224_LDCTX_HASH_ULCTX          OpId 0x4409 */
    /* SHA-224                                          */
    0x30300010,

    /* DPD_SHA224_LDCTX_IDGS_HASH_ULCTX     OpId 0x440a */
    /* SHA224, CONT, INIT                               */
    0x39300010,

    /* DPD_SHA224_CONT_HASH_ULCTX           OpId 0x440b */
    /* SHA-224, CONT                                    */
    0x38300010,

    /* DPD_SHA256_LDCTX_HASH_ULCTX_CMP      OpId 0x440c */
    /* SHA-256, CICV                                    */
    0x34100012,

    /* DPD_MD5_LDCTX_HASH_ULCTX_CMP         OpId 0x440d */
    /* MD-5, CICV                                       */
    0x34200012,

    /* DPD_SHA_LDCTX_HASH_ULCTX_CMP         OpId 0x440e */
    /* SHA-1, CICV                                      */
    0x34000012,

    /* DPD_SHA256_LDCTX_IDGS_HASH_ULCTX_CMP OpId 0x440f */
    /* SHA-256, CONT, INIT, CICV                        */
    0x3d100012,

    /* DPD_MD5_LDCTX_IDGS_HASH_ULCTX_CMP    OpId 0x4410 */
    /* MD-5, CONT, INIT, CICV                           */
    0x3d200012,

    /* DPD_SHA_LDCTX_IDGS_HASH_ULCTX_CMP    OpId 0x4411 */
    /* SHA-1, CONT, INIT, CICV                          */
    0x3d000012,

    /* DPD_SHA224_LDCTX_HASH_ULCTX_CMP      OpId 0x4412 */
    /* SHA-224, CICV                                    */
    0x34300012,

    /* DPD_SHA224_LDCTX_IDGS_HASH_ULCTX_CMP OpId 0x4413 */
    /* SHA224, CONT, INIT, CICV                         */
    0x3d300012,
    };


/*
 * Header constants for HASH_REQ / DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP
 */
const unsigned long MdhaPadReq[1 * NUM_MDHA_PAD_DESC] = {
    /* DPD_SHA256_LDCTX_HASH_PAD_ULCTX          OpId 0x4500 */
    /* SHA-256, PD                                          */
    0x30500010,

    /* DPD_MD5_LDCTX_HASH_PAD_ULCTX             OpId 0x4501 */
    /* MD-5, PD                                             */
    0x30600010,

    /* DPD_SHA_LDCTX_HASH_PAD_ULCTX             OpId 0x4502 */
    /* SHA-1, PD                                            */
    0x30400010,

    /* DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX     OpId 0x4503 */
    /* SHA-256, PD, INIT                                    */
    0x31500010,

    /* DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX        OpId 0x4504 */
    /* MD-5, PD, INIT                                       */
    0x31600010,

    /* DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX        OpId 0x4505 */
    /* SHA-1, PD, INIT                                      */
    0x31400010,
 
    /* DPD_SHA224_LDCTX_HASH_PAD_ULCTX          OpId 0x4506 */
    /* SHA-224, PD                                          */
    0x30700010,

    /* DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX     OpId 0x4507 */
    /* SHA-224, PD, INIT                                    */
    0x31700010,

    /* DPD_SHA256_LDCTX_HASH_PAD_ULCTX_CMP      OpId 0x4508 */
    /* SHA-256, PD, CICV                                    */
    0x34500012,

    /* DPD_MD5_LDCTX_HASH_PAD_ULCTX_CMP         OpId 0x4509 */
    /* MD-5, PD, CICV                                       */
    0x34600012,

    /* DPD_SHA_LDCTX_HASH_PAD_ULCTX_CMP         OpId 0x450a */
    /* SHA-1, PD, CICV                                      */
    0x34400012,

    /* DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX_CMP OpId 0x450b */
    /* SHA-256, PD, INIT, CICV                              */
    0x35500012,

    /* DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX_CMP    OpId 0x450c */
    /* MD-5, PD, INIT, CICV                                 */
    0x35600012,

    /* DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX_CMP    OpId 0x450d */
    /* SHA-1, PD, INIT, CICV                                */
    0x35400012,

    /* DPD_SHA224_LDCTX_HASH_PAD_ULCTX_CMP      OpId 0x450e */
    /* SHA-224, PD, CICV                                    */
    0x34700012,

    /* DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX_CMP OpId 0x450f */
    /* SHA-224, PD, INIT                                    */
    0x35700012,
    };



/*
 * Header constants for HMAC_PAD_REQ / DPD_HASH_LDCTX_HMAC_ULCTX_GROUP
 */

const unsigned long HmacPadReq[1 * NUM_HMAC_PAD_DESC] = {
    /* DPD_SHA256_LDCTX_HMAC_ULCTX             OpId 0x4a00 */
    /* SHA-256, INIT, HMAC                                 */
    0x31900010,

    /* DPD_MD5_LDCTX_HMAC_ULCTX                OpId 0x4a01 */
    /* MD-5, INIT, HMAC                                    */
    0x31A00010,
    
    /* DPD_SHA_LDCTX_HMAC_ULCTX                OpId 0x4a02 */
    /* SHA-1, INIT, HMAC                                   */
    0x31800010,
    
    /* DPD_SHA256_LDCTX_HMAC_PAD_ULCTX         OpId 0x4a03 */
    /* SHA-256, INIT, HMAC, PD                             */
    0x31D00010,

    /* DPD_MD5_LDCTX_HMAC_PAD_ULCTX            OpId 0x4a04 */
    /* MD-5, INIT, HMAC, PD                                */
    0x31E00010,
    
    /* DPD_SHA_LDCTX_HMAC_PAD_ULCTX            OpId 0x4a05 */
    /* SHA-1, INIT, HMAC, PD                               */
    0x31C00010,
    
    /* DPD_SHA224_LDCTX_HMAC_ULCTX             OpId 0x4a06 */
    /* SHA-224, INIT, HMAC                                 */
    0x31b00010,
    
    /* DPD_SHA224_LDCTX_HMAC_PAD_ULCTX         OpId 0x4a07 */
    /* SHA-224, INIT, HMAC, PD                             */
    0x31f00010,

    /* DPD_SHA256_LDCTX_HMAC_ULCTX_CMP         OpId 0x4a08 */
    /* SHA-256, INIT, HMAC, CICV                           */
    0x35900012,

    /* DPD_MD5_LDCTX_HMAC_ULCTX_CMP            OpId 0x4a09 */
    /* MD-5, INIT, HMAC, CICV                              */
    0x35A00012,
    
    /* DPD_SHA_LDCTX_HMAC_ULCTX_CMP            OpId 0x4a0a */
    /* SHA-1, INIT, HMAC, CICV                             */
    0x35800012,
    
    /* DPD_SHA256_LDCTX_HMAC_PAD_ULCTX_CMP     OpId 0x4a0b */
    /* SHA-256, INIT, HMAC, PD, CICV                       */
    0x35D00012,

    /* DPD_MD5_LDCTX_HMAC_PAD_ULCTX_CMP        OpId 0x4a0c */
    /* MD-5, INIT, HMAC, PD, CICV                          */
    0x35E00012,
    
    /* DPD_SHA_LDCTX_HMAC_PAD_ULCTX_CMP        OpId 0x4a0d */
    /* SHA-1, INIT, HMAC, PD, CICV                         */
    0x35C00012,
    
    /* DPD_SHA224_LDCTX_HMAC_ULCTX_CMP         OpId 0x4a0e */
    /* SHA-224, INIT, HMAC, CICV                           */
    0x35b00012,
    
    /* DPD_SHA224_LDCTX_HMAC_PAD_ULCTX_CMP     OpId 0x4a0f */
    /* SHA-224, INIT, HMAC, PD, CICV                       */
    0x35f00012,
    };



/*
 * Header constants for MOD_EXP_REQ  DPD_MM_LDCTX_EXP_ULCTX_GROUP
 */
const unsigned long PkhaMmExpReq[1*NUM_MM_EXP_DESC] = {
    /* DPD_MM_LDCTX_EXP_ULCTX Req OpId 0x5100 */
    0x50200080
    };


/*
 * Header constants for MOD_SS_EXP_REQ / DPD_MM_SS_RSA_EXP
 */
const unsigned long PkhaMmSsExpReq[1*NUM_MM_SS_EXP_DESC] = {
    /* DPD_MM_SS_RSA_EXP Req OpId 0x5b00 */
    0x58000080
    };


/*
 * Header constants for MOD_R2MODN_REQ / DPD_MM_LDCTX_R2MODN_ULCTX_GROUP
 */
const unsigned long PkhaModR2modnReq[1 * NUM_MOD_R2MODN_DESC] = {
    /*  DPD_MM_LDCTX_R2MODN_ULCTX Req OpId 0x5200 */
    0x50300080
    };
    
/*
 * Header constants for MOD_RRMODP_REQ / DPD_MM_LDCTX_RRMODP_ULCTX_GROUP
 */
const unsigned long PkhaMmRrmodpReq[1*NUM_MM_RRMODP_DESC] = {
    /* DPD_MM_LDCTX_RRMODP_ULCTX Req OpId 0x5300 */
    0x50400080
    };

/*
 * Header constants for MOD_2OP_REQ / DPD_MM_LDCTX_2OP_ULCTX_GROUP
 */
const unsigned long PkhaMod2OpReq[1 * NUM_MOD_2OP_DESC] = {
    /* DPD_MOD_LDCTX_MUL1_ULCTX        Req OpId 0x5400 */
    0x53000080,
    /* DPD_MOD_LDCTX_MUL2_ULCTX        Req OpId 0x5401 */
    0x54000080,
    /* DPD_MOD_LDCTX_ADD_ULCTX         Req OpId 0x5402 */
    0x51000080,
    /* DPD_MOD_LDCTX_SUB_ULCTX         Req OpId 0x5403 */
    0x52000080,
    /* DPD_POLY_LDCTX_A0_B0_MUL1_ULCTX Req OpId 0x5404 */
    0x56000080, 
    /* DPD_POLY_LDCTX_A0_B0_MUL2_ULCTX Req OpId 0x5405 */
    0x57000080,
    /* DPD_POLY_LDCTX_A0_B0_ADD_ULCTX  Req OpId 0x5406 */
    0x55000080, 
    /* DPD_POLY_LDCTX_A1_B0_MUL1_ULCTX Req OpId 0x5407 */
    0x56400080, 
    /* DPD_POLY_LDCTX_A1_B0_MUL2_ULCTX Req OpId 0x5408 */
    0x57400080, 
    /* DPD_POLY_LDCTX_A1_B0_ADD_ULCTX  Req OpId 0x5409 */
    0x55400080, 
    /* DPD_POLY_LDCTX_A2_B0_MUL1_ULCTX Req OpId 0x540a */
    0x56800080, 
    /* DPD_POLY_LDCTX_A2_B0_MUL2_ULCTX Req OpId 0x540b */
    0x57800080, 
    /* DPD_POLY_LDCTX_A2_B0_ADD_ULCTX  Req OpId 0x540c */
    0x55800080, 
    /* DPD_POLY_LDCTX_A3_B0_MUL1_ULCTX Req OpId 0x540d */
    0x56c00080, 
    /* DPD_POLY_LDCTX_A3_B0_MUL2_ULCTX Req OpId 0x540e */
    0x57c00080, 
    /* DPD_POLY_LDCTX_A3_B0_ADD_ULCTX  Req OpId 0x540f */
    0x55c00080, 
    /* DPD_POLY_LDCTX_A0_B1_MUL1_ULCTX Req OpId 0x5410 */
    0x56100080, 
    /* DPD_POLY_LDCTX_A0_B1_MUL2_ULCTX Req OpId 0x5411 */
    0x57100080, 
    /* DPD_POLY_LDCTX_A0_B1_ADD_ULCTX  Req OpId 0x5412 */
    0x55100080, 
    /* DPD_POLY_LDCTX_A1_B1_MUL1_ULCTX Req OpId 0x5413 */
    0x56500080, 
    /* DPD_POLY_LDCTX_A1_B1_MUL2_ULCTX Req OpId 0x5414 */
    0x57500080, 
    /* DPD_POLY_LDCTX_A1_B1_ADD_ULCTX  Req OpId 0x5415 */
    0x55500080, 
    /* DPD_POLY_LDCTX_A2_B1_MUL1_ULCTX Req OpId 0x5416 */
    0x56900080, 
    /* DPD_POLY_LDCTX_A2_B1_MUL2_ULCTX Req OpId 0x5417 */
    0x57900080, 
    /* DPD_POLY_LDCTX_A2_B1_ADD_ULCTX  Req OpId 0x5418 */
    0x55900080, 
    /* DPD_POLY_LDCTX_A3_B1_MUL1_ULCTX Req OpId 0x5419 */
    0x56d00080, 
    /* DPD_POLY_LDCTX_A3_B1_MUL2_ULCTX Req OpId 0x541a */
    0x57d00080, 
    /* DPD_POLY_LDCTX_A3_B1_ADD_ULCTX  Req OpId 0x541b */
    0x55d00080, 
    /* DPD_POLY_LDCTX_A0_B2_MUL1_ULCTX Req OpId 0x541c */
    0x56200080, 
    /* DPD_POLY_LDCTX_A0_B2_MUL2_ULCTX Req OpId 0x541d */
    0x57200080, 
    /* DPD_POLY_LDCTX_A0_B2_ADD_ULCTX  Req OpId 0x541e */
    0x55200080, 
    /* DPD_POLY_LDCTX_A1_B2_MUL1_ULCTX Req OpId 0x541f */
    0x56600080, 
    /* DPD_POLY_LDCTX_A1_B2_MUL2_ULCTX Req OpId 0x5420 */
    0x57600080, 
    /* DPD_POLY_LDCTX_A1_B2_ADD_ULCTX  Req OpId 0x5421 */
    0x55600080, 
    /* DPD_POLY_LDCTX_A2_B2_MUL1_ULCTX Req OpId 0x5422 */
    0x56a00080, 
    /* DPD_POLY_LDCTX_A2_B2_MUL2_ULCTX Req OpId 0x5423 */
    0x57a00080, 
    /* DPD_POLY_LDCTX_A2_B2_ADD_ULCTX  Req OpId 0x5424 */
    0x55a00080, 
    /* DPD_POLY_LDCTX_A3_B2_MUL1_ULCTX Req OpId 0x5425 */
    0x56e00080, 
    /* DPD_POLY_LDCTX_A3_B2_MUL2_ULCTX Req OpId 0x5426 */
    0x57e00080, 
    /* DPD_POLY_LDCTX_A3_B2_ADD_ULCTX  Req OpId 0x5427 */
    0x55e00080, 
    /* DPD_POLY_LDCTX_A0_B3_MUL1_ULCTX Req OpId 0x5428 */
    0x56300080, 
    /* DPD_POLY_LDCTX_A0_B3_MUL2_ULCTX Req OpId 0x5429 */
    0x57300080, 
    /* DPD_POLY_LDCTX_A0_B3_ADD_ULCTX  Req OpId 0x542a */
    0x55300080, 
    /* DPD_POLY_LDCTX_A1_B3_MUL1_ULCTX Req OpId 0x542b */
    0x56700080, 
    /* DPD_POLY_LDCTX_A1_B3_MUL2_ULCTX Req OpId 0x542c */
    0x57700080, 
    /* DPD_POLY_LDCTX_A1_B3_ADD_ULCTX  Req OpId 0x542d */
    0x55700080, 
    /* DPD_POLY_LDCTX_A2_B3_MUL1_ULCTX Req OpId 0x542e */
    0x56b00080, 
    /* DPD_POLY_LDCTX_A2_B3_MUL2_ULCTX Req OpId 0x542f */
    0x57b00080, 
    /* DPD_POLY_LDCTX_A2_B3_ADD_ULCTX  Req OpId 0x5430 */
    0x55b00080, 
    /* DPD_POLY_LDCTX_A3_B3_MUL1_ULCTX Req OpId 0x5431 */
    0x56f00080, 
    /* DPD_POLY_LDCTX_A3_B3_MUL2_ULCTX Req OpId 0x5432 */
    0x57f00080, 
    /* DPD_POLY_LDCTX_A3_B3_ADD_ULCTX  Req OpId 0x5433 */
    0x55f00080
    };



/*
 * Header constants for ECC_POINT_REQ / DPD_EC_LDCTX_kP_ULCTX_GROUP
 * These descriptors use the "packed" format
 */
const unsigned long PkhaEccPointReq[1*NUM_EC_POINT_DESC] = {
    /* DPD_EC_FP_AFF_PT_MULT          OpId 0x5800 */
    0x50500048, 
    /* DPD_EC_FP_PROJ_PT_MULT         OpId 0x5801 */
    0x50700048, 
    /* DPD_EC_F2M_AFF_PT_MULT         OpId 0x5802 */
    0x50600048, 
    /* DPD_EC_F2M_PROJ_PT_MULT        OpId 0x5803 */
    0x50800048,
    /* DPD_EC_FP_LDCTX_ADD_ULCTX      OpId 0x5804 */
    0x50900048, 
    /* DPD_EC_FP_LDCTX_DOUBLE_ULCTX   OpId 0x5805 */
    0x50a00048, 
    /* DPD_EC_F2M_LDCTX_ADD_ULCTX     OpId 0x5806 */
    0x50b00048, 
    /* DPD_EC_F2M_LDCTX_DOUBLE_ULCTX  OpId 0x5807 */
    0x50c00048
    };



/*
 * Header constants for ECC_2OP_REQ / DPD_EC_2OP_GROUP
 */
const unsigned long PkhaEcc2OpReq[1*NUM_EC_2OP_DESC] = {    
    /* DPD_EC_F2M_LDCTX_MUL1_ULCTX Req OpId 0x5900 */
    0x50800048
    };


/*
 * Header constants for ECC_SPKBUILD_REQ / DPD_EC_SPKBUILD_GROUP
 */
const unsigned long PkhaEccSpkbuildReq[1*NUM_EC_SPKBUILD_DESC] =
{    
  /* DPD_EC_SPKBUILD Req OpId 0x5c00 */
  0x5ff00038
};


/*
 * Header constants for ECC_PTADD_DBL
 */
 const unsigned long PkhaEccPtaddDblReq[1 * NUM_EC_PTADD_DBL_DESC] =
 {
     /* DPD_EC_FPADD      Req OpId 0x5d00 */
     0x50900058,
     /* DPD_EC_FPDBL      Req OpId 0x5d01 */
     0x50a00058,
     /* DPD_EC_F2MADD     Req OpId 0x5d02 */
     0x50b00058,
     /* DPD_EC_F2MDBL     Req OpId 0x5d03 */
     0x50c00058
 };


/*
 * Header constants for IPSEC_CBC_REQ / DPD_IPSEC_CBC_GROUP
 */
const unsigned long IpsecCbcReq[1*NUM_IPSEC_CBC_DESC] = {
    /* DPD_IPSEC_CBC_SDES_ENCRYPT_MD5_PAD    OpId 0x7000 */
    0x20531e20,
    /* DPD_IPSEC_CBC_SDES_ENCRYPT_SHA_PAD    OpId 0x7001 */
    0x20531c20,
    /* DPD_IPSEC_CBC_SDES_ENCRYPT_SHA256_PAD OpId 0x7002 */
    0x20531d20,
    /* DPD_IPSEC_CBC_SDES_DECRYPT_MD5_PAD    OpId 0x7003 */
    0x20431e22,
    /* DPD_IPSEC_CBC_SDES_DECRYPT_SHA_PAD    OpId 0x7004 */
    0x20431c22,
    /* DPD_IPSEC_CBC_SDES_DECRYPT_SHA256_PAD OpId 0x7005 */
    0x20431d22,
    /* DPD_IPSEC_CBC_TDES_ENCRYPT_MD5_PAD    OpId 0x7006 */
    0x20731e20,
    /* DPD_IPSEC_CBC_TDES_ENCRYPT_SHA_PAD    OpId 0x7007 */
    0x20731c20,
    /* DPD_IPSEC_CBC_TDES_ENCRYPT_SHA256_PAD OpId 0x7008 */
    0x20731d20,
    /* DPD_IPSEC_CBC_TDES_DECRYPT_MD5_PAD    OpId 0x7009 */
    0x20631e22,
    /* DPD_IPSEC_CBC_TDES_DECRYPT_SHA_PAD    OpId 0x700a */
    0x20631c22,
    /* DPD_IPSEC_CBC_TDES_DECRYPT_SHA256_PAD OpId 0x700b */
    0x20631d22
    };




/*
 * Header constants for IPSEC_ECB_REQ / DPD_IPSEC_ECB_GROUP
 */
const unsigned long IpsecEcbReq[1*NUM_IPSEC_ECB_DESC] = {
    /* DPD_IPSEC_ECB_SDES_ENCRYPT_MD5_PAD    OpId 0x7100 */
    0x20131e20,
    /* DPD_IPSEC_ECB_SDES_ENCRYPT_SHA_PAD    OpId 0x7101 */
    0x20131c20,
    /* DPD_IPSEC_ECB_SDES_ENCRYPT_SHA256_PAD OpId 0x7102 */
    0x20131d20,
    /* DPD_IPSEC_ECB_SDES_DECRYPT_MD5_PAD    OpId 0x7103 */
    0x20031e22,
    /* DPD_IPSEC_ECB_SDES_DECRYPT_SHA_PAD    OpId 0x7104 */
    0x20031c22,
    /* DPD_IPSEC_ECB_SDES_DECRYPT_SHA256_PAD OpId 0x7105 */
    0x20031d22,
    /* DPD_IPSEC_ECB_TDES_ENCRYPT_MD5_PAD    OpId 0x7106 */
    0x20331e20,
    /* DPD_IPSEC_ECB_TDES_ENCRYPT_SHA_PAD    OpId 0x7107 */
    0x20331c20,
    /* DPD_IPSEC_ECB_TDES_ENCRYPT_SHA256_PAD OpId 0x7108 */
    0x20331d20,
    /* DPD_IPSEC_ECB_TDES_DECRYPT_MD5_PAD    OpId 0x7109 */
    0x20231e22,
    /* DPD_IPSEC_ECB_TDES_DECRYPT_SHA_PAD    OpId 0x710a */
    0x20231c22,
    /* DPD_IPSEC_ECB_TDES_DECRYPT_SHA256_PAD OpId 0x710b */
    0x20231d22
    };



/*
 * Header constants for IPSEC_AES_CBC_REQ / DPD_IPSEC_AES_CBC_GROUP
 */
 
const unsigned long IpsecAesCbcReq[1*NUM_IPSEC_AES_CBC_DESC] = {
    /*  DPD_IPSEC_AES_CBC_ENCRYPT_MD5_APAD           OpId 0x8000 */
    0x60331E20,
    /*  DPD_IPSEC_AES_CBC_ENCRYPT_SHA_APAD           OpId 0x8001 */
    0x60331C20,
    /*  DDPD_IPSEC_AES_CBC_ENCRYPT_SHA256_APAD       OpId 0x8002 */
    0x60331D20,
    /*  DPD_IPSEC_AES_CBC_ENCRYPT_MD5                OpId 0x8003 */
    0x60331a20,
    /*  DPD_IPSEC_AES_CBC_ENCRYPT_SHA                OpId 0x8004 */
    0x60331820,
    /*  DDPD_IPSEC_AES_CBC_ENCRYPT_SHA256            OpId 0x8005 */
    0x60331920,
    /*  DPD_IPSEC_AES_CBC_DECRYPT_MD5_APAD           OpId 0x8006 */
    0x60231e22,
    /*  DPD_IPSEC_AES_CBC_DECRYPT_SHA_APAD           OpId 0x8007 */
    0x60231c22,
    /*  DDPD_IPSEC_AES_CBC_DECRYPT_SHA256_APAD       OpId 0x8008 */
    0x60231d22,
    /*  DPD_IPSEC_AES_CBC_DECRYPT_MD5                OpId 0x8009 */
    0x60231a22,
    /*  DPD_IPSEC_AES_CBC_DECRYPT_SHA                OpId 0x800a */
    0x60231822,
    /*  DDPD_IPSEC_AES_CBC_DECRYPT_SHA256            OpId 0x800b */
    0x60231922
};




/*
 * Header constants for IPSEC_AES_ECB_REQ / DPD_IPSEC_AES_ECB_GROUP
 */

const unsigned long IpsecAesEcbReq[1*NUM_IPSEC_AES_ECB_DESC] = {
    /*  DPD_IPSEC_AES_ECB_ENCRYPT_MD5_APAD           OpId 0x8100 */
    0x60131E20,
    /*  DPD_IPSEC_AES_ECB_ENCRYPT_SHA_APAD           OpId 0x8101 */
    0x60131C20,
    /*  DDPD_IPSEC_AES_ECB_ENCRYPT_SHA256_APAD       OpId 0x8102 */
    0x60131D20,
    /*  DPD_IPSEC_AES_ECB_ENCRYPT_MD5                OpId 0x8103 */
    0x60131a20,
    /*  DPD_IPSEC_AES_ECB_ENCRYPT_SHA                OpId 0x8104 */
    0x60131820,
    /*  DDPD_IPSEC_AES_ECB_ENCRYPT_SHA256            OpId 0x8105 */
    0x60131920,
    /*  DPD_IPSEC_AES_ECB_DECRYPT_MD5_APAD           OpId 0x8106 */
    0x60031e22,
    /*  DPD_IPSEC_AES_ECB_DECRYPT_SHA_APAD           OpId 0x8107 */
    0x60031c22,
    /*  DDPD_IPSEC_AES_ECB_DECRYPT_SHA256_APAD       OpId 0x8108 */
    0x60031d22,
    /*  DPD_IPSEC_AES_ECB_DECRYPT_MD5                OpId 0x8109 */
    0x60031a22,
    /*  DPD_IPSEC_AES_ECB_DECRYPT_SHA                OpId 0x810a */
    0x60031822,
    /*  DDPD_IPSEC_AES_ECB_DECRYPT_SHA256            OpId 0x810b */
    0x60031922
};



/*
 * Header constants for IPSEC_ESP_REQ / DPD_IPSEC_ESP_GROUP
 */

const unsigned long IpsecEspReq[1 * NUM_IPSEC_ESP_DESC] = {
    /*  DPD_IPSEC_ESP_OUT_SDES_ECB_CRPT_MD5_PAD          OpId 0x7500 */
    0x20131e08,

    /*  DPD_IPSEC_ESP_OUT_SDES_ECB_CRPT_SHA_PAD          OpId 0x7501 */
    0x20131c08,

    /*  DPD_IPSEC_ESP_OUT_SDES_ECB_CRPT_SHA256_PAD       OpId 0x7502 */
    0x20131d08,

    /*  DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_MD5_PAD          OpId 0x7503 */
    0x20031e0a,

    /*  DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_SHA_PAD          OpId 0x7504 */
    0x20031c0a,

    /*  DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_SHA256_PAD       OpId 0x7505 */
    0x20031d0a,

    /*  DPD_IPSEC_ESP_OUT_SDES_CBC_CRPT_MD5_PAD          OpId 0x7506 */
    0x20531e08,

    /*  DPD_IPSEC_ESP_OUT_SDES_CBC_CRPT_SHA_PAD          OpId 0x7507 */
    0x20531c08,

    /*  DPD_IPSEC_ESP_OUT_SDES_CBC_CRPT_SHA256_PAD       OpId 0x7508 */
    0x20531d08,

    /*  DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_MD5_PAD          OpId 0x7509 */
    0x20431e0a,

    /*  DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_SHA_PAD          OpId 0x750a */
    0x20431c0a,

    /*  DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_SHA256_PAD       OpId 0x750b */
    0x20431d0a,

    /*  DPD_IPSEC_ESP_OUT_TDES_CBC_CRPT_MD5_PAD          OpId 0x750c */
    0x20731e08,

    /*  DPD_IPSEC_ESP_OUT_TDES_CBC_CRPT_SHA_PAD          OpId 0x750d */
    0x20731c08,

    /*  DPD_IPSEC_ESP_OUT_TDES_CBC_CRPT_SHA256_PAD       OpId 0x750e */
    0x20731d08,

    /*  DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_MD5_PAD          OpId 0x750f */
    0x20631e0a,

    /*  DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_SHA_PAD          OpId 0x7510 */
    0x20631c0a,

    /*  DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_SHA256_PAD       OpId 0x7511 */
    0x20631d0a,

    /*  DPD_IPSEC_ESP_OUT_TDES_ECB_CRPT_MD5_PAD          OpId 0x7512 */
    0x20331e08,

    /*  DPD_IPSEC_ESP_OUT_TDES_ECB_CRPT_SHA_PAD          OpId 0x7513 */
    0x20331c08,

    /*  DPD_IPSEC_ESP_OUT_TDES_ECB_CRPT_SHA256_PAD       OpId 0x7514 */
    0x20331d08,

    /*  DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_MD5_PAD          OpId 0x7515 */
    0x20231e0a,

    /*  DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_SHA_PAD          OpId 0x7516 */
    0x20231c0a,

    /*  DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_SHA256_PAD       OpId 0x7517 */
    0x20231d0a,

    /*  DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_MD5_PAD_CMP      OpId 0x7518 */
    0x20035e0a,

    /*  DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_SHA_PAD_CMP      OpId 0x7519 */
    0x20035c0a,

    /*  DPD_IPSEC_ESP_IN_SDES_ECB_DCRPT_SHA256_PAD_CMP   OpId 0x751a */
    0x20035d0a,

    /*  DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_MD5_PAD_CMP      OpId 0x751b */
    0x20435e0a,

    /*  DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_SHA_PAD_CMP      OpId 0x751c */
    0x20435c0a,

    /*  DPD_IPSEC_ESP_IN_SDES_CBC_DCRPT_SHA256_PAD_CMP   OpId 0x751d */
    0x20435d0a,

    /*  DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_MD5_PAD_CMP      OpId 0x751e */
    0x20635e0a,

    /*  DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_SHA_PAD_CMP      OpId 0x751f */
    0x20635c0a,

    /*  DPD_IPSEC_ESP_IN_TDES_CBC_DCRPT_SHA256_PAD_CMP   OpId 0x7520 */
    0x20635d0a,

    /*  DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_MD5_PAD_CMP      OpId 0x7521 */
    0x20235e0a,

    /*  DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_SHA_PAD_CMP      OpId 0x7522 */
    0x20235c0a,

    /*  DPD_IPSEC_ESP_IN_TDES_ECB_DCRPT_SHA256_PAD_CMP   OpId 0x7523 */
    0x20235d0a,
};




/*
 * Header constants for CCMP_REQ / DPD_CCMP_GROUP
 */
 
const unsigned long CcmpReq[1 * NUM_CCMP_DESC] = {
    /*  DPD_802_11_CCMP_OUTBOUND                 OpId 0x6500 */
    0x6b100018,

    /*  DPD_802_11_CCMP_INBOUND                  OpId 0x6501 */
    0x6b00001a,

    /*  DPD_802_11_CCMP_INBOUND_CMP              0pId 0x6502 */
    0x6f00001a,
};




/*
 * Header constants for SRTP_REQ / DPD_SRTP_GROUP
 */
 
const unsigned long SrtpReq[1 * NUM_SRTP_DESC] = {
    /* DPD_SRTP_OUTBOUND                         OpId 0x8500  */
    /* Primary EU:   AES     Mode: SRT, CBC-encrypt           */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, PD, SHA1       */
    /* Type:         SRTP, Outbound                           */
    0x64731c28,

    /* DPD_SRTP_INBOUND                          OpId 0x8501  */
    /* Primary EU:   AES     Mode: SRT, CBC-decrypt           */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, PD, SHA1       */
    /* Type:         SRTP, Inbound                            */
    0x64631c2a,

    /* DPD_SRTP_INBOUND_CMP                     OpId 0x8502   */
    /* Primary EU:   AES     Mode: SRT, CBC-decrypt           */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, PD, CICV, SHA1 */
    /* Type:         SRTP, Inbound                            */
    0x64635c2a
};



/*
 * Header constants for KEA_CRYPT_REQ / DPD_KEA_CRYPT_GROUP
 */
 
const unsigned long KeaReq[1 * NUM_KEA_CRYPT_DESC] = {
    /* DPD_KEA_f8_CIPHER_INIT           (DPD_KEA_CRYPT_GROUP + 0) (OpId 0xa000) */
    /* INT, f8                                                                  */
    0x70800010,
    
    /* DPD_KEA_f8_CIPHER                (DPD_KEA_CRYPT_GROUP + 1) (OpId 0xa001) */
    /* f8                                                                       */
    0x70000010,
    
    /* DPD_KEA_f9_CIPHER_INIT           (DPD_KEA_CRYPT_GROUP + 2) (OpId 0xa002) */
    /* INT, f9                                                                  */
    0x70a00010,

    /* DPD_KEA_f9_CIPHER                (DPD_KEA_CRYPT_GROUP + 3) (OpId 0xa003) */
    /* f9                                                                       */
    0x70200010,

    /* DPD_KEA_f9_CIPHER_FINAL          (DPD_KEA_CRYPT_GROUP + 4) (OpId 0xa004) */
    /* PE, f9                                                                   */
    0x71200010,

    /* DPD_KEA_f9_CIPHER_INIT_FINAL     (DPD_KEA_CRYPT_GROUP + 5) (OpId 0xa005) */
    /* INT, PE, f9                                                              */
    0x71a00010,
    
    /* DPD_KEA_GSM_A53_CIPHER           (DPD_KEA_CRYPT_GROUP + 6) (OpId 0xa006) */
    /* GSM, PE, INT, f8                                                         */
    0x79800010,

    /* DPD_KEA_EDGE_A53_CIPHER          (DPD_KEA_CRYPT_GROUP + 7) (OpId 0xa007) */
    /* EDGE, PE, INT, f8                                                        */
    0x73800010,

    /* DPD_KEA_f9_CIPHER_FINAL_CMP      (DPD_KEA_CRYPT_GROUP + 8) (OpId 0xa008) */
    /* PE, f9, CICV                                                             */
    0x75200012,

    /* DPD_KEA_f9_CIPHER_INIT_FINAL_CMP (DPD_KEA_CRYPT_GROUP + 9) (OpId 0xa009) */
    /* INT, PE, f9, CICV                                                        */
    0x75a00012,
};



/*
 * Header constants for RAID_XOR_REQ / DPD_RAID_XOR_REQ_GROUP
 */

const unsigned long RaidXorReq[1 * NUM_RAID_XOR_DESC] = {
    /* DPD_RAID_XOR    OpId 0x6200 */
    0x6c6000a8,
};



/*
 * Header constants for TLS_BLOCK_INBOUND_REQ / DPD_TLS_BLOCK_INBOUND_GROUP
 */
const unsigned long TlsBlockInboundReq[1 * NUM_TLS_BLOCK_INBOUND_DESC] = {
    /* DPD_TLS_BLOCK_INBOUND_SDES_MD5            OpId 0x9000 */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x20431a8a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_MD5_CMP        OpId 0x9001 */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, MD5     */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x20435a8a,
 
    /* DPD_TLS_BLOCK_INBOUND_SDES_SHA1           OpId 0x9002 */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043188a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_SHA1_CMP       OpId 0x9003 */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043588a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_SHA256         OpId 0x9004 */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043198a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_SHA256_CMP     OpId 0x9005 */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA256  */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043598a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_MD5            OpId 0x9006 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x20631a8a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_MD5_CMP        OpId 0x9007 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, MD5     */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x20635a8a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_SHA1           OpId 0x9008 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063188a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_SHA1_CMP       OpId 0x9009 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063588a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_SHA256         OpId 0x900a */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063198a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_SHA256_CMP     OpId 0x900b */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA256  */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063598a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_MD5_SMAC       OpId 0x900c */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, MD5           */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043328a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_MD5_SMAC_CMP   OpId 0x900d */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, CICV, MD5     */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043728a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_SHA1_SMAC      OpId 0x900e */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, SHA1          */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043308a,

    /* DPD_TLS_BLOCK_INBOUND_SDES_SHA1_SMAC_CMP  OpId 0x900f */
    /* Primary EU:   DES     Mode: Single, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2043708a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_MD5_SMAC       OpId 0x9010 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, MD5           */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063328a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_MD5_SMAC_CMP   OpId 0x9011 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, CICV, MD5     */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063728a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_SHA1_SMAC      OpId 0x9012 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, SHA1          */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063308a,

    /* DPD_TLS_BLOCK_INBOUND_TDES_SHA1_SMAC_CMP  OpId 0x9013 */
    /* Primary EU:   DES     Mode: Triple, CBC-decrypt       */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x2063708a,

    /* DPD_TLS_BLOCK_INBOUND_AES_MD5             OpId 0x9014 */
    /* Primary EU:   AES     Mode: CBC-decrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x60231a8a,

    /* DPD_TLS_BLOCK_INBOUND_AES_MD5_CMP         OpId 0x9015 */
    /* Primary EU:   AES     Mode: CBC-decrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, MD5     */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x60235a8a,

    /* DPD_TLS_BLOCK_INBOUND_AES_SHA1            OpId 0x9016 */
    /* Primary EU:   AES     Mode: CBC-decrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023188a,

    /* DPD_TLS_BLOCK_INBOUND_AES_SHA1_CMP        OpId 0x9017 */
    /* Primary EU:   AES     Mode: CBC-decrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023588a,

    /* DPD_TLS_BLOCK_INBOUND_AES_SHA256          OpId 0x9018 */
    /* Primary EU:   AES     Mode: CBC-decrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023198a,

    /* DPD_TLS_BLOCK_INBOUND_AES_SHA256_CMP      OpId 0x9019 */
    /* Primary EU:   AES     Mode: CBC-decrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA256  */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023598a,

    /* DPD_TLS_BLOCK_INBOUND_AES_MD5_SMAC        OpId 0x901a */
    /* Primary EU:   AES     CBC-decrypt                     */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, MD5           */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023328a,

    /* DPD_TLS_BLOCK_INBOUND_AES_MD5_SMAC_CMP    OpId 0x901b */
    /* Primary EU:   AES     CBC-decrypt                     */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, MD5           */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023728a,

    /* DPD_TLS_BLOCK_INBOUND_AES_SHA1_SMAC       OpId 0x901c */
    /* Primary EU:   AES     CBC-decrypt                     */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, SHA1          */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023308a,

    /* DPD_TLS_BLOCK_INBOUND_AES_SHA1_SMAC_CMP   OpId 0x901d */
    /* Primary EU:   AES     CBC-decrypt                     */
    /* Secondary EU: MDEU    Mode: INIT, SMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Block, Inbound                  */
    0x6023708a,
};





/*
 * Header constants for TLS_BLOCK_OUTBOUND_REQ / DPD_TLS_BLOCK_OUTBOUND_GROUP
 */
const unsigned long TlsBlockOutboundReq[1 * NUM_TLS_BLOCK_OUTBOUND_DESC] = {
    /* DPD_TLS_BLOCK_OUTBOUND_SDES_MD5           OpId 0x9100 */
    /* Primary EU:   DES     Mode: Single, CBC-encrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x20531a88,

    /* DPD_TLS_BLOCK_OUTBOUND_SDES_SHA1          OpId 0x9101 */
    /* Primary EU:   DES     Mode: Single, CBC-encrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x20531888,

    /* DPD_TLS_BLOCK_OUTBOUND_SDES_SHA256        OpId 0x9102 */
    /* Primary EU:   DES     Mode: Single, CBC-encrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x20531988,

    /* DPD_TLS_BLOCK_OUTBOUND_TDES_MD5           OpId 0x9103 */
    /* Primary EU:   DES     Mode: Triple, CBC-encrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x20731a88,

    /* DPD_TLS_BLOCK_OUTBOUND_TDES_SHA1          OpId 0x9104 */
    /* Primary EU:   DES     Mode: Triple, CBC-encrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x20731888,

    /* DPD_TLS_BLOCK_OUTBOUND_TDES_SHA256        OpId 0x9105 */
    /* Primary EU:   DES     Mode: Triple, CBC-encrypt       */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x20731988,

    /* DPD_TLS_BLOCK_OUTBOUND_AES_MD5            OpId 0x9106 */
    /* Primary EU:   AES     Mode: CBC-encrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x60331a88,

    /* DPD_TLS_BLOCK_OUTBOUND_AES_SHA1           OpId 0x9107 */
    /* Primary EU:   AES     Mode: CBC-encrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x60331888,

    /* DPD_TLS_BLOCK_OUTBOUND_AES_SHA256         OpId 0x9108 */
    /* Primary EU:   AES     Mode: CBC-encrypt               */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Block, Outbound                 */
    0x60331988,

};





/*
 * Header constants for TLS_STREAM_INBOUND_REQ / DPD_TLS_STREAM_INBOUND_GROUP
 */
const unsigned long TlsStreamInboundReq[1 * NUM_TLS_STREAM_INBOUND_DESC] = {
    /* DPD_TLS_STREAM_INBOUND_MD5                OpId 0x9200 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x10031a9a,

    /* DPD_TLS_STREAM_INBOUND_CTX_MD5            OpId 0x9201 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                 */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x10731a9a,

    /* DPD_TLS_STREAM_INBOUND_SHA1               OpId 0x9202 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1003189a,

    /* DPD_TLS_STREAM_INBOUND_CTX_SHA1           OpId 0x9203 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                 */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1073189a,

    /* DPD_TLS_STREAM_INBOUND_SHA256             OpId 0x9204 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1003199a,

    /* DPD_TLS_STREAM_INBOUND_CTX_SHA256         OpId 0x9205 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                 */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1073199a,

    /* DPD_TLS_STREAM_INBOUND_MD5_CMP            OpId 0x9206 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, MD5     */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x10035a9a,

    /* DPD_TLS_STREAM_INBOUND_CTX_MD5_CMP        OpId 0x9207 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                 */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, MD5     */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x10735a9a,

    /* DPD_TLS_STREAM_INBOUND_SHA1_CMP           OpId 0x9208 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1003589a,

    /* DPD_TLS_STREAM_INBOUND_CTX_SHA1_CMP       OpId 0x9209 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                 */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA1    */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1073589a,

    /* DPD_TLS_STREAM_INBOUND_SHA256_CMP         OpId 0x920a */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA256  */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1003599a,

    /* DPD_TLS_STREAM_INBOUND_CTX_SHA256_CMP     OpId 0x920b */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                 */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, CICV, SHA256  */
    /* Type:         TLS/SSL Stream, Inbound                 */
    0x1073599a,
};




/*
 * Header constants for TLS_STREAM_OUTBOUND_REQ / DPD_TLS_STREAM_OUTBOUND_GROUP
 */
const unsigned long TlsStreamOutboundReq[1 * NUM_TLS_STREAM_OUTBOUND_DESC] = {
    /* DPD_TLS_STREAM_OUTBOUND_MD5               OpId 0x9300 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Stream, Outbound                */
    0x10031a98,

    /* DPD_TLS_STREAM_OUTBOUND_SHA1              OpId 0x9301 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Stream, Outbound                */
    0x10031898,

    /* DPD_TLS_STREAM_OUTBOUND_SHA256            OpId 0x9302 */
    /* Primary EU:   AFEU    Mode: (none)                    */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA256        */
    /* Type:         TLS/SSL Stream, Outbound                */
    0x10031998,

    /* DPD_TLS_STREAM_OUTBOUND_CTX_MD5           OpId 0x9303 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, MD5           */
    /* Type:         TLS/SSL Stream, Outbound                */
    0x10731a98,

    /* DPD_TLS_STREAM_OUTBOUND_CTX_SHA1          OpId 0x9304 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA1          */
    /* Type:         TLS/SSL Stream, Outbound                */
    0x10731898,

    /* DPD_TLS_STREAM_OUTBOUND_CTX_SHA256        OpId 0x9305 */
    /* Primary EU:   AFEU    Mode: CS, DC, PP                */
    /* Secondary EU: MDEU    Mode: INIT, HMAC, SHA156        */
    /* Type:         TLS/SSL Stream, Outbound                */
    0x10731998,
};
