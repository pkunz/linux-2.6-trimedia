/*
 * arch/ppc/platforms/83xx/sbc834x.c
 *
 * Wind River SBC834x board specific routines.
 *
 * Based largely on mpc834x_sys.c
 * by Kumar Gala, which was Copyright 2005 Freescale Semiconductor Inc.
 *
 * Copyright 2005, Wind River.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
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
#include <linux/root_dev.h>
#include <linux/serial.h>
#include <linux/tty.h>	/* for linux/serial_core.h */
#include <linux/serial_core.h>
#include <linux/initrd.h>
#include <linux/module.h>
#include <linux/fsl_devices.h>

#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/atomic.h>
#include <asm/time.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/prom.h>
#include <asm/ipic.h>
#include <asm/bootinfo.h>
#include <asm/pci-bridge.h>
#include <asm/mpc83xx.h>
#include <asm/irq.h>
#include <asm/kgdb.h>
#include <asm/ppc_sys.h>
#include <mm/mmu_decl.h>

#include <syslib/ppc83xx_setup.h>
#include <syslib/gen550.h>

#ifndef CONFIG_PCI
unsigned long isa_io_base = 0;
unsigned long isa_mem_base = 0;
#endif

extern unsigned long total_memory;	/* in mm/init */

unsigned char __res[sizeof (bd_t)];

#if defined(CONFIG_PCI)
int
mpc83xx_map_irq(struct pci_dev *dev, unsigned char idsel,unsigned char pin)
{
	char pci_irq_table[][4] =
	/*
	 *	PCI IDSEL&INTPIN -> INTLINE
	 *	INTA    INTB    INTC    INTD
	 */
	{
		{PIRQA, PIRQB, PIRQC, PIRQD}, /* idsel 0x11 */
		{PIRQC, PIRQD, PIRQA, PIRQB}, /* idsel 0x12 */
		{PIRQD, PIRQA, PIRQB, PIRQC}  /* idsel 0x13 */
	};
	/* MPC8349 MDS board specific */
	const long min_idsel = 0x11,max_idsel = 0x13,irqs_per_slot = 4;
	return PCI_IRQ_TABLE_LOOKUP;
}

int
mpc83xx_exclude_device(u_char bus, u_char devfn)
{
	return PCIBIOS_SUCCESSFUL;
}

#endif /* CONFIG_PCI */


/* ************************************************************************
 *
 * Setup the architecture
 *
 */
static void __init
sbc834x_setup_arch(void)
{
	bd_t *binfo = (bd_t *) __res;
	unsigned int freq;
	struct gianfar_platform_data *pdata;

	/* get the core frequency */
	freq = binfo->bi_intfreq;

	/* Set loops_per_jiffy to a half-way reasonable value,
	   for use until calibrate_delay gets called. */
	loops_per_jiffy = freq / HZ;

#ifdef CONFIG_PCI
	/* setup PCI host bridges */
	mpc83xx_setup_hose();
#endif
	mpc83xx_early_serial_map();

	/* setup the board related information for the enet controllers */
	pdata = (struct gianfar_platform_data *) ppc_sys_get_pdata(MPC83xx_TSEC1);
	pdata->board_flags = FSL_GIANFAR_BRD_HAS_PHY_INTR;
	pdata->interruptPHY = MPC83xx_IRQ_EXT4;
	pdata->phyid = 0x19;
	/* fixup phy address */
	pdata->phy_reg_addr += binfo->bi_immr_base;
	memcpy(pdata->mac_addr, binfo->bi_enetaddr, 6);

	pdata = (struct gianfar_platform_data *) ppc_sys_get_pdata(MPC83xx_TSEC2);
	pdata->board_flags = FSL_GIANFAR_BRD_HAS_PHY_INTR;
	pdata->interruptPHY = MPC83xx_IRQ_EXT5;
	pdata->phyid = 0x1a;
	/* fixup phy address */
	pdata->phy_reg_addr += binfo->bi_immr_base;
	memcpy(pdata->mac_addr, binfo->bi_enet1addr, 6);

#ifdef CONFIG_BLK_DEV_INITRD
	if (initrd_start)
		ROOT_DEV = Root_RAM0;
	else
#endif
#ifdef CONFIG_ROOT_NFS
		ROOT_DEV = Root_NFS;
#else
		ROOT_DEV = Root_HDA1;
#endif

}

static void __init
sbc834x_map_io(void)
{
	/* we steal the lowest ioremap addr for virt space */
	io_block_mapping(VIRT_IMMRBAR, immrbar, 1024*1024, _PAGE_IO);
}

int
sbc834x_show_cpuinfo(struct seq_file *m)
{
	uint pvid, svid, phid1;
	bd_t *binfo = (bd_t *) __res;
	unsigned int freq;

	/* get the core frequency */
	freq = binfo->bi_intfreq;

	pvid = mfspr(SPRN_PVR);
	svid = mfspr(SPRN_SVR);

	seq_printf(m, "Vendor\t\t: Wind River.\n");
	seq_printf(m, "Machine\t\t: SBC834x\n");
	seq_printf(m, "core clock\t: %d MHz\n"
			"bus clock\t: %d MHz\n",
			(int)(binfo->bi_intfreq / 1000000),
			(int)(binfo->bi_busfreq / 1000000));
	seq_printf(m, "PVR\t\t: 0x%x\n", pvid);
	seq_printf(m, "SVR\t\t: 0x%x\n", svid);

	/* Display cpu Pll setting */
	phid1 = mfspr(SPRN_HID1);
	seq_printf(m, "PLL setting\t: 0x%x\n", ((phid1 >> 24) & 0x3f));

	/* Display the amount of memory */
	seq_printf(m, "Memory\t\t: %d MB\n", (int)(binfo->bi_memsize / (1024 * 1024)));

	return 0;
}


void __init
sbc834x_init_IRQ(void)
{
	bd_t *binfo = (bd_t *) __res;

	u8 senses[8] = {
#ifdef CONFIG_PCI
		IRQ_SENSE_LEVEL,	/* EXT 0 */
		IRQ_SENSE_LEVEL,	/* EXT 1 */
		IRQ_SENSE_LEVEL,	/* EXT 2 */
		IRQ_SENSE_LEVEL,	/* EXT 3 */
#else
		0,			/* EXT 0 */
		0,			/* EXT 1 */
		0,			/* EXT 2 */
		0,			/* EXT 3 */
#endif
		0,			/* EXT 4 */
		0,			/* EXT 5 */
		0,			/* EXT 6 */
		0,			/* EXT 7 */
	};

	ipic_init(binfo->bi_immr_base + 0x00700, 0, MPC83xx_IPIC_IRQ_OFFSET, senses, 8);

	/* Initialize the default interrupt mapping priorities,
	 * in case the boot rom changed something on us.
	 */
	ipic_set_default_priority();
}

static __inline__ void
sbc834x_set_bat(void)
{
	/* we steal the lowest ioremap addr for virt space */
	mb();
	mtspr(SPRN_DBAT1U, VIRT_IMMRBAR | 0x1e);
	mtspr(SPRN_DBAT1L, immrbar | 0x2a);
	mb();
}

void __init
platform_init(unsigned long r3, unsigned long r4, unsigned long r5,
	      unsigned long r6, unsigned long r7)
{
	bd_t *binfo = (bd_t *) __res;

	/* parse_bootinfo must always be called first */
	parse_bootinfo(find_bootinfo());

	/*
	 * If we were passed in a board information, copy it into the
	 * residual data area.
	 */
	if (r3) {
		memcpy((void *) __res, (void *) (r3 + KERNELBASE),
		       sizeof (bd_t));
	}

#if defined(CONFIG_BLK_DEV_INITRD)
	/*
	 * If the init RAM disk has been configured in, and there's a valid
	 * starting address for it, set it up.
	 */
	if (r4) {
		initrd_start = r4 + KERNELBASE;
		initrd_end = r5 + KERNELBASE;
	}
#endif /* CONFIG_BLK_DEV_INITRD */

	/* Copy the kernel command line arguments to a safe place. */
	if (r6) {
		*(char *) (r7 + KERNELBASE) = 0;
		strcpy(cmd_line, (char *) (r6 + KERNELBASE));
	}

	immrbar = binfo->bi_immr_base;

	sbc834x_set_bat();

#if defined(CONFIG_SERIAL_8250) && defined(CONFIG_SERIAL_TEXT_DEBUG)
	{
		struct uart_port p;

		memset(&p, 0, sizeof (p));
		p.iotype = SERIAL_IO_MEM;
		p.membase = (unsigned char __iomem *)(VIRT_IMMRBAR + 0x4500);
		p.uartclk = binfo->bi_busfreq;

		gen550_init(0, &p);

		memset(&p, 0, sizeof (p));
		p.iotype = SERIAL_IO_MEM;
		p.membase = (unsigned char __iomem *)(VIRT_IMMRBAR + 0x4600);
		p.uartclk = binfo->bi_busfreq;

		gen550_init(1, &p);
	}
#endif

	identify_ppc_sys_by_id(mfspr(SPRN_SVR));

	/* setup the PowerPC module struct */
	ppc_md.setup_arch = sbc834x_setup_arch;
	ppc_md.show_cpuinfo = sbc834x_show_cpuinfo;

	ppc_md.init_IRQ = sbc834x_init_IRQ;
	ppc_md.get_irq = ipic_get_irq;

	ppc_md.restart = mpc83xx_restart;
	ppc_md.power_off = mpc83xx_power_off;
	ppc_md.halt = mpc83xx_halt;

	ppc_md.find_end_of_memory = mpc83xx_find_end_of_memory;
	ppc_md.setup_io_mappings = sbc834x_map_io;

	ppc_md.time_init = mpc83xx_time_init;
	ppc_md.set_rtc_time = NULL;
	ppc_md.get_rtc_time = NULL;
	ppc_md.calibrate_decr = mpc83xx_calibrate_decr;

#if defined(CONFIG_SERIAL_8250) && defined(CONFIG_SERIAL_TEXT_DEBUG)
	ppc_md.progress = gen550_progress;
#endif	/* CONFIG_SERIAL_8250 && CONFIG_SERIAL_TEXT_DEBUG */

	if (ppc_md.progress)
		ppc_md.progress("sbc834x_init(): exit", 0);

	return;
}
