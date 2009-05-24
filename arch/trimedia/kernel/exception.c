/* 
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/hardirq.h>

#include <tmExceptions.h>
#include <tmTimers.h>
#include <mmio.h>

static int s_iTimer = 0;
static timInstanceSetup_t s_setupTimer;

static void ExecutionTrapHandler(void)
{
#pragma TCS_interruptible_handler
    static unsigned long spc, dpc;

    spc=readspc();
    dpc=readdpc();

    printk("Trap at spc=%x,dpc=%x\n",spc,dpc);
    exit(spc);
}

static void InstallTrapHandler(timSource_t source, intHandler_t handler)
{
	tmLibdevErr_t err;

	if (s_iTimer==0)
	{
		err = timOpen(&s_iTimer);
		if(err!=TMLIBDEV_OK){
			panic("InstallTrapHandler: can not hold the Timer!\n");	
		}
	}

	s_setupTimer.handler = handler;
	s_setupTimer.source = source;
	s_setupTimer.prescale = 1;
	s_setupTimer.priority = intPRIO_6;
	s_setupTimer.modulus = 1;
	s_setupTimer.running = True;

	err = timInstanceSetup(s_iTimer, &s_setupTimer);
	if(err!=TMLIBDEV_OK){
		panic("InstallTrapHandler: can not set timer instance.\n");	
	}

	return;
}

/* Traps attempted execution inside or outside the given range */
/* Calling this function disables data access traps. */
/* Note: successive calls override previous ones. */
/* Set both addresses to Null to disable execution trap handling */
extern void tmbreak_trapexecution(void *low, void *high, int inside)
{
	/* disable */
	MMIO(BDCTL) = 0;
	MMIO(BICTL) = 0;

	if (low!=Null || high!=Null)
	{
		InstallTrapHandler(timINSTBREAK, ExecutionTrapHandler);

		MMIO(BINSTLOW) = (UInt32)low;
		MMIO(BINSTHIGH) = (UInt32)high;
		MMIO(BICTL) = 0x01 | (inside ? 0x00 : 0x100);
	}
	printk(KERN_INFO "trimedia execution check installed over %x to %x\n", low, high);
}

static void ExceptionCommon(excException_t exception, u32 dpc, u32 spc)
{
	printk("\n! WARNING !\n");
	printk("Exception [%d] occurred, dpc[0x%x] spc[0x%x] task[%04s]\n", (int) exception, dpc, spc, current->comm);
	exit(dpc);
	return;
}

static void ExceptionDBZ(u32 dpc,u32 spc)
{
	ExceptionCommon(excDBZ, dpc, spc);
	return;
}

static void ExceptionINX(u32 dpc,u32 spc)
{
	ExceptionCommon(excINX, dpc, spc);
	return;
}

static void ExceptionUNF(u32 dpc,u32 spc)
{
	ExceptionCommon(excUNF, dpc, spc);
	return;
}

static void ExceptionOVF(u32 dpc,u32 spc)
{
	ExceptionCommon(excOVF, dpc, spc);
	return;
}

static void ExceptionINV(u32 dpc,u32 spc)
{
	ExceptionCommon(excINV, dpc, spc);
	return;
}

static void ExceptionIFZ(u32 dpc,u32 spc)
{
	ExceptionCommon(excIFZ, dpc, spc);
	return;
}

static void ExceptionOFZ(u32 dpc,u32 spc)
{
	ExceptionCommon(excOFZ, dpc, spc);
	return;
}

static void ExceptionRSE(u32 dpcUnused,u32 spcUnused)
{
	u32              excRseCause;
	u32              excRsePrgCtr;
	int 		 index;
	static const String pErrorMsg =
		"\nCPU RSE reserved store exception !\n"
		"ERROR: CPU RSE exception cause: %s\n"
		"   RSE_PC Program Counter address:  0x%08X\n"
		"   RSE_CAUSE status register value: 0x%08X\n"
		"   Interrupt context (!0 = In ISR): 0x%08X\n";
	static const String pRseErrMsgs [5] =
	{
		{   "ILJMP - Illegal jump destination address"  }, // Bit0: ILJMP error
		{   "ILSTO - Illegal data store/write address"  }, // Bit1: ILSTO error
		{   "ILOP  - Illegal CPU instruction operation" }, // Bit2: ILOP error
		{   "ILSTM - MMIO address partial store/write"  }, // Bit3: ILSTM error
		{   "????  - Unrecognized RSE exception cause"  }  // Bit?: Unknown err
	}; // pRseErrMsgs []

	(void) dpcUnused;
	(void) spcUnused;

	excRseCause  = MMIO (RSE_CAUSE);   // RSE cause status
	excRsePrgCtr = MMIO (RSE_PC); // RSE program counter

	MMIO (RSE_CAUSE) = 0xF;    // clear RSE exception causes
	exit(excRsePrgCtr);

	for (index = 0; index < 4; index++)
	{
		if ((excRseCause & (1 << index)) != 0)
		{
			break;      // RSE cause identified
		}
	}

	(void) intSET_IEN();    // enable interrupts to allow printf to stdout

	printk(pErrorMsg,pRseErrMsgs[index],excRsePrgCtr,excRseCause,in_interrupt());

	for (;;)
	{
		// HALT (loop forever)
	}

}

static void ExceptionWBE(u32 dpc,u32 spc)
{
	ExceptionCommon(excWBE, dpc, spc);
	return;
}

static void ExceptionMSE(u32 dpc,u32 spc)
{
	ExceptionCommon(excMSE, dpc, spc);
	return;
}

// structure to map exception IDs to their handlers
typedef struct
{
	excException_t exception;
	excHandler handler;
} ExceptionHandler_t;

// table to map exception IDs to their handlers
ExceptionHandler_t ExceptionHandlerTable[] =
{
	{ excDBZ, ExceptionDBZ },
#ifdef USE_INX 
	{ excINX, ExceptionINX },
#endif 
	{ excUNF, ExceptionUNF },
	{ excOVF, ExceptionOVF },
	{ excINV, ExceptionINV },
	{ excIFZ, ExceptionIFZ },
	{ excOFZ, ExceptionOFZ },
	{ excRSE, ExceptionRSE },
	{ excWBE, ExceptionWBE },
	{ excMSE, ExceptionMSE },
	// put new exception handlers here
	{ excRSE, NULL }   // the NULL marks the end of the table
};


extern void register_exception_handlers(void)
{
	excInstanceSetup_t setup;
	ExceptionHandler_t *p;

	printk(KERN_INFO "Register handler for exception(s): ");


	// for each entry in the exception handler table
	for(p = ExceptionHandlerTable; p->handler; ++p)
	{
		setup.enabled = True;
		setup.handler = p->handler;

		printk("%d ", (int)(p->exception));

		// this grants us access to the exception handlere 
		excClose(p->exception);
		if (excOpen(p->exception) != TMLIBDEV_OK)
		{
			printk("failed to open handler for exception %d\n", (int)(p->exception));
			continue;
		}

		// this does the actual registration of the handler
		if (excInstanceSetup(p->exception, &setup) != TMLIBDEV_OK)
		{
			printk("failed to setup handler for exception %d\n", (int)(p->exception));
			continue;
		}
	}

	printk("\n");

	return;
}
