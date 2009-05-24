/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef __ASM_TARGET_IRQ_H
#define __ASM_TARGET_IRQ_H

#include <asm/irq_internal.h>

#define FIRST_TIMER_INTERRUPT_NUM   5
#define INTERRUPT_OF(instance)    ((FIRST_TIMER_INTERRUPT_NUM)+(instance))

#define TM_IRQ_LIST()  							  \
    /*------------------------+--------+------+---------------+--------*/ \
    /* Interrupt Name         | Int ID |enable|level-triggered|priority*/ \
    /*------------------------+--------+------+---------------+--------*/ \
    VIC_INTDEF( PCI_INTA,            0,   1,     1  	      , 6       ) \
    VIC_INTDEF( PCI_GNTA,            1,   1,     1            , 6       ) \
    VIC_INTDEF( PCI_GNTB,            2,   1,     1            , 6 	) \
    VIC_INTDEF( PCI_REQA,            3,   1,     1            , 6 	) \
    VIC_INTDEF( PCI_REQB,            4,   1,     1            , 6 	) \
    VIC_INTDEF( TIMER0,              5,   0,     0            , 4 	) \
    VIC_INTDEF( TIMER1,              6,   0,     0            , 4 	) \
    VIC_INTDEF( TIMER2,              7,   0,     0            , 4 	) \
    VIC_INTDEF( SYS_TIMER,           8,   0,     0            , 4 	) \
    VIC_INTDEF( VIP0,                9,   0,     1            , 6 	) \
    VIC_INTDEF( QVCP0,              10,   0,     1            , 6 	) \
    VIC_INTDEF( AI0,                11,   0,     1            , 6 	) \
    VIC_INTDEF( AO0,                12,   0,     1            , 6 	) \
    VIC_INTDEF( SPDI0,              13,   0,     1            , 6 	) \
    VIC_INTDEF( SPDO0,              14,   0,     1            , 6 	) \
    VIC_INTDEF( ETHERNET0,          15,   1,     1            , 6 	) \
    VIC_INTDEF( IIC0,               16,   0,     1            , 6 	) \
    VIC_INTDEF( TMDBG0,             17,   0,     1            , 6 	) \
    VIC_INTDEF( FGPI0,              18,   0,     1            , 6 	) \
    VIC_INTDEF( FGPO0,              19,   0,     1            , 6 	) \
    VIC_INTDEF( NC20,               20,   0,     0            , 6 	) \
    VIC_INTDEF( NC21,               21,   0,     0            , 6 	) \
    VIC_INTDEF( MBS0,               22,   0,     1            , 6 	) \
    VIC_INTDEF( DRAW0,              23,   0,     1            , 6 	) \
    VIC_INTDEF( VLD0,               24,   0,     1            , 6 	) \
    VIC_INTDEF( DVDD0,              25,   0,     1            , 6 	) \
    VIC_INTDEF( GPIO0_10,           26,   1,     1            , 6 	) \
    VIC_INTDEF( GPIO0_11,           27,   0,     1            , 6 	) \
    VIC_INTDEF( HOSTCOMM,           28,   0,     0            , 6 	) \
    VIC_INTDEF( APP,                29,   0,     0            , 6 	) \
    VIC_INTDEF( DEBUGGER,           30,   0,     0            , 6 	) \
    VIC_INTDEF( RTOS,               31,   0,     0            , 6 	) \
    VIC_INTDEF( GPIO0_0,            32,   0,     1            , 6 	) \
    VIC_INTDEF( GPIO0_1,            33,   0,     1            , 6 	) \
    VIC_INTDEF( GPIO0_2,            34,   0,     1            , 6 	) \
    VIC_INTDEF( GPIO0_3,            35,   0,     1            , 6 	) \
    VIC_INTDEF( GPIO0_4,            36,   0,     1            , 6 	) \
    VIC_INTDEF( PCI,                37,   0,     1            , 6 	) \
    VIC_INTDEF( PCI_PM,             38,   0,     1            , 6 	) \
    VIC_INTDEF( PCI_XIO,            39,   0,     1            , 6 	) \
    VIC_INTDEF( PCI_DMA,            40,   0,     1            , 6 	) \
    VIC_INTDEF( CLOCK0,             41,   0,     1            , 6 	) \
    VIC_INTDEF( WATCHDOG,           42,   0,     1            , 6 	) \
    VIC_INTDEF( IPC_TM_0,           43,   0,     0            , 6 	) \
    VIC_INTDEF( IPC_TM_1,           44,   0,     0            , 6 	) \
    VIC_INTDEF( IPC_TM_2,           45,   0,     0            , 6 	) \
    VIC_INTDEF( IPC_TM_3,           46,   0,     0            , 6 	) \
    VIC_INTDEF( IPC_TM_4,           47,   0,     0            , 6 	) \
    VIC_INTDEF( IPC_TM_5,           48,   0,     0            , 6 	) \
    VIC_INTDEF( IPC_TM_6,           49,   0,     0            , 6 	) \
    VIC_INTDEF( IPC_TM_7,           50,   0,     0            , 6 	) \
    VIC_INTDEF( NC51,               51,   0,     0            , 6 	) \
    VIC_INTDEF( NC52,               52,   0,     0            , 6 	) \
    VIC_INTDEF( NC53,               53,   0,     0            , 6 	) \
    VIC_INTDEF( NC54,               54,   0,     0            , 6 	) \
    VIC_INTDEF( NC55,               55,   0,     0            , 6 	) \
    VIC_INTDEF( NC56,               56,   0,     0            , 6 	) \
    VIC_INTDEF( NC57,               57,   0,     0            , 6 	) \
    VIC_INTDEF( NC58,               58,   0,     0            , 6 	) \
    VIC_INTDEF( NC59,               59,   0,     0            , 6 	) \
    VIC_INTDEF( TDCN,               60,   0,     1            , 6 	) \
    VIC_INTDEF( MMI0,               61,   0,     1            , 6 	) \
    VIC_INTDEF( NC62,               62,   0,     0            , 6 	) \
    VIC_INTDEF( NC63,               63,   0,     0            , 6 	) 

#endif /*__ASM_TARGET_IRQ_H*/
