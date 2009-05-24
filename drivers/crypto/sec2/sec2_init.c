

/****************************************************************************
 * sec2_init.c -- The Initialization code for SEC2 Driver
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

/* Revision History:
 * 1.0   Aug 24,2003 dgs - adapted from the final version of mpc18x
 * 1.1.0 Nov 16,2004 sec - incorporate linux changes from prior versions
 * 1.1.1 Dec 16,2004 sec - remove old unused diagnostics
 * 1.2   Jan 27,2005 sec - fix comments associated with global inhibit
 * 1.3   Jul 28,2005 sec - add KFHA indexing
 * 1.5   Oct 13,2005 sec - initialization for aux channel handlers
 */

#define SEC2_GLOBAL
#define INIT_C

#include "Sec2Driver.h"



/*
 * SEC2DebugLevel is simply a global containing a bitmask that can be used
 * to enable certain types of relevant error reporting at runtime.
 */

#if defined(DBG)
/* unsigned long SEC2DebugLevel = (DBGTXT_SETRQ | DBGTXT_SVCRQ | DBGTXT_INITDEV | DBGTXT_DPDSHOW | DBGTXT_INFO | DBGTXT_DATASHOW); */
unsigned long SEC2DebugLevel = (DBGTXT_INFO);
#else
unsigned long SEC2DebugLevel = 0x0;
#endif

#ifdef __KERNEL__
#include <linux/init.h>
EXPORT_SYMBOL(SEC2DebugLevel);
#endif



/*************************************************************************
 * Function Name: SEC2DriverInit
 * Purpose:
 *          Step1: Build base address by requesting peripheral base
 *          Step2: Init ChannelRegister & Cha Register
 *          Step3: Allocate Channel Assignments
 *          Step4: Init ChannelAssignments & ChaAssignments
 *          Step5: Init ProcessQueueTop & ProcessQueueBottom 
 *          Step6: Init Semaphores
 *          Step7: Reset all Chas and Channels
 *          Step8: Config all Channels
 *          Step9: Connect Interrupt
 *          Step10: Enable Interrupt
 *          Step11: Return Success
 *          
 * Input: None
 *        
 * Return: SEC2_SUCCESS if everything passed, otherwise the error code
 *************************************************************************/ 
/* static unsigned long CurrentCHAStatus[2]; */

int SEC2DriverInit(void)
{
    int             status, i, count;
    int             cha;
    int             channel;
    unsigned long   address;
    char           *coreName;
    unsigned long   coreid, corerev, corecfg;

    /* CHA index array  */
    unsigned int    idx[] = {0, 2, 4, 3, 5, 1, 6};



#ifdef __KERNEL__
    bd_t *binfo = (bd_t *) __res;
#endif



#ifdef __KERNEL__
    /* Anything not done in the module init hook? Do here...*/
    
#endif



/* get base from the kernel, and map it into an accessible region */
#ifdef __KERNEL__
    IOBaseAddress = (unsigned long) ioremap((phys_addr_t)(binfo->bi_immr_base + TALITOS2X_BASE_ADDRESS), SEC2_BASE_SIZE);
#endif


/* Identify the running hardware */
    SEC2Dump(DBGTXT_INFO, ("SEC 2.x Security Processor Core\n"));
    SEC2Dump(DBGTXT_INFO, ("Driver Version %s %s %s\n", VERSION, __DATE__,__TIME__));

    corerev       = 0;
    corecfg       = 0;
    sec_int_opts  = 0;
    sec_cfg_opts  = 0;

    coreid = *((volatile unsigned long *)(IOBaseAddress + IDENTIFICATION_OFFSET) + 1);
    if (coreid == SEC_COREID_2_0_0)
    {
        corerev  = *((volatile unsigned long *)(IOBaseAddress + IDENTIFICATION_BLOCKREV));
        corecfg  = *((volatile unsigned long *)(IOBaseAddress + IDENTIFICATION_BLOCKREV + 4));
    }

    switch(coreid)
    {
        case SEC_COREID_2_0_0:
            if (!corerev) /* original 2.0 core, assume 2.0.0 */
            {
                sec_ID        = SEC2_BLOCK_ID;
                sec_major_rev = 0;
                sec_minor_rev = 0;
                coreName      = "2.0.0";
            }
            else
                if ((corerev == 0x00300000) && (corecfg == 0))
                {
                    /* this is first-rev 8548, new cfgID scheme not yet in use */
                    sec_ID        = SEC2_BLOCK_ID;
                    sec_major_rev = 1;
                    sec_minor_rev = 0;
                    coreName      = "2.1.0";
                }
                else /* then the block ID register is interpreted directly */
                {
                    sec_ID        = (corerev & ID_BLOCK_MASK) >> ID_BLOCK_SHIFT;
                    sec_major_rev = (corerev & ID_MAJOR_MASK) >> ID_MAJOR_SHIFT;
                    sec_minor_rev = (corerev & ID_MINOR_MASK) >> ID_MINOR_SHIFT;
                    sec_int_opts  = (corecfg & ID_INTEGRATION_MASK) >> ID_INTEGRATION_SHIFT;
                    sec_cfg_opts  = (corecfg & ID_CONFIG_MASK) >> ID_CONFIG_SHIFT;
                    coreName      = "2.0.0";
                    coreName[2]   = sec_major_rev + 48; /* add ASCII offset for ID string */
                    coreName[4]   = sec_minor_rev + 48;
                }
            break;

        case SEC_COREID_2_0_1:
            coreName      = "2.0.1";
            sec_ID        = SEC2_BLOCK_ID;
            sec_major_rev = 0;
            sec_minor_rev = 1;
            break;

        case SEC_COREID_2_1_0:
            coreName      = "2.1.0";
            sec_ID        = SEC2_BLOCK_ID;
            sec_major_rev = 1;
            sec_minor_rev = 0;
            break;

        case SEC_COREID_2_2_0:
            coreName      = "2.2.0";
            sec_ID        = SEC2_BLOCK_ID;
            sec_major_rev = 2;
            sec_minor_rev = 0;
            break;

        default:
            coreName      = "unknown";
            sec_ID        = 0;
            sec_major_rev = 0;
            sec_minor_rev = 0;
            break;
    }

    SEC2Dump(DBGTXT_INFO,
             ("Security Core Version Mask: %s (0x%08lx/0x%08lx) options 0x%02x:0x%02x\n",
             coreName, coreid, corerev, sec_int_opts, sec_cfg_opts));


    /* set up rebound cha index table */
    memcpy ((char *)cha_idx, (char *)idx, sizeof (idx));


    /* set up SEC2 register addresses */
    /* controller registers */

    ChaAssignmentStatusRegister = 
        (volatile unsigned long*)(IOBaseAddress + CHA_ASSIGNMENT_STATUS_OFFSET);

    InterruptControlRegister =
        (volatile unsigned long*)(IOBaseAddress + INTERRUPT_CONTROL_OFFSET);

    InterruptStatusRegister =
        (volatile unsigned long*)(IOBaseAddress + INTERRUPT_STATUS_OFFSET);

    InterruptClearRegister =
        (volatile unsigned long*)(IOBaseAddress + INTERRUPT_CLEAR_OFFSET);

    IdRegister = 
        (volatile unsigned long*)(IOBaseAddress + IDENTIFICATION_OFFSET);

    MasterControlRegister = 
        (volatile unsigned long *)(IOBaseAddress + SEC2_MASTER_CONTROL_REGISTER);

    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->IOBaseAddress=0x%08lx\n", IOBaseAddress));



    /* Reset the device */
    /* Note that we turn on global inhibit for all processors. At the time    */
    /* of coding, GI only exists in the 8349; it's irrelevant to PQ3 devices. */
    /* So, it's just turned on for all processors, which is OK for now.       */
    /* In time, this may be need to become an option in build configuration.  */
    
    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->reset Master Control Register\n"));
    *(MasterControlRegister)     = SEC2_RESET | SEC2_GLOBAL_INHIBIT;
    *(MasterControlRegister + 1) = 0;


    SEC2Dump(DBGTXT_INITDEV,
            ("SEC2DriverInit->IdRegister=0x%08lx 0x%08lx\n",
            *(IdRegister), 
            *(IdRegister + 1)));



    /* channel registers */
    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->channel registers\n"));

    for (channel=0, address=CHANNEL_CONFIG_BASE; channel<NUM_CHANNELS;
        channel++,address+=CHANNEL_DISTANCE)
    {
        ChannelConfigRegister[channel] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    }

    for (channel=0,address=CHANNEL_POINTER_STATUS_BASE; channel<NUM_CHANNELS;
        channel++,address+=CHANNEL_DISTANCE)
    {
        ChannelPointerStatusRegister[channel] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    }

    for (channel=0,address=CHANNEL_NEXT_DESCRIPTOR_BASE; channel<NUM_CHANNELS;
        channel++,address+=CHANNEL_DISTANCE)
    {
        ChannelNextDescriptorRegister[channel] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    }

    for (channel=0,address=CHANNEL_DBR_BASE; channel<NUM_CHANNELS;
        channel++,address+=CHANNEL_DISTANCE)
    {
        ChannelDataBufferDescriptorRegister[channel] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    }

    for (channel=0,address=CHANNEL_CDPR_BASE; channel<NUM_CHANNELS;
        channel++,address+=CHANNEL_DISTANCE)
    {
        ChannelCurrentDescriptorPointerRegister[channel] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    }


    /* cha registers */
    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->cha registers\n"));

    for (cha=0,address=CHA_RESET_CONTROL_BASE; cha<NUM_CHAS;
        cha++,address+=CHA_DISTANCE)
    {
        ChaResetControlRegister[cha] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    } 

    for (cha=0,address=CHA_INTERRUPT_STATUS_BASE; cha<NUM_CHAS;
        cha++,address+=CHA_DISTANCE)
    {
        ChaInterruptStatusRegister[cha_idx[cha]] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    }

    for (cha=0,address=CHA_INTERRUPT_CONTROL_BASE; cha<NUM_CHAS;
        cha++,address+=CHA_DISTANCE)
    {
        ChaInterruptControlRegister[cha] = 
            (volatile unsigned long*)(IOBaseAddress + address);
    }

    /* allocate other driver variables */
    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->allocating other driver variables\n"));

    /* variable init */
    FreeChannels = NUM_CHANNELS;
    FreeRngas    = NUM_RNGAS;
    FreeAfhas    = NUM_AFHAS;
    FreeDesas    = NUM_DESAS;
    FreeMdhas    = NUM_MDHAS;
    FreePkhas    = NUM_PKHAS;
    FreeAesas    = NUM_AESAS;
    FreeKeas     = NUM_KEAS;




    /*
     * Before initializing the channel assignment table, allocate
     * a table of DPDs that exist in DMA-safe memory. Bail if this
     * fails on us. We're trusting here that malloc() is really
     * being redefined as kmalloc() or some other memory-type-aware
     * mechanism
     */
     
    dpdBlock = malloc(sizeof(DPD) * NUM_CHANNELS * MAX_DPDS);
    if (dpdBlock == NULL)
        return SEC2_MEMORY_ALLOCATION;
      
    /* Init ChannelAssignments */
    for (channel = 0; channel < NUM_CHANNELS; channel++)
    {
        ChannelAssignments[channel].assignment  = CHANNEL_FREE;
        ChannelAssignments[channel].ownerTaskId = 0;
        ChannelAssignments[channel].pReq        = NULL;
        ChannelAssignments[channel].pCurrReq    = NULL;
	ChannelAssignments[channel].auxHandler	= NULL;
        ChannelAssignments[channel].Dpds        = &dpd[channel][0];

        for (count = 0; count < MAX_DPDS; count++)
            ChannelAssignments[channel].Dpds[count] =
                (DPD *)(dpdBlock + (channel * (sizeof(DPD) * MAX_DPDS)) + (count * sizeof(DPD)));
    }


    /* Init ChaAssignments */
    for (cha=0; cha<16; cha++)
        ChaAssignments[cha] = CHA_DYNAMIC;


    /* initialize process queue */
    for (i=0; i<QUEUE_ENTRY_DEPTH-1;i++)
        Queue[i].next = &Queue[i+1];

    Queue[QUEUE_ENTRY_DEPTH-1].next = &Queue[0];

    for (i=1; i<QUEUE_ENTRY_DEPTH;i++)
        Queue[i].previous = &Queue[i-1];

    Queue[0].previous = &Queue[QUEUE_ENTRY_DEPTH-1];

    ProcessQueueTop    = &Queue[0];
    ProcessQueueBottom = &Queue[0];

    IOInitSemaphores();


    /* reset all the CHA's */
    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->reset Reset Control Register %d\n", NUM_CHAS));
    for (cha=0; cha<NUM_CHAS; cha++)
    {

        *(ChaResetControlRegister[cha])     = 0;
        *(ChaResetControlRegister[cha] + 1) = CHA_SOFTWARE_RESET; 

        if (cha == CHA_DES)  /* is this cha des? */
        {
            *(ChaInterruptControlRegister[cha])     = 0;
            *(ChaInterruptControlRegister[cha] + 1) = CHA_DESA_KEY_PARITY_ERROR_DISABLE;
        }

        if (cha == CHA_PKHA) /* is this cha the pkha? */
        {
            *(ChaInterruptControlRegister[cha]) = 0;
            *(ChaInterruptControlRegister[cha] + 1) = CHA_PKHA_ILLEGAL_ADDR_ERROR_DISABLE;
        }
    }

    /* reset all the channels */
    SEC2Dump(DBGTXT_INITDEV,
             ("SEC2DriverInit->reset Crypto-Channel Configuration Registers %d\n",
             NUM_CHANNELS));

    for (channel=0; channel<NUM_CHANNELS; channel++)
    {
        /* reset the channel & configure the channels */
        *(ChannelConfigRegister[channel])     = CHANNEL_RESET;
        *(ChannelConfigRegister[channel] + 1) = 0;

        *(ChannelConfigRegister[channel])     = 0;
        *(ChannelConfigRegister[channel] + 1) = CHANNEL_INTEN;
    }

    /* Enable the main interrupt from SEC core at the controller/OS level */
    if ((status = IOConnectInterrupt()) != SEC2_SUCCESS)
        return status;

    /* enable interrupts at the core level -- channels done and all errors */
    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->enable interrupts\n"));



    /* Unmask all "done" bits from all channels and accelerators */
    *(InterruptControlRegister)     = ALL_CHANNEL_INT_DONE_MASK | ALL_CHANNEL_INT_ERROR_MASK;
    *(InterruptControlRegister + 1) = ALL_CHA_INT_ERROR_MASK;
    
    SEC2Dump(DBGTXT_INITDEV, ("SEC2DriverInit->return SEC2_SUCCESS!\n"));
    
    return SEC2_SUCCESS;
}  /* end of DriverEntry */







void SEC2DriverShutdown(void)
{

    /* Shut interrupts off in the device */
    *(InterruptControlRegister)     = 0;
    *(InterruptControlRegister + 1) = 0;
        
    /* return the DPD block to the kernel */
    free(dpdBlock);
   
#ifdef __KERNEL__
    iounmap((void *)IOBaseAddress);
    free_irq(INUM_SEC2, NULL);
#endif


}





/* Module initialization stuff for Linux */
#ifdef __KERNEL__

/* fopts struct from device registration */
SEC2_GLOBAL struct file_operations sec2_fopts = {
    .open    = SEC2Open,
    .release = SEC2Close,
    .ioctl   = SEC2_ioctl,
};

static int __init sec2_init(void)
{
    int rtstat;
    
    /* Assume that this is going to be a chrdev */
    /* If a major number isn't specified, default to dynamic assignment */
    rtstat = register_chrdev(SEC2_DEVMAJOR, SEC2_DEVNAME, &sec2_fopts);
    if (rtstat < 0) {
        printk(KERN_WARNING "sec2drv: can't register driver, fatal\n");
        return rtstat;
    }

    if (SEC2_DEVMAJOR == 0) /* if no specific major requested, and there was no error above */
        sec2_devmajor = rtstat; /* set the dynamic number */
        
    /* Now go call the standard driver initialization */
    rtstat = SEC2DriverInit();
    if (rtstat == 0)
	    printk(KERN_INFO "MPC8xxx SEC 2.x driver initialized\n");
    return rtstat;
}


static void __exit sec2_exit(void)
{
    int rtstat;

    SEC2DriverShutdown();
    
    rtstat = unregister_chrdev(sec2_devmajor, SEC2_DEVNAME);
}


EXPORT_SYMBOL (SEC2_ioctl);
EXPORT_SYMBOL (MarkScatterBuffer);
/* EXPORT_SYMTAB */

module_init(sec2_init);
module_exit(sec2_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SEC 2.x Security Core Device Driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
#endif
