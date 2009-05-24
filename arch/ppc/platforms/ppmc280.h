/*
 * arch/ppc/platforms/ppmc280.h
 * 
 * Definitions for Force PPMC280 development board.
 *
 * Based on db64360.h, Written by Rabeeh Khoury - rabeeh@galileo.co.il
 * Based on code done by Mark A. Greer <mgreer@mvista.com>
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

/*
 * The MV64360 has 2 PCI buses each with 1 window from the CPU bus to
 * PCI I/O space and 4 windows from the CPU bus to PCI MEM space.
 * We'll only use one PCI MEM window on each PCI bus.
 *
 * This is the CPU physical memory map (windows must be at least 1MB and start
 * on a boundary that is a multiple of the window size):
 *
 *     0xff800000-0xffffffff           - Boot flash (BootCS#)
 *     0xf2040000-0xff7fffff           - Unused
 *     0xf2000000-0xf203ffff           - Integrated SRAM
 *     0xf1010000-0xf1ffffff           - Unused
 *     0xf1000000-0xf100ffff           - MV64360 Registers
 *     0xa8000000-0xf0ffffff           - Unused
 *     0xa4000000-0xa7ffffff           - User flash expansion
 *     0xa2000000-0xa3ffffff           - User flash 1 ( CS1)
 *     0xa0000000-0xa1ffffff           - User flash 0 ( CS0)
 *     0x99000000-0x9fffffff           - <hole>
 *     0x98000000-0x98ffffff           - PCI 1 I/O (defined in mv64x60.h)
 *     0x90000000-0x97ffffff           - PCI 1 MEM (defined in mv64x60.h)
 *     0x88000000-0x88ffffff           - PCI 0 I/O (defined in mv64x60.h)
 *     0x80000000-0x87ffffff           - PCI 0 MEM (defined in mv64x60.h)
 *     0x20000000-0x7fffffff           - Reserved for SDRAM expansion
 *     0x00000000-0x1fffffff           - On Board SDRAM ( CS0)
 */

#ifndef __PPC_PLATFORMS_PPMC280_H
#define __PPC_PLATFORMS_PPMC280_H

#ifndef	MAX
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

/*
 * CPU Physical Memory Map setup.
 */

#define PPMC280_EXT_FLASH_BASE_1        0xa0000000
#define PPMC280_EXT_FLASH_BASE_2        0xa2000000
#define PPMC280_BOOT_FLASH_BASE          0xff800000  /* BOOT FLASH Base */
#define PPMC280_BRIDGE_REG_BASE         0xf1000000
#define PPMC280_INTERNAL_SRAM_BASE      0xf2000000
#define PPMC280_EXT_FLASH_SIZE_ACTUAL   0x02000000  /* 32*2 MB Extern FLASH */
#define PPMC280_BOOT_FLASH_SIZE_ACTUAL   0x00800000  /* =< 8M of Boot FLASH */
#define PPMC280_INTERNAL_SRAM_ACTUAL    _256K       /* 256KByte internal SRAM */ 



#define	PPMC280_EXT_FLASH_SIZE		MAX(MV64360_WINDOW_SIZE_MIN,	\
						PPMC280_EXT_FLASH_SIZE_ACTUAL)
#define	PPMC280_BOOT_FLASH_SIZE		MAX(MV64360_WINDOW_SIZE_MIN,	\
						PPMC280_BOOT_FLASH_SIZE_ACTUAL)
#define PPMC280_INTERNAL_SRAM_SIZE      _256K       /* 256KByte internal SRAM fixed size */

#define PPMC280_PCI_0_MEM_START_PROC	0x80000000U
#define PPMC280_PCI_0_MEM_START		0x80000000U
#define	PPMC280_PCI_0_MEM_SIZE		0x08000000U	/* 128MB */

#define PPMC280_PCI_0_IO_START_PROC	0x88000000U
#define PPMC280_PCI_0_IO_SIZE		0x01000000U	/* 16MB */
#define PPMC280_PCI_0_IO_START		0x00000000U

#define PPMC280_PCI_1_MEM_START_PROC	0x90000000U
#define PPMC280_PCI_1_MEM_START		0x90000000U
#define	PPMC280_PCI_1_MEM_SIZE		0x08000000U

#define PPMC280_PCI_1_IO_START_PROC	0x98000000U
#define PPMC280_PCI_1_IO_SIZE		0x01000000U
#define PPMC280_PCI_1_IO_START		0x10000000U

#if 0

/*
 * Window sizes & base addresses
 */
#define	PPMC280_PCI_0_MEM_SIZE		0x08000000U
#define PPMC280_PCI_0_IO_SIZE		0x01000000U

#define	PPMC280_PCI_1_MEM_SIZE		0x08000000U
#define PPMC280_PCI_1_IO_SIZE		0x01000000U

#define	PPMC280_PCI_MEM_BASE_ADDR_PROC	0x80000000U	/* CPU Address */

#define	PPMC280_PCI_IO_BASE_ADDR_PROC	(PPMC280_PCI_MEM_BASE_ADDR_PROC + PPMC280_PCI_0_MEM_SIZE)

#define	PPMC280_PCI_MEM_BASE_ADDR	0x80000000U	/* PCI Address */
#define	PPMC280_PCI_IO_BASE_ADDR	0x00000000U

/*
 * PCI Bus 0 Definitions
 */
/* Processor Physical addresses */
#define	PPMC280_PCI_0_MEM_START_PROC	PPMC280_PCI_MEM_BASE_ADDR_PROC
#define	PPMC280_PCI_0_MEM_END_PROC	(PPMC280_PCI_0_MEM_START_PROC +	 PPMC280_PCI_0_MEM_SIZE - 1)

/* PCI 0 MEM address */
#define	PPMC280_PCI_0_MEM_START		PPMC280_PCI_MEM_BASE_ADDR
#define	PPMC280_PCI_0_MEM_END		(PPMC280_PCI_0_MEM_START + PPMC280_PCI_0_MEM_SIZE - 1)

/* Processor Physical addresses */
#define	PPMC280_PCI_0_IO_START_PROC	PPMC280_PCI_IO_BASE_ADDR_PROC
#define	PPMC280_PCI_0_IO_END_PROC	(PPMC280_PCI_0_IO_START_PROC + PPMC280_PCI_0_IO_SIZE - 1)

/* PCI 0 I/O address */
#define	PPMC280_PCI_0_IO_START		PPMC280_PCI_IO_BASE_ADDR
#define	PPMC280_PCI_0_IO_END		(PPMC280_PCI_0_IO_START + PPMC280_PCI_0_IO_SIZE - 1)

/*
 * PCI Bus 1 Definitions
 */
/* Processor Physical addresses */
#define	PPMC280_PCI_1_MEM_START_PROC	(PPMC280_PCI_0_MEM_END_PROC + 1)
#define	PPMC280_PCI_1_MEM_END_PROC	(PPMC280_PCI_1_MEM_START_PROC + PPMC280_PCI_1_MEM_SIZE - 1)

/* PCI 0 MEM address */
#define	PPMC280_PCI_1_MEM_START		(PPMC280_PCI_0_MEM_END + 1)
#define	PPMC280_PCI_1_MEM_END		(PPMC280_PCI_1_MEM_START + PPMC280_PCI_1_MEM_SIZE - 1)

/* Processor Physical addresses */
#define	PPMC280_PCI_1_IO_START_PROC	(PPMC280_PCI_0_IO_END_PROC + 1)
#define	PPMC280_PCI_1_IO_END_PROC	(PPMC280_PCI_1_IO_START_PROC + PPMC280_PCI_1_IO_SIZE - 1)

/* PCI 0 I/O address */
#define	PPMC280_PCI_1_IO_START		(PPMC280_PCI_0_IO_END + 1)
#define	PPMC280_PCI_1_IO_END		(PPMC280_PCI_1_IO_START +  PPMC280_PCI_1_IO_SIZE - 1)

#endif

/* Only two of the three interfaces have BCM5421 attached */
#define PPMC280_ETH0_PHY_ADDR            1
#define PPMC280_ETH1_PHY_ADDR            3
#define PPMC280_ETH2_PHY_ADDR            -1

#define PPMC280_ETH_TX_QUEUE_SIZE        200
#define PPMC280_ETH_RX_QUEUE_SIZE        200

/*
 * Serial driver setup
 */

#define PPMC280_DEFAULT_BAUD	115200
#define PPMC280_MPSC_CLK_SRC 	8	/* TCLK */
#define PPMC280_MPSC_CLK_FREQ	133000000
#define PPMC280_SYSTEM_TCLK	133000000


/* Ravi : Remove Later */
#if 0
#define BASE_BAUD ( 3686400 / 16 )

#ifdef CONFIG_SERIAL_MANY_PORTS
#define RS_TABLE_SIZE	64
#else
#define RS_TABLE_SIZE	2
#endif

#ifdef CONFIG_SERIAL_DETECT_IRQ
#define STD_COM_FLAGS (ASYNC_BOOT_AUTOCONF|ASYNC_SKIP_TEST|ASYNC_AUTO_IRQ)
#else
#define STD_COM_FLAGS (ASYNC_BOOT_AUTOCONF|ASYNC_SKIP_TEST)
#endif

#if !defined(CONFIG_SERIAL_MPSC_CONSOLE)
/* Required for bootloader's ns16550.c code */
#define STD_SERIAL_PORT_DFNS 						\
        { 0, BASE_BAUD, DB64360_SERIAL_0, 85, STD_COM_FLAGS, /* ttyS0 */\
	iomem_base: (u8 *)DB64360_SERIAL_0,				\
	iomem_reg_shift: 2,						\
	io_type: SERIAL_IO_MEM },

#define SERIAL_PORT_DFNS \
        STD_SERIAL_PORT_DFNS
#else
#define SERIAL_PORT_DFNS
#endif

#endif
/*
 * Board-specific IRQ info
 */

#define	PPMC280_PCI_0_IRQ		64+27
#define	PPMC280_PCI_0_IRQ_B		64+29
#define	PPMC280_PCI_0_IRQ_C		64+16
#define	PPMC280_PCI_0_IRQ_D		64+17
#define	PPMC280_PCI_1_IRQ		64+27

#endif /* __PPC_PLATFORMS_PPMC280_H */
