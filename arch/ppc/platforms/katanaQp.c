/*! \file arch/ppc/platforms/katanaQp.c
 *
 * Board setup routines for the Artesyn KatanaQP
 *
 * Tim Montgomery <timm@artesyncp.com> 
 *
 * Based on code by Rabeeh Khoury - rabeeh@galileo.co.il
 * Based on code by Mark A. Greer - <mgreer@mvista.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */


#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/kdev_t.h>
#include <linux/console.h>
#include <linux/initrd.h>
#include <linux/root_dev.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/bootmem.h>
#include <linux/mtd/physmap.h>
#include <linux/mv643xx.h>
#include <linux/interrupt.h>
#ifdef CONFIG_BOOTIMG
#include <linux/bootimg.h>
#endif
#include <asm/page.h>
#include <asm/time.h>
#include <asm/smp.h>
#include <asm/todc.h>
#include <asm/bootinfo.h>
#include <asm/pci-bridge.h>
#include <asm/ppcboot.h>
#include <asm/mv64x60.h>
#include <platforms/katanaQp.h>

static struct 		mv64x60_handle bh;
static u32		sram_base;
static u32		cpld_base;

static u32              katanaQp_flash_size_0;
static u32              katanaQp_flash_size_1;

extern char cmd_line[];
extern void ipmb_i2c_addr_get_fcn_set(short *(*fcn) (void));

extern ulong m41t00_get_rtc_time(void);
extern int m41t00_set_rtc_time(ulong nowtime);

#define KATANA_MBOX_RESET_REQUEST   0xC83A
#define KATANA_MBOX_RESET_ACK       0xE430
#define KATANA_MBOX_RESET_DONE      0x32E5

/* Global Variables */
unsigned char __res[sizeof(bd_t)];

bd_t ppcboot_bd;
int ppcboot_bd_valid = 0;

extern struct i2c_adapter *mv64x60_i2c_adapter_get(int adapter);

static int __init katanaQp_table_lookup(unsigned char idsel, unsigned char pin)
{
	static char pci_irq_table[][4] = {

	/* PCI IDSEL/INTPIN ==> INTLINE */

	/* IDSEL 1  (PMC 1) */
	{KATANAQP_PCI1_INTA_IRQ, KATANAQP_PCI1_INTB_IRQ,
	 KATANAQP_PCI1_INTC_IRQ, KATANAQP_PCI1_INTD_IRQ},

	/* IDSEL 2  (PMC 2) */
	{KATANAQP_PCI1_INTB_IRQ, KATANAQP_PCI1_INTC_IRQ,
	 KATANAQP_PCI1_INTD_IRQ, KATANAQP_PCI1_INTA_IRQ},

	/* IDSEL 3  (PMC 3) */
	{KATANAQP_PCI1_INTC_IRQ, KATANAQP_PCI1_INTD_IRQ,
	 KATANAQP_PCI1_INTA_IRQ, KATANAQP_PCI1_INTB_IRQ},

	/* IDSEL 4  (PMC 4) */
	{KATANAQP_PCI1_INTD_IRQ, KATANAQP_PCI1_INTA_IRQ,
	 KATANAQP_PCI1_INTB_IRQ, KATANAQP_PCI1_INTC_IRQ},

	/* IDSEL 5  (Marvell Ethernet Switch) */
	{KATANAQP_PCI1_INTB_IRQ, 0, 0, 0},

	};

	const long min_idsel = 1, max_idsel = 5, irqs_per_slot = 4;

	return (PCI_IRQ_TABLE_LOOKUP);
}

/**
 * katanaQp_map_irq - top level PCI IRQ map callback
 *
 * \param dev   PCI device structure associated with the device
 * \param idsel the IDSEL on which we are located
 * \param pin   not used
 *
 * This routine determines the appropriate PCI IRQ table lookup function 
 * to call, based on the board ID.
 */

static int __init katanaQp_map_irq(struct pci_dev *dev,
				   unsigned char idsel, unsigned char pin)
{
	struct pci_controller *hose = pci_bus_to_hose(dev->bus->number);

	if (hose == bh.hose_a) {
		/* Return the maps associated with hose_a / interface PCI0 */
		return (0);
	} else if (hose == bh.hose_b) {
		return (katanaQp_table_lookup(idsel, pin));
	} else {
		/* Only two interfaces available - return error if not one of the two */
		printk("Error (katanaQp_map_irq): Hose match not found\n");
		return (-1);
	}
	return (-1);
}

#if defined(CONFIG_SERIAL_TEXT_DEBUG) && defined(CONFIG_SERIAL_MPSC_CONSOLE)
static void __init katanaQp_map_io(void)
{
	io_block_mapping(0xf8100000, 0xf8100000, 0x00020000, _PAGE_IO);
}
#endif

/**
 * katanaQp_set_bat - set a bat to enable early I/O acces
 *
 * This routine configures a (temporary) bat mapping for early access to 
 * device I/O.  
 */

static __inline__ void katanaQp_set_bat(void)
{
	mb();
	mtspr(SPRN_DBAT2U, 0xf0001ffe);
	mtspr(SPRN_DBAT2L, 0xf000002a);
	mb();

}

/**
 * katanaQp_find_end_of_memory - ppc_md memory size callback
 *
 * This routine determines the memory size of the board.
 */

unsigned long __init katanaQp_find_end_of_memory(void)
{
	return mv64x60_get_mem_size(KATANAQP_BRIDGE_REG_BASE,
				    MV64x60_TYPE_MV64360);
}

/**
 * katanaQp_is_monarch - determine monarch status
 *
 * This routine checks the PLD to see if we are designated as a PPMC
 * Monarch.
 */

static int katanaQp_is_monarch(void)
{
	return (!(in_8((volatile char *)(cpld_base + KATANAQP_CPLD_PSR)) &
		  KATANAQP_CPLD_PSR_PMCM));
}

void __init katanaQp_intr_setup(void)
{
	/*
	 * Configure the following MPP pins
	 *
	 * Board-specific interrupt vectors
	 * 
	 * PCI0 INTA --> MPP19
	 * PCI0 INTB --> MPP21
	 * PCI0 INTC --> MPP22 
	 * PCI0 INTD --> MPP25
	 * 
	 * PCI1 INTA --> MPP26
	 * PCI1 INTB --> MPP27
	 * PCI1 INTC --> MPP30
	 * PCI1 INTD --> MPP31
	 *
	 *    PERR/SERR --> MPP23
	 * NMI Watchdog --> MPP18
	 */

	mv64x60_clr_bits(&bh, MV64x60_MPP_CNTL_2, (0xf << 20) | (0xf << 24));

	/* configure MPP pins as GPIO */

	mv64x60_clr_bits(&bh, MV64x60_MPP_CNTL_2,
			 (MV64x60_MPP_CNTL2_MPP23_GPIO |
			  MV64x60_MPP_CNTL2_MPP22_GPIO |
			  MV64x60_MPP_CNTL2_MPP21_GPIO |
			  MV64x60_MPP_CNTL2_MPP19_GPIO |
			  MV64x60_MPP_CNTL2_MPP18_GPIO));

	mv64x60_clr_bits(&bh, MV64x60_MPP_CNTL_3,
			 (MV64x60_MPP_CNTL3_MPP31_GPIO |
			  MV64x60_MPP_CNTL3_MPP30_GPIO |
			  MV64x60_MPP_CNTL3_MPP27_GPIO |
			  MV64x60_MPP_CNTL3_MPP26_GPIO |
			  MV64x60_MPP_CNTL3_MPP25_GPIO));

	/* define PCI interrupts as active low, level trigger */
	/* TODO: enable watchdog NMI, P/SERR */

	mv64x60_set_bits(&bh, MV64x60_GPP_LEVEL_CNTL,
			 (1 << 19) | (1 << 20) | (1 << 22) | (1 << 25) |
			 (1 << 26) | (1 << 27) | (1 << 30) | (1 << 31));

	mv64x60_clr_bits(&bh, MV64x60_GPP_IO_CNTL,
			 (1 << 19) | (1 << 20) | (1 << 22) | (1 << 25) |
			 (1 << 26) | (1 << 27) | (1 << 30) | (1 << 31));

	/* Config GPP interrupt controller to respond to level trigger */

	mv64x60_set_bits(&bh, MV64x60_COMM_ARBITER_CNTL, (1 << 10));

	/*
	 * Dismiss and then enable interrupt on GPP interrupt cause for CPU #0
	 */
	mv64x60_write(&bh, MV64x60_GPP_INTR_CAUSE,
		      ~((1 << 19) | (1 << 20) | (1 << 22) | (1 << 25) |
			(1 << 26) | (1 << 27) | (1 << 30) | (1 << 31)));

	mv64x60_set_bits(&bh, MV64x60_GPP_INTR_MASK,
			 (1 << 19) | (1 << 20) | (1 << 22) | (1 << 25) |
			 (1 << 26) | (1 << 27) | (1 << 30) | (1 << 31));

	/*
	 * Dismiss and then enable interrupt on CPU #0 high cause register
	 * BIT26 summarizes GPP interrupts 16-23
	 * BIT27 summarizes GPP interrupts 24-31
	 */
	mv64x60_set_bits(&bh, MV64360_IC_CPU0_INTR_MASK_HI,
			 (1 << 26) | (1 << 27));

}

void __init katanaQp_setup_peripherals(void)
{
	u32 base;

	/* Set up windows for boot CS, soldered and socketed flash, and CPLD */
	mv64x60_set_32bit_window(&bh, MV64x60_CPU2BOOT_WIN,
		KATANAQP_BOOT_WINDOW_BASE, KATANAQP_BOOT_WINDOW_SIZE, 0);

	bh.ci->enable_window_32bit(&bh, MV64x60_CPU2BOOT_WIN);

	/* Assume monitor firmware set up window sizes correctly for dev 0 & 1 */
	mv64x60_get_32bit_window(&bh, MV64x60_CPU2DEV_0_WIN, &base, 
		&katanaQp_flash_size_0);

	if (katanaQp_flash_size_0 > 0) {
		mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_0_WIN,
			KATANAQP_SOLDERED_FLASH_BASE, katanaQp_flash_size_0, 0);
		bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_0_WIN);
	}

	mv64x60_get_32bit_window(&bh, MV64x60_CPU2DEV_1_WIN, &base, 
		&katanaQp_flash_size_1);

	if (katanaQp_flash_size_1 > 0) {
		mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_1_WIN, 
			(KATANAQP_SOLDERED_FLASH_BASE + katanaQp_flash_size_0), 
			katanaQp_flash_size_1, 0);
		bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_1_WIN);
	}

        mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_2_WIN,
                 KATANAQP_SOCKET_BASE, KATANAQP_SOCKETED_FLASH_SIZE, 0);
        bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_2_WIN);

	mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_3_WIN,
				 KATANAQP_CPLD_BASE, KATANAQP_CPLD_SIZE, 0);
	bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_3_WIN);

	cpld_base = (u32) ioremap(KATANAQP_CPLD_BASE, KATANAQP_CPLD_SIZE);

	mv64x60_set_32bit_window(&bh, MV64x60_CPU2SRAM_WIN,
				 KATANAQP_INTERNAL_SRAM_BASE,
				 KATANAQP_INTERNAL_SRAM_SIZE, 0);

	bh.ci->enable_window_32bit(&bh, MV64x60_CPU2SRAM_WIN);

	sram_base = (u32) ioremap(KATANAQP_INTERNAL_SRAM_BASE,
				  KATANAQP_INTERNAL_SRAM_SIZE);

	/* set up Enet->SRAM window */
	mv64x60_set_32bit_window(&bh, MV64x60_ENET2MEM_4_WIN,
				 KATANAQP_INTERNAL_SRAM_BASE,
				 KATANAQP_INTERNAL_SRAM_SIZE, 0x2);
	bh.ci->enable_window_32bit(&bh, MV64x60_ENET2MEM_4_WIN);

	/* Give enet r/w access to memory region */
	mv64x60_set_bits(&bh, MV64360_ENET2MEM_ACC_PROT_0, (0x3 << (4 << 1)));
	mv64x60_set_bits(&bh, MV64360_ENET2MEM_ACC_PROT_1, (0x3 << (4 << 1)));
	mv64x60_set_bits(&bh, MV64360_ENET2MEM_ACC_PROT_2, (0x3 << (4 << 1)));

	mv64x60_clr_bits(&bh, MV64x60_PCI1_PCI_DECODE_CNTL, (1 << 3));
	mv64x60_clr_bits(&bh, MV64x60_TIMR_CNTR_0_3_CNTL,
			 ((1 << 0) | (1 << 8) | (1 << 16) | (1 << 24)));

#if defined(CONFIG_NOT_COHERENT_CACHE)
	mv64x60_write(&bh, MV64360_SRAM_CONFIG, 0x00160000);
#else
	mv64x60_write(&bh, MV64360_SRAM_CONFIG, 0x001600b2);
#endif

	/* Enumerate pci buses */
	if (katanaQp_is_monarch()) {
		mv64x60_set_bus(&bh, 0, 0);
		bh.hose_a->first_busno = 0;
		bh.hose_a->last_busno = 0xff;
		bh.hose_a->last_busno = pciauto_bus_scan(bh.hose_a, 0);
		bh.hose_b->first_busno = bh.hose_a->last_busno + 1;
		mv64x60_set_bus(&bh, 1, bh.hose_b->first_busno);
		bh.hose_b->last_busno = 0xff;
		bh.hose_b->last_busno = pciauto_bus_scan(bh.hose_b,
							 bh.hose_b->
							 first_busno);
	}

	/*
	 * Setting the SRAM to 0. Note that this generates parity errors on
	 * internal data path in SRAM since it's first time accessing it
	 * while after reset it's not configured.
	 */
	memset((void *)sram_base, 0, KATANAQP_INTERNAL_SRAM_SIZE);

	/* Set up interrupts only if monarch */
	if (katanaQp_is_monarch())
		katanaQp_intr_setup();
}

/**
 * katanaQp_setup_bridge - initalize board-specific settings on the MV64460
 *
 * This routine configures the PCI mapping for this board by modifying a 
 * mv64x60_bridge_info_t structure appropriately, then passing it into the
 * mv64x60_find_bridges() function.  It then maps the device chip selects
 * and programs GPP interrupt pins for this board.
 */

static void __init katanaQp_setup_bridge(void)
{
	struct mv64x60_setup_info si;
	int i;

	memset(&si, 0, sizeof(si));
	si.phys_reg_base = CONFIG_MV64X60_NEW_BASE;

	si.pci_0.enable_bus = 1;
	si.pci_0.pci_io.cpu_base = KATANAQP_PCI0_IO_START_PROC_ADDR;
	si.pci_0.pci_io.pci_base_hi = 0;
	si.pci_0.pci_io.pci_base_lo = KATANAQP_PCI0_IO_START_PCI_ADDR;
	si.pci_0.pci_io.size = KATANAQP_PCI0_IO_SIZE;
	si.pci_0.pci_io.swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_0.pci_mem[0].cpu_base = KATANAQP_PCI0_MEM_START_PROC_ADDR;
	si.pci_0.pci_mem[0].pci_base_hi = KATANAQP_PCI0_MEM_START_PCI_HI_ADDR;
	si.pci_0.pci_mem[0].pci_base_lo = KATANAQP_PCI0_MEM_START_PCI_LO_ADDR;
	si.pci_0.pci_mem[0].size = KATANAQP_PCI0_MEM_SIZE;
	si.pci_0.pci_mem[0].swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_0.pci_cmd_bits = 0;
	si.pci_0.latency_timer = 0x80;

	si.pci_1.enable_bus = 1;
	si.pci_1.pci_io.cpu_base = KATANAQP_PCI1_IO_START_PROC_ADDR;
	si.pci_1.pci_io.pci_base_hi = 0;
	si.pci_1.pci_io.pci_base_lo = KATANAQP_PCI1_IO_START_PCI_ADDR;
	si.pci_1.pci_io.size = KATANAQP_PCI1_IO_SIZE;
	si.pci_1.pci_io.swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_1.pci_mem[0].cpu_base = KATANAQP_PCI1_MEM0_START_PROC_ADDR;
	si.pci_1.pci_mem[0].pci_base_hi = KATANAQP_PCI1_MEM0_START_PCI_HI_ADDR;
	si.pci_1.pci_mem[0].pci_base_lo = KATANAQP_PCI1_MEM0_START_PCI_LO_ADDR;
	si.pci_1.pci_mem[0].size = KATANAQP_PCI1_MEM0_SIZE;
	si.pci_1.pci_mem[0].swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_1.pci_mem[1].cpu_base = KATANAQP_PCI1_MEM1_START_PROC_ADDR;
	si.pci_1.pci_mem[1].pci_base_hi = KATANAQP_PCI1_MEM1_START_PCI_HI_ADDR;
	si.pci_1.pci_mem[1].pci_base_lo = KATANAQP_PCI1_MEM1_START_PCI_LO_ADDR;
	si.pci_1.pci_mem[1].size = KATANAQP_PCI1_MEM1_SIZE;
	si.pci_1.pci_mem[1].swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_1.pci_cmd_bits = 0;
	si.pci_1.latency_timer = 0x80;

	for (i = 0; i < MV64x60_CPU2MEM_WINDOWS; i++) {
#if defined(CONFIG_NOT_COHERENT_CACHE)
		si.cpu_prot_options[i] = 0;
		si.enet_options[i] = MV64360_ENET2MEM_SNOOP_NONE;
		si.mpsc_options[i] = MV64360_MPSC2MEM_SNOOP_NONE;
		si.idma_options[i] = MV64360_IDMA2MEM_SNOOP_NONE;

		si.pci_0.acc_cntl_options[i] =
		    MV64360_PCI_ACC_CNTL_SNOOP_NONE |
		    MV64360_PCI_ACC_CNTL_SWAP_NONE |
		    MV64360_PCI_ACC_CNTL_MBURST_128_BYTES |
		    MV64360_PCI_ACC_CNTL_RDSIZE_256_BYTES;

		si.pci_1.acc_cntl_options[i] =
		    MV64360_PCI_ACC_CNTL_SNOOP_NONE |
		    MV64360_PCI_ACC_CNTL_SWAP_NONE |
		    MV64360_PCI_ACC_CNTL_MBURST_128_BYTES |
		    MV64360_PCI_ACC_CNTL_RDSIZE_256_BYTES;
#else
		si.cpu_prot_options[i] = 0;
		si.enet_options[i] = MV64360_ENET2MEM_SNOOP_NONE;	/* errata */
		si.mpsc_options[i] = MV64360_MPSC2MEM_SNOOP_NONE;	/* errata */
		si.idma_options[i] = MV64360_IDMA2MEM_SNOOP_NONE;	/* errata */

		si.pci_0.acc_cntl_options[i] =
		    MV64360_PCI_ACC_CNTL_SNOOP_WB |
		    MV64360_PCI_ACC_CNTL_SWAP_NONE |
		    MV64360_PCI_ACC_CNTL_MBURST_32_BYTES |
		    MV64360_PCI_ACC_CNTL_RDSIZE_32_BYTES;

		si.pci_1.acc_cntl_options[i] =
		    MV64360_PCI_ACC_CNTL_SNOOP_WB |
		    MV64360_PCI_ACC_CNTL_SWAP_NONE |
		    MV64360_PCI_ACC_CNTL_MBURST_32_BYTES |
		    MV64360_PCI_ACC_CNTL_RDSIZE_32_BYTES;
#endif
	}

	/* Lookup PCI host bridges */
	if (mv64x60_init(&bh, &si))
		printk(KERN_WARNING "Bridge initialization failed.\n");

	pci_dram_offset = 0;	/* sys mem at same addr on PCI & cpu bus */
	ppc_md.pci_swizzle = common_swizzle;
	ppc_md.pci_map_irq = katanaQp_map_irq;
	ppc_md.pci_exclude_device = mv64x60_pci_exclude_device;
}

/**************************************************************************
 * FUNCTION: katanaQp_setup_arch
 *
 *
 ****/
static void __init katanaQp_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("katanaQp_setup_arch: enter", 0);

	if (ppc_md.progress)
		ppc_md.progress(cmd_line, 0x0);

	set_tb(0, 0);

#ifdef CONFIG_BLK_DEV_INITRD
	if (initrd_start)
		ROOT_DEV = Root_RAM0;
	else
#endif
#ifdef CONFIG_ROOT_NFS
		ROOT_DEV = Root_NFS;
#else
		ROOT_DEV = Root_SDA2;	/* TODO: maybe a better choice would be HDA1? */
#endif

	/* enable the L2 cache */
	_set_L2CR(L2CR_L2E | L2CR_L2PE);

	/* initialize PCI bridge and enumerate PCI (if appropriate) */
	katanaQp_setup_bridge();
	katanaQp_setup_peripherals();

	printk("Artesyn Communication Products, LLC - KatanaQp(TM)\n");

	if (ppc_md.progress)
		ppc_md.progress("katanaQp_setup_arch: exit", 0);
}

/* Platform device data fixup routines. */
#if defined(CONFIG_SERIAL_MPSC)
static void __init katanaQp_fixup_mpsc_pdata(struct platform_device *pdev)
{
	struct mpsc_pdata *pdata;

	pdata = (struct mpsc_pdata *)pdev->dev.platform_data;

	pdata->max_idle = 40;
	pdata->default_baud = KATANAQP_DEFAULT_BAUD;
	pdata->brg_clk_src = KATANAQP_MPSC_CLK_SRC;
	pdata->brg_clk_freq = KATANAQP_MPSC_CLK_FREQ;
}
#endif

#if defined(CONFIG_MV643XX_ETH)
static void __init katanaQp_fixup_eth_pdata(struct platform_device *pdev)
{
	struct mv643xx_eth_platform_data *eth_pd;

#if 0 /* Ok, this is ugly.  On the current version of the KatanaQp, the
       * PHYs attached to the MV64460 GMII are not all attached to the 
       * MV64460 SMI interface.  Specifically, ports 0 and 1 are attached
       * instead to the SMI interface on the Marvell Ethernet switch.
       * This results in the Ethernet driver's disabling of ports 0 & 1
       * since its PHY detection routine fails.  
       * Since all PHYs attached to the MV64460 are force to be link up, 
       * full duplex, 1000BT, we will feed the SMI address of the port 2
       * PHY (which *is* attached to the MV64460 SMI) to the driver.
       * 
       * Moving the PHYs on port 0 & 1 to the SMI interface on future board
       * revisions would be highly desirable.
       **/
	static u16 phy_addr[] = {
		KATANAQP_ETH0_PHY_ADDR,
		KATANAQP_ETH1_PHY_ADDR,
		KATANAQP_ETH2_PHY_ADDR,
	};
#endif
	static u16 phy_addr[] = {
		KATANAQP_ETH2_PHY_ADDR,
		KATANAQP_ETH2_PHY_ADDR,
		KATANAQP_ETH2_PHY_ADDR,
	};

	eth_pd = pdev->dev.platform_data;
	eth_pd->force_phy_addr = 1;
	eth_pd->phy_addr = phy_addr[pdev->id];
	eth_pd->tx_queue_size = KATANAQP_ETH_TX_QUEUE_SIZE;
	eth_pd->rx_queue_size = KATANAQP_ETH_RX_QUEUE_SIZE;

#define KATANAQP_MV643XXX_PSCR \
	(MV643XX_ETH_SERIAL_PORT_ENABLE	|\
	MV643XX_ETH_FORCE_LINK_PASS	|\
	MV643XX_ETH_DISABLE_AUTO_NEG_FOR_DUPLX	|\
	MV643XX_ETH_DISABLE_AUTO_NEG_FOR_FLOW_CTRL |\
	MV643XX_ETH_ADV_NO_FLOW_CTRL |\
	MV643XX_ETH_FORCE_FC_MODE_NO_PAUSE_DIS_TX |\
	MV643XX_ETH_FORCE_BP_MODE_NO_JAM |\
	(1 << 9)			 |\
	MV643XX_ETH_DO_NOT_FORCE_LINK_FAIL |\
	MV643XX_ETH_RETRANSMIT_16_ATTEMPTS |\
	MV643XX_ETH_DISABLE_AUTO_NEG_SPEED_GMII |\
	MV643XX_ETH_MAX_RX_PACKET_9700BYTE |\
	MV643XX_ETH_SET_FULL_DUPLEX_MODE |\
	MV643XX_ETH_DISABLE_FLOW_CTRL_TX_RX_IN_FULL_DUPLEX |\
	MV643XX_ETH_SET_GMII_SPEED_TO_1000)

	eth_pd->port_serial_control = KATANAQP_MV643XXX_PSCR;
	eth_pd->port_config = 0x0000C000;
	eth_pd->port_config_extend = 0x00000000;

}
#endif

static int __init katanaQp_platform_notify(struct device *dev)
{
	static struct {
		char *bus_id;
		void ((*rtn) (struct platform_device * pdev));
	} dev_map[] = {
#if defined(CONFIG_SERIAL_MPSC)
		{
		MPSC_CTLR_NAME ".0", katanaQp_fixup_mpsc_pdata}, {
		MPSC_CTLR_NAME ".1", katanaQp_fixup_mpsc_pdata},
#endif
#if defined(CONFIG_MV643XX_ETH)
		{
		MV643XX_ETH_NAME ".0", katanaQp_fixup_eth_pdata}, {
		MV643XX_ETH_NAME ".1", katanaQp_fixup_eth_pdata}, {
		MV643XX_ETH_NAME ".2", katanaQp_fixup_eth_pdata},
#endif
	};
	struct platform_device *pdev;
	int i;

	if (dev && dev->bus_id)
		for (i = 0; i < ARRAY_SIZE(dev_map); i++)
			if (!strncmp(dev->bus_id, dev_map[i].bus_id,
				     BUS_ID_SIZE)) {

				pdev = container_of(dev,
						    struct platform_device,
						    dev);
				dev_map[i].rtn(pdev);
			}

	return 0;
}

/**
 * katanaQp_restart - ppc_md machine reset callback
 * 
 * \param cmd not used
 *
 * This routine invokes process_shutdown(), then resets the board 
 * via the CPLD command register.
 *
 */

static void katanaQp_restart(char *cmd)
{
	volatile ulong i = 10000000;

	/* issue hard reset to the reset command register */

	out_8((volatile char *)(cpld_base + KATANAQP_CPLD_RCR),
	      KATANAQP_CPLD_RCR_CPUHR);

	while (i-- > 0) ;
	panic("restart failed\n");
}

/**
 * katanaQp_halt - ppc_md machine halt callback
 *
 * This routine invokes process_shutdown(), then halts the board
 * by entering an infinite loop.
 */

static void katanaQp_halt(void)
{
	//process_shutdown(); 

	while (1) ;

	/* NOTREACHED */
}

/**
 * katanaQp_halt - ppc_md machine power down callback
 * 
 * This routine simply calls the katanaQp_halt() function (power-down
 * and halt are the same for this board).
 */

static void katanaQp_power_off(void)
{
	katanaQp_halt();

	/* NOTREACHED */
}

/**
 * katanaQp_show_cpuinfo - ppc_md cpuinfo callback
 *
 * This routine returns extra board information to be displayed in 
 * /proc/cpuinfo
 */

static int katanaQp_show_cpuinfo(struct seq_file *m)
{
	seq_printf(m, "vendor\t\t: Artesyn Communication Products, LLC\n");

	seq_printf(m, "board\t\t: KatanaQp\n");

	seq_printf(m, "hardware rev\t: %d\n",
		   in_8((volatile char *)(cpld_base + KATANAQP_CPLD_HVR)));

	seq_printf(m, "flash size\t: %ldMB\n", (unsigned long)
		(katanaQp_flash_size_0 + katanaQp_flash_size_1)/ 0x100000);

	seq_printf(m, "CPLD rev\t: %d\n",
		   in_8((volatile char *)(cpld_base + KATANAQP_CPLD_PVR)));

	seq_printf(m, "PLB freq\t: %ldMhz\n", katanaQp_bus_freq(cpld_base) / 1000000);

	seq_printf(m, "PCI\t\t: %sMonarch\n",
		   katanaQp_is_monarch()? "" : "Non-");

#ifdef DEBUG
	seq_printf(m, "L2CR\t\t: 0x%x\n", mfspr(L2CR));

	seq_printf(m, "HID0\t\t: 0x%x\n", mfspr(HID0));

	seq_printf(m, "HID1\t\t: 0x%x\n", mfspr(HID1));

	seq_printf(m, "HID2\t\t: 0x%x\n", mfspr(1016));

	seq_printf(m, "PSCR0\t\t: 0x%x\n", mv64x60_read(&bh, MV643XX_ETH_PORT_SERIAL_CONTROL_REG(0)));
	seq_printf(m, "PSCR1\t\t: 0x%x\n", mv64x60_read(&bh, MV643XX_ETH_PORT_SERIAL_CONTROL_REG(1)));
	seq_printf(m, "PSCR2\t\t: 0x%x\n", mv64x60_read(&bh, MV643XX_ETH_PORT_SERIAL_CONTROL_REG(2)));

#endif

	return (0);
}

/**
 * katanaQp_calibrate_decr - ppc_md decrementer calibration callback 
 *
 * This routine determines and sets the value used to initialize decrementer 
 * interrupt frequency (used as system timer).
 */

static void __init katanaQp_calibrate_decr(void)
{
	ulong freq;

	freq = katanaQp_bus_freq(cpld_base) / 4;

	printk("time_init: decrementer frequency = %lu.%.6lu MHz\n",
	       freq / 1000000, freq % 1000000);

	tb_ticks_per_jiffy = freq / HZ;
	tb_to_us = mulhwu_scale_factor(freq, 1000000);
}

#ifdef CONFIG_SMP
char katanaQp_smp0[] = "SMP Message CPU0";
char katanaQp_smp1[] = "SMP Message CPU1";

static irqreturn_t
katanaQp_smp_cpu0_int_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	u32 doorbell, bit;

	/* read the doorbell interrupt value */
	doorbell = mv64x60_read(&bh, MV64360_CPU0_DOORBELL);

	/* ack the doorbell interrupt */
	mv64x60_write(&bh, MV64360_CPU0_DOORBELL_CLR, doorbell);

	//  printk("SMP CPU0 INT: 0x%02x\n", doorbell);

	for (bit = 0; bit < 4; bit++) {
		if (doorbell & (1 << bit)) {
			smp_message_recv(bit, regs);
		}
	}
	return (IRQ_HANDLED);
}

static irqreturn_t
katanaQp_smp_cpu1_int_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	u32 doorbell, bit;

	/* read the doorbell interrupt value */
	doorbell = mv64x60_read(&bh, MV64360_CPU1_DOORBELL);

	/* ack the doorbell interrupt */
	mv64x60_write(&bh, MV64360_CPU1_DOORBELL_CLR, doorbell);

	//printk("SMP CPU1 INT: 0x%02x\n", doorbell);

	for (bit = 0; bit < 4; bit++) {
		if (doorbell & (1 << bit)) {
			smp_message_recv(bit, regs);
		}
	}

	return (IRQ_HANDLED);
}

static void
smp_katanaQp_message_pass(int target, int msg, unsigned long data, int wait)
{
	if (msg > 0x3) {
		printk(KERN_ERR
		       "SMP %d: smp_message_pass() - unknown message %d\n",
		       smp_processor_id(), msg);
	}

	switch (target) {
	case 0:
		mv64x60_write(&bh, MV64360_CPU0_DOORBELL, 1 << msg);
		break;
	case 1:
		mv64x60_write(&bh, MV64360_CPU1_DOORBELL, 1 << msg);
		break;
	case MSG_ALL:
		mv64x60_write(&bh, MV64360_CPU0_DOORBELL, 1 << msg);
		mv64x60_write(&bh, MV64360_CPU1_DOORBELL, 1 << msg);
		break;
	case MSG_ALL_BUT_SELF:
		if (smp_processor_id()) {
			mv64x60_write(&bh, MV64360_CPU0_DOORBELL, 1 << msg);
		} else {
			mv64x60_write(&bh, MV64360_CPU1_DOORBELL, 1 << msg);
		}
		break;

	default:
		printk(KERN_ERR
		       "SMP %d: smp_message_pass() - unknown target 0x%04x\n",
		       smp_processor_id(), target);
		break;
	}
}
static int smp_katanaQp_probe(void)
{
	/* return number of CPUs, hard-coded to two for now */

	/* TODO: need register to determine number of installed CPUs */

	return (2);
}

static void smp_katanaQp_kick_cpu(int nr)
{
	/* release processor two from reset */
	out_8((volatile char *)(cpld_base + KATANAQP_CPLD_CPUE),
	      KATANAQP_CPLD_CPUE_CPUE);
	printk("done.\n");

	/* enable BR1 (bus request processor 1) */
	mv64x60_clr_bits(&bh, MV64x60_CPU_MASTER_CNTL, 0x200);
}
extern volatile unsigned long cpu_callin_map[NR_CPUS];

#define KATANAQP_SMP_CPU0_MSG_IRQ 60
#define KATANAQP_SMP_CPU1_MSG_IRQ 28

static void smp_katanaQp_setup_cpu(int cpu_nr)
{
	u32 hid1;

	/* enable HID1 Address Broadcast Enable */
	hid1 = mfspr(SPRN_HID1);
	hid1 |= HID1_ABE;
	mtspr(SPRN_HID1, hid1);

	if (cpu_nr == 0) {
		/* clear and enable all valid doorbell ints */
		mv64x60_write(&bh, MV64360_CPU0_DOORBELL_CLR, 0xf);
		mv64x60_write(&bh, MV64360_CPU0_DOORBELL_MASK, 0xf);
		request_irq(KATANAQP_SMP_CPU0_MSG_IRQ,
			    katanaQp_smp_cpu0_int_handler, SA_INTERRUPT,
			    katanaQp_smp0, 0);
	}

	if (cpu_nr == 1) {
		cpu_callin_map[cpu_nr] = 2;	/* TODO: verify this */

		/* clear and enable all valid doorbell ints */
		mv64x60_write(&bh, MV64360_CPU1_DOORBELL_CLR, 0xf);
		mv64x60_write(&bh, MV64360_CPU1_DOORBELL_MASK, 0xf);
		request_irq(KATANAQP_SMP_CPU1_MSG_IRQ,
			    katanaQp_smp_cpu1_int_handler, SA_INTERRUPT,
			    katanaQp_smp1, 0);
		_set_L2CR(L2CR_L2E | L2CR_L2PE);
	}
}

static struct smp_ops_t katanaQp_smp_ops = {
	smp_katanaQp_message_pass,
	smp_katanaQp_probe,
	smp_katanaQp_kick_cpu,
	smp_katanaQp_setup_cpu,
	.give_timebase = smp_generic_give_timebase,
	.take_timebase = smp_generic_take_timebase,
};
#endif				/* CONFIG_SMP */

#ifdef CONFIG_MTD_PHYSMAP

#ifndef MB
#define MB      (1 << 20)
#endif

/*
 * MTD Layout depends on amount of soldered FLASH in system. Sizes in MB.
 *
 * FLASH Amount:        128     64      32      16
 * -------------        ---     --      --      --
 * Monitor:             1       1       1       1
 * Primary Kernel:      1.5     1.5     1.5     1.5
 * Primary fs:          30      30      <end>   <end>
 * Secondary Kernel:    1.5     1.5     N/A     N/A
 * Secondary fs:        <end>   <end>   N/A     N/A
 * User:                <overlays entire FLASH except for "Monitor" section>
 */
static int __init
katanaQp_setup_mtd(void)
{
        u32     size;
        int     ptbl_entries;
        static struct mtd_partition     *ptbl;

        size = katanaQp_flash_size_0 + katanaQp_flash_size_1;
        if (!size)
                return -ENOMEM;

        ptbl_entries = (size >= (64*MB)) ? 6 : 4;

        if ((ptbl = kmalloc(ptbl_entries * sizeof(struct mtd_partition),
                GFP_KERNEL)) == NULL) {

                printk(KERN_WARNING "Can't alloc MTD partition table\n");
                return -ENOMEM;
        }
        memset(ptbl, 0, ptbl_entries * sizeof(struct mtd_partition));

        ptbl[0].name = "Monitor";
        ptbl[0].size = KATANAQP_MTD_MONITOR_SIZE;
        ptbl[1].name = "Primary Kernel";
        ptbl[1].offset = MTDPART_OFS_NXTBLK;
        ptbl[1].size = 0x00180000; /* 1.5 MB */
        ptbl[2].name = "Primary Filesystem";
        ptbl[2].offset = MTDPART_OFS_APPEND;
        ptbl[2].size = MTDPART_SIZ_FULL; /* Correct for 16 & 32 MB */
        ptbl[ptbl_entries-1].name = "User FLASH";
        ptbl[ptbl_entries-1].offset = KATANAQP_MTD_MONITOR_SIZE;
        ptbl[ptbl_entries-1].size = MTDPART_SIZ_FULL;

        if (size >= (64*MB)) {
                ptbl[2].size = 30*MB;
                ptbl[3].name = "Secondary Kernel";
                ptbl[3].offset = MTDPART_OFS_NXTBLK;
                ptbl[3].size = 0x00180000; /* 1.5 MB */
                ptbl[4].name = "Secondary Filesystem";
                ptbl[4].offset = MTDPART_OFS_APPEND;
                ptbl[4].size = MTDPART_SIZ_FULL;
        }

        physmap_map.size = size;
        physmap_set_partitions(ptbl, ptbl_entries);
        return 0;
}

arch_initcall(katanaQp_setup_mtd);
#endif


static int __init
katanaQp_rtc_init(void)
{
	struct timespec tv;

	ppc_md.set_rtc_time = m41t00_set_rtc_time;
	ppc_md.get_rtc_time = m41t00_get_rtc_time;

	tv.tv_nsec = 0;
	tv.tv_sec = (ppc_md.get_rtc_time)();
	do_settimeofday(&tv);

	return 0;
}

late_initcall(katanaQp_rtc_init);

static int __init
katanaQp_fixup_eth_mtu(void)
{
	/* Fixup to clear Ethernet MTU register
	 *
	 * This register is also cleared in the boot firmware if
         * an Ethernet operation (such as TFTP) is performed.
	 *
	 * Previous versions of the Marvell Gig Ethernet driver did 
	 * clear this register; ultimately the newer driver may as
	 * well.  Until that is the case, this workaround is necessary.  
	 */

	mv64x60_write(&bh,  MV643XX_ETH_MAXIMUM_TRANSMIT_UNIT(0), 0);
	mv64x60_write(&bh,  MV643XX_ETH_MAXIMUM_TRANSMIT_UNIT(1), 0);
	mv64x60_write(&bh,  MV643XX_ETH_MAXIMUM_TRANSMIT_UNIT(2), 0);
	return 0;
}

late_initcall(katanaQp_fixup_eth_mtu);

/**
 * platform_init - early platform-specific initialization callback
 *
 * \param r3 value of r3 at bootup
 * \param r4 value of r4 at bootup
 * \param r5 value of r5 at bootup
 * \param r6 value of r6 at bootup
 * \param r7 value of r7 at bootup
 *
 * This routine is the main entry point for configuring board-specific
 * machine callbacks.  It also is responsible for copying the kernel
 * command line and any other board information passed in from the 
 * monitor.
 */

void __init platform_init(unsigned long r3,
			  unsigned long r4,
			  unsigned long r5, unsigned long r6, unsigned long r7)
{
	parse_bootinfo(find_bootinfo());

	/* ASSUMPTION:  If both r3 (bd_t pointer) and r6 (cmdline pointer)
	 * are non-zero, then we should use the board info from the bd_t
	 * structure and the cmdline pointed to by r6 instead of the
	 * information from birecs, if any.  Otherwise, use the information
	 * from birecs as discovered by the preceeding call to
	 * parse_bootinfo().  This rule should work with both PPCBoot, which
	 * uses a bd_t board info structure, and the kernel boot wrapper,
	 * which uses birecs.
	 */
	if (r3 && r6) {
		/* copy board info structure */
		memcpy((void *)__res, (void *)(r3 + KERNELBASE), sizeof(bd_t));
		/* copy command line */
		*(char *)(r7 + KERNELBASE) = 0;
		strcpy(cmd_line, (char *)(r6 + KERNELBASE));
	}

	isa_mem_base = 0;

	ppc_md.setup_arch = katanaQp_setup_arch;
	ppc_md.show_cpuinfo = katanaQp_show_cpuinfo;
	ppc_md.init_IRQ = mv64360_init_irq;
	ppc_md.get_irq = mv64360_get_irq;
	ppc_md.restart = katanaQp_restart;
	ppc_md.power_off = katanaQp_power_off;
	ppc_md.halt = katanaQp_halt;
	ppc_md.find_end_of_memory = katanaQp_find_end_of_memory;
	ppc_md.calibrate_decr = katanaQp_calibrate_decr;

#if defined(CONFIG_SERIAL_TEXT_DEBUG) && defined(CONFIG_SERIAL_MPSC_CONSOLE)
	ppc_md.setup_io_mappings = katanaQp_map_io;
	ppc_md.progress = mv64x60_mpsc_progress;
	mv64x60_progress_init(KATANAQP_BRIDGE_REG_BASE);
#endif

#if defined(CONFIG_SERIAL_MPSC) || defined(CONFIG_MV643XX_ETH)
	platform_notify = katanaQp_platform_notify;
#endif

#ifdef CONFIG_SMP
	ppc_md.smp_ops = &katanaQp_smp_ops;
#endif				/* CONFIG_SMP */

	katanaQp_set_bat();
}
