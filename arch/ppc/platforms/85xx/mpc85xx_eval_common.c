/*
 * arch/ppc/platforms/85xx/mpc85xx_eval_common.c
 *
 * MPC85xx EVAL board common routines
 *
 * Maintainer: Kumar Gala <kumar.gala@freescale.com>
 *
 * Jason Jin Modified the file mpc85xx_ads_common.c for eval board.
 * 
 * Copyright 2004 Freescale Semiconductor Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/config.h>
#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/reboot.h>
#include <linux/pci.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/seq_file.h>
#include <linux/serial.h>
#include <linux/module.h>

#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/atomic.h>
#include <asm/time.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/prom.h>
#include <asm/open_pic.h>
#include <asm/bootinfo.h>
#include <asm/pci-bridge.h>
#include <asm/mpc85xx.h>
#include <asm/irq.h>
#include <asm/immap_85xx.h>

#include <mm/mmu_decl.h>

#include <platforms/85xx/mpc85xx_eval_common.h>

#ifndef CONFIG_PCI
unsigned long isa_io_base = 0;
unsigned long isa_mem_base = 0;
#endif

extern unsigned long total_memory;	/* in mm/init */

unsigned char __res[sizeof (bd_t)];

/* Internal interrupts are all Level Sensitive, and Positive Polarity */

static u_char mpc85xx_eval_openpic_initsenses[] __initdata = {
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  0: L2 Cache */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  1: ECM */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  2: DDR DRAM */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  3: LBIU */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  4: DMA 0 */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  5: DMA 1 */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  6: DMA 2 */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  7: DMA 3 */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  8: PCI/PCI-X */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal  9: RIO Inbound Port Write Error */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 10: RIO Doorbell Inbound */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 11: RIO Outbound Message */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 12: RIO Inbound Message */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 13: TSEC 0 Transmit */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 14: TSEC 0 Receive */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 15: Unused */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 16: Unused */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 17: Unused */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 18: TSEC 0 Receive/Transmit Error */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 19: TSEC 1 Transmit */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 20: TSEC 1 Receive */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 21: Unused */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 22: Unused */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 23: Unused */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 24: TSEC 1 Receive/Transmit Error */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 25: Fast Ethernet */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 26: DUART */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 27: I2C */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 28: Performance Monitor */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 29: Unused */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 30: CPM */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),	/* Internal 31: Unused */
	0x0,						/* External  0: */
#if defined(CONFIG_PCI)
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),	/* External 1: PCI slot 0 */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),	/* External 2: PCI slot 1 */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),	/* External 3: PCI slot 2 */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),	/* External 4: PCI slot 3 */
#else
	0x0,				/* External  1: */
	0x0,				/* External  2: */
	0x0,				/* External  3: */
	0x0,				/* External  4: */
#endif
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),	/* External 5: PHY */
	0x0,				/* External  6: */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),	/* External 7: PHY */
	0x0,				/* External  8: */
	0x0,				/* External  9: */
	0x0,				/* External 10: */
	0x0,				/* External 11: */
#ifdef CONFIG_DETHERNET 
        /*For message interrupt,added by Jason Jin,
	 *message interrupt 0 trig by agent and direct to IRQ_OUT,
         *message interrupt 1 trigged by agent and the destination is the agent cpu.
        */
        (IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),  /*message interrupt 0*/
        (IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),  /*message interrupt 1*/
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_POSITIVE),
#endif
					
};

/* ************************************************************************ */
int
mpc85xx_eval_show_cpuinfo(struct seq_file *m)
{
	uint pvid, svid, phid1;
	uint memsize = total_memory;
	bd_t *binfo = (bd_t *) __res;
	unsigned int freq;

	/* get the core frequency */
	freq = binfo->bi_intfreq;

	pvid = mfspr(PVR);
	svid = mfspr(SVR);

	seq_printf(m, "Vendor\t\t: Freescale Semiconductor\n");

	switch (svid & 0xffff0000) {
	case SVR_8540:
		seq_printf(m, "Machine\t\t: mpc8540eval\n");
		break;
	case SVR_8560:
		seq_printf(m, "Machine\t\t: mpc8560ads\n");
		break;
	default:
		seq_printf(m, "Machine\t\t: unknown\n");
		break;
	}
	seq_printf(m, "clock\t\t: %dMHz\n", freq / 1000000);
	seq_printf(m, "PVR\t\t: 0x%x\n", pvid);
	seq_printf(m, "SVR\t\t: 0x%x\n", svid);

	/* Display cpu Pll setting */
	phid1 = mfspr(HID1);
	seq_printf(m, "PLL setting\t: 0x%x\n", ((phid1 >> 24) & 0x3f));

	/* Display the amount of memory */
	seq_printf(m, "Memory\t\t: %d MB\n", memsize / (1024 * 1024));

	return 0;
}

void __init
mpc85xx_eval_init_IRQ(void)
{
	bd_t *binfo = (bd_t *) __res;
	/* Determine the Physical Address of the OpenPIC regs */
	phys_addr_t OpenPIC_PAddr =
	    binfo->bi_immr_base + MPC85xx_OPENPIC_OFFSET;
	OpenPIC_Addr = ioremap(OpenPIC_PAddr, MPC85xx_OPENPIC_SIZE);
	OpenPIC_InitSenses = mpc85xx_eval_openpic_initsenses;
	OpenPIC_NumInitSenses = sizeof (mpc85xx_eval_openpic_initsenses);

	/* Skip reserved space and internal sources */
	openpic_set_sources(0, 32, OpenPIC_Addr + 0x10200);
	/* Map PIC IRQs 0-11 */
	openpic_set_sources(32, 12, OpenPIC_Addr + 0x10000);

#ifdef CONFIG_DETHERNET 
        /*Map PIC Message interrupt 0~3,by Jason Jin*/
        openpic_set_sources(44, 4, OpenPIC_Addr + 0x11600);
#endif	
	/* we let openpic interrupts starting from an offset, to
	 * leave space for cascading interrupts underneath.
	 */
	openpic_init(MPC85xx_OPENPIC_IRQ_OFFSET);

	return;
}

#ifdef CONFIG_PCI
/*
 * interrupt routing
 */

int
mpc85xx_map_irq(struct pci_dev *dev, unsigned char idsel, unsigned char pin)
{
	static char pci_irq_table[][4] =
	    /*
	     * This is little evil, but works around the fact
	     * that revA boards have IDSEL starting at 18
	     * and others boards (older) start at 12
	     *
	     *      PCI IDSEL/INTPIN->INTLINE
	     *       A      B      C      D
	     */
	{
		{PIRQA, PIRQB, PIRQC, PIRQD},	/* IDSEL 2 */
		{PIRQD, PIRQA, PIRQB, PIRQC},
		{PIRQC, PIRQD, PIRQA, PIRQB},
		{PIRQB, PIRQC, PIRQD, PIRQA},	/* IDSEL 5 */
		{0, 0, 0, 0},	/* -- */
		{0, 0, 0, 0},	/* -- */
		{0, 0, 0, 0},	/* -- */
		{0, 0, 0, 0},	/* -- */
		{0, 0, 0, 0},	/* -- */
		{0, 0, 0, 0},	/* -- */
		{PIRQA, PIRQB, PIRQC, PIRQD},	/* IDSEL 12 */
		{PIRQD, PIRQA, PIRQB, PIRQC},
		{PIRQC, PIRQD, PIRQA, PIRQB},
		{PIRQB, PIRQC, PIRQD, PIRQA},	/* IDSEL 15 */
		{0, 0, 0, 0},	/* -- */
		{0, 0, 0, 0},	/* -- */
		{PIRQA, PIRQB, PIRQC, PIRQD},	/* IDSEL 18 */
		{PIRQD, PIRQA, PIRQB, PIRQC},
		{PIRQC, PIRQD, PIRQA, PIRQB},
		{PIRQB, PIRQC, PIRQD, PIRQA},	/* IDSEL 21 */
	};

	const long min_idsel = 2, max_idsel = 21, irqs_per_slot = 4;
	return PCI_IRQ_TABLE_LOOKUP;
}

int
mpc85xx_exclude_device(u_char bus, u_char devfn)
{
	if (bus == 0 && PCI_SLOT(devfn) == 0)
		return PCIBIOS_DEVICE_NOT_FOUND;
	else
		return PCIBIOS_SUCCESSFUL;
}

#endif /* CONFIG_PCI */
