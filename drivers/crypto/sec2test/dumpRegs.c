
/****************************************************************************
 * dumpRegs.c - Register content dumper for SEC2 driver tests
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
 *  1.1.0 Dec 05,2004 sec - prep for release
 *  1.2   02-Feb-2005
 *  1.3   Aug 11,2005
 */


#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/config.h>
#define printf printk
#endif


#ifdef CONFIG_MPC834x
#ifdef T2_REGBASE_CTRL
#error Offset of SEC in IMMR can only be specified once
#endif
#define T2_REGBASE_CTRL 0xfe031000
#define T2_REGBASE_CH   0xfe031000
#define T2_REGBASE_DEU  0xfe032000
#define T2_REGBASE_AES  0xfe034000
#define T2_REGBASE_MDEU 0xfe036000
#define T2_REGBASE_AFEU 0xfe038000
#define T2_REGBASE_RNG  0xfe03a000
#define T2_REGBASE_PKEU 0xfe03c000
#endif	/* 834x */

#ifdef CONFIG_MPC854x
#ifdef T2_REGBASE_CTRL
#error Offset of SEC in IMMR can only be specified once
#endif
#define T2_REGBASE_CTRL 0xe0031000
#define T2_REGBASE_CH   0xe0031000
#define T2_REGBASE_DEU  0xe0032000
#define T2_REGBASE_AES  0xe0034000
#define T2_REGBASE_MDEU 0xe0036000
#define T2_REGBASE_AFEU 0xe0038000
#define T2_REGBASE_RNG  0xe003a000
#define T2_REGBASE_PKEU 0xe003c000
#endif	/* 854x */


typedef volatile unsigned long *hwreg;



int dumpT2ctl(void)
{
  unsigned long rh, rl;

  rh = *(hwreg)(T2_REGBASE_CTRL | 0x08);
  rl = *(hwreg)(T2_REGBASE_CTRL | 0x0c);
  printf("IntEn:   0x%08lx%08lx\n", rh, rl);
  if (rh & 0x00010000) printf("ITO ");
  if (rh & 0x00000f00)
  {
    printf("DONE Ovfl:");
    if (rh & 0x00000800) printf("4");
    if (rh & 0x00000400) printf("3");
    if (rh & 0x00000200) printf("2");
    if (rh & 0x00000100) printf("1");
    printf(" ");
  }
  if (rh & 0x000000c0)
  {
    printf("ch4:");
    if (rh & 0x00000080) printf("err ");
    if (rh & 0x00000040) printf("dn");
    printf(" ");
  }
  if (rh & 0x00000030)
  {
    printf("ch3:");
    if (rh & 0x00000020) printf("err ");
    if (rh & 0x00000010) printf("dn");
    printf(" ");
  }
  if (rh & 0x0000000c)
  {
    printf("ch2:");
    if (rh & 0x00000008) printf("err ");
    if (rh & 0x00000004) printf("dn");
    printf(" ");
  }
  if (rh & 0x00000003)
  {
    printf("ch1:");
    if (rh & 0x00000002) printf("err ");
    if (rh & 0x00000001) printf("dn");
    printf(" ");
  }
  if (rl & 0x00300000)
  {
    printf("PKEU:");
    if (rl & 0x00200000) printf("err ");
    if (rl & 0x00100000) printf("dn");
    printf(" ");
  }
  if (rl & 0x00030000)
  {
    printf("RNG:");
    if (rl & 0x00020000) printf("err ");
    if (rl & 0x00010000) printf("dn");
    printf(" ");
  }
  if (rl & 0x00003000)
  {
    printf("AFEU:");
    if (rl & 0x00002000) printf("err ");
    if (rl & 0x00001000) printf("dn");
    printf(" ");
  }
  if (rl & 0x00000300)
  {
    printf("MDEU:");
    if (rl & 0x00000200) printf("err ");
    if (rl & 0x00000100) printf("dn");
    printf(" ");
  }
  if (rl & 0x00000030)
  {
    printf("AESU:");
    if (rl & 0x00000020) printf("err ");
    if (rl & 0x00000010) printf("dn");
    printf(" ");
  }
  if (rl & 0x00000003)
  {
    printf("DEU:");
    if (rl & 0x00000002) printf("err ");
    if (rl & 0x00000001) printf("dn");
    printf(" ");
  }
  printf("\n");
    
  rh = *(hwreg)(T2_REGBASE_CTRL | 0x10);
  rl = *(hwreg)(T2_REGBASE_CTRL | 0x14);
  printf("IntStat: 0x%08lx%08lx\n", rh, rl);
  if (rh & 0x00010000) printf("ITO ");
  if (rh & 0x00000f00)
  {
    printf("DONE Ovfl:");
    if (rh & 0x00000800) printf("4");
    if (rh & 0x00000400) printf("3");
    if (rh & 0x00000200) printf("2");
    if (rh & 0x00000100) printf("1");
    printf(" ");
  }
  if (rh & 0x000000c0)
  {
    printf("ch4:");
    if (rh & 0x00000080) printf("err ");
    if (rh & 0x00000040) printf("dn");
    printf(" ");
  }
  if (rh & 0x00000030)
  {
    printf("ch3:");
    if (rh & 0x00000020) printf("err ");
    if (rh & 0x00000010) printf("dn");
    printf(" ");
  }
  if (rh & 0x0000000c)
  {
    printf("ch2:");
    if (rh & 0x00000008) printf("err ");
    if (rh & 0x00000004) printf("dn");
    printf(" ");
  }
  if (rh & 0x00000003)
  {
    printf("ch1:");
    if (rh & 0x00000002) printf("err ");
    if (rh & 0x00000001) printf("dn");
    printf(" ");
  }
  if (rl & 0x00300000)
  {
    printf("PKEU:");
    if (rl & 0x00200000) printf("err ");
    if (rl & 0x00100000) printf("dn");
    printf(" ");
  }
  if (rl & 0x00030000)
  {
    printf("RNG:");
    if (rl & 0x00020000) printf("err ");
    if (rl & 0x00010000) printf("dn");
    printf(" ");
  }
  if (rl & 0x00003000)
  {
    printf("AFEU:");
    if (rl & 0x00002000) printf("err ");
    if (rl & 0x00001000) printf("dn");
    printf(" ");
  }
  if (rl & 0x00000300)
  {
    printf("MDEU:");
    if (rl & 0x00000200) printf("err ");
    if (rl & 0x00000100) printf("dn");
    printf(" ");
  }
  if (rl & 0x00000030)
  {
    printf("AESU:");
    if (rl & 0x00000020) printf("err ");
    if (rl & 0x00000010) printf("dn");
    printf(" ");
  }
  if (rl & 0x00000003)
  {
    printf("DEU:");
    if (rl & 0x00000002) printf("err ");
    if (rl & 0x00000001) printf("dn");
    printf(" ");
  }
  printf("\n");  

  
  rh = *(hwreg)(T2_REGBASE_CTRL | 0x28);
  rl = *(hwreg)(T2_REGBASE_CTRL | 0x2c);
  printf("EUAssgn: 0x%08lx%08lx ", rh, rl);
  if (rh & 0x0f000000)
    printf("AFEU:%ld ", ((rh & 0x0f000000) >> 24));
  if (rh & 0x000f0000)
    printf("MDEU:%ld ", ((rh & 0x000f0000) >> 16));
  if (rh & 0x00000f00)
    printf("AESU:%ld ", ((rh & 0x00000f00) >> 8));
  if (rh & 0x0000000f)
    printf("DEU:%ld ", (rh & 0x0000000f));
  if (rl & 0x00000f00)
    printf("PKEU:%ld ", ((rl & 0x00000f00) >> 8));
  if (rl & 0x0000000f)
    printf("RNG:%ld ", (rl & 0x0000000f));
  printf("\n");
  
 
  rh = *(hwreg)(T2_REGBASE_CTRL | 0x30);
  rl = *(hwreg)(T2_REGBASE_CTRL | 0x34);
  printf("MstrCtl: 0x%08lx%08lx\n", rh, rl);

  return(0);
}

int dumpT2chan(int ch)
{

  int           j,  k;
  unsigned long rh, rl;
  
  rh = *(hwreg)(T2_REGBASE_CH | (ch << 8) | 0x08);
  rl = *(hwreg)(T2_REGBASE_CH | (ch << 8) | 0x0c);
  printf("Config:  0x%08lx%08lx\n", rh, rl );

  rh = *(hwreg)(T2_REGBASE_CH | (ch << 8) | 0x10);
  rl = *(hwreg)(T2_REGBASE_CH | (ch << 8) | 0x14);
  printf("PtrStat: 0x%08lx%08lx\n", rh, rl );
  printf("FF_COUNTER:0x%02lx   G_STATE:0x%02lx   S_STATE:0x%02lx   CHN_STATE:0x%02lx\n",
         (rh & 0x1f000000) >> 24,
         (rh & 0x000f0000) >> 16,
         (rh & 0x00000f00) >> 8,
         (rh & 0x000000ff));
  if (rl & 0x02000000) printf("MI ");
  if (rl & 0x01000000) printf("MO ");
  if (rl & 0x00800000) printf("PR ");
  if (rl & 0x00400000) printf("SR ");
  if (rl & 0x00200000) printf("PG ");
  if (rl & 0x00100000) printf("SG ");
  if (rl & 0x00080000) printf("PRD ");
  if (rl & 0x00040000) printf("SRD ");
  if (rl & 0x00020000) printf("PD ");
  if (rl & 0x00010000) printf("SD ");
  if (rl & 0x00008000) printf("DOF ");
  if (rl & 0x00004000) printf("SOF ");
  if (rl & 0x00002000) printf("MDTE ");
  if (rl & 0x00001000) printf("SGZERO ");
  if (rl & 0x00000800) printf("FPNULL ");
  if (rl & 0x00000400) printf("ILLDESC ");
  if (rl & 0x00000200) printf("INV_EU ");
  if (rl & 0x00000100) printf("EU_ERR ");
  if (rl & 0x00000080) printf("GBOUND ");
  if (rl & 0x00000040) printf("GRTN ");
  if (rl & 0x00000020) printf("SBOUND ");
  if (rl & 0x00000010) printf("SRTN");
  printf("\n");
  printf("Error:0x%04lx   PAIR_PTR 0x%02lx\n", ((rl & 0x0000fff0) >> 4), (rl & 0x0000000f));

  rh = *(hwreg)(T2_REGBASE_CH | (ch << 8) | 0x40);
  rl = *(hwreg)(T2_REGBASE_CH | (ch << 8) | 0x44);
  printf("CurDesc: 0x%08lx%08lx\n", rh, rl );

  k = 0;
  printf(" ");
  for (j = 0; j < 4; j++)
      for (k = 0; k < 32; k += 4)
          printf(" 0x%08lx", *(hwreg)(T2_REGBASE_CH | (ch << 8) | 0x80 | k));

  printf("\n");
  return(0);
}


int dumpT2(void)
{

  int i;
  
  
  printf("T2 Controller Registers:\n");
  dumpT2ctl();
  
  for (i = 1; i <= 4; i++)
  {
    printf("T2 Channel %d\n", i);
    dumpT2chan(i);
  }
  
  return(0);
}




int dumpT2desc(void *desc)
{
  unsigned long dh, dl;
  int           i;
  
  
  dh = *(unsigned long *)desc;
  printf("DPD Header = 0x%08lx\n", dh);
  printf("Primary EU=%ld mode=0x%02lx   Secondary  EU=%ld mode=0x%02lx   Type=0x%02lx ",
         ((dh & 0xf0000000) >> 28),
         ((dh & 0x0ff00000) >> 20),
         ((dh & 0x000f0000) >> 16),
         ((dh & 0x0000ff00) >> 8),
         ((dh & 0x000000f8) >> 3));
  if (dh & 0x00000002) printf("DIR ");
  if (dh & 0x00000001) printf("DN");
  printf("\n");
  
  desc += 4; /* skip header and reserved */
  
  for (i = 1; i <= 7; i++)
  {
    desc += 4;
    dh = *(unsigned long *)desc;
    desc += 4;
    dl = *(unsigned long *)desc;
    printf("0x%08lx 0x%08lx\n", dh, dl);
    printf("ddesc %d: length=%ld  jump=%ld  extent=%ld  pointer=0x%02lx%08lx\n",
           i,
           ((dh & 0xffff0000) >> 16),
           ((dh & 0x00008000) >> 15),
           ((dh & 0x00007f00) >> 8),
           (dh & 0x00000003),
           dl);
  }
  
  return(0);
}




int dumpT2pkha(void)
{
  unsigned long rl;
  
  rl = *(hwreg)(T2_REGBASE_PKEU | 0x04);
  printf("ModeReg:0x%02lx ", rl);
  
  rl = *(hwreg)(T2_REGBASE_PKEU | 0x0c);
  printf("KeySize:0x%04lx ", rl);
  
  rl = *(hwreg)(T2_REGBASE_PKEU | 0x44);
  printf("ABsize:0x%04lx ", rl);
    
  rl = *(hwreg)(T2_REGBASE_PKEU | 0x14);
  printf("DataSize:0x%04lx\n", rl);
  
  rl = *(hwreg)(T2_REGBASE_PKEU | 0x2c);
  printf("PKEUStatus: " );
  if (rl & 0x00000040) printf("Z ");
  if (rl & 0x00000020) printf("HALT ");
  if (rl & 0x00000004) printf("IE ");
  if (rl & 0x00000002) printf("ID ");
  if (rl & 0x00000001) printf("RD");
  printf("\n");
  
  rl = *(hwreg)(T2_REGBASE_PKEU | 0x34);
  printf("IntStatus: " );
  if (rl & 0x00002000) printf("INV ");
  if (rl & 0x00001000) printf("IE ");
  if (rl & 0x00000400) printf("CE ");
  if (rl & 0x00000200) printf("KSE ");
  if (rl & 0x00000100) printf("DSE ");
  if (rl & 0x00000080) printf("ME ");
  if (rl & 0x00000040) printf("AE ");
  printf("\n");

  return(0);
}
