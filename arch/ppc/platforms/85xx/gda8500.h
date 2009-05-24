/*
 * arch/ppc/platforms/85xx/gda8500.h
 *
 * GDA RMC-G8500 board definitions
 *
 * Maintainer: Christopher P. Hayden <chayden@mc.com>
 * Author: Christopher P. Hayden
 *
 * Copyright 2004 Mercury Computer Systems, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef __MACH_GDA8500_H__
#define __MACH_GDA8500_H__

#include <linux/config.h>
#include <linux/initrd.h>
#include <syslib/ppc85xx_setup.h>
#include <linux/serial.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <asm/ppcboot.h>
#include <asm/irq.h>

#define BOARD_CCSRBAR			((uint)0xfc000000)
#define BCSR_ADDR			((uint)0xfc000000)
#define BCSR_SIZE			((uint)32 * 1024)

extern int gda8500_show_cpuinfo(struct seq_file *m);
extern void gda8500_init_IRQ(void) __init; 
extern void gda8500_map_io(void) __init;

/* PCI interrupt controller */
#define PIRQA		MPC85xx_IRQ_EXT1
#define PIRQB		MPC85xx_IRQ_EXT2
#define PIRQC		MPC85xx_IRQ_EXT3
#define PIRQD		MPC85xx_IRQ_EXT4

#define MPC85XX_PCI1_LOWER_IO	0x00000000
#define MPC85XX_PCI1_UPPER_IO	0x00ffffff

#define MPC85XX_PCI1_LOWER_MEM       0xe0000000
#define MPC85XX_PCI1_UPPER_MEM       0xefffffff

#define MPC85XX_PCI1_IO_BASE         0xf8000000
#define MPC85XX_PCI1_MEM_OFFSET      0x00000000

#define MPC85XX_PCI1_IO_SIZE         0x01000000

/* Serial Config */
#define MPC85XX_0_SERIAL                (BOARD_CCSRBAR + 0x4500)
#define MPC85XX_1_SERIAL                (BOARD_CCSRBAR + 0x4600)

#define SERIAL_PORT_DFNS \
	{ 0, BASE_BAUD, 0, MPC85xx_IRQ_DUART,                 \
	(ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST),        \
	iomem_base: (u8 *)MPC85XX_0_SERIAL,       \
	io_type: SERIAL_IO_MEM},

#endif /* __MACH_GDA8500_H__ */
