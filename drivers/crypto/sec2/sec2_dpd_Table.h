
/****************************************************************************
 * sec2_dpd_Table.h -- Request to DPD Table translation table
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2000.  All rights reserved
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/


/* Revision History:
 * 1.0   Aug 24,2003 dgs - adapted from the final version of mpc18x
 *       May 24,2003 sec - re-ordered RRMODP pointers per T2.0
 *       Aug 23,2004 sec - add TLS support
 *       Oct 15,2004 sec - reordered R2modN and MODEXP pointers
 * 1.1.0 Dec 05,2004 sec - prep for release
 * 1.1.1 Dec 16,2004 sec - remove old unused diagnostics
 *       Jan 17,2005 sec - add in missing IPSEC table entry
 * 1.2   Jan 27,2005 sec - fix up some obsolete names
 * 1.3   Jul 18,2005 sec - expand KFHA and TLS support, add ICV
 */

extern BOOLEAN ChkDesIvLen(unsigned long len);
extern BOOLEAN ChkDesKeyLen(unsigned long len);
extern BOOLEAN ChkDesStaticDataLen(unsigned long len);
extern BOOLEAN ChkDesDataLen(unsigned long len);
extern BOOLEAN ChkDesCryptLen(unsigned long len);
extern BOOLEAN ChkDesCtxLen(unsigned long len);
extern BOOLEAN ChkArcKeyLen(unsigned long len);
extern BOOLEAN ChkArcCtxLen(unsigned long len);
extern BOOLEAN ChkOptionalArcCtxLen(unsigned long len);
extern BOOLEAN ChkEccLen(unsigned long len);
extern BOOLEAN ChkAesIvLen(unsigned long len);
extern BOOLEAN ChkAesKeyLen(unsigned long len);
extern BOOLEAN ChkCcmpKeyLen(unsigned long len);

/*! \enum FLD_TYPE
    \brief A field can either be for reading from or writing to
*/
typedef enum {
    Read,
    Write,
    Extent
} FLD_TYPE;

/*
 * DPD_FLD_DETAILS_ENTRY
 *    Describes where and how a field in a request goes to a field in a DPD
 */
typedef struct
{
    char                    *txt;               /* Description of the
                                                 * field within the request
                                                 * a NULL indicates the end
                                                 *  of field entries */
    unsigned int            lenOffset1st;       /* Offset into request pointer
                                                 * for the initial length field */
    unsigned int            lenOffsetNxt;       /* Offset into request pointer
                                                 * for the next length field
                                                 * Used when input points to
                                                 * output of the previous
                                                 * request */
    unsigned int            ptrOffset1st;       /* Offset into request pointer
                                                 * for the initial data area */
    unsigned int            extOffset;
    FLD_TYPE                dataType;           /* Data type either:
                                                 * a "Read" or "Write" area */
    BOOLEAN                 (*pFncSize)(unsigned long len);
                                                /* Pointer to function that checks
                                                 * whether the length is
                                                 * consistent with the request */
} DPD_FLD_DETAILS_ENTRY;

/*
 * DPD_DETAILS_ENTRY
 * Describes how a request is broken into a single DPD or a set of chained
 * DPDs
 */
typedef struct DPD_DETAILS_ENTRY
{
    unsigned long            opId;               /* Operation ID for entry */
    char                    *txt;                /* Description of request
                                                  * a NULL indicates the end
                                                  * of the table */
    unsigned long            sz;                 /* Size of request */
    const unsigned long     *hdrDesc;            /* Descriptor Header */
    unsigned int             lenOffsetBlockLen;  /* Offset into request pointer
                                                  * for total length of data */
    DPD_FLD_DETAILS_ENTRY    fld[NUM_DPD_FLDS];
} DPD_DETAILS_ENTRY;        /* Each request is enumerated here */

#ifndef offsetof
/*  offsetof(s,m)
    Macro that identifies the byte offset of a field m within a structure s
*/
#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif
#define STD_OFFSETS(s,l1,l2,p1,p2)  offsetof(s,l1), offsetof(s,l2),\
                                    offsetof(s,p1), 0
#define EXT_OFFSETS(s,l1,l2,p1,p2)  offsetof(s,l1), offsetof(s,l2),\
                                    offsetof(s,p1), offsetof(s,p2)
#define NULL_PTR_OFFSETS(s,l1,l2)   offsetof(s,l1), offsetof(s,l2), 0, 0
#define ZERO_LEN_OFFSETS(s,p1,p2)   0, 0, offsetof(s,p1), offsetof(s,p2)
#define ALL_ZERO_OFFSETS            0, 0, 0, 0

static char NIL[] = {"NIL"};


DPD_DETAILS_ENTRY DpdDetails[] =
{


    /*
     * DPD_RNG_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  (none)                (none)             (none)
     * p2  (none)                (none)             (none)
     * p3  (none)                (none)             (none)
     * p4  rngData               rngBytes           (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_RNG_GROUP,
        "DPD_RNG_GROUP",
        sizeof(RNG_REQ),
        RngDesc,
        offsetof(RNG_REQ, rngBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                           Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                           Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                           Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                           Read,  NULL},
            {"rngData",     STD_OFFSETS(RNG_REQ, rngBytes, rngBytes, rngData, rngData), Write, NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                           Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                           Read,  NULL}
        },
    },




    /*
     * DPD_DES_CBC_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  inIvData              inIvBytes          (none)
     * p2  keyData               keyBytes           (none)
     * p3  inData                inBytes            (none)
     * p4  outData               outBytes           (none)
     * p5  outIvData             outIvBytes         (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_DES_CBC_GROUP,
        "DPD_DES_CBC_GROUP",
        sizeof(DES_CBC_CRYPT_REQ),
        DesCbcReq,
        offsetof(DES_CBC_CRYPT_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                             Read,  NULL},
            {"inIvData",    STD_OFFSETS(DES_CBC_CRYPT_REQ, inIvBytes,  inIvBytes,  inIvData,  outIvData), Read,  ChkDesIvLen},
            {"keyData",     STD_OFFSETS(DES_CBC_CRYPT_REQ, keyBytes,   keyBytes,   keyData,   keyData),   Read,  ChkDesKeyLen},
            {"inData",      STD_OFFSETS(DES_CBC_CRYPT_REQ, inBytes,    inBytes,    inData,    inData),    Read,  ChkDesDataLen},
            {"outData",     STD_OFFSETS(DES_CBC_CRYPT_REQ, inBytes,    inBytes,    outData,   outData),   Write, NULL},
            {"outIvData",   STD_OFFSETS(DES_CBC_CRYPT_REQ, outIvBytes, outIvBytes, outIvData, outIvData), Write, ChkDesIvLen},
            {NULL,          ALL_ZERO_OFFSETS,                                                             Read,  NULL}
        },
    },




    /*
     * DPD_DES_ECB_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  (none)                (none)             (none)
     * p2  keyData               keyBytes           (none)
     * p3  inData                inBytes            (none)
     * p4  outData               inBytes            (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_DES_ECB_GROUP,
        "DPD_DES_ECB_GROUP",
        sizeof(DES_CRYPT_REQ),
        DesReq,
        offsetof(DES_CRYPT_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                 Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                 Read,  NULL},
            {"keyData",     STD_OFFSETS(DES_CRYPT_REQ, keyBytes, keyBytes, keyData, keyData), Read,  ChkDesKeyLen},
            {"inData",      STD_OFFSETS(DES_CRYPT_REQ, inBytes,  inBytes,  inData,  inData),  Read,  ChkDesDataLen},
            {"outData",     STD_OFFSETS(DES_CRYPT_REQ, inBytes,  inBytes,  outData, outData), Write, NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                 Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                 Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_RC4_LDCTX_CRYPT_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  inCtxData             inCtxBytes         (none)
     * p2  (none)                (none)             (none)
     * p3  inData                inBytes            (none)
     * p4  outData               inBytes            (none)
     * p5  outCtxData            outCtxBytes        (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_RC4_LDCTX_CRYPT_ULCTX_GROUP,
        "DPD_RC4_LDCTX_CRYPT_ULCTX_GROUP",
        sizeof(ARC4_LOADCTX_CRYPT_REQ),
        Rc4LoadCtxUnloadCtxReq,
        offsetof(ARC4_LOADCTX_CRYPT_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                                      Read,  NULL},
            {"inCtxData",   STD_OFFSETS(ARC4_LOADCTX_CRYPT_REQ, inCtxBytes,  inCtxBytes,  inCtxData,  outCtxData), Read,  ChkArcCtxLen},
            {NIL,           ALL_ZERO_OFFSETS,                                                                      Read,  NULL},
            {"inData",      STD_OFFSETS(ARC4_LOADCTX_CRYPT_REQ, inBytes,     inBytes,     inData,     inData),     Read,  NULL},
            {"outData",     STD_OFFSETS(ARC4_LOADCTX_CRYPT_REQ, inBytes,     inBytes,     outData,    outData),    Write, NULL},
            {"outCtxData",  STD_OFFSETS(ARC4_LOADCTX_CRYPT_REQ, outCtxBytes, outCtxBytes, outCtxData, outCtxData), Write, ChkArcCtxLen},
            {NULL,          ALL_ZERO_OFFSETS,                                                                      Read,  NULL}
        },
    },




    /*
     * DPD_RC4_LDKEY_CRYPT_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  (none)                (none)             (none)
     * p2  keyData               keyBytes           (none)
     * p3  inData                inBytes            (none)
     * p4  outData               inBytes            (none)
     * p5  outCtxData            outCtxBytes        (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_RC4_LDKEY_CRYPT_ULCTX_GROUP,
        "DPD_RC4_LDKEY_CRYPT_ULCTX_GROUP",
        sizeof(ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ),
        Rc4LoadKeyUnloadCtxReq,
        offsetof(ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                                                Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                                                Read,  NULL},
            {"keyData",     STD_OFFSETS(ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ, keyBytes,    keyBytes,    keyData,    keyData),    Read,  ChkArcKeyLen},
            {"inData",      STD_OFFSETS(ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ, inBytes,     inBytes,     inData,     inData),     Read,  NULL},
            {"outData",     STD_OFFSETS(ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ, inBytes,     inBytes,     outData,    outData),    Write, NULL},
            {"outCtxData",  STD_OFFSETS(ARC4_LOADKEY_CRYPT_UNLOADCTX_REQ, outCtxBytes, outCtxBytes, outCtxData, outCtxData), Write, ChkArcCtxLen},
            {NULL,          ALL_ZERO_OFFSETS,                                                                                Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_HASH_LDCTX_HASH_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  ctxData               ctxBytes           (none)
     * p2  (none)                (none)             (none)
     * p3  inData                inBytes            (none)
     * p4  cmpData               outBytes           (none)
     * p5  outData               outBytes           (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_HASH_LDCTX_HASH_ULCTX_GROUP,
        "DPD_HASH_LDCTX_HASH_ULCTX_GROUP",
        sizeof(HASH_REQ),
        MdhaReq,
        offsetof(HASH_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                            Read,  NULL},
            {"ctxData",     STD_OFFSETS(HASH_REQ, ctxBytes, outBytes, ctxData, outData), Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                            Read,  NULL},
            {"inData",      STD_OFFSETS(HASH_REQ, inBytes,  inBytes,  inData,  inData),  Read,  NULL},
            {"cmpData",     STD_OFFSETS(HASH_REQ, outBytes, outBytes, cmpData, cmpData), Read,  NULL},
            {"outData",     STD_OFFSETS(HASH_REQ, outBytes, outBytes, outData, outData), Write, NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                            Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  ctxData               ctxBytes           (none)
     * p2  (none)                (none)             (none)
     * p3  inData                inBytes            (none)
     * p4  cmpData               outBytes           (none)
     * p5  outData               outBytes           (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP,
        "DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP",
        sizeof(HASH_REQ),
        MdhaPadReq,
        offsetof(HASH_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                            Read,  NULL},
            {"ctxData",     STD_OFFSETS(HASH_REQ, ctxBytes, ctxBytes, ctxData, ctxData), Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                            Read,  NULL},
            {"inData",      STD_OFFSETS(HASH_REQ, inBytes,  inBytes,  inData,  inData),  Read,  NULL},
            {"cmpData",     STD_OFFSETS(HASH_REQ, outBytes, outBytes, cmpData, cmpData), Read,  NULL},
            {"outData",     STD_OFFSETS(HASH_REQ, outBytes, outBytes, outData, outData), Write, NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                            Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_HASH_LDCTX_HMAC_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  (none)                (none)             (none)
     * p2  keyData               keyBytes           (none)
     * p3  inData                inBytes            (none)
     * p4  cmpData               outBytes           (none)
     * p5  outData               outBytes           (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_HASH_LDCTX_HMAC_ULCTX_GROUP,
        "DPD_HASH_LDCTX_HMAC_ULCTX_GROUP",
        sizeof(HMAC_PAD_REQ),
        HmacPadReq,
        offsetof(HMAC_PAD_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                Read,  NULL},
            {"keyData",     STD_OFFSETS(HMAC_PAD_REQ, keyBytes, keyBytes, keyData, keyData), Read,  NULL},
            {"inData",      STD_OFFSETS(HMAC_PAD_REQ, inBytes,  inBytes,  inData,  inData),  Read,  NULL},
            {"cmpData",     STD_OFFSETS(HMAC_PAD_REQ, outBytes, outBytes, cmpData, cmpData), Read,  NULL},
            {"outData",     STD_OFFSETS(HMAC_PAD_REQ, outBytes, outBytes, outData, outData), Write, NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_MM_LDCTX_EXP_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  modData               modBytes           (none)
     * p1  (none)                (none)             (none)
     * p2  aData                 aDataBytes         (none)
     * p3  expData               expBytes           (none)
     * p4  outData               outBytes           (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_MM_LDCTX_EXP_ULCTX_GROUP,
        "DPD_MM_LDCTX_EXP_ULCTX_GROUP",
        sizeof(MOD_EXP_REQ),
        PkhaMmExpReq,
        offsetof(MOD_EXP_REQ, outBytes),
        {
            {"modData",     STD_OFFSETS(MOD_EXP_REQ, modBytes,   modBytes,   modData, modData), Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                   Read,  NULL},
            {"aData",       STD_OFFSETS(MOD_EXP_REQ, aDataBytes, aDataBytes, aData,   aData),   Read,  NULL},
            {"expData",     STD_OFFSETS(MOD_EXP_REQ, expBytes,   expBytes,   expData, expData), Read,  NULL},
            {"outData",     STD_OFFSETS(MOD_EXP_REQ, outBytes,   outBytes,   outData, outData), Write, NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                   Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                   Read,  NULL}
        },
    },
    
  
  
    
    /*
     * DPD_MM_SS_RSA_EXP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  modData               modBytes           (none)
     * p1  (none)                (none)             (none)
     * p2  aData                 aDataBytes         (none)
     * p3  expData               expBytes           (none)
     * p4  bData                 bDataBytes         (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_MM_SS_RSA_EXP,
        "DPD_MM_SS_RSA_EXP",
        sizeof(MOD_SS_EXP_REQ),
        PkhaMmSsExpReq,
        0,
        {
            {"modData",     STD_OFFSETS(MOD_SS_EXP_REQ, modBytes,   modBytes,   modData, modData), Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                      Read,  NULL},
            {"aData",       STD_OFFSETS(MOD_SS_EXP_REQ, aDataBytes, aDataBytes, aData,   aData),   Read,  NULL},
            {"expData",     STD_OFFSETS(MOD_SS_EXP_REQ, expBytes,   expBytes,   expData, expData), Read,  NULL},
            {"bData",       STD_OFFSETS(MOD_SS_EXP_REQ, bDataBytes, bDataBytes, bData,   bData),   Write, NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                      Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                      Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_MM_LDCTX_R2MODN_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  modData               modBytes           (none)
     * p1  (none)                (none)             (none)
     * p2  (none)                (none)             (none)
     * p3  (none)                (none)             (none)
     * p4  outData               outBytes           (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_MM_LDCTX_R2MODN_ULCTX_GROUP,
        "DPD_MM_LDCTX_R2MODN_ULCTX_GROUP",
        sizeof(MOD_R2MODN_REQ),
        PkhaModR2modnReq,
        offsetof(MOD_R2MODN_REQ, outBytes),
        {
            {"modData",     STD_OFFSETS(MOD_R2MODN_REQ, modBytes, modBytes, modData, modData), Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                  Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                  Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                  Read,  NULL},
            {"outData",     STD_OFFSETS(MOD_R2MODN_REQ, outBytes, outBytes, outData, outData), Write, NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                  Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                  Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_MM_LDCTX_RRMODP_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  pData                 pBytes             (none)
     * p1  (none)                (none)             (none)
     * p2  (none)                (none)             (none)
     * p3  NULL                  nBytes             (none)
     * p4  outData               outBytes           (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_MM_LDCTX_RRMODP_ULCTX_GROUP,
        "DPD_MM_LDCTX_RRMODP_ULCTX_GROUP",
        sizeof(MOD_RRMODP_REQ),
        PkhaMmRrmodpReq,
        offsetof(MOD_RRMODP_REQ, outBytes),
        {
            {"pData",       STD_OFFSETS(MOD_RRMODP_REQ,      pBytes,   pBytes,   pData,   pData),   Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                       Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                       Read,  NULL},
            {"nBytes",      NULL_PTR_OFFSETS(MOD_RRMODP_REQ, nBytes,   nBytes),                     Read,  NULL},
            {"outData",     STD_OFFSETS(MOD_RRMODP_REQ,      outBytes, outBytes, outData, outData), Write, NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                       Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                       Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_MM_LDCTX_2OP_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  modData               modBytes           (none)
     * p1  bData                 bDataBytes         (none)
     * p2  aData                 aDataBytes         (none)
     * p3  (none)                (none)             (none)
     * p4  outData               outBytes           (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_MOD_LDCTX_2OP_ULCTX_GROUP,
        "DPD_MOD_LDCTX_2OP_ULCTX_GROUP",
        sizeof(MOD_2OP_REQ),
        PkhaMod2OpReq,
        offsetof(MOD_2OP_REQ, outBytes),
        {
            {"modData",     STD_OFFSETS(MOD_2OP_REQ, modBytes,   modBytes,   modData, modData), Read,  NULL},
            {"bData",       STD_OFFSETS(MOD_2OP_REQ, bDataBytes, bDataBytes, bData,   bData),   Read,  NULL},
            {"aData",       STD_OFFSETS(MOD_2OP_REQ, aDataBytes, aDataBytes, aData,   aData),   Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                   Read,  NULL},
            {"outData",     STD_OFFSETS(MOD_2OP_REQ, outBytes,   outBytes,   outData, outData), Write, NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                   Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                   Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_EC_LDCTX_kP_ULCTX_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  nData                 nDataBytes         (none)
     * p1  eData                 eDataBytes         (none)
     * p2  buildData             buildDataBytes     (none)
     * p3  b1Data                b1DataBytes        (none)
     * p4  b2Data                b2DataBytes        (none)
     * p5  b3Data                b3DataBytes        (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_EC_LDCTX_kP_ULCTX_GROUP,
        "DPD_EC_LDCTX_kP_ULCTX_GROUP",
        sizeof(ECC_POINT_REQ),
        PkhaEccPointReq,
        offsetof(ECC_POINT_REQ, b3DataBytes),
        {
            {"nData",     STD_OFFSETS(ECC_POINT_REQ, nDataBytes,     nDataBytes,     nData,     nData),     Read,  NULL},
            {"eData",     STD_OFFSETS(ECC_POINT_REQ, eDataBytes,     eDataBytes,     eData,     eData),     Read,  NULL},
            {"buildData", STD_OFFSETS(ECC_POINT_REQ, buildDataBytes, buildDataBytes, buildData, buildData), Read,  NULL},
            {"b1Data",    STD_OFFSETS(ECC_POINT_REQ, b1DataBytes,    b1DataBytes,    b1Data,    b1Data),    Write, NULL},
            {"b2Data",    STD_OFFSETS(ECC_POINT_REQ, b2DataBytes,    b2DataBytes,    b2Data,    b2Data),    Write, NULL},
            {"b3Data",    STD_OFFSETS(ECC_POINT_REQ, b3DataBytes,    b3DataBytes,    b3Data,    b3Data),    Write, NULL},
            {NULL,        ALL_ZERO_OFFSETS,                                                                 Read,  NULL}
        },
    },
    
    
    
    
    /*
     * DPD_EC_2OP_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  bData                 bDataBytes         (none)
     * p1  aData                 aDataBytes         (none)
     * p2  modData               modBytes           (none)
     * p3  outData               outBytes           (none)
     * p4  (none)                (none)             (none)
     * p5  (none)                (none)             (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_EC_2OP_GROUP,
        "DPD_EC_2OP_GROUP",
        sizeof(ECC_2OP_REQ),
        PkhaEcc2OpReq,
        offsetof(ECC_2OP_REQ, outBytes),
        {
            {"bData",       STD_OFFSETS(ECC_2OP_REQ, bDataBytes, bDataBytes, bData,   bData),   Read,  ChkEccLen},
            {"aData",       STD_OFFSETS(ECC_2OP_REQ, aDataBytes, aDataBytes, aData,   aData),   Read,  ChkEccLen},
            {"modData",     STD_OFFSETS(ECC_2OP_REQ, modBytes,   modBytes,   modData, modData), Read,  ChkEccLen},
            {"outData",     STD_OFFSETS(ECC_2OP_REQ, outBytes,   outBytes,   outData, outData), Write, ChkEccLen},
            {NIL,           ALL_ZERO_OFFSETS,                                                   Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                                   Read,  NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                   Read,  NULL}
        },
    },




    /*
     * DPD_EC_SPKBUILD_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  a0Data                a0DataBytes        (none)
     * p1  a1Data                a1DataBytes        (none)
     * p2  a2Data                a2DataBytes        (none)
     * p3  a3Data                a3DataBytes        (none)
     * p4  b0Data                b0DataBytes        (none)
     * p5  b1Data                b1DataBytes        (none)
     * p6  buildData             buildDataBytes     (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_EC_SPKBUILD_GROUP,
        "DPD_EC_SPKBUILD_GROUP",
        sizeof(ECC_SPKBUILD_REQ),
        PkhaEccSpkbuildReq,
        offsetof(ECC_SPKBUILD_REQ, buildDataBytes),
        {
            {"a0Data",    STD_OFFSETS(ECC_SPKBUILD_REQ, a0DataBytes,    a0DataBytes,    a0Data,    a0Data),    Read,  NULL},
            {"a1Data",    STD_OFFSETS(ECC_SPKBUILD_REQ, a1DataBytes,    a1DataBytes,    a1Data,    a1Data),    Read,  NULL},
            {"a2Data",    STD_OFFSETS(ECC_SPKBUILD_REQ, a2DataBytes,    a2DataBytes,    a2Data,    a2Data),    Read,  NULL},
            {"a3Data",    STD_OFFSETS(ECC_SPKBUILD_REQ, a3DataBytes,    a3DataBytes,    a3Data,    a3Data),    Read,  NULL},
            {"b0Data",    STD_OFFSETS(ECC_SPKBUILD_REQ, b0DataBytes,    b0DataBytes,    b0Data,    b0Data),    Read,  NULL},
            {"b1Data",    STD_OFFSETS(ECC_SPKBUILD_REQ, b1DataBytes,    b1DataBytes,    b1Data,    b1Data),    Read,  NULL},
            {"buildData", STD_OFFSETS(ECC_SPKBUILD_REQ, buildDataBytes, buildDataBytes, buildData, buildData), Write, NULL},
        },
    },




    /*
     * DPD_EC_PTADD_DBL_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  modData               modBytes           (none)
     * p1  buildData             buildDataBytes     (none)
     * p2  b2InData              b2InDataBytes      (none)
     * p3  b3InData              b3InDataBytes      (none)
     * p4  b1Data                b1DataBytes        (none)
     * p5  b2Data                b2DataBytes        (none)
     * p6  b3Data                b3DataBytes        (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_EC_PTADD_DBL_GROUP,
        "DPD_EC_PTADD_DBL_GROUP",
        sizeof(ECC_PTADD_DBL_REQ),
        PkhaEccPtaddDblReq,
        offsetof(ECC_PTADD_DBL_REQ, b3DataBytes),
        {
            {"modData",   STD_OFFSETS(ECC_PTADD_DBL_REQ, modBytes,       modBytes,       modData,   modData),   Read,  NULL},
            {"buildData", STD_OFFSETS(ECC_PTADD_DBL_REQ, buildDataBytes, buildDataBytes, buildData, buildData), Read,  NULL},
            {"b2InData",  STD_OFFSETS(ECC_PTADD_DBL_REQ, b2InDataBytes,  b2InDataBytes,  b2InData,  b2InData),  Read,  NULL},
            {"b3InData",  STD_OFFSETS(ECC_PTADD_DBL_REQ, b3InDataBytes,  b3InDataBytes,  b3InData,  b3InData),  Read,  NULL},
            {"b1Data",    STD_OFFSETS(ECC_PTADD_DBL_REQ, b1DataBytes,    b1DataBytes,    b1Data,    b1Data),    Write, NULL},
            {"b2Data",    STD_OFFSETS(ECC_PTADD_DBL_REQ, b2DataBytes,    b2DataBytes,    b2Data,    b2Data),    Write, NULL},
            {"b3Data",    STD_OFFSETS(ECC_PTADD_DBL_REQ, b3DataBytes,    b3DataBytes,    b3Data,    b3Data),    Write, NULL},
        },
    },





    /*
     * DPD_IPSEC_CBC_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       (none)
     * p1  hashInData            hashInDataBytes    (none)
     * p2  cryptKeyData          cryptKeyBytes      (none)
     * p3  cryptCtxInData        cryptCtxInBytes    (none)
     * p4  inData                inDataBytes        (none)
     * p5  cryptDataOut          inDataBytes        (none)
     * p6  hashDataOut           hashDataOutBytes   (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_IPSEC_CBC_GROUP,
        "DPD_IPSEC_CBC_GROUP",
        sizeof(IPSEC_CBC_REQ),
        IpsecCbcReq,
        offsetof(IPSEC_CBC_REQ, inDataBytes),
        {
            {"hashKeyData",    STD_OFFSETS(IPSEC_CBC_REQ, hashKeyBytes,     hashKeyBytes,     hashKeyData,    hashKeyData),    Read,  NULL},
            {"hashInData",     STD_OFFSETS(IPSEC_CBC_REQ, hashInDataBytes,  hashInDataBytes,  hashInData,     hashInData),     Read,  NULL},
            {"cryptKeyData",   STD_OFFSETS(IPSEC_CBC_REQ, cryptKeyBytes,    cryptKeyBytes,    cryptKeyData,   cryptKeyData),   Read,  ChkDesKeyLen},
            {"cryptCtxInData", STD_OFFSETS(IPSEC_CBC_REQ, cryptCtxInBytes,  cryptCtxInBytes,  cryptCtxInData, cryptCtxInData), Read,  ChkDesCtxLen},
            {"inData",         STD_OFFSETS(IPSEC_CBC_REQ, inDataBytes,      inDataBytes,      inData,         inData),         Read,  ChkDesStaticDataLen},
            {"cryptDataOut",   STD_OFFSETS(IPSEC_CBC_REQ, inDataBytes,      inDataBytes,      cryptDataOut,   cryptDataOut),   Write, NULL},
            {"hashDataOut",    STD_OFFSETS(IPSEC_CBC_REQ, hashDataOutBytes, hashDataOutBytes, hashDataOut,    hashDataOut),    Write, NULL}
        },
    },





    /*
     * DPD_IPSEC_ECB_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       (none)
     * p1  hashInData            hashInDataBytes    (none)
     * p2  cryptKeyData          cryptKeyBytes      (none)
     * p3  (none)                (none)             (none)
     * p4  inData                inDataBytes        (none)
     * p5  cryptDataOut          inDataBytes        (none)
     * p6  hashDataOut           hashDataOutBytes   (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_IPSEC_ECB_GROUP,
        "DPD_IPSEC_ECB_GROUP",
        sizeof(IPSEC_ECB_REQ),
        IpsecEcbReq,
        offsetof(IPSEC_ECB_REQ, inDataBytes),
        {
            {"hashKeyData",  STD_OFFSETS(IPSEC_ECB_REQ, hashKeyBytes,     hashKeyBytes,     hashKeyData,  hashKeyData),  Read,  NULL},
            {"hashInData",   STD_OFFSETS(IPSEC_ECB_REQ, hashInDataBytes,  hashInDataBytes,  hashInData,   hashInData),   Read,  NULL},
            {"cryptKeyData", STD_OFFSETS(IPSEC_ECB_REQ, cryptKeyBytes,    cryptKeyBytes,    cryptKeyData, cryptKeyData), Read,  ChkDesKeyLen},
            {NIL,            ALL_ZERO_OFFSETS,                                                                           Read,  NULL},
            {"inData",       STD_OFFSETS(IPSEC_ECB_REQ, inDataBytes,      inDataBytes,      inData,       inData),       Read,  ChkDesStaticDataLen},
            {"cryptDataOut", STD_OFFSETS(IPSEC_ECB_REQ, inDataBytes,      inDataBytes,      cryptDataOut, cryptDataOut), Write, NULL},
            {"hashDataOut",  STD_OFFSETS(IPSEC_ECB_REQ, hashDataOutBytes, hashDataOutBytes, hashDataOut,  hashDataOut),  Write, NULL}
        },
    },
    
    
    
    
    /*
     * DPD_IPSEC_ESP_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       (none)
     * p1  hashKeyData           hashInDataBytes    (none)
     * p2  cryptCtxInData        cryptCtxInBytes    (none)
     * p3  cryptKeyData          cryptKeyBytes      (none)
     * p4  inData                inDataBytes        (none)
     * p5  cryptDataOut          inDataBytes        hashOutDataBytes
     * p6  cryptCtxOutData       cryptCtxOutBytes   (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_IPSEC_ESP_GROUP,
        "DPD_IPSEC_ESP_GROUP",
        sizeof(IPSEC_ESP_REQ),
        IpsecEspReq,
        offsetof(IPSEC_ESP_REQ, inDataBytes),
        {
            {"hashKeyData",     STD_OFFSETS(IPSEC_ESP_REQ, hashKeyBytes,     hashKeyBytes,     hashKeyData,     hashKeyData),      Read,  NULL},
            {"hashInData",      STD_OFFSETS(IPSEC_ESP_REQ, hashInDataBytes,  hashInDataBytes,  hashKeyData,     hashKeyData),      Read,  NULL},
            {"cryptCtxInData",  STD_OFFSETS(IPSEC_ESP_REQ, cryptCtxInBytes,  cryptCtxInBytes,  cryptCtxInData,  cryptCtxInData),   Read,  ChkDesCtxLen},
            {"cryptKeyData",    STD_OFFSETS(IPSEC_ESP_REQ, cryptKeyBytes,    cryptKeyBytes,    cryptKeyData,    cryptKeyData),     Read,  ChkDesKeyLen},
            {"inData",          STD_OFFSETS(IPSEC_ESP_REQ, inDataBytes,      inDataBytes,      inData,          inData),           Read,  ChkDesStaticDataLen},
            {"cryptDataOut",    EXT_OFFSETS(IPSEC_ESP_REQ, inDataBytes,      inDataBytes,      cryptDataOut,    hashDataOutBytes), Write, NULL},
            {"cryptCtxOutData", STD_OFFSETS(IPSEC_ESP_REQ, cryptCtxOutBytes, cryptCtxOutBytes, cryptCtxOutData, cryptCtxOutData),  Write, ChkDesCtxLen}
        },
    },
    
    


    /*
     * DPD_IPSEC_AES_CBC_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       (none)
     * p1  hashInData            hashInDataBytes    (none)
     * p2  cryptKeyData          cryptKeyBytes      (none)
     * p3  cryptCtxInData        cryptCtxInBytes    (none)
     * p4  inData                inDataBytes        (none)
     * p5  cryptDataOut          inDataBytes        (none)
     * p6  hashDataOut           hashDataOutBytes   (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_IPSEC_AES_CBC_GROUP,
        "DPD_IPSEC_AES_CBC_GROUP",
        sizeof(IPSEC_AES_CBC_REQ),
        IpsecAesCbcReq,
        offsetof(IPSEC_AES_CBC_REQ, inDataBytes),
        {
            {"hashKeyData",    STD_OFFSETS(IPSEC_CBC_REQ, hashKeyBytes,     hashKeyBytes,     hashKeyData,    hashKeyData),    Read,  NULL},
            {"hashInData",     STD_OFFSETS(IPSEC_CBC_REQ, hashInDataBytes,  hashInDataBytes,  hashInData,     hashInData),     Read,  NULL},
            {"cryptKeyData",   STD_OFFSETS(IPSEC_CBC_REQ, cryptKeyBytes,    cryptKeyBytes,    cryptKeyData,   cryptKeyData),   Read,  ChkAesKeyLen},
            {"cryptCtxInData", STD_OFFSETS(IPSEC_CBC_REQ, cryptCtxInBytes,  cryptCtxInBytes,  cryptCtxInData, cryptCtxInData), Read,  ChkAesIvLen},
            {"inData",         STD_OFFSETS(IPSEC_CBC_REQ, inDataBytes,      inDataBytes,      inData,         inData),         Read,  ChkDesStaticDataLen},
            {"cryptDataOut",   STD_OFFSETS(IPSEC_CBC_REQ, inDataBytes,      inDataBytes,      cryptDataOut,   cryptDataOut),   Write, NULL},
            {"hashDataOut",    STD_OFFSETS(IPSEC_CBC_REQ, hashDataOutBytes, hashDataOutBytes, hashDataOut,    hashDataOut),    Write, NULL}
        },
    },
    
    
    
    
    /*
     * DPD_IPSEC_AES_ECB_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       (none)
     * p1  hashInData            hashInDataBytes    (none)
     * p2  cryptKeyData          cryptKeyBytes      (none)
     * p3  (none)                (none)             (none)
     * p4  inData                inDataBytes        (none)
     * p5  cryptDataOut          inDataBytes        (none)
     * p6  hashDataOut           hashDataOutBytes   (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_IPSEC_AES_ECB_GROUP,
        "DPD_IPSEC_AES_ECB_GROUP",
        sizeof(IPSEC_AES_ECB_REQ),
        IpsecAesEcbReq,
        offsetof(IPSEC_AES_ECB_REQ, inDataBytes),
        {
            {"hashKeyData",  STD_OFFSETS(IPSEC_AES_ECB_REQ, hashKeyBytes,     hashKeyBytes,     hashKeyData,  hashKeyData),  Read,  NULL},
            {"hashInData",   STD_OFFSETS(IPSEC_AES_ECB_REQ, hashInDataBytes,  hashInDataBytes,  hashInData,   hashInData),   Read,  NULL},
            {"cryptKeyData", STD_OFFSETS(IPSEC_AES_ECB_REQ, cryptKeyBytes,    cryptKeyBytes,    cryptKeyData, cryptKeyData), Read,  ChkAesKeyLen},
            {NIL,            ALL_ZERO_OFFSETS,                                                                               Read,  NULL},
            {"inData",       STD_OFFSETS(IPSEC_AES_ECB_REQ, inDataBytes,      inDataBytes,      inData,       inData),       Read,  ChkDesStaticDataLen},
            {"cryptDataOut", STD_OFFSETS(IPSEC_AES_ECB_REQ, inDataBytes,      inDataBytes,      cryptDataOut, cryptDataOut), Write, NULL},
            {"hashDataOut",  STD_OFFSETS(IPSEC_AES_ECB_REQ, hashDataOutBytes, hashDataOutBytes, hashDataOut,  hashDataOut),  Write, NULL}
        },
    },





    /*
     * DPD_AESA_CRYPT_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  inIvData              inIvBytes          (none)
     * p2  keyData               keyBytes           (none)
     * p3  inData                inBytes            (none)
     * p4  outData               inBytes            (none)
     * p5  outCtxData            outCtxBytes        (none)
     * p6  (none)                (none)             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_AESA_CRYPT_GROUP,
        "DPD_AESA_CRYPT_GROUP",
        sizeof(AESA_CRYPT_REQ),
        AesaDesc,
        offsetof(AESA_CRYPT_REQ, inBytes),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                              Read,  NULL},
            {"inIvData",    STD_OFFSETS(AESA_CRYPT_REQ, inIvBytes,   inIvBytes,   inIvData,   inIvData),   Read,  ChkAesIvLen},
            {"keyData",     STD_OFFSETS(AESA_CRYPT_REQ, keyBytes,    keyBytes,    keyData,    keyData),    Read,  ChkAesKeyLen},
            {"inData",      STD_OFFSETS(AESA_CRYPT_REQ, inBytes,     inBytes,     inData,     inData),     Read,  0},
            {"outData",     STD_OFFSETS(AESA_CRYPT_REQ, inBytes,     inBytes,     outData,    outData),    Write, NULL},
            {"outCtxData",  STD_OFFSETS(AESA_CRYPT_REQ, outCtxBytes, outCtxBytes, outCtxData, outCtxData), Write, NULL},
            {NULL,          ALL_ZERO_OFFSETS,                                                              Read,  NULL}
        },
    },
    
    
    

    /*
     * DPD_CCMP_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  context               ctxBytes           (none)
     * p2  keyData               keyBytes           (none)
     * p3  AADData               AADBytes           (none)
     * p4  FrameData             FrameDataBytes     (none)
     * p5  cryptDataOut          cryptDataBytes     (none)
     * p6  MICData               MICBytes           (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_CCMP_GROUP,
        "DPD_CCMP_GROUP",
        sizeof(CCMP_REQ),
        CcmpReq,
        offsetof(CCMP_REQ, MICData),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                                  Read,  NULL},
            {"context",     STD_OFFSETS(CCMP_REQ, ctxBytes,       ctxBytes,       context,      context),      Read,  NULL},
            {"keyData",     STD_OFFSETS(CCMP_REQ, keyBytes,       keyBytes,       keyData,      keyData),      Read,  ChkCcmpKeyLen},
            {"AADData",     STD_OFFSETS(CCMP_REQ, AADBytes,       AADBytes,       AADData,      AADData),      Read,  0},
            {"FrameData",   STD_OFFSETS(CCMP_REQ, FrameDataBytes, FrameDataBytes, FrameData,    FrameData),    Read,  0},
            {"cryptDataOut",STD_OFFSETS(CCMP_REQ, cryptDataBytes, cryptDataBytes, cryptDataOut, cryptDataOut), Write, NULL},
            {"MICData",     STD_OFFSETS(CCMP_REQ, MICBytes,       MICBytes,       MICData,      MICData),      Write, NULL}
        },
    },
    
    
    
    
    /* 
     * DPD_SRTP_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       (none)
     * p1  ivData                ivBytes            (none)
     * p2  keyData               keyBytes           (none)
     * p3  inData                inBytes            HeaderBytes
     * p4  cryptDataOut          cryptDataBytes     ROCBytes
     * p5  digestData            digestBytes        (none)
     * p6  outIvData             outIvBytes         (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_SRTP_GROUP,
        "DPD_SRTP_GROUP",
        sizeof(SRTP_REQ),
        SrtpReq,
        offsetof(SRTP_REQ, outIvData),
        {
            {"hashKeyData", STD_OFFSETS(SRTP_REQ, hashKeyBytes,   hashKeyBytes,   hashKeyData,  hashKeyData), Read,  NULL},
            {"ivData",      STD_OFFSETS(SRTP_REQ, ivBytes,        ivBytes,        ivData,       ivData),      Read,  NULL},
            {"keyData",     STD_OFFSETS(SRTP_REQ, keyBytes,       keyBytes,       keyData,      keyData),     Read,  ChkAesKeyLen},
            {"inData",      EXT_OFFSETS(SRTP_REQ, inBytes,        inBytes,        inData,       HeaderBytes), Read,  0},
            {"cryptDataOut",EXT_OFFSETS(SRTP_REQ, cryptDataBytes, cryptDataBytes, cryptDataOut, ROCBytes),    Write, NULL},
            {"digestData",  STD_OFFSETS(SRTP_REQ, digestBytes,    digestBytes,    digestData,   digestData),  Read,  0},
            {"outIvData",   STD_OFFSETS(SRTP_REQ, outIvBytes,     outIvBytes,     outIvData,    outIvData),   Write, NULL}
        },
    },
    
    
    


    /*
     * DPD_KEA_CRYPT_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  ivData                ivBytes            (none)
     * p2  keyData               keyBytes           (none)
     * p3  inData                inBytes            (none)
     * p4  outData               outBytes           (none)
     * p5  (none)                (none)             (none)
     * p6  ctxData               ctxBytes           (none)
     * ----------------------------------------------------------------
     */
    { 
        DPD_KEA_CRYPT_GROUP,
        "DPD_KEA_CRYPT_GROUP",
        sizeof(KEA_CRYPT_REQ),
        KeaReq,
        offsetof(KEA_CRYPT_REQ, ctxData),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                                         Read,  NULL},
            {"ivData",      STD_OFFSETS(KEA_CRYPT_REQ, ivBytes,    ivBytes,    ivData,    ivData),    Read,  NULL},
            {"keyData",     STD_OFFSETS(KEA_CRYPT_REQ, keyBytes,   keyBytes,   keyData,   keyData),   Read,  NULL},
            {"inData",      STD_OFFSETS(KEA_CRYPT_REQ, inBytes,    inBytes,    inData,    inData),    Read,  NULL},
            {"outData",     STD_OFFSETS(KEA_CRYPT_REQ, outBytes,   outBytes,   outData,   outData),   Write, NULL},
            {"outIvData",   STD_OFFSETS(KEA_CRYPT_REQ, outIvBytes, outIvBytes, outIvData, outIvData), Write, NULL},
            {"ctxData",     STD_OFFSETS(KEA_CRYPT_REQ, ctxBytes,   ctxBytes,   ctxData,   ctxData),   Write, NULL}
        },
    },


    /*
     * DPD_RAID_XOR_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  (none)                (none)             (none)
     * p1  (none)                (none)             (none)
     * p2  (none)                (none)             (none)
     * p3  inDataA               opSize             (none)
     * p4  inDataB               opSize             (none)
     * p5  inDataC               opSize             (none)
     * p6  outData               opSize             (none)
     * ----------------------------------------------------------------
     */
    {
        DPD_RAID_XOR_GROUP,
        "DPD_RAID_XOR_GROUP",
        sizeof(RAID_XOR_REQ),
        RaidXorReq,
        offsetof(RAID_XOR_REQ, outData),
        {
            {NIL,           ALL_ZERO_OFFSETS,                                            Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                            Read,  NULL},
            {NIL,           ALL_ZERO_OFFSETS,                                            Read,  NULL},
            {"inDataA",     STD_OFFSETS(RAID_XOR_REQ, opSize, opSize, inDataA, inDataA), Read,  NULL},
            {"inDataB",     STD_OFFSETS(RAID_XOR_REQ, opSize, opSize, inDataB, inDataB), Read,  NULL},
            {"inDataC",     STD_OFFSETS(RAID_XOR_REQ, opSize, opSize, inDataC, inDataC), Read,  NULL},
            {"outData",     STD_OFFSETS(RAID_XOR_REQ, opSize, opSize, outData, outData), Write, NULL},
        },
    },


    /*
     * DPD_TLS_BLOCK_INBOUND_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       
     * p1  hashOnlyData                             hashOnlyBytes
     * p2  ivData                ivBytes
     * p3  cipherKeyData         cipherKeyBytes
     * p4  inData                inBytes            MACcmpBytes
     * p5  outData               outBytes           MACoutBytes
     * p6  ivOutData             ivOutBytes
     * ----------------------------------------------------------------
     */
    { 
        DPD_TLS_BLOCK_INBOUND_GROUP,
        "DPD_TLS_BLOCK_INBOUND_GROUP",
        sizeof(TLS_BLOCK_INBOUND_REQ),
        TlsBlockInboundReq,
        offsetof(TLS_BLOCK_INBOUND_REQ, ivOutData),
        {
            {"hashKeyData",   STD_OFFSETS(TLS_BLOCK_INBOUND_REQ, hashKeyBytes,   hashKeyBytes,   hashKeyData,   hashKeyData),   Read,  NULL},
            {"hashOnlyData",  EXT_OFFSETS(TLS_BLOCK_INBOUND_REQ, hashOnlyBytes,  hashOnlyBytes,  hashOnlyData,  hashOnlyBytes), Read,  NULL},
            {"ivData",        STD_OFFSETS(TLS_BLOCK_INBOUND_REQ, ivBytes,        ivBytes,        ivData,        ivData),        Read,  NULL},
            {"cipherKeyData", STD_OFFSETS(TLS_BLOCK_INBOUND_REQ, cipherKeyBytes, cipherKeyBytes, cipherKeyData, cipherKeyData), Read,  NULL},
            {"inData",        EXT_OFFSETS(TLS_BLOCK_INBOUND_REQ, inBytes,        inBytes,        inData,        MACcmpBytes),   Read,  NULL},
            {"outData",       EXT_OFFSETS(TLS_BLOCK_INBOUND_REQ, outBytes,       outBytes,       outData,       MACoutBytes),   Write, NULL},
            {"ivOutData",     STD_OFFSETS(TLS_BLOCK_INBOUND_REQ, ivOutBytes,     ivOutBytes,     ivOutData,     ivOutData),     Write, NULL}
        },
    },
     
     
     
    /*
     * DPD_TLS_BLOCK_OUTBOUND_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       
     * p1  ivData                ivBytes
     * p2  cipherKeyData         cipherKeyBytes
     * p3  hashOnlyData          mainDataBytes      hashOnlyBytes
     * p4  cipherOnlyData        cipherOnlyBytes    MACbytes
     * p5  outData               outBytes
     * p6  ivOutData             ivOutBytes
     * ----------------------------------------------------------------
     */
    { 
        DPD_TLS_BLOCK_OUTBOUND_GROUP,
        "DPD_TLS_BLOCK_OUTBOUND_GROUP",
        sizeof(TLS_BLOCK_OUTBOUND_REQ),
        TlsBlockOutboundReq,
        offsetof(TLS_BLOCK_OUTBOUND_REQ, ivOutData),
        {
            {"hashKeyData",    STD_OFFSETS(TLS_BLOCK_OUTBOUND_REQ, hashKeyBytes,    hashKeyBytes,    hashKeyData,    hashKeyData),    Read,  NULL},
            {"ivData",         STD_OFFSETS(TLS_BLOCK_OUTBOUND_REQ, ivBytes,         ivBytes,         ivData,         ivData),         Read,  NULL},
            {"cipherKeyData",  STD_OFFSETS(TLS_BLOCK_OUTBOUND_REQ, cipherKeyBytes,  cipherKeyBytes,  cipherKeyData,  cipherKeyData),  Read,  NULL},
            {"hashOnlyData",   EXT_OFFSETS(TLS_BLOCK_OUTBOUND_REQ, mainDataBytes,   mainDataBytes,   hashOnlyData,   hashOnlyBytes),  Read,  NULL},
            {"cipherOnlyData", EXT_OFFSETS(TLS_BLOCK_OUTBOUND_REQ, cipherOnlyBytes, cipherOnlyBytes, cipherOnlyData, MACbytes),       Write, NULL},
            {"outData",        STD_OFFSETS(TLS_BLOCK_OUTBOUND_REQ, outBytes,        outBytes,        outData,        outData),        Read,  NULL},
            {"ivOutData",      STD_OFFSETS(TLS_BLOCK_OUTBOUND_REQ, ivOutBytes,      ivOutBytes,      ivOutData,      ivOutData),      Write, NULL}
        },
    },
     
     
     
     
     
    /*
     * DPD_TLS_STREAM_INBOUND_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       
     * p1  hashOnlyData                             hashOnlyBytes
     * p2  ivData                ivBytes
     * p3  cipherKeyData         cipherKeyBytes
     * p4  inData                inBytes            MACcmpBytes
     * p5  outData               outBytes           MACoutBytes
     * p6  ivOutData             ivOutBytes
     * ----------------------------------------------------------------
     */
    { 
        DPD_TLS_STREAM_INBOUND_GROUP,
        "DPD_TLS_STREAM_INBOUND_GROUP",
        sizeof(TLS_STREAM_INBOUND_REQ),
        TlsStreamInboundReq,
        offsetof(TLS_STREAM_INBOUND_REQ, ivOutBytes),
        {
            {"hashKeyData",   STD_OFFSETS(TLS_STREAM_INBOUND_REQ, hashKeyBytes,   hashKeyBytes,   hashKeyData,   hashKeyData),   Read,  NULL},
            {"hashOnlyData",  EXT_OFFSETS(TLS_STREAM_INBOUND_REQ, hashOnlyBytes,  hashOnlyBytes,  hashOnlyData,  hashOnlyBytes), Read,  NULL},
            {"ivData",        STD_OFFSETS(TLS_STREAM_INBOUND_REQ, ivBytes,        ivBytes,        ivData,        ivData),        Read,  NULL},
            {"cipherKeyData", STD_OFFSETS(TLS_STREAM_INBOUND_REQ, cipherKeyBytes, cipherKeyBytes, cipherKeyData, cipherKeyData), Read,  NULL},
            {"inData",        EXT_OFFSETS(TLS_STREAM_INBOUND_REQ, inBytes,        inBytes,        inData,        MACcmpBytes),   Read,  NULL},
            {"outData",       EXT_OFFSETS(TLS_STREAM_INBOUND_REQ, outBytes,       outBytes,       outData,       MACoutBytes),   Write, NULL},
            {"ivOutData",     STD_OFFSETS(TLS_STREAM_INBOUND_REQ, ivOutBytes,     ivOutBytes,     ivOutData,     ivOutData),     Write, NULL}
        },
    },




     
     
    /*
     * DPD_TLS_STREAM_OUTBOUND_GROUP
     *     pointer               length             extent
     * ----------------------------------------------------------------
     * p0  hashKeyData           hashKeyBytes       
     * p1  ivData                ivBytes
     * p2  cipherKeyData         cipherKeyBytes
     * p3  hashOnlyData          mainDataBytes      hashOnlyBytes
     * p4  outData               outBytes           MACbytes
     * p5  
     * p6  ivOutData             ivOutBytes
     * ----------------------------------------------------------------
     */
    {
        DPD_TLS_STREAM_OUTBOUND_GROUP, "DPD_TLS_STREAM_OUTBOUND_GROUP",
        sizeof(TLS_STREAM_OUTBOUND_REQ), TlsStreamOutboundReq,
        offsetof(TLS_STREAM_OUTBOUND_REQ, ivOutBytes),
        {
            {"hashKeyData",    STD_OFFSETS(TLS_STREAM_OUTBOUND_REQ, hashKeyBytes,   hashKeyBytes,   hashKeyData,   hashKeyData),   Read,  NULL},
            {"ivData",         STD_OFFSETS(TLS_STREAM_OUTBOUND_REQ, ivBytes,        ivBytes,        ivData,        ivData),        Read,  NULL},
            {"cipherKeyData",  STD_OFFSETS(TLS_STREAM_OUTBOUND_REQ, cipherKeyBytes, cipherKeyBytes, cipherKeyData, cipherKeyData), Read,  NULL},
            {"hashOnlyData",   EXT_OFFSETS(TLS_STREAM_OUTBOUND_REQ, mainDataBytes,  mainDataBytes,  hashOnlyData,  hashOnlyBytes), Read,  NULL},
            {"outData",        EXT_OFFSETS(TLS_STREAM_OUTBOUND_REQ, outBytes,       outBytes,       outData,       MACbytes),      Write, NULL},
            {NIL,              ALL_ZERO_OFFSETS,                                                                                  Read,  NULL},
            {"ivOutData",      STD_OFFSETS(TLS_STREAM_OUTBOUND_REQ, ivOutBytes,     ivOutBytes,     ivOutData,     ivOutData),     Write, NULL}
        },
    },
     


    /* Always terminates list, keep this line at the end when adding functions */
    {
        0, NULL,
        0, NULL,
        0,
        {
            {NULL, ALL_ZERO_OFFSETS, Read, NULL},
        },
    },

};
