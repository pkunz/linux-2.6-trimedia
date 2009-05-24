/*
 * arch/ppc/platform/85xx/gda8500.c
 * 
 * GDA RMC-G8500 board specific routines
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
#include <linux/blkdev.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/seq_file.h>
#include <linux/root_dev.h>
#include <linux/serial.h>
#include <linux/module.h>
#include <linux/fsl_devices.h>
#include <linux/serial_core.h>

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
#include <asm/kgdb.h>
#include <asm/ocp.h>
#include <asm/ppc_sys.h>
#include <mm/mmu_decl.h>
#include <platforms/85xx/gda8500.h>
#include <syslib/ppc85xx_common.h>
#include <syslib/ppc85xx_setup.h>
#include <syslib/gen550.h>

#ifdef CONFIG_MTD
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#endif

#ifndef CONFIG_PCI
unsigned long isa_io_base = 0;
unsigned long isa_mem_base = 0;
#endif

extern unsigned long total_memory;	/* in mm/init */

unsigned char __res[sizeof (bd_t)];

/* Internal interrupts are all Level Sensitive, and Positive Polarity */

static u_char gda8500_openpic_initsenses[] __initdata = {
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
	(IRQ_SENSE_LEVEL |  IRQ_POLARITY_POSITIVE),	/* Internal 12: RIO Inbound Message */
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
	0x0,				/* External  0: */
#if defined(CONFIG_PCI)
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),      /* External 1: PCI_INTB# */
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),      /* External 2: PCI_INTC#*/
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),      /* External 3: PCI_INTD#*/
#else
	0x0,				/* External  1: */
	0x0,				/* External  2: */
	0x0,				/* External  3: */
#endif
	(IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),      /* External 4: LXT971 PHY */
       (IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),      /* External 5: 88E1011 PHY */
#if defined(CONFIG_PCI)
       (IRQ_SENSE_LEVEL | IRQ_POLARITY_NEGATIVE),      /* External 6: PCI_INTA#*/
#else
	0x0,				/* External  6: */
#endif
	0x0,				/* External  7: */
	0x0,				/* External  8: */
	0x0,				/* External  9: */
	0x0,				/* External 10: */
	0x0,				/* External 11: */
};

#if 0
struct ocp_gfar_data mpc85xx_tsec1_def = {
	.interruptTransmit = MPC85xx_IRQ_TSEC1_TX,
	.interruptError = MPC85xx_IRQ_TSEC1_ERROR,
	.interruptReceive = MPC85xx_IRQ_TSEC1_RX,
	.interruptPHY = MPC85xx_IRQ_EXT5,
	.flags = (GFAR_HAS_GIGABIT | GFAR_HAS_MULTI_INTR 
			| GFAR_HAS_RMON
			| GFAR_HAS_PHY_INTR | GFAR_HAS_COALESCE),
	.phyid = 7,
	.phyregidx = 0,
};

struct ocp_gfar_data mpc85xx_fec_def = {
	.interruptTransmit = MPC85xx_IRQ_FEC,
	.interruptError = MPC85xx_IRQ_FEC,
	.interruptReceive = MPC85xx_IRQ_FEC,
	.interruptPHY = MPC85xx_IRQ_EXT5,
	.flags = 0,
	.phyid = 3,
	.phyregidx = 0,
};

struct ocp_fs_i2c_data mpc85xx_i2c1_def = {
	.flags = FS_I2C_SEPARATE_DFSRR,
};
#endif

static void __init
gda8500_early_serial_map(void)
{
        struct uart_port uart_req;
	bd_t *binfo = (bd_t *) __res;
	phys_addr_t duart_paddr = binfo->bi_immr_base + MPC85xx_UART0_OFFSET;
 
        /* Setup serial port access */
        memset(&uart_req, 0, sizeof (uart_req));
        uart_req.uartclk = binfo->bi_busfreq;
        uart_req.line = 0;
        uart_req.irq = MPC85xx_IRQ_DUART;
        uart_req.flags = ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST;
        uart_req.iotype = SERIAL_IO_MEM;
        uart_req.membase = ioremap(duart_paddr, MPC85xx_UART0_SIZE);
        uart_req.mapbase = duart_paddr;
        uart_req.regshift = 0;

#ifdef CONFIG_SERIAL_8250
	if (early_serial_setup(&uart_req) != 0)
		printk("Early serial init of port 0 failed\n");
#endif
#ifdef CONFIG_SERIAL_TEXT_DEBUG
	gen550_init(0, &uart_req);
#endif
#ifdef CONFIG_KGDB_8250
	kgdb8250_add_port(0, &uart_req);
#endif

        /* Assume early_serial_setup() doesn't modify uart_req */
	duart_paddr = binfo->bi_immr_base + MPC85xx_UART1_OFFSET;
        uart_req.line = 1;
        uart_req.mapbase = duart_paddr;
        uart_req.membase = ioremap(duart_paddr, MPC85xx_UART1_SIZE);

#ifdef CONFIG_SERIAL_8250
	if (early_serial_setup(&uart_req) != 0)
		printk("Early serial init of port 1 failed\n");
#endif
#ifdef CONFIG_SERIAL_TEXT_DEBUG
	gen550_init(1, &uart_req);
#endif
#ifdef CONFIG_KGDB_8250
	kgdb8250_add_port(1, &uart_req);
#endif
}
/* ************************************************************************
 *
 * Setup the architecture
 *
 */
static void __init
gda8500_setup_arch(void)
{
	struct ocp_def *def;
	struct ocp_gfar_data *einfo;
	bd_t *binfo = (bd_t *) __res;
	unsigned int freq;
	struct gianfar_platform_data *pdata;

	/* get the core frequency */
	freq = binfo->bi_intfreq;

	if (ppc_md.progress)
		ppc_md.progress("gda8500_setup_arch()", 0);

	/* Set loops_per_jiffy to a half-way reasonable value,
	   for use until calibrate_delay gets called. */
	loops_per_jiffy = freq / HZ;

#ifdef CONFIG_PCI
	/* setup PCI host bridges */
	mpc85xx_setup_hose();
#endif

#ifdef CONFIG_SERIAL_8250
	gda8500_early_serial_map();
#endif

#ifdef CONFIG_SERIAL_TEXT_DEBUG
	/* Invalidate the entry we stole earlier the serial ports
	 * should be properly mapped */
	invalidate_tlbcam_entry(num_tlbcam_entries - 1);
#endif

	/* setup the board related information for the enet controllers */
	pdata = (struct gianfar_platform_data *) ppc_sys_get_pdata(MPC85xx_TSEC1);
	if (pdata) {
		pdata->board_flags = FSL_GIANFAR_BRD_HAS_PHY_INTR;
		pdata->interruptPHY = MPC85xx_IRQ_EXT5;
		pdata->phyid = 7;
		/* fixup phy address */
		pdata->phy_reg_addr += binfo->bi_immr_base;
		memcpy(pdata->mac_addr, binfo->bi_enetaddr, 6);
	}

	pdata = (struct gianfar_platform_data *) ppc_sys_get_pdata(MPC85xx_FEC);
	if (pdata) {
		pdata->board_flags = FSL_GIANFAR_BRD_HAS_PHY_INTR;
		pdata->interruptPHY = MPC85xx_IRQ_EXT5;
		pdata->phyid = 3;
		/* fixup phy address */
		pdata->phy_reg_addr += binfo->bi_immr_base;
		memcpy(pdata->mac_addr, binfo->bi_enet1addr, 6);
	}

#ifdef CONFIG_BLK_DEV_INITRD
	if (initrd_start)
		ROOT_DEV = Root_RAM0;
	else
#endif
#ifdef  CONFIG_ROOT_NFS
		ROOT_DEV = Root_NFS;
#endif
}

/* ************************************************************************ */
void __init
platform_init(unsigned long r3, unsigned long r4, unsigned long r5,
	      unsigned long r6, unsigned long r7)
{
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
#ifdef CONFIG_SERIAL_TEXT_DEBUG
	{
		bd_t *binfo = (bd_t *) __res;
		struct uart_port p;

		/* Use the last TLB entry to map CCSRBAR to allow access to DUART regs */
		settlbcam(num_tlbcam_entries - 1, binfo->bi_immr_base,
			  binfo->bi_immr_base, MPC85xx_CCSRBAR_SIZE, _PAGE_IO, 0);

		memset(&p, 0, sizeof (p));
		p.iotype = SERIAL_IO_MEM;
		p.membase = (void *) binfo->bi_immr_base + MPC85xx_UART0_OFFSET;
		p.uartclk = binfo->bi_busfreq;

		gen550_init(0, &p);
	}
#endif

#if defined(CONFIG_BLK_DEV_INITRD)
	/*
	 * If the init RAM disk has been configured in, and there's a valid
	 * starting address for it, set it up.
	 */
	if (r4) {
		initrd_start = r4 + KERNELBASE;
		initrd_end = r5 + KERNELBASE;
	}
#endif				/* CONFIG_BLK_DEV_INITRD */

	/* Copy the kernel command line arguments to a safe place. */

	if (r6) {
		*(char *) (r7 + KERNELBASE) = 0;
		strcpy(cmd_line, (char *) (r6 + KERNELBASE));
	}

	identify_ppc_sys_by_id(mfspr(SPRN_SVR));

	/* setup the PowerPC module struct */
	ppc_md.setup_arch = gda8500_setup_arch;
	ppc_md.show_cpuinfo = gda8500_show_cpuinfo;

	ppc_md.init_IRQ = gda8500_init_IRQ;
	ppc_md.get_irq = openpic_get_irq;

	ppc_md.restart = mpc85xx_restart;
	ppc_md.power_off = mpc85xx_power_off;
	ppc_md.halt = mpc85xx_halt;

	ppc_md.find_end_of_memory = mpc85xx_find_end_of_memory;

	ppc_md.time_init = NULL;
	ppc_md.set_rtc_time = NULL;
	ppc_md.get_rtc_time = NULL;
	ppc_md.calibrate_decr = mpc85xx_calibrate_decr;

#if defined(CONFIG_SERIAL_8250) && defined (CONFIG_SERIAL_TEXT_DEBUG)
	ppc_md.progress = gen550_progress;
#endif

	if (ppc_md.progress)
		ppc_md.progress("gda8500_init(): exit", 0);

	return;
}

/* ************************************************************************ */
int
gda8500_show_cpuinfo(struct seq_file *m)
{
	uint pvid, svid, phid1;
	uint memsize = total_memory;
	bd_t *binfo = (bd_t *) __res;
	unsigned int freq;

	/* get the core frequency */
	freq = binfo->bi_intfreq;

	pvid = mfspr(SPRN_PVR);
	svid = mfspr(SPRN_SVR);

	seq_printf(m, "Vendor\t\t: GDA\n");
	seq_printf(m, "Machine\t\t: RMC%s\n", cur_ppc_sys_spec->ppc_sys_name);
	seq_printf(m, "bus freq\t: %u.%.6u MHz\n", freq / 1000000,
		   freq % 1000000);
	seq_printf(m, "PVR\t\t: 0x%x\n", pvid);
	seq_printf(m, "SVR\t\t: 0x%x\n", svid);

	/* Display cpu Pll setting */
	phid1 = mfspr(SPRN_HID1);
	seq_printf(m, "PLL setting\t: 0x%x\n", ((phid1 >> 24) & 0x3f));

	/* Display the amount of memory */
	seq_printf(m, "Memory\t\t: %d MB\n", memsize / (1024 * 1024));

	return 0;
}

void __init
gda8500_init_IRQ(void)
{
	bd_t *binfo = (bd_t *) __res;
	/* Determine the Physical Address of the OpenPIC regs */
	phys_addr_t OpenPIC_PAddr =
	    binfo->bi_immr_base + MPC85xx_OPENPIC_OFFSET;
	OpenPIC_Addr = ioremap(OpenPIC_PAddr, MPC85xx_OPENPIC_SIZE);
	OpenPIC_InitSenses = gda8500_openpic_initsenses;
	OpenPIC_NumInitSenses = sizeof (gda8500_openpic_initsenses);

	/* Skip reserved space and internal sources */
	openpic_set_sources(0, 32, OpenPIC_Addr + 0x10200);
	/* Map PIC IRQs 0-11 */
	openpic_set_sources(32, 12, OpenPIC_Addr + 0x10000);

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
	     *      PCI IDSEL/INTPIN->INTLINE
	     *        A      B      C      D
	     */
	{
               {PIRQC, PIRQD, PIRQA, PIRQB},   /* 0x12 */
               {PIRQD, PIRQA, PIRQB, PIRQC},   /* 0x13 */
               {PIRQB, PIRQC, PIRQD, PIRQA},   /* 0x14 */
               {PIRQC, PIRQD, PIRQA, PIRQB},   /* 0x15 */
               {PIRQD, PIRQA, PIRQB, PIRQC},   /* 0x16 */
               {PIRQA, PIRQB, PIRQC, PIRQD},   /* 0x17 */
	};

	const long min_idsel = 0x12, max_idsel = 0x17, irqs_per_slot = 4;
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
