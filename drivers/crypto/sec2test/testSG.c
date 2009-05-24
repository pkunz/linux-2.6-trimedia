

/****************************************************************************
 * testSG.c - Simulates scatter-gather buffer test for SEC2 device driver
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
 * 1.2   02-Feb-2005 sec - fix types, allow for contig buffers in Linux
 * 1.3   Aug 11,2005 sec - fix up buffer locations
 */


/*
 * Note that for Linux targets (or any virtual-memory target for that
 * matter), this test cannot be run in user-mode, it must be run from
 * kernel mode only
 */
 
 
#include "sec2drvTest.h"
#include "Sec2.h"


static const unsigned char iv_in[] = "abcedf01";

static const unsigned char cipherKey[] =
{
  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
  0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0
};


/* Raw input string */
/* Basic string is 33 characters, repeated 48 times for 1584 characters */
/* Broken up into 7 chunks of varying sizes, out of natural order  */

/* segment 7 - 364 bytes */
static const unsigned char inp7[] = { "yallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboy" }; 

/* segment 4 - 64 bytes */
static const unsigned char inp4[] = { "andnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallwo" };

/* segment 2 - 256 bytes */
static const unsigned char inp2[] = { "akesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkan" };

/* segment 1 - 512 bytes */
static const unsigned char inp1[] = { "allworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaym" };

/* segment 3 - 64 bytes */
static const unsigned char inp3[] = { "dnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallwork" };

/* segment 6 - 196 bytes */
static const unsigned char inp6[] = { "llworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullbo" };

/* segment 5 - 128 bytes */
static const unsigned char inp5[] = { "rkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboyallworkandnoplaymakesJackadullboya" };



/* scatter lists */
EXT_SCATTER_ELEMENT inpList[7], outList[8], cmpList[2];


int testSG(int fd)
{
  DES_CBC_CRYPT_REQ  encReq;
  DES_CBC_CRYPT_REQ  decReq;
  int                opLen, status, result, cmpfail;
  unsigned char      ctx[8];


  /* cleartext input buffer */
  unsigned char *in7, *in4, *in2, *in1, *in3, *in6, *in5;
  
  /* ciphertext output buffer */
  unsigned char *out8, *out1, *out3, *out5, *out2, *out6, *out4, *out7;

  /* compare buffer */
  unsigned char *cmp2, *cmp1;

  



#ifdef _LINUX_USERMODE_
  printf("testSG() can't be used from user mode\n");
  result = -1;
#else

  printf("\n*** Test SG ***\n");
  opLen = 1584;  /* overall length */
  
  /* Clear out the request blocks, each direction */
  memset(&encReq, 0, sizeof(encReq));
  memset(&decReq, 0, sizeof(decReq));

  /* Alloc storage space in contiguous (possibly) kernel mem */
  in7  = (unsigned char *)malloc(364);
  in4  = (unsigned char *)malloc(64);
  in2  = (unsigned char *)malloc(256);
  in1  = (unsigned char *)malloc(512);
  in3  = (unsigned char *)malloc(64);
  in6  = (unsigned char *)malloc(196);
  in5  = (unsigned char *)malloc(128);

  out8 = (unsigned char *)malloc(48);
  out1 = (unsigned char *)malloc(128);
  out3 = (unsigned char *)malloc(256);
  out5 = (unsigned char *)malloc(256);
  out2 = (unsigned char *)malloc(128);
  out6 = (unsigned char *)malloc(256);
  out4 = (unsigned char *)malloc(256);
  out7 = (unsigned char *)malloc(256);
  
  cmp2 = (unsigned char *)malloc(688);
  cmp1 = (unsigned char *)malloc(896);


  if ((in7  == NULL) ||
      (in4  == NULL) ||
      (in2  == NULL) ||
      (in1  == NULL) ||
      (in3  == NULL) ||
      (in6  == NULL) ||
      (in5  == NULL) ||
      (out8 == NULL) ||
      (out1 == NULL) ||
      (out3 == NULL) ||
      (out5 == NULL) ||
      (out2 == NULL) ||
      (out6 == NULL) ||
      (out4 == NULL) ||
      (out7 == NULL) ||
      (cmp2 == NULL) ||
      (cmp1 == NULL))
  {
      free(in7);
      free(in4);
      free(in2);
      free(in1);
      free(in3);
      free(in6);
      free(in5);
      free(out8);
      free(out1);
      free(out3);
      free(out5);
      free(out2);
      free(out6);
      free(out4);
      free(out7);
      free(cmp2);
      free(cmp1);
  }

  /* copy input blocks to buffer */
  memcpy(in7, inp7, 364);
  memcpy(in4, inp4, 64);
  memcpy(in2, inp2, 256);
  memcpy(in1, inp1, 512);
  memcpy(in3, inp3, 64);
  memcpy(in6, inp6, 196);
  memcpy(in5, inp5, 128);
  

  /* cleartext input */
  inpList[0].size     = 512;
  inpList[0].fragment = (unsigned char *)in1;
  inpList[0].next     = &inpList[1];
  
  inpList[1].size     = 256;
  inpList[1].fragment = (unsigned char *)in2;
  inpList[1].next     = &inpList[2];
  
  inpList[2].size     = 64;
  inpList[2].fragment = (unsigned char *)in3;
  inpList[2].next     = &inpList[3];
  
  inpList[3].size     = 64;
  inpList[3].fragment = (unsigned char *)in4;
  inpList[3].next     = &inpList[4];
  
  inpList[4].size     = 128;
  inpList[4].fragment = (unsigned char *)in5;
  inpList[4].next     = &inpList[5];
  
  inpList[5].size     = 196;
  inpList[5].fragment = (unsigned char *)in6;
  inpList[5].next     = &inpList[6];
  
  inpList[6].size     = 364;
  inpList[6].fragment = (unsigned char *)in7;
  inpList[6].next     = NULL;




/* cipher output list (output first pass, input second pass) */
  outList[0].size     = 128;
  outList[0].fragment = out1;
  outList[0].next     = &outList[1];
  
  outList[1].size     = 128;
  outList[1].fragment = out2;
  outList[1].next     = &outList[2];

  outList[2].size     = 256;
  outList[2].fragment = out3;
  outList[2].next     = &outList[3];

  outList[3].size     = 256;
  outList[3].fragment = out4;
  outList[3].next     = &outList[4];

  outList[4].size     = 256;
  outList[4].fragment = out5;
  outList[4].next     = &outList[5];

  outList[5].size     = 256;
  outList[5].fragment = out6;
  outList[5].next     = &outList[6];

  outList[6].size     = 256;
  outList[6].fragment = out7;
  outList[6].next     = &outList[7];

  outList[7].size     = 48;
  outList[7].fragment = out8;
  outList[7].next     = NULL;



  /* cleartext comparison, two chunks */
  cmpList[0].size     = 896;
  cmpList[0].fragment = cmp1;
  cmpList[0].next     = &cmpList[1];
  
  cmpList[1].size     = 688;
  cmpList[1].fragment = cmp2;
  cmpList[1].next     = NULL;

 

  /* Scatter lists are built for both input and output */
  /* Set up the encryption request */
  encReq.opId            = DPD_TDES_CBC_CTX_ENCRYPT;
  encReq.inIvBytes       = 8;
  encReq.keyBytes        = 24;
  encReq.inBytes         = opLen;
  encReq.inIvData        = (unsigned char *)iv_in;
  encReq.keyData         = (unsigned char *)cipherKey;
  encReq.inData          = (unsigned char *)inpList;
  encReq.outIvBytes      = 8;
  encReq.outIvData       = ctx;
  encReq.outData         = (unsigned char *)outList;

  MarkScatterBuffer(&encReq, &encReq.inData);
  MarkScatterBuffer(&encReq, &encReq.outData);

  status = putKmem(fd, (void *)iv_in, (void **)&encReq.inIvData, encReq.inIvBytes);
  if (status)
    return status;

  status = putKmem(fd, (void *)cipherKey, (void **)&encReq.keyData, encReq.keyBytes);
  if (status) {
    freeKmem(fd, (void **)&encReq.inIvData);
    return status;
  }

  status = putKmem(fd, NULL, (void **)&encReq.outIvData, encReq.outIvBytes);
  if (status) {
    freeKmem(fd, (void **)&encReq.inIvData);
    freeKmem(fd, (void **)&encReq.keyData);
    return status;
  }

  armCompletion(&encReq);
  status = ioctl(fd, IOCTL_PROC_REQ, (int)&encReq);

  if ((status = waitCompletion("testSG(): DES encryption with scattered input", status, &encReq)))
  {
      free(in7);
      free(in4);
      free(in2);
      free(in1);
      free(in3);
      free(in6);
      free(in5);
      free(out8);
      free(out1);
      free(out3);
      free(out5);
      free(out2);
      free(out6);
      free(out4);
      free(out7);
      free(cmp2);
      free(cmp1);
      freeKmem(fd, (void **)&encReq.inIvData);
      freeKmem(fd, (void **)&encReq.keyData);
      freeKmem(fd, (void **)&encReq.outIvData);
      return status;
  }

  freeKmem(fd, (void **)&encReq.inIvData);
  freeKmem(fd, (void **)&encReq.keyData);
  freeKmem(fd, (void **)&encReq.outIvData);
  

  /* Now reverse the operation */
  /* Set up the decryption request */
  decReq.opId            = DPD_TDES_CBC_CTX_DECRYPT;
  decReq.inIvBytes       = 8;
  decReq.keyBytes        = 24;
  decReq.inBytes         = opLen;
  decReq.inIvData        = (unsigned char *)iv_in;
  decReq.keyData         = (unsigned char *)cipherKey;
  decReq.inData          = (unsigned char *)outList;
  decReq.outIvBytes      = 8;
  decReq.outIvData       = ctx;
  decReq.outData         = (unsigned char *)cmpList;

  MarkScatterBuffer(&decReq, &decReq.inData);
  MarkScatterBuffer(&decReq, &decReq.outData);

  status = putKmem(fd, (void *)iv_in, (void **)&decReq.inIvData, decReq.inIvBytes);
  if (status)
    return status;

  status = putKmem(fd, (void *)cipherKey, (void **)&decReq.keyData, decReq.keyBytes);
  if (status) {
    freeKmem(fd, (void **)&decReq.inIvData);
    return status;
  }

  status = putKmem(fd, NULL, (void **)&decReq.outIvData, decReq.outIvBytes);
  if (status) {
    freeKmem(fd, (void **)&decReq.inIvData);
    freeKmem(fd, (void **)&decReq.keyData);
    return status;
  }

  armCompletion(&decReq);
  status = ioctl(fd, IOCTL_PROC_REQ, (int)&decReq);

  if ((status = waitCompletion("testSG(): DES decryption with scattered output", status, &decReq)))
  {
      free(in7);
      free(in4);
      free(in2);
      free(in1);
      free(in3);
      free(in6);
      free(in5);
      free(out8);
      free(out1);
      free(out3);
      free(out5);
      free(out2);
      free(out6);
      free(out4);
      free(out7);
      free(cmp2);
      free(cmp1);
      freeKmem(fd, (void **)&decReq.inIvData);
      freeKmem(fd, (void **)&decReq.keyData);
      freeKmem(fd, (void **)&decReq.outIvData);
      return status;
  }    

  freeKmem(fd, (void **)&decReq.inIvData);
  freeKmem(fd, (void **)&decReq.keyData);
  freeKmem(fd, (void **)&decReq.outIvData);

  /* Compare the results */
  /* Note that so far, we only encrypt to a buffer, then decrypt to another */
  /* buffer with the same key, so the end result should match the input */
  /* There is no test of the DES encrypted data, that's done in a separate test */
  cmpfail = 0;
  if (memcmp(inp1, &cmp1[0],   512))
  {
      cmpfail++;
      printf("compare fragment 1\n");
      printf("expected:\n");
      dumpm((unsigned char *)inp1, 512);
      printf("actual:\n");
      dumpm(&cmp1[0], 512);
  }

  if (memcmp(inp2, &cmp1[512], 256))
  {
      cmpfail++;
      printf("compare fragment 2\n");
      printf("expected:\n");
      dumpm((unsigned char *)inp2, 256);
      printf("actual:\n");
      dumpm(&cmp1[512], 256);
  }

  if (memcmp(inp3, &cmp1[768],  64))
  {
      cmpfail++;
      printf("compare fragment 3\n");
      printf("expected:\n");
      dumpm((unsigned char *)inp3, 64);
      printf("actual:\n");
      dumpm(&cmp1[768], 64);
  }

  if (memcmp(inp4, &cmp1[832],  64))
  {
      cmpfail++;
      printf("compare fragment 4\n");
      printf("expected:\n");
      dumpm((unsigned char *)inp4, 64);
      printf("actual:\n");
      dumpm(&cmp1[832], 64);
  }

  if (memcmp(inp5, &cmp2[0],   128))
  {
      cmpfail++;
      printf("compare fragment 5\n");
      printf("expected:\n");
      dumpm((unsigned char *)inp4, 128);
      printf("actual:\n");
      dumpm(&cmp2[0], 128);
  }

  if (memcmp(inp6, &cmp2[128], 196))
  {
      cmpfail++;
      printf("compare fragment 6\n");
      printf("expected:\n");
      dumpm((unsigned char *)inp6, 196);
      printf("actual:\n");
      dumpm(&cmp2[128], 196);
  }
  
  if (memcmp(inp7, &cmp2[324], 364))
  {
      cmpfail++;
      printf("compare fragment 7\n");
      printf("expected:\n");
      dumpm((unsigned char *)inp7, 364);
      printf("actual:\n");
      dumpm(&cmp2[324], 364);
  }

  free(in7);
  free(in4);
  free(in2);
  free(in1);
  free(in3);
  free(in6);
  free(in5);
  free(out8);
  free(out1);
  free(out3);
  free(out5);
  free(out2);
  free(out6);
  free(out4);
  free(out7);
  free(cmp2);
  free(cmp1);

  if (cmpfail)
  {
    printf("*** Test SG: data compare error in %d fragments, test failure ***\n", cmpfail);
    result = -1;
  }
  else
  {
    printf("*** Test SG passed ***\n");
    result = 0;
  }

#endif /* not usermode */  
  return(result);
}

