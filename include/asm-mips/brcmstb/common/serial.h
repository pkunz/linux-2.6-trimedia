/*
 * include/asm/brcm/serial.h
 *
 * Copyright (C) 2001 Broadcom Corporation
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
 * UART defines for Broadcom eval boards
 *
 * 10-19-2001   SJH    Created 
 */

#ifndef __ASM_BRCM_SERIAL_H
#define __ASM_BRCM_SERIAL_H

#include <asm/brcmstb/common/brcmstb.h>

/*
 * UART base addresses
 */
#if defined(CONFIG_MIPS_BCM7038) || defined(CONFIG_MIPS_BCM3560)
#define BRCM_SERIAL1_BASE	UARTA_ADR_BASE
#define BRCM_SERIAL2_BASE	UARTB_ADR_BASE
#elif defined(CONFIG_MIPS_BCM7401A0) 
#define BRCM_SERIAL1_BASE	UARTB_ADR_BASE
#define BRCM_SERIAL2_BASE	UARTC_ADR_BASE
#else
#define BRCM_SERIAL1_BASE	UPG_UART_A_BASE
#define BRCM_SERIAL2_BASE	UPG_UART_B_BASE
#ifdef CONFIG_MIPS_BCM7115
#define BRCM_SERIAL3_BASE	UPG_UART_C_BASE
#define BRCM_SERIAL4_BASE	UPG_UART_D_BASE
#endif
#endif

/*
 * IRQ stuff
 */
#define BRCM_SERIAL1_IRQ	BCM_LINUX_UARTA_IRQ
#define BRCM_SERIAL2_IRQ	BCM_LINUX_UARTB_IRQ
#ifdef CONFIG_MIPS_BCM7115
#define BRCM_SERIAL3_IRQ	BCM_LINUX_UARTC_IRQ
#define BRCM_SERIAL4_IRQ	BCM_LINUX_UARTD_IRQ
#endif

#if !defined( CONFIG_MIPS_BCM7038 ) && !defined( CONFIG_MIPS_BCM3560 ) && !defined( CONFIG_MIPS_BCM7401A0 )
/* bit defines in UPG int control reg. */
//#define UAIRQ_BIT		4
//#define UBIRQ_BIT		3
#define UAIRQ			UPG_UA_IRQ //0x10
#define UBIRQ			UPG_UB_IRQ //0x08
#endif

/*
 * UPG Clock is determined by:
 * 				XTAL_FREQ * SYSPLL_FSEL / PB_SEL_VALUE
 *					= 24MHz * 9 / 8 = 27MHz
 */


/*
 * UART register offsets
 */
 #if defined(CONFIG_MIPS_BCM7038) || defined(CONFIG_MIPS_BCM3560) \
 	|| defined(CONFIG_MIPS_BCM7401)
#define UART_RECV_STATUS	UART_RXSTAT //0x03	/* UART recv status register */
#define UART_RECV_DATA		UART_RXDATA //0x02	/* UART recv data register */
//#define UART_CONTROL		UART_CONTROL //0x00	/* UART control register */
#define UART_BAUDRATE_HI	UART_BAUDHI //0x07	/* UART baudrate register */
#define UART_BAUDRATE_LO	UART_BAUDLO //0x06	/* UART baudrate register */
#define UART_XMIT_STATUS	UART_TXSTAT //0x05	/* UART xmit status register */
#define UART_XMIT_DATA		UART_TXDATA //0x04	/* UART xmit data register */
#define BRCM_BASE_BAUD		(XTALFREQ/16) //1687500		/* (UPG Clock / 16) */
#else
#define BRCM_BASE_BAUD		(XTALFREQ1/16) //1687500		/* (UPG Clock / 16) */
#define UART_RECV_STATUS	UARTA_RXSTAT //0x03	/* UART recv status register */
#define UART_RECV_DATA		UARTA_RXDATA //0x02	/* UART recv data register */
#ifndef UART_CONTROL
#define UART_CONTROL		UARTA_CONTROL //0x00	/* UART control register */
#endif
#define UART_BAUDRATE_HI	UARTA_BAUDHI //0x07	/* UART baudrate register */
#define UART_BAUDRATE_LO	UARTA_BAUDLO //0x06	/* UART baudrate register */
#define UART_XMIT_STATUS	UARTA_TXSTAT //0x05	/* UART xmit status register */
#define UART_XMIT_DATA		UARTA_TXDATA //0x04	/* UART xmit data register */
#endif

/*
 * UART control register definitions
 */
#define UART_PODD		PODD //1	/* odd parity */
#define UART_RE			RXEN //2	/* receiver enable */
#define UART_TE			TXEN //4	/* transmitter enable */
#define UART_PAREN		PAREN //8	/* parity enable */
#define UART_BIT8M		BITM8 //16	/* 8 bits character */

/*
 * Receiver status and control register definitions
 */
#define UART_RIE		RXINTEN //2	/* receiver interrupt enable */
#define UART_RDRF		RXDATARDY //4	/* receiver data register full flag */
#define UART_OVRN		OVERRUNERR //8	/* data overrun error */
#define UART_FE			FRAMEERR //16	/* framing error */
#define UART_PE			PARERR //32	/* parity error */

/*
 * Transmitter status and control register definitions
 */
#define UART_TDRE		TXDREGEMT //1	/* transmit data register empty flag */
#define UART_IDLE		IDLE //2	/* transmit in idle state   */
#define UART_TIE		TXINTEN //4	/* transmit interrupt enable */

#endif /* __ASM_BRCM_SERIAL_H */
