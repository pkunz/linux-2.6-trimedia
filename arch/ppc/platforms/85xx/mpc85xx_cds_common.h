/*
 * arch/ppc/platforms/85xx/mpc85xx_cds_common.h
 *
 * MPC85xx CDS board definitions
 *
 * Maintainer: Kumar Gala <kumar.gala@freescale.com>
 *
 * Copyright 2004 Freescale Semiconductor, Inc
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef __MACH_MPC85XX_CDS_H__
#define __MACH_MPC85XX_CDS_H__

#include <linux/config.h>
#include <linux/serial.h>
#include <asm/ppcboot.h>
#include <linux/initrd.h>
#include <syslib/ppc85xx_setup.h>

#define BOARD_CCSRBAR           ((uint)0xe0000000)
#define CCSRBAR_SIZE            ((uint)1024*1024)

/* CADMUS info */
#define CADMUS_BASE (0xf8004000)
#define CADMUS_SIZE (256)
#define CM_VER	(0)
#define CM_CSR	(1)
#define CM_RST	(2)

/* CDS NVRAM/RTC */
#define CDS_RTC_ADDR	(0xf8000000)
#define CDS_RTC_SIZE	(8 * 1024)

#ifndef CONFIG_PEX
/* PCI config */
#define PCI1_CFG_ADDR_OFFSET	(0x8000)
#define PCI1_CFG_DATA_OFFSET	(0x8004)

#define PCI2_CFG_ADDR_OFFSET	(0x9000)
#define PCI2_CFG_DATA_OFFSET	(0x9004)
#endif

/* PCI interrupt controller */
#define PIRQ0A                   MPC85xx_IRQ_EXT0
#define PIRQ0B                   MPC85xx_IRQ_EXT1
#define PIRQ0C                   MPC85xx_IRQ_EXT2
#define PIRQ0D                   MPC85xx_IRQ_EXT3
#define PIRQ1A                   MPC85xx_IRQ_EXT11

/* 8259 interrupt */
#ifndef NUM_8259_INTERRUPTS
#define NUM_8259_INTERRUPTS 16
#endif
 
/* PCI 1 memory map */
#define MPC85XX_PCI1_LOWER_IO        0x00000000
#ifdef CONFIG_PEX
#define MPC85XX_PCI1_UPPER_IO	     0x007fffff
#else
#define MPC85XX_PCI1_UPPER_IO        0x00ffffff
#endif

#define MPC85XX_PCI1_LOWER_MEM       0x80000000
#ifdef CONFIG_PEX
#define MPC85XX_PCI1_UPPER_MEM       0x8fffffff
#else
#define MPC85XX_PCI1_UPPER_MEM	     0x9fffffff
#endif

#define MPC85XX_PCI1_IO_BASE         0xe2000000
#define MPC85XX_PCI1_MEM_OFFSET      0x00000000

#ifdef CONFIG_PEX
#define MPC85XX_PCI1_IO_SIZE         0x00800000
#else 
#define MPC85XX_PCI1_IO_SIZE	     0x01000000
#endif 

/* PCI 2 memory map */
/* Note: the standard PPC fixups will cause IO space to get bumped by
 * hose->io_base_virt - isa_io_base => MPC85XX_PCI1_IO_SIZE */
#define MPC85XX_PCI2_LOWER_IO        0x00000000
#ifdef CONFIG_PEX
#define MPC85XX_PCI2_UPPER_IO	     0x007fffff
#else
#define MPC85XX_PCI2_UPPER_IO        0x00ffffff
#endif

#ifdef CONFIG_PEX
#define MPC85XX_PCI2_LOWER_MEM	     0x90000000
#define MPC85xX_PCI2_UPPER_MEM	     0x9fffffff
#else
#define MPC85XX_PCI2_LOWER_MEM       0xa0000000
#define MPC85XX_PCI2_UPPER_MEM       0xbfffffff
#endif

#ifdef CONFIG_PEX
#define MPC85XX_PCI2_IO_BASE	     0xe2800000
#else
#define MPC85XX_PCI2_IO_BASE         0xe3000000
#endif
#define MPC85XX_PCI2_MEM_OFFSET      0x00000000

#ifdef CONFIG_PEX
#define MPC85XX_PCI2_IO_SIZE	     0x00800000
#else
#define MPC85XX_PCI2_IO_SIZE         0x01000000
#endif

#ifdef CONFIG_PEX
/* PEX memory map */
#define MPC85XX_PEX_LOWER_IO        0x00000000
#define MPC85XX_PEX_UPPER_IO        0x00ffffff

#define MPC85XX_PEX_LOWER_MEM       0xa0000000
#define MPC85XX_PEX_UPPER_MEM       0xbfffffff

#define MPC85XX_PEX_IO_BASE         0xe3000000
#define MPC85XX_PEX_MEM_OFFSET      0x00000000

#define MPC85XX_PEX_IO_SIZE         0x01000000
#endif

#define NR_8259_INTS		     16
#define CPM_IRQ_OFFSET		     NR_8259_INTS

#endif /* __MACH_MPC85XX_CDS_H__ */
