/*
 * arch/mips/brcm/int-handler.S
 *
 * Copyright (C) 2001-2006 Broadcom Corporation
 *                    Steven J. Hill <shill@broadcom.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Low level interrupt code for Broadcom eval boards
 *
 * 10-23-2001   SJH    Created
 * 03-10-2006   TDT    Modified for SMP
 */
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>

	/*
	 * Please read at the comments in 'irq.c' for
	 * the mapping of the IRQs.
	 */
	.text
	.set	noreorder
	.set	noat
	.align	5
	NESTED(brcmIRQ, PT_SIZE, sp)
	SAVE_ALL
	CLI
	.set	at
	mfc0	s0, CP0_CAUSE			# get irq mask

	andi	a0, s0, CAUSEF_IP7
	beq	a0, zero, 1f
	andi	a0, s0, CAUSEF_IP2		# delay slot, check for IRQ2
	move	a0, sp
	jal	brcm_mips_int7_dispatch
	nop					# delay slot
	j	ret_from_irq
	nop					# delay slot
1:
	beq	a0, zero, 1f
	andi	a0, s0, CAUSEF_IP6		# delay slot, check for IRQ6 (enet)
	move	a0, sp
	jal	brcm_mips_int2_dispatch
	nop					# delay slot
	j	ret_from_irq
	nop					# delay slot

/* For 2425-2.5a build only, otherwise we need to regress test all platforms. */
#ifdef CONFIG_MIPS_BCM7318
1:
	beq	a0, zero, 1f
	andi	a0, s0, CAUSEF_IP5		# delay slot, check for IRQ5 (enet)
	move	a0, sp
	jal	brcm_mips_int6_dispatch
	nop					# delay slot
	j	ret_from_irq
	nop					# delay slot
1:
	beq	a0, zero, 1f
	andi	a0, s0, CAUSEF_IP4		# delay slot, check for IRQ4 (enet)
	move	a0, sp
	jal	brcm_mips_int5_dispatch
	nop					# delay slot
	j	ret_from_irq
	nop					# delay slot
1:
	beq	a0, zero, 1f
	andi	a0, s0, CAUSEF_IP3		# delay slot, check for IRQ3 
	move	a0, sp
	jal	brcm_mips_int4_dispatch
	nop					# delay slot
	j	ret_from_irq
	nop					# delay slot

#else
1:
	beq	a0, zero, 1f
	andi	a0, s0, CAUSEF_IP3		# delay slot, check for IRQ3 
	move	a0, sp
	jal	brcm_mips_int6_dispatch
	nop					# delay slot
	j	ret_from_irq
	nop					# delay slot
#endif
1:
	beq	a0, zero, 1f
	nop
	move	a0, sp
	jal	brcm_mips_int3_dispatch
	nop					# delay slot
	j	ret_from_irq
	nop					# delay slot
1:
#ifdef CONFIG_SMP
	andi	a0, s0, CAUSEF_IP0
	beq	a0, zero, 1f
	nop
	move	a0, sp
	jal	brcm_mips_int0_dispatch
	nop					# delay slot
1:
	andi	a0, s0, CAUSEF_IP1
	beq	a0, zero, 1f
	nop
	move	a0, sp
	jal	brcm_mips_int1_dispatch
	nop					# delay slot
1:
#endif	
	j	ret_from_irq
	nop					# delay slot
	END(brcmIRQ)
