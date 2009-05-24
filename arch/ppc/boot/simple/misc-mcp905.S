/*
 * arch/ppc/boot/simple/misc-mcp905.S
 * 
 * Host bridge init code for the MCP905 board
 *
 * Ajit Prem
 * Based on code written by Rabeeh Khoury - rabeeh@galileo.co.il
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

#include "../../platforms/mcp905.h"

#define MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(port) (0x2680 + (port<<10))

	.globl	mcp905_init
mcp905_init:
	li	r20,0
	li	r23,16


	/* stack in cache - cache touch and invalidate first 2MByte */
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

	/* Relocate MV64360 internal regs */

#if 0
	lis	r25, CONFIG_MV64360_ORIG_REG_BASE@h
	ori	r25,r25,CONFIG_MV64360_ORIG_REG_BASE@l
        ori	r25,r25,MV64360_INTERNAL_SPACE_BASE_ADDR
        lwbrx	r26,0,(r25)
	lis	r24,0xffff
	and	r26,r26,r24
	addis	r24,0,MCP905_BRIDGE_REG_BASE@h
	srw	r24,r24,r23
	or	r26,r26,r24
        stwbrx  r26,0,(r25)
	sync

	/* Wait for write to take effect */
        addis	r25,0,MCP905_BRIDGE_REG_BASE@h
	ori	r25,r25,MV64360_INTERNAL_SPACE_BASE_ADDR
1:	lwbrx	r24,0,(r25)
	cmpw	r24,r26
	bne	1b

        /* Change CS1 (UARTS on device module) window */
        addis	r25,0,MCP905_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_DEV_CS1_BASE_ADDR             
	addis	r26,0,MCP905_UART_BASE@h
	srw	r26,r26,r23
        stwbrx  r26,0,(r25)
	sync

        addis	r25,0,MCP905_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_DEV_CS1_SIZE
/*
	addis	r26,0,MCP905_UART_SIZE@h
*/
	addis	r26,0,MV64360_WINDOW_SIZE_MIN@h
	srw	r26,r26,r23
        stwbrx  r26,0,(r25)
	sync

#endif

        /* Stop GigE Rx DMA engines */
        addis	r25,0,MCP905_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(0)             
	addis	r26,0,0
	ori	r26,r26,0xff00
        stwbrx  r26,0,(r25)
	sync

        addis	r25,0,MCP905_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(1)             
	addis	r26,0,0
	ori	r26,r26,0xff00
        stwbrx  r26,0,(r25)
	sync

        addis	r25,0,MCP905_BRIDGE_REG_BASE@h
        ori	r25,r25,MV64360_ETH_RECEIVE_QUEUE_COMMAND_REG(2)             
	addis	r26,0,0
	ori	r26,r26,0xff00
        stwbrx  r26,0,(r25)
	sync

	blr


