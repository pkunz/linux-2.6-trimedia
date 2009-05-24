
/****************************************************************************
 * Sec2Driver.h - Private/local include file for SEC2 security driver
 ****************************************************************************
 * Copyright (c) 2003-2005 Freescale Semiconductor
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 ***************************************************************************/


/* Revision History:
 * 1.0   Aug 24,2003 dgs adapted from the final version of mpc18x
 *       May 12,2004 sec - moved interrupt assign for MPC8555
 *       May 26,2004 sec - add support for scatter buffers
 *       Aug 27,2004 sec - fixed channel interrupt masks
 * 1.1.0 Nov 05,2004 sec - integrate/merge linux changes from prior revs
 * 1.2   Jan 27,2005 sec - move global inhibit bit
 * 1.3   Jul 21,2005 sec - updates for 8548 and kernel 2.6
 * 1.5   Oct 13,2005 sec - add support for reserved channels, and per-channel
 *                         auxiliary service handlers
 */
 
#ifndef SEC2DRIVER_H
#define SEC2DRIVER_H

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/semaphore.h>
#include <asm/irq.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/io.h>

#undef malloc
#undef free
#define free kfree
#define malloc(a) kmalloc(a, GFP_KERNEL | GFP_DMA)
#undef semGive
#undef semTake
#define semDelete(x)
#define semGive(a) up(&a)
#define semTake(a, b) down_interruptible_timeout(&a, b)

extern long __down_interruptible_timeout(struct semaphore *sem, long timeout);
extern inline int down_interruptible_timeout(struct semaphore *sem, long timeout)
{
    int ret = 0;
#ifdef WAITQUEUE_DEBUG
    CHECK_MAGIC(sem->__magic);
#endif
    
    if (atomic_dec_return(&sem->count) < 0)
        ret = __down_interruptible_timeout(sem, timeout);
    
    smp_wmb();
    return ret;
}

#define taskIdSelf() current->pid
#define SEC2_DEVMAJOR 0
#endif


#include "Sec2.h"

#define VERSION "1.5"

#define SEC2_DEVNAME "sec2"



/* This might move out to a board-specfic header somewhere, */
/* not a core-specific header such as this */


#ifdef __KERNEL__
#ifdef CONFIG_83xx
#define INUM_SEC2 MPC83xx_IRQ_SEC2
#else /* assume 85xx */
#define INUM_SEC2 MPC85xx_IRQ_SEC2
#endif
#endif

/* Define an offset from the peripheral base register where the core */
/* should be located relative to */
#define TALITOS2X_BASE_ADDRESS (0x00030000)

/* Define the size of the register region */
#define SEC2_BASE_SIZE (0x10000)


/* When static scatter pool fully functional, define here */
/* Else, use dynamic pool                                 */
#define DYNAMIC_SCATTER_POOL


/* Do right-justifies output shift for PKHA results due to errata */
#define PKHA_OUTPUT_SHIFT



/* 
 * Debug message type enabler switches for SEC2DebugLevel:
 * DBGTXT_SETRQ    - all messages from request setups
 * DBGTXT_SVCRQ    - all messages from request servicing
 * DBGTXT_INITDEV  - all messages from initialization
 * DBGTXT_DPDSHOW  - content of each DPD before it's queued
 * DBGTXT_INFO     - general driver information messages
 * DBGTXT_DATASHOW - content of all DPD-referenced buffers before request
 *                   (useful for verifying bounce buffer fills)
 * DBGTXT_DESCBUF  - dump descriptor buffer after processing to show
 *                   what was actually read by the core
 */
 
#ifdef DBG
#define DBGTXT_SETRQ              ((unsigned long)0x00000001)
#define DBGTXT_SVCRQ              ((unsigned long)0x00000002)
#define DBGTXT_INITDEV            ((unsigned long)0x00000004)
#define DBGTXT_DPDSHOW            ((unsigned long)0x00000008)
#define DBGTXT_INFO               ((unsigned long)0x00000010)
#define DBGTXT_DATASHOW           ((unsigned long)0x00000020)
#define DBGTXT_DESCBUF            ((unsigned long)0x00000040)


extern unsigned long SEC2DebugLevel;



#define SEC2Dump(LEVEL,STRING) \
        do { \
            unsigned long _level = (LEVEL); \
            if ((_level == 0)||(SEC2DebugLevel & _level)) { \
                printk STRING; \
            } \
        } while (0)


#else   /* ndef DBG */
#define SEC2Dump(LEVEL,STRING)
#endif  /* DBG */



#ifdef __KERNEL__
extern unsigned long irq_flag;

#define IOLockChannelAssignment() spin_lock_irqsave(&ChannelAssignLock,irq_flag)
#define IOUnLockChannelAssignment() spin_unlock_irqrestore(&ChannelAssignLock,irq_flag)

#define M_ioLib (12 << 16)
#define S_ioLib_UNKNOWN_REQUEST (M_ioLib | 2)

typedef long DEV_HDR;

#endif



#define IO_STATUS              int
#define MAX_DPDS               (24)
#define QUEUE_ENTRY_DEPTH      (16)

/* internal flag for pointer register - indirect pointer */
#define PTRTYPE_JBIT           (0x00008000)


/* SEC2_MASTER_CONTROL_REGISTER - Master Control Register offset */
#define SEC2_MASTER_CONTROL_REGISTER     ((unsigned long)0x00001030)

/* SEC2_RESET - software-controller reset of the SEC2 core */
#define SEC2_RESET          ((unsigned long)0x00000001)

/* SEC2_GLOBAL_INHIBIT - */
#define SEC2_GLOBAL_INHIBIT ((unsigned long)0x00000002)



/*
 * Indicates that the channel assignment is FREE or available for allocation
 */
#define CHANNEL_FREE (unsigned char)(0)


/*
 * Indicates that the channel assignment has been dynamically allocated.
 */
#define CHANNEL_DYNAMIC (unsigned char)(1)

/*
 * Indicates that the channel has been requested by and is assigned to
 * a requesting task
 */
#define CHANNEL_STATIC_ASSIGNED (unsigned char)(2)
                                                                                
                                                                                
/*
 * Indicates that the channel assignment is static.
 * A requestor is performing a static operation.
 */
#define CHANNEL_STATIC_BUSY (unsigned char)(3)
                                                                                
/*
 * Indicates that the channel assignment is manual.
 * A process is performing a manual operation.
 */
#define CHANNEL_MANUAL (unsigned char)(4)


/*
 * This applies to the #ChaAssignments array.  Any element in this
 * array that is set to CHA_DYNAMIC means that the CHA represented
 * by the array index is available
 */
 
#define CHA_DYNAMIC (0)



/* ID's of common chas in any SEC2.x subsystem */
#define CHA_DES      (0)
#define CHA_AESA     (1)
#define CHA_MDHA     (2)  
#define CHA_AFHA     (3)
#define CHA_RNG      (4)
#define CHA_PKHA     (5)
#define CHA_KEA      (6)
#define CHA_MDHA_DES (106)
#define CHA_MDHA_AES (105)


#define CHA_AFHA_BITMASK (0x1)
#define CHA_DESA_BITMASK (0x2)
#define CHA_MDHA_BITMASK (0x4)
#define CHA_RNGA_BITMASK (0x8)
#define CHA_PKHA_BITMASK (0x10)
#define CHA_AESA_BITMASK (0x20)
#define CHA_KEA_BITMASK  (0x40)


/* Size of DES block in bytes */
#define DES_BLOCK (8)

/* Minimum ARC4 key length */
#define ARC4_MIN_KEYBYTES (1)

/* Maximum ARC4 key length */
#define ARC4_MAX_KEYBYTES (16)

/* Context length for ARC4 */
#define ARC4_CONTEXTBYTES (259)

/*
 * Mask to be applied to address in order to determine
 * if it is double word aligned or not
 */
#ifdef NO_DOUBLE_WORD_ALIGNMENT
#define DOUBLE_WORD_ALIGNMENT ((unsigned long)0x00000000)
#else
#define DOUBLE_WORD_ALIGNMENT ((unsigned long)0x00000007)
#endif

/* Mask to be applied to base address value return from IO bus */
#define IO_BASE_ADDRESS_MASK ((unsigned long)0xFFFE0000)

/*
 * Offset for the Interrupt Control Register.
 *  
 *  This is relative to the base address register for the
 *   SEC2 coprocessor.  It is used to setup the
 *   #InterruptControlRegister variable during Driver Startup
 */
#define INTERRUPT_CONTROL_OFFSET ((unsigned long)0x00001008)

/*
 * Offset for the Interrupt Status Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #InterruptStatusRegister variable during Driver Startup
 */
#define INTERRUPT_STATUS_OFFSET ((unsigned long)0x00001010)


/*
 * Offset for the Interrupt Clear Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #InterruptClearRegister variable during Driver Startup
 */
#define INTERRUPT_CLEAR_OFFSET ((unsigned long)0x00001018)


/*
 * Location of the primary core ID register
 *
 * This takes some explaining. Older SEC2 revs used a simple
 * ID register word located at 0x1020 from the core base.
 * To accomodate a higher degree of revision information, a second
 * ID register was placed at 0x1bf8 from the base, this has a core
 * ID, plus a major and minor revision. This practice did not begin
 * until SEC2.1
 *
 * Thus, if the primary ID comes up as SEC2.0, check the secondary
 * ID register for more info.
 */
 
#define IDENTIFICATION_OFFSET      ((unsigned long)0x00001020)
#define IDENTIFICATION_BLOCKREV    ((unsigned long)0x00001bf8)

#define ID_BLOCK_MASK              (0xffff0000)
#define ID_BLOCK_SHIFT             (16)
#define ID_MAJOR_MASK              (0x0000ff00)
#define ID_MAJOR_SHIFT             (8)
#define ID_MINOR_MASK              (0x000000ff)
#define ID_MINOR_SHIFT             (0)
#define ID_INTEGRATION_MASK        (0x00ff0000)
#define ID_INTEGRATION_SHIFT       (16)
#define ID_CONFIG_MASK             (0x000000ff)
#define ID_CONFIG_SHIFT            (0)

#define SEC2_BLOCK_ID              (0x0030)  /* All SEC2 blocks should have... */

/* Possible core ID mask values */
#define SEC_COREID_2_0_0 (0x00000040)  /* low word  - MPC8555E/MPC8548E */
#define SEC_COREID_2_0_1 (0x00000041)  /* low word  - MPC8349E */
#define SEC_COREID_2_1_0 (0x00000080)  /* low word  - original MPC8548 */
#define SEC_COREID_2_1_1 (0x00300101)  /* high word - MPC8548 */
#define SEC_COREID_2_1_2 (0x00300102)  /* high word - MPC8548 */
#define SEC_COREID_2_2_0 (0x000000a0)  /* low word  - future use */
#define SEC_COREID_2_2_1 (0x00300200)  /* high word - future use */
#define SEC_COREID_2_5_0 (0x00300501)  /* high word - future use */



/*
 * Offset for the CHA Assignment Control Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #ChaAssignmentStatusRegister variable during Driver Startup
 */
#define CHA_ASSIGNMENT_STATUS_OFFSET ((unsigned long)0x00001028)



/*
 * Base address for the Channel # 1's Channel Configuration Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #ChannelConfigRegister variable array during Driver Startup
 * Each channel has a Channel Configuration Register that is offset
 * #CHANNEL_DISTANCE from the previous.
 */
#define CHANNEL_CONFIG_BASE ((unsigned long)0x00001108)


/*
 * Base address for the Channel # 1's Channel Pointer Status Register.
 *   
 *   This is relative to the base address register for the
 *   SEC2 coprocessor.  It is used to setup the
 *   #ChannelPointerStatusRegister variable array during Driver Startup
 *   Each channel has a Channel Pointer Status Register that is offset
 *   #CHANNEL_DISTANCE from the previous.
 */
#define CHANNEL_POINTER_STATUS_BASE ((unsigned long)0x00001110)



/*
 * Base address for the Channel # 1's Channel Next Descriptor Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #ChannelNextDescriptorRegister variable array during Driver Startup
 * Each channel has a Channel Next Descriptor Register that is offset
 * #CHANNEL_DISTANCE from the previous.
 */
#define CHANNEL_NEXT_DESCRIPTOR_BASE ((unsigned long)0x00001148)



/*  Base offsets for channel register pointer allocations during init */
#define CHANNEL_DBR_BASE  ((unsigned long)0x00001180)
#define CHANNEL_CDPR_BASE ((unsigned long)0x00002040)

/* Number of bytes to each successive channel register */
#define CHANNEL_DISTANCE ((unsigned long)0x00000100)


/*
 * Base address for the CHA # 1 (DES) CHA Reset Control Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #ChaResetControlRegister variable array during Driver Startup
 * Each CHA has a CHA Reset Control Register that is offset
 * #CHA_DISTANCE from the previous.
 */
#define CHA_RESET_CONTROL_BASE ((unsigned long)0x00002018)



/*
 * Base address for the CHA # 1 (DES) CHA Interrupt Status Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #ChaInterruptStatusRegister variable array during Driver Startup
 * Each CHA has a CHA Interrupt Status Register that is offset
 * #CHA_DISTANCE from the previous.
 */
#define CHA_INTERRUPT_STATUS_BASE ((unsigned long)0x00002030)




/*
 * Base address for the CHA # 1 (DES) CHA Interrupt Control Register.
 *   
 * This is relative to the base address register for the
 * SEC2 coprocessor.  It is used to setup the
 * #ChaInterruptControlRegister variable array during Driver Startup
 * Each CHA has a CHA Interrupt Control Register that is offset
 * #CHA_DISTANCE from the previous.
 */
#define CHA_INTERRUPT_CONTROL_BASE ((unsigned long)0x00002038)



#if 0
/*  Debugging Only definition. */
#define AFHA_STATUS_OFFSET ((unsigned long)0x0008028)
#define PKHA_STATUS_BASE ((unsigned long)0x0000C028)
#endif

/* offset bytes between CHA register origins */
#define CHA_DISTANCE ((unsigned long)0x00002000)


/*  Channel Interrupt Enable */
#define CHANNEL_INTEN ((unsigned long)0x00000002)

/* Force channel burst size to 128 bytes (normal default is 64) */
#define CHANNEL_BURST128 ((unsigned long)0x00000100)



/* 
 * Bit field setting to write to the 
 *   #ChannelConfigRegister in order to reset the channel. First Word
 *
 *   The bit setting indicates that we are resetting the channel
 */
#define CHANNEL_RESET ((unsigned long)0x00000001)



/*
 * Bit field mask for reading from the
 * #ChannelPointerStatusRegister in order
 * to determine if a CHA assigned to the channel generated
 * an error or not. First Word
 */
#define CHANNEL_CHA_ERROR_DOF ((unsigned long)0x80000000)

#define CHANNEL_CHA_ERROR_SOF ((unsigned long)0x40000000)

#define CHANNEL_CHA_ERROR_GER ((unsigned long)0x00100000)

#define CHANNEL_CHA_ERROR_SES ((unsigned long)0x00001000)



/*
 * Bit field mask for reading from the
 * #ChannelPointerStatusRegister in order
 * to determine if there is an illegal descriptor or not
 * Second Word
 */

#define CHANNEL_SCATTER_LENGTH_ERROR      ((unsigned long) 0x00000010)
#define CHANNEL_SCATTER_BOUND_ERROR       ((unsigned long) 0x00000020)
#define CHANNEL_GATHER_LENGTH_ERROR       ((unsigned long) 0x00000040)
#define CHANNEL_GATHER_BOUND_ERROR        ((unsigned long) 0x00000080)
#define CHANNEL_CHA_ERROR                 ((unsigned long) 0x00000100)
#define CHANNEL_CHA_ASSIGN_ERROR          ((unsigned long) 0x00000200)
#define CHANNEL_UNRECOGNIZED_HEADER_ERROR ((unsigned long) 0x00000400)
#define CHA_PARITY_SYSTEM_ERROR           ((unsigned long) 0x00000800) /* compatibility */
#define CHANNEL_NULL_FETCH_POINTER        ((unsigned long) 0x00000800)
#define CHANNEL_ZERO_SCATTER_LEN_ERROR    ((unsigned long) 0x00001000)
#define CHANNEL_BUS_MASTER_ERROR          ((unsigned long) 0x00002000)
#define CHANNEL_FETCH_FIFO_NEARMISS_ERROR ((unsigned long) 0x00004000)
#define CHANNEL_FETCH_FIFO_OVERFLOW_ERROR ((unsigned long) 0x00008000)



/*
 * Bit field setting to write to the 
 * #InterruptControlRegister in order to disable interrupts when
 * any of the CHAs have completed an operation
 */

#define SET_ALL_CHA_INT_DONE_MASK            ((unsigned long)0x00111111)


/*
 * Bit field setting to write to any of the 
 * #ChaResetControlRegister"s" in order to software reset the CHA
 */
#define CHA_SOFTWARE_RESET                   ((unsigned long)0x00000001)


/*
 * Bit field setting to write to any of the
 * #ChaInterruptControlRegister"s" in order to disable parity errors
 * for the CHA.  This is only good for DES specific CHAs.
 */
#define CHA_DESA_KEY_PARITY_ERROR_DISABLE    ((unsigned long)0x00002000)

/*
 * Bit field setting to write to any of the
 * #ChaInterruptControlRegister"s" in order to disable illegal address errors
 * for the CHA.  This is only good for PK specific CHAs.
 */
#define CHA_PKHA_ILLEGAL_ADDR_ERROR_DISABLE  ((unsigned long)0x00000040)


/* Number of pointer/length pairs within a DPD */
#define NUM_DPD_FLDS    7

/* Physical representation of a Data Packet Descriptor. */
typedef struct dpd
{
    unsigned long header;
    unsigned long dpd_reserved;
    /*  Array of pointer/length pair within a DPD */
    struct dpd_fld
    {
        unsigned long len;
        unsigned long ptr;
    } fld[NUM_DPD_FLDS];
} DPD;



/*  CHANNEL_ASSIGNMENT Controls the assignment of channels  */
typedef struct channel_assignment
{
    /* Current state of the channel. Should normally be CHANNEL_FREE   */
    /* or CHANNEL_DYNAMIC if in use. If using a static assignment, may */
    /* also be CHANNEL_STATIC_ASSIGNED or CHANNEL_STATIC_BUSY          */
    unsigned char       assignment;     

    /* ID of task using channel, if static assignment used           */
    int                 ownerTaskId;    
     
    /* Pointer to the head of a list of requests                    */              GENERIC_REQ         *pReq;         

    /* Pointer to current entry in request list                        */
    GENERIC_REQ         *pCurrReq;  
     
    /* List of pointers to DPDs that will be used to build requests    */
    DPD                 **Dpds;        
    
    /* Pointer to auxiliary deferred-service handler to be registered  */
    /* by the owner of a static channel. Auxiliary handlers will be    */
    /* passed the number of the channel being handled, in case one     */
    /* handler will service multiple channels. Return value is not     */
    /* used at this time.                                              */
    int                (*auxHandler)(int);
} CHANNEL_ASSIGNMENT;



/*  A double linked list to pending dynamic requests */
typedef struct queue_entry
{
    int                 requestingTaskId;   /* Requesting task's Id.
                                            This becomes
                                            channel_assignment::ownerTaskId
                                            when a channel can be assigned. */
                                            
    int                 chan;               /* channel if static
                                            be free before the entry can
                                            be assigned to a channel */
    GENERIC_REQ         *pReq;              /* Pointer to the start of a
                                            linked list describing the request
                                            This becomes
                                            channel_assignment::pReq
                                            when a channel can be assigned. */
    struct queue_entry  *next;              /* pointer to next entry */
    struct queue_entry  *previous;          /* pointer to previous entry */
} QUEUE_ENTRY;


#define ALL_INT_ERRORS_MASK_LOW    ((unsigned long)0x000000ff)
#define ALL_INT_ERRORS_MASK_HIGH   ((unsigned long)0x00333333)


/* All possible channel done interrupts from high interrupt status word */
#define ALL_CHANNEL_INT_DONE_MASK  ((unsigned long)0x00000055)
#define CHANNEL_INT_DONE_1         ((unsigned long)0x00000001)
#define CHANNEL_INT_DONE_2         ((unsigned long)0x00000004)
#define CHANNEL_INT_DONE_3         ((unsigned long)0x00000010)
#define CHANNEL_INT_DONE_4         ((unsigned long)0x00000040)

/* All possible CHA done interrupts from low interrupt status word */
#define ALL_CHA_INT_DONE_MASK      ((unsigned long)0x01111111)
#define CHA_INT_DONE_DEU           ((unsigned long)0x00000001)
#define CHA_INT_DONE_AESU          ((unsigned long)0x00000010)
#define CHA_INT_DONE_MDEU          ((unsigned long)0x00000100)
#define CHA_INT_DONE_AFEU          ((unsigned long)0x00001000)
#define CHA_INT_DONE_RNG           ((unsigned long)0x00010000)
#define CHA_INT_DONE_PKEU          ((unsigned long)0x00100000)
#define CHA_INT_DONE_KEU           ((unsigned long)0x01000000)

/* All possible channel error interrupts from high interrupt status word */
#define ALL_CHANNEL_INT_ERROR_MASK ((unsigned long)0x000000aa)
#define CHANNEL_INT_ERROR_1        ((unsigned long)0x00000002)
#define CHANNEL_INT_ERROR_2        ((unsigned long)0x00000008)
#define CHANNEL_INT_ERROR_3        ((unsigned long)0x00000020)
#define CHANNEL_INT_ERROR_4        ((unsigned long)0x00000080)

/* All possible CHA done interrupts from low interrupt status word */
#define ALL_CHA_INT_ERROR_MASK     ((unsigned long)0x02222222)
#define CHA_INT_ERROR_DEU          ((unsigned long)0x00000002)
#define CHA_INT_ERROR_AESU         ((unsigned long)0x00000020)
#define CHA_INT_ERROR_MDEU         ((unsigned long)0x00000200)
#define CHA_INT_ERROR_AFEU         ((unsigned long)0x00002000)
#define CHA_INT_ERROR_RNG          ((unsigned long)0x00020000)
#define CHA_INT_ERROR_PKEU         ((unsigned long)0x00200000)
#define CHA_INT_ERROR_KEU          ((unsigned long)0x02000000)


/*
 * DEVICE_REGS is used to store interrupt staus/error information 
 * by the ISR, so that the deferred service routine can access it
 * after interrupt acknowledgement has completed
 */
typedef struct device_regs
{
    unsigned long IntStatus[2];                     /* Copy of the ISR */
    unsigned long IntStatusDbg[2];                  /* (unused)        */
    unsigned long ChaAssignmentStatus[2];           /* Current CHA assignments */
    unsigned long ChannelError[NUM_CHANNELS][2];    /* Channel status registers */
    unsigned long ChaError[NUM_CHAS][2];            /* CHA status registers */
} DEVICE_REGS;



/* Queueing */
#ifdef __KERNEL__
#define PREGS_QUEUE_DEPTH 16
extern DEVICE_REGS pRegs_queue[PREGS_QUEUE_DEPTH];
extern unsigned PQ_head, PQ_tail, PQ_entries;

#define QmsgSend() \
        if (PQ_entries == PREGS_QUEUE_DEPTH) { \
                panic("SEC driver's pRegs queue overflow!"); \
        } \
        pRegs_queue[PQ_tail] = *pRegs; \
        PQ_tail = (PQ_tail + 1) % PREGS_QUEUE_DEPTH; \
        PQ_entries ++; \
        tasklet_schedule(&isr_tasklet)

#define QmsgWait() \
        disable_irq(INUM_SEC2); \
        if (!PQ_entries) { \
                enable_irq(INUM_SEC2); break;} \
        *pRegs = pRegs_queue[PQ_head]; \
        PQ_head = (PQ_head + 1) % PREGS_QUEUE_DEPTH; \
        PQ_entries --; \
        enable_irq(INUM_SEC2)
#endif




/* Components of a SEC2 scatter/gather list element. This is the native format */
/* that the channel processor expects to see so as to translate a scattered    */
/* buffer in the course of processing                                          */


#define MAX_SCATTER_FRAGS 1024  /* limit of static fragment descriptor pool */

/* used in "linkStat" below */
#define SEGLIST_RETURN 0x0200
#define SEGLIST_NEXT   0x0100

typedef struct _sec2scatterListElement
{
  unsigned short segLen;
  unsigned short linkStat;
  void          *segAddr;
} SCATTER_ELEMENT;



#if defined(SEC2_GLOBAL) || defined(__cplusplus)

/*
 * unsigned long ChaDoneInterruptMasks[NUM_CHAS]
 * An array of CHA done completion bit masks.
 *
 * The index of the array represents the CHA.  This bit mask
 * is applied against #InterruptStatusRegister in order to
 * determine if an interrupt is due to a CHA completion or not.
 */
const unsigned long ChaDoneInterruptMasks[NUM_CHAS] = 
{
    0x00000001,
    0x00000010,
    0x00000100,
    0x00001000,
    0x00010000,
    0x00100000

};

/*
 * unsigned long ChannelDoneInterruptMasks[NUM_CHANNELS]
 * An array of channel done completion bit masks.
 *
 * The index of the array represents the channel.  This bit mask
 * is applied against #InterruptStatusRegister in order to
 * determine if an interrupt is due to a channel completion or not.
 */
const unsigned long ChannelDoneInterruptMasks[NUM_CHANNELS] = 
{
    0x00000001,
    0x00000004,
    0x00000010,
    0x00000040
};


/*
 * unsigned long ChannelErrorInterruptMasks[NUM_CHANNELS]
 * An array of channel error bit masks.
 *
 * The index of the array represents the channel.  This bit mask
 * is applied against #InterruptStatusRegister in order to
 * determine if an interrupt is due to a channel error or not.
 */
const unsigned long ChannelErrorInterruptMasks[NUM_CHANNELS] = 
{
    0x00000002,
    0x00000008,
    0x00000020,
    0x00000080
};

/*
 * unsigned long ChaErrorInterruptMasks[NUM_CHAS]
 * An array of CHA error bit masks.
 *
 * The index of the array represents the CHA.  This bit mask
 * is applied against #InterruptStatusRegister in order to
 * determine if an interrupt is due to a CHA error or not.
 */
const unsigned long ChaErrorInterruptMasks[NUM_CHAS] =
{
    0x00000002,
    0x00000020,
    0x00000200,
    0x00002000,
    0x00020000,
    0x00200000
};

#else

extern const unsigned long ChaDoneInterruptMasks[];
extern const unsigned long ChannelDoneInterruptMasks[];
extern const unsigned long ChannelErrorInterruptMasks[];
extern const unsigned long ChaErrorInterruptMasks[];

#endif

#ifndef SEC2_GLOBAL
#define SEC2_GLOBAL extern
#endif

/*
 * SEC2_GLOBAL
 * A definition used to control allocation and referencing
 * of global variables.
 *   
 * When the definition is not set it is considered
 * to be used as a reference (extern is prepended).
 *
 * When the definition is set (to nothing) it is considered to be
 * allocating the global variable.  This is done in one place
 * only.  In init.c
 */




/*
 * Note that the "Descriptors" array has been moved from this
 * global area to an allocated (DMA-safe) buffer during init.
 * This change was made in the linux 2.6 transition
 */
/* SEC2_GLOBAL DPD Descriptors[NUM_CHANNELS][MAX_DPDS]; */

SEC2_GLOBAL unsigned char *dpdBlock;

/*
 * CHANNEL_ASSIGNMENT ChannelAssignments
 * A global variable which is a array of channel assignment
 * structures.  Each channel is reqresented by its own channel
 * assignment structure.
 */
SEC2_GLOBAL CHANNEL_ASSIGNMENT ChannelAssignments[NUM_CHANNELS];

SEC2_GLOBAL DPD *dpd[NUM_CHANNELS][MAX_DPDS];

SEC2_GLOBAL QUEUE_ENTRY Queue[QUEUE_ENTRY_DEPTH];


/* 
 * A global variable which is an array of CHAs that
 * are assigned to channels.
 *       
 * The value in each element is either
 * #CHA_BUSY which is unassigned or the channel that is
 * assigned (1-4) to the CHA represented by the array index.
 */
SEC2_GLOBAL unsigned char ChaAssignments[16];

/*
 * A global variable which is the address of the base
 * address register for the SEC2 coprocessor
 *   
 * All registers accessed are offsets from this register.
 * This variable is initialized during startup.
 */
SEC2_GLOBAL unsigned long IOBaseAddress;

/*
 * A global variable which is a pointer to the CHA
 * Assignment Status Register
 *  
 * This variable is initialized during startup.  It is determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChaAssignmentStatusRegister;

/*
 * A global variable which is a pointer to the CHA
 * Assignment Control Register
 *
 * This variable is initialized during startup.  It is determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChaAssignmentControlRegister;

/*
 * A global variable which is a pointer to the Interrupt
 * Control Register
 *   
 * This variable is initialized during startup.  It is determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *InterruptControlRegister;

/*
 * A global variable which is a pointer to the Interrupt
 * Status Register
 * 
 * This variable is initialized during startup.  It is determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *InterruptStatusRegister;

/*
 * A global variable which is a pointer to the Interrupt
 * Clear Register
 *   
 *  This variable is initialized during startup.  It is determined
 *  to be a fixed offset from the #IOBaseAddress
 *  register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *InterruptClearRegister;

/*
 * A global variable which is a pointer to the ID Register
 *   
 * This variable is initialized during startup.  It is determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *IdRegister;

/*
 * A global variable which is a array of pointers to the
 * Channel Config Registers.  There is one per channel.
 *
 * This pointers are initialized during startup.  They are determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChannelConfigRegister[NUM_CHANNELS];

/*
 * A global variable which is a array of pointers to the
 * Channel Pointer Status Registers.  There is one per channel.
 *
 * This pointers are initialized during startup.  They are determined
 * to be a fixed offset from the #IOBaseAddress register.
 * See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChannelPointerStatusRegister[NUM_CHANNELS];

/*
 * A global variable which is a array of pointers to the
 * Channel Next Descriptor Registers.  There is one per channel.
 *   
 * This pointers are initialized during startup.  They are determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChannelNextDescriptorRegister[NUM_CHANNELS];

/*
 * A global variable which is a array of pointers to the
 * CHA Reset Control Registers.  There is one per CHA.
 *
 * This pointers are initialized during startup.  They are determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChaResetControlRegister[NUM_CHAS];

/*
 * A global variable which is a array of pointers to the
 * CHA Interrupt Status Registers.  There is one per CHA.
 *   
 * This pointers are initialized during startup.  They are determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChaInterruptStatusRegister[NUM_CHAS];

/*
 * A global variable which is a array of pointers to the
 * CHA Interrupt Control Registers.  There is one per CHA.
 *  
 * This pointers are initialized during startup.  They are determined
 * to be a fixed offset from the #IOBaseAddress
 * register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *ChaInterruptControlRegister[NUM_CHAS];

/*
 * Debugging Only variables.
 */
SEC2_GLOBAL volatile unsigned long *ChannelDataBufferDescriptorRegister[NUM_CHANNELS];
SEC2_GLOBAL volatile unsigned long *ChannelCurrentDescriptorPointerRegister[NUM_CHANNELS];

/*
 * long *MasterControlRegister
 * A global variable which is a pointer to the Master Control Register
 *  
 *  This variable is initialized during startup.  It is determined
 *  to be a fixed offset from the #IOBaseAddress
 *  register.  See the SEC2 manual for details.
 */
SEC2_GLOBAL volatile unsigned long *MasterControlRegister;

/*
 * FreeChannels
 * A global variable which keeps track of the number of
 * free channels
 */
SEC2_GLOBAL int FreeChannels;

/*
 * int FreeRngas
 * A global variable which keeps track of the number of
 * free RNG CHAs
 */
SEC2_GLOBAL int FreeRngas;

/*
 * int FreeAfhas
 * A global variable which keeps track of the number of
 * free ARC4 CHAs
 */
SEC2_GLOBAL int FreeAfhas;

/*
 * int FreeDesas
 * A global variable which keeps track of the number of
 * free DES CHAs
 */
SEC2_GLOBAL int FreeDesas;

/*
 * int FreeMdhas
 * A global variable which keeps track of the number of
 * free MD CHAs
 */
SEC2_GLOBAL int FreeMdhas;

/*
 * int FreePkhas
 * A global variable which keeps track of the number of
 * free PK CHAs
 */
SEC2_GLOBAL int FreePkhas;

/*
 * int FreeAesas
 * A global variable which keeps track of the number of
 * free AESA CHAs
 */
SEC2_GLOBAL int FreeAesas;

/*
 * int FreeKeas
 * A global variable which keeps track of the number of
 *  free KEA CHAs
 */
SEC2_GLOBAL int FreeKeas;

/*
 * unsigned long BlockSize
 * A global variable which indicates what the current block size
 *  is
 */
SEC2_GLOBAL unsigned long BlockSize;

/*
 * ProcessQueueTop
 * A global variable which is a pointer to the top
 * of the Pending dynamic request list (list of ::queue_entry"s").
 */
SEC2_GLOBAL QUEUE_ENTRY *ProcessQueueTop;

/*
 * ProcessQueueBottom
 * A global variable which is a pointer to the bottom
 *  of the Pending dynamic request list (list of ::queue_entry"s").
 */
SEC2_GLOBAL QUEUE_ENTRY *ProcessQueueBottom;

/*
 * unsigned long QueueEntryDepth
 * A global variable which keeps track of the number
 * of entries in the Pending dynamic request list pointed to
 * by #ProcessQueueTop
 */
SEC2_GLOBAL unsigned long QueueEntryDepth;

#ifdef __KERNEL__
extern spinlock_t ChannelAssignLock;
#else
#ifdef INIT_C
DECLARE_MUTEX(ChannelAssignSemId);
#endif
#endif

#ifdef INIT_C
DECLARE_MUTEX(QueueSemId);
#else
extern struct semaphore QueueSemId;
#endif


#ifdef __KERNEL__
/* Device major number registered at device initialization */
SEC2_GLOBAL int sec2_devmajor;

#endif



SEC2_GLOBAL unsigned int cha_idx[6];




/* Core ID/rev selection */
SEC2_GLOBAL int           sec_ID, sec_major_rev, sec_minor_rev;
SEC2_GLOBAL unsigned char sec_int_opts, sec_cfg_opts;






/* Forward declarations for internal driver functions not meant for "public use" */
int ReserveChannelStatic(unsigned char *, int);
int ReleaseChannel(unsigned char, int, BOOLEAN);

int MapScatterFragments(unsigned long  ,
                        unsigned long *);

void UnmapScatterFragments(unsigned long *);



/* Read/write a block of data from hardware registers (obsolete) */
void IORead(unsigned long *, int, volatile unsigned long *);
void IOWrite(unsigned long *, int, volatile unsigned long *);


/* Register driver and it's handlers with the OS */
IO_STATUS IORegisterDriver(void);

/* Set up internal driver semaphores */
void IOInitSemaphores(void);

/* Initialize drive queues */
IO_STATUS IOInitQs(void);

/* Enable/Disable the hardware interrupt */
IO_STATUS IOConnectInterrupt(void);
void IODisconnectInterrupt(void);

/* Allocate/free a dynamic queue entry (obsolete) */
QUEUE_ENTRY *IOAllocateQEntry(void);
void IOFreeQEntry(QUEUE_ENTRY *);

/* Initialize/Allocate/Release channel assignment table */
IO_STATUS InitChannelAssignment(int);
void FreeChannelAssignment(int);

/* Translate CHA index to type */
int ChaNumToType(int);

/* Check to see if the CHA type is available */
extern int CheckChas(unsigned long);

/* Open a new connection to this driver */
#ifdef __KERNEL__
int SEC2Open(struct inode *, struct file *);
#else
int SEC2Open(DEV_HDR *, int, int);
#endif

/* Close a connection to this driver */
#ifdef __KERNEL__
int SEC2Close(struct inode *, struct file *);
#else
int SEC2Close(int);
#endif

/* Request a driver operation*/
#ifdef __KERNEL__
int SEC2_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
#else
int SEC2_ioctl(int, register int, register void *);
#endif

/* Driver's top level ISR handler */
#ifdef __KERNEL__
irqreturn_t InterruptServiceRoutine(int irq, void *dev_id, struct pt_regs *pt_regs);
#else
void InterruptServiceRoutine(void);
#endif

/* ISR's Deferred processing (bottom half) handler */
#if defined(_MSC_VER)
extern void ProcessingComplete(void);
#else
extern void ProcessingComplete(unsigned long not_used);
#endif

/* External function to mark scatter buffers for kernel-state usage */
#ifdef __KERNEL__
int MarkScatterBuffer(void *, void *);
#endif

/* Take a user request, build a DPD, and submit it for processing */
int ProcessRequest(GENERIC_REQ *, int);

/* Start any pending requests */
void ScheduleNext(void);

/*
 * BOOLEAN StartDescriptor(int channel, unsigned long addr)
 * Start a DPD request to the SEC2
 *
 *  Write addr to the #ChannelNextDescriptorRegister register
 *  for this channel
 *
 *  Timers are launched if USE_TIMERS are defined
 *
 *  Arguments:
 *  channel
 *  addr = address to be saved in #ChannelNextDescriptorRegister
 *  returns TRUE if successful
 */
extern BOOLEAN StartDescriptor(int channel, unsigned long addr);

/*
 * int CompleteRequest(int channel, int status, BOOLEAN bDoNotify, DEVICE_REGS *pRegs)
 * Complete a request
 *
 * Call FinishIoRequest()
 *
 *  Cleanup the ::req_list associated with this request by calling
 *  CleanupRequestList()
 *
 *  Make the #ChannelAssignment entry for the \a channel available
 *
 *  If bDoNotify is set, call a notify routine
 *
 *  Arguments
 *  channel   Channel request was issued on
 *  status    Status of the request
 *  bDoNotify Notify requester?
 *  pRegs     Pointer to register snapshot
 *  returns   status
 */
extern int CompleteRequest(int channel, int status, BOOLEAN bDoNotify, DEVICE_REGS *pRegs);

/*
 * void RemoveQueueEntry(QUEUE_ENTRY *entry)
 * Remove entry from pending dynamic request list
 *
 * entry = queue_entry to be deleted
 */
extern void RemoveQueueEntry(QUEUE_ENTRY *entry);


/*
 * int RequestToDpd(int channel)
 * Transform the current request into a chain of DPDs
 *
 * This routine will traverse a req_list populating
 * CHANNEL_ASSIGNMENT:Dpds for the given channel.
 * The req_list will have been built by BuildRequestList().
 *
 * After successful completion, the caller will be able to
 * get the hardware to process the request by calling
 * StartDescriptor().  StartDescriptor() expects a Hardware
 * specific address, i.e. use CHANNEL_ASSIGNMENT::addrHwDpds
 * for the given \a channel.
 *
 *  channel = Channel that DPD(s) will be launched on
 *  returns SEC2_SUCCESS if successful
 */
extern int RequestToDpd(GENERIC_REQ *pReq, int channel);

/*
 *void CopyRegsToStatus(STATUS_REQ *pStatus, DEVICE_REGS *pRegs)
 * A helper function to copy a register snapshot into a status request
 *
 * pStatus = Pointer to status request
 * pRegs   = Pointer to register snapshot
 */
extern void CopyRegsToStatus(STATUS_REQ *pStatus, DEVICE_REGS *pRegs);

/*
 * int SetBlockSize(unsigned long* newBlockSize)
 * Set the Block Size for DPD requests
 *
 *  newBlockSize = Size to set BlockSize to.
 *  returns SEC2_SUCCESS if successful
 */
extern int SetBlockSize(unsigned long* newBlockSize);

/*
 * int GetStatus(STATUS_REQ *pStatus, int callingTaskId)
 * Get Current Status of the Driver
 *
 * pStatus        Pointer to status request.
 * callingTaskId  Id of Task that is making the request
 * returns SEC2_SUCCESS
 */
extern int GetStatus(STATUS_REQ *pStatus, int callingTaskId);

#endif /* SEC2DRIVER_H */
