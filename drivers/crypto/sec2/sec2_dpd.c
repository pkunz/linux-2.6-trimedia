


/****************************************************************************
 * sec2_dpd.c -- Code to construct a packet descriptor for SEC2 Driver
 ****************************************************************************
 * Copyright (c) Certicom Corp. 1996-2002.  All rights reserved
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/

/*
 * Revision History:
 * 1.0   Aug 24,2003 dgs - adapted from the final version of mpc18x
 *       May 26,2004 sec - add support for scatter pointers
 * 1.1.0 Nov 16,2004 sec - incorporate linux changes from prior versions
 * 1.1.1 Dec 16,2004 sec - clean up prototypes and warnings
 * 1.2   Jan 27,2005 sec - bugfix in DPD dumper
 * 1.3   Jul 21,2005 sec - support for debug buffer viewing, 36-bit pointers
 *                         for 2.1, and scatterlist debug support
 */


#include "Sec2Driver.h"
#include "Sec2Descriptors.h"
#include "sec2_dpd_Table.h"


static int VerifyRequest(void *request, DPD_DETAILS_ENTRY *pDesc);
static DPD_DETAILS_ENTRY *GetRequestDescEntry(unsigned long opId);


#ifdef DBG
static void dumpBuffer(unsigned char *data, unsigned long size)
{
    unsigned long  i, block, remain;

    remain = size;
        
    while(remain)
    {
        if (remain < 16)
        {
           block = remain;
           remain = 0;
        }
        else
        {
            block = 16;
            remain -= block;
        }
            
        SEC2Dump(DBGTXT_DATASHOW, ("0x%08lx: ", (unsigned long)data));

        for (i = 0; i < block; i++)
            SEC2Dump(DBGTXT_DATASHOW, ("%02x ", *(data++)));
        SEC2Dump(DBGTXT_DATASHOW, ("\n"));
    }
}
#endif


/* Build a SEC2-channel-compatible fragment map from a generic one   */
/* inList: external linked list                                      */
/* ptrEnt: will become head of compatible list we'll construct       */
/*                                                                   */
int MapScatterFragments(unsigned long  inList,
                        unsigned long *ptrEnt)
{
    int                  i, listSiz;
    EXT_SCATTER_ELEMENT *inElem;
    SCATTER_ELEMENT     *outList;


    inElem = (EXT_SCATTER_ELEMENT *)inList;
    
    /* How many elements to alloc? */
    listSiz = 0;
    while (inElem != NULL)
    {
      listSiz++;
      inElem = inElem->next;
    }

    SEC2Dump(DBGTXT_SETRQ, ("MapScatterFragments(): list 0x%08lx has %d fragments\n", inList, listSiz));

    /* Allocate a chunk off the driver's available heap */
    outList = malloc(sizeof(SCATTER_ELEMENT) * listSiz);
    if (outList == NULL)
        return(SEC2_FRAGMENT_POOL_EXHAUSTED);

    /* Stuff allocated chunk with caller's info */
    i = 0;
    inElem = (EXT_SCATTER_ELEMENT *)inList;
    while (inElem != NULL)
    {
      outList[i].segAddr  = (void *)__pa((unsigned long)inElem->fragment);
      outList[i].segLen   = (unsigned short)inElem->size;
      outList[i].linkStat = 0;
      inElem = inElem->next;
      SEC2Dump(DBGTXT_SETRQ,
               ("MapScatterFragments(): new fragment 0x%08x, %d bytes\n",
               (unsigned)outList[i].segAddr,
               outList[i].segLen));
      i++;
    }

    /* once done, set the "return" bit in the last element */
    outList[i - 1].linkStat |= SEGLIST_RETURN;

    *ptrEnt = __pa((unsigned long)outList);
  
    return(SEC2_SUCCESS);
}  

/* Free a SEC2-hardware-compatible fragment chain. antithesis of MapScatterFragments */
/* Should be called upon as part of a channel interrupt cleanup process              */

void UnmapScatterFragments(unsigned long *ptrEnt)
{
/*    free((void *)ChannelAssignments[channel].Dpds[i]->fld[j].ptr); */
}


/*
 * Convert driver-native request structure to DPD and insert
 */

int RequestToDpd(register GENERIC_REQ *pReq, register int channel)
{

#define DPDPTR      (ChannelAssignments[channel].Dpds[dpdCount])

    register DPD_DETAILS_ENTRY  *pDesc;
    register unsigned long       opId;
    register int                 i, status;
    register int                 dpdCount;

#ifdef DBG
    SCATTER_ELEMENT             *linkent;
    unsigned long                eptr;
    int                          linkct;
#endif
    
    SEC2Dump(DBGTXT_SETRQ, ("RequestToDpd(): req @0x%08x ch%d\n", (unsigned)pReq, channel+1));

    if (pReq == NULL)
        return SEC2_NULL_REQUEST;

    dpdCount = 0;
        
    /* process request(s) */
    /* do {stuff} while pReq points to real request */
    do
    {
        SEC2Dump(DBGTXT_SETRQ, ("RequestToDpd(): get entry for opId 0x%08lx\n", pReq->opId));
        pDesc = GetRequestDescEntry (pReq->opId);
        if (pDesc == NULL)
            return SEC2_INVALID_OPERATION_ID;

        /* Validate the contents of the request */
        SEC2Dump(DBGTXT_SETRQ, ("RequestToDpd(): VerifyRequest(0x%08x, 0x%08x)\n", (unsigned)pReq, (unsigned)pDesc));
        status = VerifyRequest(pReq, pDesc);
        if (status != SEC2_SUCCESS)
            return status;

        /* If there are insufficient DPDs save the current request
           list entry and offset.  Indicate that the request is "chunked"
        */
        if (dpdCount >= MAX_DPDS)
            return (SEC2_INSUFFICIENT_REQS);

        /*
         * We have sufficient resources (DPD slots) to process this request.
         * Initialize a DPD and link the prior DPD to this one (if it is not the first)
         */

        SEC2Dump(DBGTXT_SETRQ, ("RequestToDpd(): clear DPD 0x%08x\n", (unsigned)DPDPTR));
        memset(DPDPTR, 0x00, sizeof(DPD));

        /* fill in the dpd header */
        opId =  pReq->opId;
        DPDPTR->header = pDesc->hdrDesc[1*(opId & DESC_NUM_MASK)];

        /* For each field in a request: */
        for (i = 0; i < NUM_DPD_FLDS && pDesc->fld[i].txt != NULL; i++)
        {
            if (*(long *)pDesc->fld[i].txt == *(long *)"NIL")
            {
                DPDPTR->fld[i].ptr = 0;
                DPDPTR->fld[i].len = 0;
            }
            else
            {
                /* deal with basic pointer */
                SEC2Dump(DBGTXT_SETRQ, ("RequestToDpd(): converting entry %d->0x%08x\n", i, *(unsigned int *)((unsigned int)pReq + pDesc->fld[i].ptrOffset1st)));
                /* if offset AND caller specfied non-NULL pointer, assign/convert that pointer */
                if ((pDesc->fld[i].ptrOffset1st != 0) && 
                    *(unsigned int *)((unsigned int)pReq + pDesc->fld[i].ptrOffset1st))
                {
                    DPDPTR->fld[i].ptr = __pa(*(unsigned int *) ((unsigned int)pReq + pDesc->fld[i].ptrOffset1st));
#ifdef DBG
                    SEC2Dump(DBGTXT_SETRQ, ("content @p%d:\n", i));
                    dumpBuffer((unsigned char *)*(unsigned int *)((unsigned int)pReq + pDesc->fld[i].ptrOffset1st),
                               *(unsigned int *)((unsigned int)pReq + pDesc->fld[i].lenOffset1st));
#endif
                }
                /* if scatter pointer, deal with special case */
                if (pReq->scatterBufs & (0x01 << i))
                {
                  SEC2Dump(DBGTXT_SETRQ, ("RequestToDpd(): pointer word %d is scattered\n", i+1));

                  /* this should be linked list to fragment map, do conversion */
                  status = MapScatterFragments(*(unsigned int *)((unsigned int)pReq + pDesc->fld[i].ptrOffset1st),
                                               &(DPDPTR->fld[i].ptr));
                  if (status != SEC2_SUCCESS) /* map failed? */
                    return(status);
                }

                SEC2Dump(DBGTXT_SETRQ, ("RequestToDpd(): set length = %d\n", *(unsigned int *)((unsigned int)pReq + pDesc->fld[i].lenOffset1st)));
                DPDPTR->fld[i].len = *(unsigned int *) ((unsigned int)pReq + pDesc->fld[i].lenOffset1st);
                DPDPTR->fld[i].len <<= 16;

                if (pDesc->fld[i].extOffset != 0)
                    DPDPTR->fld[i].len |= *(unsigned int *) ((unsigned int)pReq + pDesc->fld[i].extOffset) << 8;
                
                /* if scatter buf requested, flag it */
                if (pReq->scatterBufs & (0x01 << i))
                  DPDPTR->fld[i].len |= PTRTYPE_JBIT; /* set scatter flag bit */
            }
        }

#ifdef DBG
        /* If DPD dump requested, show this packet descriptor content */
        if (SEC2DebugLevel | DBGTXT_DPDSHOW) {

            SEC2Dump(DBGTXT_DPDSHOW,
                     ("DPD header         = 0x%08lx\n", DPDPTR->header));

            SEC2Dump(DBGTXT_DPDSHOW,
                     ("Primary EU/Mode 0x%02lx:%02lx, ",
                     (DPDPTR->header & 0xf0000000) >> 28,
                     (DPDPTR->header & 0x0ff00000) >> 20));
                     
            SEC2Dump(DBGTXT_DPDSHOW,
                     ("Secondary EU/Mode 0x%02lx:%02lx, ",
                     (DPDPTR->header & 0x000f0000) >> 16,
                     (DPDPTR->header & 0x0000ff00) >> 8));

            SEC2Dump(DBGTXT_DPDSHOW,
                     ("Type 0x%02lx, ",
                     (DPDPTR->header & 0x000000f8) >> 3));

            if (DPDPTR->header & 0x00000002)
                SEC2Dump(DBGTXT_DPDSHOW, ("Inbound, "));
            else
                SEC2Dump(DBGTXT_DPDSHOW, ("Outbound, "));
                
            if (DPDPTR->header & 0x00000001)
                SEC2Dump(DBGTXT_DPDSHOW, ("Done\n"));
            else
                SEC2Dump(DBGTXT_DPDSHOW, ("\n"));
            
            for (i = 0; i < NUM_DPD_FLDS; i++)
            {
                SEC2Dump(DBGTXT_DPDSHOW,
                         ("DPD ptr:len:ext p%d = 0x%1lx%08lx:%5ld:%3ld ",
                         i,
                         (DPDPTR->fld[i].len & 0x0000000f),
                         DPDPTR->fld[i].ptr,
                         (DPDPTR->fld[i].len & 0xffff0000) >> 16,
                         (DPDPTR->fld[i].len & 0x00007f00) >> 8));
                         
                if (DPDPTR->fld[i].len & 0x00008000)
                    SEC2Dump(DBGTXT_DPDSHOW, ("scatter ptr, "));
                else
                    SEC2Dump(DBGTXT_DPDSHOW, ("data ptr, "));
                
                if (DPDPTR->fld[i].len & 0x0000000f)
                    SEC2Dump(DBGTXT_DPDSHOW, ("36-bit\n"));
                else
                    SEC2Dump(DBGTXT_DPDSHOW, ("32-bit\n"));

                /* if scatter pointer, dump list */
                if (DPDPTR->fld[i].len & 0x00008000)
                {
                    linkct  = 0;
                    linkent = (SCATTER_ELEMENT *)__va(DPDPTR->fld[i].ptr);
                    do
                    {
                        eptr = linkent->linkStat & 0x000f;
                        if (linkent->linkStat & SEGLIST_NEXT)
                        {
                            SEC2Dump(DBGTXT_DPDSHOW, ("  new link table at 0x%1lx%08lx\n", eptr, (unsigned long)linkent->segAddr));
                            linkent = __va(linkent->segAddr);
                        }
                        else
                        {
                            SEC2Dump(DBGTXT_DPDSHOW, ("  link entry %3d: data at 0x%1lx%08lx, len %5d\n", linkct, eptr, (unsigned long)linkent->segAddr, linkent->segLen));
                            dumpBuffer(__va(linkent->segAddr), linkent->segLen);
                            if (linkent->linkStat & SEGLIST_RETURN)
                                break;
                            linkent++;
                            linkct++;
                        }
                    } while (1);
                } /* if (scatter list pointer) */
            } /* for (NUM_DPD_FLDS) */
        } /* if (DPD SHOW) */
#endif

        /* Move onto the next block */
        pReq = (GENERIC_REQ *)pReq->nextReq;
        dpdCount++;

    } while (pReq != NULL);  /* End while (request != NULL) */

    return SEC2_SUCCESS;
}  /* end of RequestToDpd */




/*
 * Validate request content 
 */
int VerifyRequest(register void *request, register DPD_DETAILS_ENTRY *pDesc)
{
  register int           i;
  register unsigned long len;

  for (i = 0; i < NUM_DPD_FLDS && pDesc->fld[i].txt != NULL; i++)
  {
    /* A NULL pointer to a size checking function means "don't bother checking" */
    if (pDesc->fld[i].pFncSize != NULL)
    {
      len = *(unsigned int *)((unsigned int)request + pDesc->fld[i].lenOffset1st);

      if (!pDesc->fld[i].pFncSize(len))
      {
        SEC2Dump(DBGTXT_SETRQ,
                 ("VerifyRequest->fld(%s) Invalid Length=%ld\n",
                 pDesc->fld[i].txt, len));
        return SEC2_INVALID_LENGTH;
      }
    }
    /* check Extent Length < 128 */
    if (pDesc->fld[i].extOffset != 0)
    {
      len = *(unsigned int *)((unsigned int)request + pDesc->fld[i].extOffset);
      if (len >= 128)
        return SEC2_INVALID_LENGTH;
    }           
  }
  
  return SEC2_SUCCESS;
}





/* Scan the DPDDetails table for a particular opId value.  Return a pointer
   to the entry if it is found, NULL otherwise
*/
DPD_DETAILS_ENTRY *GetRequestDescEntry(unsigned long opId)
{
    int i = 0;

    opId &= DESC_TYPE_MASK;
    while (DpdDetails[i].txt != NULL && DpdDetails[i].opId != opId) i++;

    return(DpdDetails[i].opId == 0 ? NULL : &DpdDetails[i]);
}




/*
 * The following Chk___Len() routines and the FitInBlock() routine are used
 * to verify that the length of a data area is consistent with a request type.
 * They are supplied as pointers to functions in the DPDDetails table
*/


BOOLEAN ChkDesIvLen(unsigned long len)
{
    return len != 0 && len != DES_BLOCK ? FALSE : TRUE;
}



BOOLEAN ChkDesKeyLen(unsigned long len)
{
    return len != DES_BLOCK && len != (2*DES_BLOCK) && len != (3*DES_BLOCK) ? FALSE : TRUE;
}



BOOLEAN ChkDesStaticDataLen(unsigned long len)
{
    return (len & 0x7) != 0 ? FALSE : TRUE;
}



BOOLEAN ChkDesDataLen(unsigned long len)
{
    return (len & 0x7) != 0 ? FALSE : TRUE;
}



BOOLEAN ChkDesCtxLen(unsigned long len)
{
    return len != 0 && len != DES_BLOCK != 0 ? FALSE : TRUE;
}



BOOLEAN ChkAesKeyLen(unsigned long len)
{
    return len != 16 && len != 24 && len != 32 ? FALSE : TRUE;
}



BOOLEAN ChkAesIvLen(unsigned long len)
{
    return len != 0 && len != 16 ? FALSE : TRUE;
}



BOOLEAN ChkArcKeyLen(unsigned long len)
{
    return len < ARC4_MIN_KEYBYTES || len > ARC4_MAX_KEYBYTES ? FALSE : TRUE;
}



BOOLEAN ChkArcCtxLen(unsigned long len)
{
    return len != ARC4_CONTEXTBYTES ? FALSE : TRUE;
}



BOOLEAN ChkOptionalArcCtxLen(unsigned long len)
{
    return len != ARC4_CONTEXTBYTES && len != 0 ? FALSE : TRUE;
}



BOOLEAN ChkEccLen(unsigned long len)
{
    return len > 64 ? FALSE : TRUE;
}



BOOLEAN FitInBlock(unsigned long len)
{
    return len > BlockSize ? FALSE : TRUE;
}



BOOLEAN ChkCcmpKeyLen(unsigned long len)
{
    return len != 0 && len != 16 ? FALSE : TRUE;
}

