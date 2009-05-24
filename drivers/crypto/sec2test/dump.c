

/****************************************************************************
 * dump.c - Memory dumper for SEC2 driver tests
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
 *  1.2   02-Feb-2005 sec - fix output bug
 *  1.3   Aug 11,2005
 */


#ifdef _LINUX_USERMODE_
#include <memory.h> /* memcpy(), memset() */
#include <stdio.h>  /* printf() */
#endif

#ifdef __KERNEL__
#include <linux/string.h>
#include <linux/kernel.h>
#define printf printk
#endif

#ifndef NULL
#define NULL (void *)0
#endif

void dumpm(unsigned char *csData, int nBytes)
{
    int nHex, nAscii;
    int nIndex;
    char csOutputBuffer[80];
    char *csLocalPtr;

    if (csData == NULL) {
        printf("00000000 Null pointer passed to dumpm()\n");
        return;
    }

    nHex = nAscii = 0;

    for (nIndex = 0; nIndex < nBytes; nIndex++) {
        if (nIndex % 16 == 0) {
            csOutputBuffer[nAscii] = 0;
            if (nHex) printf("%s\n", csOutputBuffer);
            memset(csOutputBuffer, ' ', sizeof(csOutputBuffer) - 1);
            csLocalPtr = csData + nIndex;
            sprintf(csOutputBuffer, "%08lx", (unsigned long)csLocalPtr);
            csOutputBuffer[8] = ' ';
            nHex = 11;
            nAscii = 63;
        }

        sprintf(&csOutputBuffer[nHex], "%02x", csData[nIndex]);
        csOutputBuffer[nHex+2] = ' ';

        if (csData[nIndex] >= 0x20 && csData[nIndex] < 0x7f)
            csOutputBuffer[nAscii] = csData[nIndex];
        else
            csOutputBuffer[nAscii] = '.';

        nAscii++, nHex += 3;
        if (nIndex % 16 == 7)
            nHex += 2;
    }
    
    if (nHex > 11) {
        csOutputBuffer[nAscii] = 0;
        printf("%s\n", csOutputBuffer);
    }
}
