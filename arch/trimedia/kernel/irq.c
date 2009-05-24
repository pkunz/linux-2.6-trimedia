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

#include <linux/interrupt.h>
#include <linux/seq_file.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/target/irq.h>

#include <tmInterrupts.h>
#include <mmio.h>
#include <tmTimers.h>

#define ASSERT(x)    BUG_ON(!(x))

#undef 	IRQ_DEBUG
#ifndef IRQ_DEBUG 
#define DEBUGP(format, args...)
#else
#define DEBUGP printk
#endif

extern void 
ack_bad_irq(unsigned int irq)
{
	printf("Unexpected IRQ trap at %d\n",irq);
}

static void trimedia_irq_enable(int irq)
{
	intInstanceSetup_t setup;
	tmLibdevErr_t ret;

	ASSERT( (irq>=0) && (irq<NR_IRQS));
	
	ret=intGetInstanceSetup((intInterrupt_t)irq,&setup);
	ASSERT(ret==TMLIBDEV_OK);

	setup.enabled=True;

	ret=intInstanceSetup((intInterrupt_t)irq,&setup);
	ASSERT(ret==TMLIBDEV_OK);
}

static void trimedia_irq_disable(int irq)
{
	intInstanceSetup_t setup;
	tmLibdevErr_t ret;

	ASSERT( (irq>=0) && (irq<NR_IRQS));
	
	ret=intGetInstanceSetup((intInterrupt_t)irq,&setup);
	ASSERT(ret==TMLIBDEV_OK);

	setup.enabled=False;

	ret=intInstanceSetup((intInterrupt_t)irq,&setup);
	ASSERT(ret==TMLIBDEV_OK);
}

static void trimedia_irq_ack(int irq) { }

static void trimedia_irq_end(int irq) { }

static hw_irq_controller trimedia_irq_controller={
	.typename = "trimedia irq controller",
	.enable   = trimedia_irq_enable,
	.disable  = trimedia_irq_disable,
	.ack  	  = trimedia_irq_ack,
	.end 	  = trimedia_irq_end,
};

struct tm_irq_info
{
	char* name;
	Bool enable;
	Bool level_triggered;
	intInterrupt_t number;
	intPriority_t prio;
	intHandler_t handler;
};

static struct tm_irq_info tm_irq_table[]={
	TM_IRQ_LIST()
};

unsigned long cycs_per_10ns = 0;

extern void __init init_IRQ(void)
{
	int i=0;
	tmLibdevErr_t ret;
	extern unsigned long irq_timer;

	/*init system timer*/
	Int timer;
	timInstanceSetup_t setup;
	UInt32  cycles;
	UInt32  one_s_in_nano_seconds = 1000000000;

	ret=timOpen(&timer);
	ASSERT(ret==TMLIBDEV_OK);
	
#if defined(__TCS_pnx1005__) && defined(__TCS_ttisim__)
	cycs_per_10ns = 0x00000001;
	cycles        = 0x0016e360;
#else
	timToCycles(10,&cycs_per_10ns);
	timToCycles(one_s_in_nano_seconds/HZ,&cycles);
#endif 
	setup.source=timCLOCK;
	setup.prescale=1;
	setup.modulus=cycles;
	setup.handler=tm_irq_table[INTERRUPT_OF(timer)].handler;
	setup.priority=tm_irq_table[INTERRUPT_OF(timer)].prio;
	setup.running=True;

	ret=timInstanceSetup(timer,&setup);
	ASSERT(ret==TMLIBDEV_OK);

	irq_timer=INTERRUPT_OF(timer);
	irq_desc[irq_timer].handler=&trimedia_irq_controller;

	/*init irqs used by linux*/
	for(i=0;i<(sizeof(tm_irq_table)/sizeof(tm_irq_table[0]));i++){
		struct tm_irq_info* irq_entry=&tm_irq_table[i];

		if(irq_entry->enable){
			intInstanceSetup_t intSetup;

			intSetup.enabled=False; /*enable it when request_irq*/
			intSetup.handler=irq_entry->handler;
			intSetup.level_triggered=irq_entry->level_triggered;
			intSetup.priority=irq_entry->prio;	

			ret=intOpen(irq_entry->number);
			ASSERT(ret==TMLIBDEV_OK);

			ret=intInstanceSetup(irq_entry->number,&intSetup);
			ASSERT(ret==TMLIBDEV_OK);

			irq_desc[irq_entry->number].handler=&trimedia_irq_controller;

			DEBUGP("Own interrupt '%s':%d \n",irq_entry->name,irq_entry->number);
		}else{
			DEBUGP("Giveup interrupt '%s':%d \n",irq_entry->name,irq_entry->number);
		}
	}

	return;
}

/*
 * FIXME: need to implement it
 */
int show_interrupts(struct seq_file *p, void *v)
{
	return 0;
}

extern unsigned int __do_IRQ(unsigned int irq,struct pt_regs* regs);
/*
 * do_IRQ handles all normal device IRQ's (the special
 * SMP cross-CPU interrupts have their own specific
 * handlers).
 */
asmlinkage unsigned int do_IRQ(int irq, struct pt_regs *regs)
{
	/*
	 * Local CPU interrupt is disabled
	 */
	irq_enter();
	
#ifdef CONFIG_DEBUG_STACKOVERFLOW
	/*FIXME: trimedia*/	
#endif	
	trimedia_irq_disable(irq);
	__do_IRQ(irq, regs);
	trimedia_irq_enable(irq);
	irq_exit();

	return 1;
}

