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

#define FIRST_TIMER_INTERRUPT_NUM   4
#define INTERRUPT_OF(instance)    ((FIRST_TIMER_INTERRUPT_NUM)+(instance))

#define TM_IRQ_LIST() 							  \
    /*------------------------+--------+------+---------------+--------*/ \
    /* Interrupt Name         | Int ID |enable|level-triggered|priority*/ \
    /*------------------------+--------+------+---------------+--------*/ \
    VIC_INTDEF( HOSTCOMM,        0,       0,     1            , 6       ) \
    VIC_INTDEF( APP,             1,       0,     1            , 6       ) \
    VIC_INTDEF( DEBUGGER,        2,       0,     1            , 6       ) \
    VIC_INTDEF( RTOS,            3,       0,     1            , 6       ) \
    VIC_INTDEF( SYSTIMER0,       4,       0,     0            , 6       ) \
    VIC_INTDEF( SYSTIMER1,       5,       0,     0            , 6       ) \
    VIC_INTDEF( SYSTIMER2,       6,       0,     0            , 6       ) \
    VIC_INTDEF( TIMER3,          7,       0,     0            , 6       ) \
    VIC_INTDEF( TIMER4,          8,       0,     0            , 6       ) \
    VIC_INTDEF( TIMER5,          9,       0,     0            , 6       ) \
    VIC_INTDEF( TIMER6,         10,       0,     0            , 6       ) \
    VIC_INTDEF( TIMER7,         11,       0,     0            , 6       ) \
    VIC_INTDEF( PCI_INTA,       12,       0,     1            , 6       ) \
    VIC_INTDEF( SVIP_VIDEO,     13,       0,     1            , 6       ) \
    VIC_INTDEF( SVIP_AUX,       14,       0,     1            , 6       ) \
    VIC_INTDEF( SVIP_ANC,       15,       0,     1            , 6       ) \
    VIC_INTDEF( VIDEO_OUT,      16,       0,     1            , 6       ) \
    VIC_INTDEF( MBS0,           17,       0,     1            , 6       ) \
    VIC_INTDEF( FGPI0,          18,       0,     1            , 6       ) \
    VIC_INTDEF( AI1,            19,       0,     1            , 6       ) \
    VIC_INTDEF( AI2,            20,       0,     1            , 6       ) \
    VIC_INTDEF( AO1,            21,       0,     1            , 6       ) \
    VIC_INTDEF( AO2,            22,       0,     1            , 6       ) \
    VIC_INTDEF( USB0,           23,       0,     1            , 6       ) \
    VIC_INTDEF( HOSTIF,         24,       0,     1            , 6       ) \
    VIC_INTDEF( IIC0,           25,       0,     1            , 6       ) \
    VIC_INTDEF( DVDD0,          26,       0,     1            , 6       ) \
    VIC_INTDEF( GPIO0,          27,       0,     1            , 6       ) \
    VIC_INTDEF( PCI,            28,       0,     1            , 6       ) \
    VIC_INTDEF( SYS_INT,        29,       0,     1            , 6       ) \
    VIC_INTDEF( PIN_INT1,       30,       0,     1            , 6       ) \
    VIC_INTDEF( PIN_INT2,       31,       0,     1            , 6       ) 

#endif /*__ASM_TARGET_IRQ_H*/
