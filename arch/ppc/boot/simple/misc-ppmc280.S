/*
 * arch/ppc/boot/simple/misc-ppmc280.S
 * 
 * Host bridge init code for the Marvell DB-64360-BP development board
 *
 * Written by Rabeeh Khoury - rabeeh@galileo.co.il
 * Based on code done by - Mark Greer <mgreer@mvista.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <asm/ppc_asm.h>
#include <asm/processor.h>
#include <asm/cache.h>
#include <asm/mv64x60_defs.h>

#include <platforms/ppmc280.h>

#define MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(port) (0x2680 + (port<<10))

	.globl	ppmc280_init
ppmc280_init:
	li	r20,0
	li	r23,16


	/* Dink has it's stack in cache - cache touch and invalidate first 2MByte */
	/* R24 = 2MByte */
	lis	r24, 0x0020
	/* R25 = 0 (the counter */
	lis	r25,0x0
	/* R26 = 0x80000000 - BAT offset mapping physical memory at offset 0x0 - cacheable */
	lis	r26,0x8000
	
1:	dcbf	r26,r25
	icbi	r26,r25
	addi	r25,r25,32
	cmplw   r25,r24
	ble	1b

	/* Relocation of MV64360 internal regs that was here is handled
	 in misc-mv64x60.S now */

#ifndef CONFIG_POWERPMC280
        /* Change CS2 (UARTS on device module) window */
        addis	r25,0,PPMC280_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_DEV_CS2_BASE_ADDR             
	addis	r26,0,DB64360_UART_BASE@h
	srw	r26,r26,r23
        stwbrx  r26,0,(r25)
	sync
#endif

        addis	r25,0,PPMC280_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64x60_CPU2DEV_2_SIZE
	addis	r26,0,MV64360_WINDOW_SIZE_MIN@h
	srw	r26,r26,r23
        stwbrx  r26,0,(r25)
	sync

        /* Stop GigE Rx DMA engines */
        addis	r25,0,PPMC280_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(0)             
	addis	r26,0,0
	ori	r26,r26,0xff00
        stwbrx  r26,0,(r25)
	sync

        addis	r25,0,PPMC280_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(1)             
	addis	r26,0,0
	ori	r26,r26,0xff00
        stwbrx  r26,0,(r25)
	sync

        addis	r25,0,PPMC280_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(2)             
	addis	r26,0,0
	ori	r26,r26,0xff00
        stwbrx  r26,0,(r25)
	sync

	blr
