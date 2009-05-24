/*
 * arch/ppc/platforms/ppmc280_setup.c
 *
 * Board setup routines for the Force PPMC280 Development Board.
 *
 * Based on ppmc280.c Written by Rabeeh Khoury - rabeeh@galileo.co.il
 * 
 * Based on code done by - Mark A. Greer <mgreer@mvista.com>
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
#include <linux/types.h>
#include <linux/major.h>
#include <linux/initrd.h>
#include <linux/blkdev.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/ide.h>
#include <linux/seq_file.h>
#include <linux/smp.h>
#include <linux/root_dev.h>
#include <linux/bcd.h>
#include <linux/mv643xx.h>
#if !defined(CONFIG_SERIAL_MPSC_CONSOLE)
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/serial_core.h>
#endif

#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/hardirq.h>
#include <asm/time.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/prom.h>
#include <asm/smp.h>
#include <asm/todc.h>
#include <asm/bootinfo.h>
#include <asm/mv64x60.h>

#include <platforms/ppmc280.h>

#undef DEBUG_SRAM_ALLOCATION
u_char ppmc280_spd[3] = { 0xFF, 0xFF, 0xFF } ;

struct mv64x60_handle bh;

EXPORT_SYMBOL(ppmc280_spd);

extern char cmd_line[];
unsigned long ppmc280_find_end_of_memory(void);
extern uint i2c_word_read(u_char, uint, unsigned int, u_char *);

#define _256K           0x00040000
#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000

/*
 * Marvell DB-64360-BP Evaluation Board PCI interrupt routing.
 */
static int __init
ppmc280_map_irq(struct pci_dev *dev, unsigned char idsel, unsigned char pin)
{
  struct pci_controller	*hose = pci_bus_to_hose(dev->bus->number);
	
  if (hose->index == 0) {
    /* TODO : Add later, Interrupt lines for B,C,D */
    static char pci_irq_table[][4] =
      /*
       *	PCI IDSEL/INTPIN->INTLINE 
       * 	   A   B   C   D
       */
	
      {
	/* IDSEL 1 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 2 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 3 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 4 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 5 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 6 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 7 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 8 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 9 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 10 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D,
	 PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B},
	/* IDSEL 11 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 12 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 13 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ_B, PPMC280_PCI_0_IRQ_C,
	 PPMC280_PCI_0_IRQ_D, PPMC280_PCI_0_IRQ},
	/* IDSEL 14 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 15 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 16 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 17 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 18 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 19 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 20 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 21 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 22 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 23 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 24 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 25 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 26 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D,
	 PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B},
	/* IDSEL 27 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ_D, PPMC280_PCI_0_IRQ,
	 PPMC280_PCI_0_IRQ_B, PPMC280_PCI_0_IRQ_C},
	/* IDSEL 28 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B,
	 PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D},
	/* IDSEL 29 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ_B, PPMC280_PCI_0_IRQ_C,
	 PPMC280_PCI_0_IRQ_D, PPMC280_PCI_0_IRQ},
	/* IDSEL 30 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ_C, PPMC280_PCI_0_IRQ_D,
	 PPMC280_PCI_0_IRQ, PPMC280_PCI_0_IRQ_B},
	/* IDSEL 31 - PCI bus 0 */
	{PPMC280_PCI_0_IRQ_D, PPMC280_PCI_0_IRQ,
	 PPMC280_PCI_0_IRQ_B, PPMC280_PCI_0_IRQ_C}
      };
	
    const long min_idsel = 1, max_idsel = 31, irqs_per_slot = 4;
    return PCI_IRQ_TABLE_LOOKUP;
  }
  else {
    /* delete/report error, No device can be connected 
       on 2nd PCI controller ? */
    static char pci_irq_table[][4] =
      /*
       *	PCI IDSEL/INTPIN->INTLINE 
       * 	   A   B   C   D
       */
      {
	{ PPMC280_PCI_1_IRQ, 0, 0, 0 },/* IDSEL 7 - PCI bus 1 */
      };
	
    const long min_idsel = 7, max_idsel = 9, irqs_per_slot = 4;
    return PCI_IRQ_TABLE_LOOKUP;
  }
}

#if 0
static void __init ppmc280_pcibios_fixup(void)
{
  struct pci_dev *dev = NULL;
  u16 status;
  u8 id, next, current, bus;
  u32 val;
  return;
  /* 
   * go over scanned PCI devices and modify their 
   * PCI-X capability list, if valid.
   * (Code done by Mordi Blaunstein)
   */

  /* First update MV64360 PCI-X capability */
  for (bus = 0; bus < 1 /* num_of_buses */ ; bus++) {
    val = mv64360_hw_read_config_dword(bus, 0, 0, 0, 0x60, 0);
    if ((val & 0xff) == 0x07) {
      mv64360_hw_write_config_dword(bus, 0, 0, 0, 0x60, 0,
				    (0x0000 << 16) | (val &
						      0x0000ffff));
    }
  }
  while ((dev = pci_find_device(PCI_ANY_ID, PCI_ANY_ID, dev)) != NULL) {
    /* Update all devices cache line size to be 32 bytes ! */
    pci_write_config_byte(dev, 0xc, L1_CACHE_LINE_SIZE >> 2);

    /* Mordi - PCI-X capabiltiies List initialization */
    pci_read_config_word(dev, PCI_STATUS, &status);
    /* Check if capabilty pointer is located at offset PCI_CAPABILITY_LIST */
    if ((status & BIT4) != 0) {
      pci_read_config_byte(dev, PCI_CAPABILITY_LIST, &next);
      do {
	current = next;
	pci_read_config_byte(dev, current, &id);
	if (id == 0x7) {
	  pci_read_config_dword(dev, current,
				&val);
	  pci_write_config_word(dev,
				(current + 0x2),
				0x0000);
	} else if (id > 0x7) {
	  printk (KERN_ERR
		 "mv64360_pcibios_fixup: ERROR Capabilty ID %x on device # %x\n",
		 id, dev->devfn);
	}
	pci_read_config_byte(dev, (current + 0x1),
			     &next);
      } while (next != 0x0);
    }
  }
}

#endif

/* The extra values or'd in in the original patch were zeros anyway */

#define MV64360_PCI_ACC_CNTL_CACHE_COHERENT \
	MV64360_PCI_ACC_CNTL_ENABLE | \
	MV64360_PCI_ACC_CNTL_SNOOP_WB | \
	MV64360_PCI_ACC_CNTL_MBURST_32_BYTES | \
	MV64360_PCI_ACC_CNTL_RDSIZE_128_BYTES

#define MV64360_PCI_ACC_CNTL_NONE_CACHE_COHERENT \
	MV64360_PCI_ACC_CNTL_ENABLE | \
	MV64360_PCI_ACC_CNTL_MBURST_128_BYTES | \
	MV64360_PCI_ACC_CNTL_RDSIZE_256_BYTES

static void __init
ppmc280_setup_bridge(void)
{
  struct mv64x60_setup_info si;
  int i;
	
  if (ppc_md.progress)
    ppc_md.progress("ppmc280_setup_bridge: enter", 0);

  memset(&si, 0, sizeof(si));

  si.phys_reg_base = CONFIG_MV64X60_NEW_BASE;

  si.pci_0.enable_bus = 1;
  si.pci_0.pci_io.cpu_base = PPMC280_PCI_0_IO_START_PROC;
  si.pci_0.pci_io.pci_base_hi = 0;
  si.pci_0.pci_io.pci_base_lo = PPMC280_PCI_0_IO_START;
  si.pci_0.pci_io.size = PPMC280_PCI_0_IO_SIZE;
  si.pci_0.pci_io.swap = MV64x60_CPU2PCI_SWAP_NONE;
  si.pci_0.pci_mem[0].cpu_base = PPMC280_PCI_0_MEM_START_PROC;
  si.pci_0.pci_mem[0].pci_base_hi = 0;
  si.pci_0.pci_mem[0].pci_base_lo = PPMC280_PCI_0_MEM_START;
  si.pci_0.pci_mem[0].size = PPMC280_PCI_0_MEM_SIZE;
  si.pci_0.pci_mem[0].swap = MV64x60_CPU2PCI_SWAP_NONE;
  si.pci_0.pci_cmd_bits = 0;
  si.pci_0.latency_timer = 0x80;

  si.pci_1.enable_bus = 1;
  si.pci_1.pci_io.cpu_base = PPMC280_PCI_1_IO_START_PROC;
  si.pci_1.pci_io.pci_base_hi = 0;
  si.pci_1.pci_io.pci_base_lo = PPMC280_PCI_1_IO_START;
  si.pci_1.pci_io.size = PPMC280_PCI_1_IO_SIZE;
  si.pci_1.pci_io.swap = MV64x60_CPU2PCI_SWAP_NONE;
  si.pci_1.pci_mem[0].cpu_base = PPMC280_PCI_1_MEM_START_PROC;
  si.pci_1.pci_mem[0].pci_base_hi = 0;
  si.pci_1.pci_mem[0].pci_base_lo = PPMC280_PCI_1_MEM_START;
  si.pci_1.pci_mem[0].size = PPMC280_PCI_1_MEM_SIZE;
  si.pci_1.pci_mem[0].swap = MV64x60_CPU2PCI_SWAP_NONE;
  si.pci_1.pci_cmd_bits = 0;
  si.pci_1.latency_timer = 0x80;

  for (i=0; i<MV64x60_CPU2MEM_WINDOWS; i++) {
  	si.cpu_prot_options[i] = 0;
  	si.enet_options[i] = MV64360_ENET2MEM_SNOOP_NONE; /* errata */
  	si.mpsc_options[i] = MV64360_MPSC2MEM_SNOOP_NONE; /* errata */
  	si.idma_options[i] = MV64360_IDMA2MEM_SNOOP_NONE; /* errata */

#ifdef CONFIG_MV64360_NOT_COHERENT_CACHE
  	si.pci_0.acc_cntl_options[i] = MV64360_PCI_ACC_CNTL_NONE_CACHE_COHERENT;
//  	si.pci_1.acc_cntl_options[i] = MV64360_PCI_ACC_CNTL_NONE_CACHE_COHERENT;
#else
  	si.pci_0.acc_cntl_options[i] = MV64360_PCI_ACC_CNTL_CACHE_COHERENT;
// 	si.pci_1.acc_cntl_options[i] = MV64360_PCI_ACC_CNTL_CACHE_COHERENT;
#endif
  }

  /* Lookup PCI host bridges */
  i = mv64x60_init(&bh, &si);
  if (i)
  	printk (KERN_WARNING "Bridge initialization failed.\n");

  pci_dram_offset = 0;
  ppc_md.pci_swizzle = common_swizzle;
  ppc_md.pci_map_irq = ppmc280_map_irq;
  ppc_md.pci_exclude_device = mv64x60_pci_exclude_device;

  if (ppc_md.progress)
    ppc_md.progress("ppmc280_setup_bridge: exit", 0);

  return;
}
	
void __init ppmc280_setup_peripherals(void)
{

  u32 val;
  u32 temp;

  if (ppc_md.progress)
    ppc_md.progress("ppmc280_setup_periph: enter", 0);

  /*
   * Enabling of PCI internal-vs-external arbitration
   * is a platform- and errata-dependent decision.
   */
#ifdef CONFIG_SMP
  /* RABEEH - TBD - check if the following really works */
  if (!(mv64x60_read(&bh, MV64x60_CPU_MASTER_CNTL) & BIT9)) {
    printk (KERN_ERR "Error - Firmware has already enabled secondary CPU\n");
  }
#else
  mv64x60_set_bits(&bh, MV64x60_CPU_MASTER_CNTL, BIT9); /* Only 1 cpu */
#endif
	
  /*
   * Set up windows for boot FLASH (using boot CS window),
   * and for the USER FLASH on the device module.
   *
   * Assumes that DINK has set up the Device/Boot Bank Param regs.
   */

  /*
   * Don't map boot flash, or user flash just to be safe; esp. 
   * important for MOTload, as most of it is in user flash.
   */
#ifdef CONFIG_PPMC280_SAFE_FLASH
#undef WINDOW_BOOT_FLASH
#undef WINDOW_USER_FLASH
#else
#define WINDOW_BOOT_FLASH
#define WINDOW_USER_FLASH
#endif

#ifdef WINDOW_BOOT_FLASH
  mv64x60_set_32bit_window(&bh, MV64x60_CPU2BOOT_WIN,
     PPMC280_BOOT_FLASH_BASE, PPMC280_BOOT_FLASH_SIZE, 0);
  bh.ci->enable_window_32bit(&bh, MV64x60_CPU2BOOT_WIN);
#endif

#ifdef WINDOW_USER_FLASH
  mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_0_WIN,
     PPMC280_EXT_FLASH_BASE_1, PPMC280_EXT_FLASH_SIZE, 0);
  bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_0_WIN);

  mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_1_WIN,
     PPMC280_EXT_FLASH_BASE_2, PPMC280_EXT_FLASH_SIZE, 0);
  bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_1_WIN);
#endif

  /* Set up window for internal sram (256KByte winsize) */
  mv64x60_set_32bit_window(&bh, MV64x60_CPU2SRAM_WIN,
    PPMC280_INTERNAL_SRAM_BASE, _256K, 0);
    bh.ci->enable_window_32bit(&bh, MV64x60_CPU2SRAM_WIN);

  /* 
   * Configure internal SRAM - 
   * Cache coherent write back, incase CONFIG_MV64360_SRAM_CACHE_COHERENT set
   * Parity enabled.
   * Parity error propagation
   * Arbitration not parked for CPU only
   * Other bits are reserved.
   */
#ifdef CONFIG_MV64360_SRAM_CACHE_COHERENT
  mv64x60_write(&bh, MV64360_SRAM_CONFIG, 0x001600b2);
#else
  mv64x60_write(&bh, MV64360_SRAM_CONFIG, 0x001600b0);
#endif

  /* Enumerate PCI Bus */

  mv64x60_set_bus(&bh, 0, 0);
  bh.hose_a->first_busno = 0;
  bh.hose_a->last_busno = 0xff;
  bh.hose_a->last_busno = pciauto_bus_scan(bh.hose_a, 0);

  bh.hose_b->first_busno = bh.hose_a->last_busno + 1;
  mv64x60_set_bus(&bh, 1, bh.hose_b->first_busno);
  bh.hose_b->last_busno = 0xff;
  bh.hose_b->last_busno = pciauto_bus_scan(bh.hose_b, bh.hose_b->first_busno);

  /* 
   * Setting the SRAM to 0. Note that this generates parity errors on internal
   * data path in SRAM since it's first time accessing it while after reset
   * it's not configured
   */

  memset((void*)PPMC280_INTERNAL_SRAM_BASE, 0, PPMC280_INTERNAL_SRAM_SIZE);

  /* Set MPP Control0: MPPSel0 to TxD0, MPPSel1 to RxD0, MPPSel3 to TxD1, MPPSel5 to RxD1 */
  temp = mv64x60_read(&bh, MV64x60_MPP_CNTL_0);
  temp |= 0x00303000;
  mv64x60_write(&bh, MV64x60_MPP_CNTL_0, temp);

#if 0 /* Rabeeh - must be debugged for PMON2000 */
  /* This code is taken from Dink32 */
  /* Enable PCI internal arbitter */
#if 1
  temp = mv64x60_read(&bh, MV64x60_MPP_CNTL_1);
  temp = temp & 0x00ffff00;
  temp = temp | 0x11000011;
  mv64x60_write(&bh, MV64x60_MPP_CNTL_1, temp);

  temp = mv64x60_read(&bh, MV64x60_MPP_CNTL_2);
  temp = temp & 0xffff0000;
  temp = temp | 0x00001111;
  mv64x60_write(&bh, MV64x60_MPP_CNTL_1, temp);

  mv64x60_set_bits(&bh, MV64x60_GPP_LEVEL_CNTL, BIT8 | BIT10 | BIT14 | BIT15 |
		   BIT16 | BIT17 | BIT18 | BIT19);
  mv64x60_clr_bits(&bh, MV64x60_GPP_IO_CNTL, BIT8 | BIT10 | BIT14 | BIT15 |
		     BIT16 | BIT17 | BIT18 | BIT19);

  //	mv64x60_write(&bh, MV64x60_GPP_LEVEL_CNTL, 0xffffffff);
  //	mv64x60_write(&bh, MV64x60_MPP_CNTL_0, 0x02222222);
  //	mv64x60_write(&bh, MV64x60_MPP_CNTL_2, 0x00431111); /* This is tricky */
  //	mv64x60_write(&bh, MV64x60_MPP_CNTL_3, 0x00000044);
  //	mv64x60_write(&bh, MV64x60_MPP_IO_CONTROL, 0x00000000);
#else
  mv64x60_write(&bh, MV64x60_GPP_LEVEL_CNTL, 0xffffffff);
  mv64x60_write(&bh, MV64x60_MPP_CNTL_0, 0x02222222);
  mv64x60_write(&bh, MV64x60_MPP_CNTL_1, 0x11333011);
  mv64x60_write(&bh, MV64x60_MPP_CNTL_2, 0x00431111); /* This is tricky */
  mv64x60_write(&bh, MV64x60_MPP_CNTL_3, 0x00000044);
  mv64x60_write(&bh, MV64x60_GPP_IO_CNTL, 0x00000000);
#endif

  /* Enable PCI internal arbitter */
  mv64x60_write(&bh, MV64x60_PCI0_ARBITER_CNTL, BIT31);
  mv64x60_write(&bh, MV64x60_PCI1_ARBITER_CNTL, BIT31);
#endif


  /*
   * Configure the following MPP pins to indicate a low level
   * triggered interrupt
   *
   * PCI 0 IntA intB intC intD to MPP 27 29 16 17
   * ETH PHY0 12 PHY1 13
   */

  /* Enable only MPP 27 for now */

  /* MPP 27,29 */
  mv64x60_clr_bits(&bh, MV64x60_MPP_CNTL_3, BIT12 | BIT13 | BIT14 | BIT15 |     /* MPP 27 */
		    BIT20 | BIT21 | BIT22 | BIT23);   /* MPP 29 */
  mv64x60_clr_bits(&bh, MV64x60_MPP_CNTL_2, BIT0 | BIT1 | BIT2 | BIT3 | /*MPP 16 */
		    BIT4 | BIT5 | BIT6 | BIT7);       /* MPP 17 */
  mv64x60_clr_bits(&bh, MV64x60_MPP_CNTL_1, BIT16 | BIT17 | BIT18 | BIT19 |     /* MPP 12 */
		    BIT20 | BIT21 | BIT22 | BIT23);   /* MPP 13 */

  /*
   * Define GPP 27 interrupt polarity as active low
   * input signal and level triggered
   */
  mv64x60_set_bits(&bh, MV64x60_GPP_LEVEL_CNTL,
		  BIT27 | BIT29 | BIT16 | BIT17 | BIT12 | BIT13);
  mv64x60_clr_bits(&bh, MV64x60_GPP_IO_CNTL,
		    BIT27 | BIT29 | BIT16 | BIT17 | BIT12 | BIT13);

  /* Config GPP interrupt controller to respond to level trigger */
  mv64x60_set_bits(&bh, MV64x60_COMM_ARBITER_CNTL, BIT10);


  /*
   * Dismiss and then enable interrupt on GPP interrupt cause for CPU #0
   */
  mv64x60_write(&bh, MV64x60_GPP_INTR_CAUSE, ~(BIT27 | BIT26 | BIT25));
  mv64x60_set_bits(&bh, MV64x60_GPP_INTR_MASK, BIT27 | BIT26 | BIT25);

  /*
   * Dismiss and then enable interrupt on CPU #0 high cause register
   * BIT25 summarizes GPP interrupts 8-15 (Need MPP 12,13)
   * BIT26 summarizes GPP interrupts 16-23 (Need MPP 16,17)
   * BIT27 summarizes GPP interrupts 24-31 (Need MPP 27,29)
   */
  mv64x60_set_bits(&bh, MV64360_IC_CPU0_INTR_MASK_HI,
		  BIT27 | BIT26 | BIT25);

  mv64x60_set_bits(&bh, MV64360_IC_CPU0_INTR_MASK_HI,
		  BIT27 | BIT26 | BIT25);

  /*
   * Change DRAM read buffer assignment.
   * Assign read buffer 0 dedicated only for CPU, and the rest read buffer 1.
   */
  val = mv64x60_read(&bh, MV64360_SDRAM_CONFIG);
  val = val & 0x03ffffff;
  val = val | 0xf8000000;
  mv64x60_write(&bh, MV64360_SDRAM_CONFIG, val);

  /* Set GPP 23,26 as o/p  active high for Debug LEDS */
  mv64x60_clr_bits(&bh, MV64x60_GPP_LEVEL_CNTL, BIT23 | BIT26);
  mv64x60_set_bits(&bh, MV64x60_GPP_IO_CNTL, BIT23 | BIT26);

  /* Program WatchDog o/p MPP19:WDE# i.e. 0x4, MPP24:WDNMI# i.e. 0x4 */

  val = mv64x60_read(&bh, MV64x60_MPP_CNTL_2);
  val &= 0xFFFF0FFF;
  val |= 0x00004000;
  mv64x60_write(&bh, MV64x60_MPP_CNTL_2, val);

  val = mv64x60_read(&bh, MV64x60_MPP_CNTL_3);
  val &= 0xFFFFFFF0;
  val |= 0x00000004;
  mv64x60_write(&bh, MV64x60_MPP_CNTL_3, val);

  /* Make sure WatchDog is disabled */
  temp = mv64x60_read(&bh, MV64x60_WDT_WDC);
  if (temp & 0x80000000) {    /* Is the WD enabled ? */
    temp &= ~0x3000000;
    temp |= 0x1000000;
    mv64x60_write(&bh, MV64x60_WDT_WDC, temp);
    temp &= ~0x3000000;
    temp |= 0x2000000;
    mv64x60_write(&bh, MV64x60_WDT_WDC, temp);
  }

  if (ppc_md.progress)
    ppc_md.progress("ppmc280_setup_periph: exit", 0);

  return;

}


static void __init
ppmc280_setup_arch(void)
{
  u32 val;

  if ( ppc_md.progress )
    ppc_md.progress("ppmc280_setup_arch: enter", 0);

  loops_per_jiffy = 500000000 / HZ;

#ifdef CONFIG_BLK_DEV_INITRD
  if (initrd_start)
    ROOT_DEV = Root_RAM0;
  else
#endif
#ifdef	CONFIG_ROOT_NFS
    ROOT_DEV = Root_NFS;
#else
  ROOT_DEV = Root_SDA2;
#endif
#if 1
  /* enable L2 cache for CPU 0 */
  val = _get_L2CR();
  val |= L2CR_L2E;
  _set_L2CR(val);
#endif
  if (ppc_md.progress)
    ppc_md.progress("ppmc280_setup_arch: find_bridges", 0);

  printk("Force PPMC280 Development Board\n");

  /* NB: bh is not initialized until you get here */
  ppmc280_setup_bridge();

  /* Set all GPP IOs to input */
  mv64x60_write(&bh, MV64x60_GPP_IO_CNTL, 0x00000000);

  /* i2c_word_read uses bh */
  i2c_word_read(0xA4, 0x1FE0, 3, ppmc280_spd);

  ppmc280_setup_peripherals();

  printk( "Board Version: %c/%s%c.%c\n",
	  ((ppmc280_spd[2] & 0xF) < 7) ? 'A' + (ppmc280_spd[2] & 0x7) : 'X',
	  (0x0 == (ppmc280_spd[0] & 0x7)) ? "E" : "",
	  (0x7 == (ppmc280_spd[0] & 0x7)) ? '0' :
	  ((0x0 == (ppmc280_spd[0] & 0x7)) ? '0' :
	   ('A' + (ppmc280_spd[0] & 0x7))),
	  (0x7 == ((ppmc280_spd[0] >> 3) & 0x7)) ? 0 :
	  '0' + ((ppmc280_spd[0] >> 3) & 0x7));
    
#ifdef	CONFIG_DUMMY_CONSOLE
  conswitchp = &dummy_con;
#endif

  if ( ppc_md.progress )
    ppc_md.progress("ppmc280_setup_arch: exit", 0);

  return;
}
#if defined(CONFIG_SERIAL_MPSC)
static void __init
ppmc280_fixup_mpsc_pdata(struct platform_device *pdev)
{
        struct mpsc_pdata *pdata;

        pdata = (struct mpsc_pdata *)pdev->dev.platform_data;

        pdata->max_idle = 40;
        pdata->default_baud = PPMC280_DEFAULT_BAUD;
        pdata->brg_clk_src = PPMC280_MPSC_CLK_SRC;
        pdata->brg_clk_freq = PPMC280_MPSC_CLK_FREQ;
}
#endif

#if defined(CONFIG_MV643XX_ETH)
static void __init
ppmc280_fixup_eth_pdata(struct platform_device *pdev)
{
	struct mv643xx_eth_platform_data *eth_pd;
	static u16 phy_addr[] = {
		PPMC280_ETH0_PHY_ADDR,
		PPMC280_ETH1_PHY_ADDR,
		PPMC280_ETH2_PHY_ADDR,
	};

	eth_pd = pdev->dev.platform_data;
	eth_pd->force_phy_addr = 1;
	eth_pd->phy_addr = phy_addr[pdev->id];
	eth_pd->tx_queue_size = PPMC280_ETH_TX_QUEUE_SIZE;
	eth_pd->rx_queue_size = PPMC280_ETH_RX_QUEUE_SIZE;

#define PPMC280_MV643XXX_PSCR \
  MV643XX_ETH_FORCE_LINK_PASS      |       \
  MV643XX_ETH_ENABLE_AUTO_NEG_FOR_DUPLX   |       \
  MV643XX_ETH_DISABLE_AUTO_NEG_FOR_FLOW_CTRL |    \
  MV643XX_ETH_ADV_SYMMETRIC_FLOW_CTRL     |       \
  MV643XX_ETH_FORCE_FC_MODE_NO_PAUSE_DIS_TX |     \
  MV643XX_ETH_FORCE_BP_MODE_NO_JAM        |       \
  (1<<9)  /* reserved */                  |       \
  MV643XX_ETH_DO_NOT_FORCE_LINK_FAIL      |       \
  MV643XX_ETH_RETRANSMIT_16_ATTEMPTS      |       \
  MV643XX_ETH_ENABLE_AUTO_NEG_SPEED_GMII  |       \
  MV643XX_ETH_DTE_ADV_0                   |       \
  MV643XX_ETH_DISABLE_AUTO_NEG_BYPASS     |       \
  MV643XX_ETH_AUTO_NEG_NO_CHANGE          |       \
  MV643XX_ETH_MAX_RX_PACKET_9700BYTE      |       \
  MV643XX_ETH_CLR_EXT_LOOPBACK            |       \
  MV643XX_ETH_SET_FULL_DUPLEX_MODE        |       \
  MV643XX_ETH_ENABLE_FLOW_CTRL_TX_RX_IN_FULL_DUPLEX

	eth_pd->port_serial_control = PPMC280_MV643XXX_PSCR;
}
#endif

static int __init
ppmc280_platform_notify(struct device *dev)
{
	static struct {
		char	*bus_id;
		void	((*rtn)(struct platform_device *pdev));
	} dev_map[] = {
#if defined(CONFIG_SERIAL_MPSC)
		{ MPSC_CTLR_NAME ".0", ppmc280_fixup_mpsc_pdata },
		{ MPSC_CTLR_NAME ".1", ppmc280_fixup_mpsc_pdata },
#endif

/* Note that only two of the three have BCM5421 PHY attached */
#if defined(CONFIG_MV643XX_ETH)
		{ MV643XX_ETH_NAME ".0", ppmc280_fixup_eth_pdata },
		{ MV643XX_ETH_NAME ".1", ppmc280_fixup_eth_pdata },
		{ MV643XX_ETH_NAME ".2", ppmc280_fixup_eth_pdata },
#endif
	};
	struct platform_device	*pdev;
	int	i;

	if (dev && dev->bus_id)
		for (i=0; i<ARRAY_SIZE(dev_map); i++)
			if (!strncmp(dev->bus_id, dev_map[i].bus_id,
				BUS_ID_SIZE)) {

				pdev = container_of(dev,
					struct platform_device, dev);
				dev_map[i].rtn(pdev);
			}

	return 0;
}

/*
 * Set BAT 3 to map 0xf0000000 to end of physical memory space.
 */
static __inline__ void
ppmc280_set_bat(void)
{
  unsigned long   bat3u, bat3l;
  static int	mapping_set = 0;

  if (!mapping_set) {

    __asm__ __volatile__(
			 " lis %0,0xf000\n \
		  ori %1,%0,0x002a\n \
		  ori %0,%0,0x1ffe\n \
		  mtspr 0x21e,%0\n \
		  mtspr 0x21f,%1\n \
		  isync\n \
		  sync ":"=r" (bat3u), "=r" (bat3l));

    mapping_set = 1;
  }

  return;
}

#ifdef CONFIG_SPECIAL_CONSISTENT_MEMORY

#define TOTAL_MEMORY            _256K /* 256KByte */
#define CHUNK_SIZE              256 /* 256Bytes */
#define CHUNK_BITS              8 /* 2^8 = 256 bytes */
#define TOTAL_MEMORY_ENTRIES    TOTAL_MEMORY / CHUNK_SIZE /* 256/4=64 entries */

char sram_bitmap[TOTAL_MEMORY_ENTRIES];

static spinlock_t       sram_alloc_lock ;


/* pci_alloc_consistent / pci_free_consistent takes memory from internal SRAM */

static void 
ppmc280_pci_consistent_init (void) {
  int i;
    
  for( i = 0; i < TOTAL_MEMORY_ENTRIES; i++)
    sram_bitmap[i] = 0;
    
  spin_lock_init (&sram_alloc_lock);
}


static void * 
ppmc280_pci_alloc_consistent(struct pci_dev *dev, size_t size,
			     dma_addr_t *dma_addr) 
{
  unsigned int len = size;
  unsigned int chunks;
  unsigned long flags;
  int i,j;
  static int did_initialize = 0;

  if (did_initialize == 0) {
    ppmc280_pci_consistent_init();
    did_initialize = 1;
  }
	
  len += (1 << CHUNK_BITS) - 1;
	
  chunks = len >> CHUNK_BITS;
	
  spin_lock_irqsave(&sram_alloc_lock, flags);
  for (j = 0 ; j <= (TOTAL_MEMORY_ENTRIES - chunks) ; j++) {
    for (i = j; i < (j + chunks) ; i++) {
      if (sram_bitmap[i] == 1) break;
    }
    if (i < (j + chunks)) continue;

    for (i = j; i < (j + chunks) ; i++) {
      sram_bitmap[i] = 1;
    }
    memset (PPMC280_INTERNAL_SRAM_BASE + CHUNK_SIZE * j, 0,chunks * CHUNK_SIZE);
    *dma_addr = (dma_addr_t)PPMC280_INTERNAL_SRAM_BASE + CHUNK_SIZE * j;
    spin_unlock_irqrestore(&sram_alloc_lock, flags);
#ifdef DEBUG_SRAM_ALLOCATION
    printk ("Allocating SRAM area on %08x (%08x in size)\n",
	    (PPMC280_INTERNAL_SRAM_BASE + CHUNK_SIZE * j), chunks * CHUNK_SIZE);
#endif
    return (void *)(PPMC280_INTERNAL_SRAM_BASE + CHUNK_SIZE * j);
  }
  printk (KERN_ERR "ppmc280_pci_alloc_consistent: Failed to allocate consistent memory on sram\n");
  spin_unlock_irqrestore(&sram_alloc_lock, flags);
  return 0;
}

static void 
ppmc280_pci_free_consistent(struct pci_dev *dev, size_t size,
			     void *vaddr, dma_addr_t dma_addr) {
  unsigned int chunks;
  unsigned int addr = (unsigned int) vaddr - PPMC280_INTERNAL_SRAM_BASE;
  int i;
  unsigned int len = size;
	
  if (size > PPMC280_INTERNAL_SRAM_SIZE) {
    printk (KERN_ERR "ppmc280_pci_free_consistent: Error in freeing consistent memory\n");
    return;
  }

  if (((unsigned int)vaddr < PPMC280_INTERNAL_SRAM_BASE) ||
      ((unsigned int)vaddr > (PPMC280_INTERNAL_SRAM_BASE +
			      PPMC280_INTERNAL_SRAM_SIZE))) {
    printk (KERN_ERR "ppmc280_pci_free_consistent: Address out of range\n");
    return;
  }
	
  len += (1 << CHUNK_BITS) - 1;
  chunks = len >> CHUNK_BITS;
  addr = addr >> CHUNK_BITS;
	
  for (i = addr ; i < (addr + chunks) ; i++) {
    if (sram_bitmap[i] == 0) {
      printk (KERN_ERR "ppmc280_pci_free_consistent: Error - trying to free an already free memory\n");
      break;
    }
    sram_bitmap[i] = 0;
  }
  return;   
}
#endif

unsigned long __init
ppmc280_find_end_of_memory(void)
{
  static int	mem_size = 0;
	
  if (mem_size == 0) {
#ifdef OLD
    /* Next 2 lines are a kludge for mv64360_get_mem_size() */
    mv64360_base = PPMC280_BRIDGE_REG_BASE;
    ppmc280_set_bat();
#endif
    mem_size = mv64x60_get_mem_size(CONFIG_MV64X60_NEW_BASE, MV64x60_TYPE_MV64360);
  }
	
  return mem_size;
}

static void
ppmc280_reset_board(void)
{
  unsigned int mpp_ctrl_00;
  unsigned int mpp_ctrl_02;
  volatile ulong i = 100000;
				
  local_irq_disable();

  /* Set exception prefix high - to the firmware */
  _nmask_and_or_msr(0, MSR_IP);


  /* MPP19 defined as WDExp# in Eng Spec,
   * programming as GPP active low o/p & setting would cause reset 
   */
  printk("ppmc280_reset_board: begin\n");

  mpp_ctrl_00 = mv64x60_read(&bh, MV64x60_MPP_CNTL_0);
  mpp_ctrl_00 &= 0xffff0fff;
  mv64x60_write(&bh, MV64x60_MPP_CNTL_0,mpp_ctrl_00);

  mv64x60_set_bits(&bh, MV64x60_GPP_LEVEL_CNTL, BIT2);
  mv64x60_set_bits(&bh, MV64x60_GPP_IO_CNTL, BIT2);

  mpp_ctrl_02 = mv64x60_read(&bh, MV64x60_MPP_CNTL_2);
  mpp_ctrl_02 &= 0xffff0fff;
  mv64x60_write(&bh, MV64x60_MPP_CNTL_2,mpp_ctrl_02);

  mv64x60_set_bits(&bh, MV64x60_GPP_LEVEL_CNTL, BIT19);
  mv64x60_set_bits(&bh, MV64x60_GPP_IO_CNTL, BIT19);
			
  mv64x60_write(&bh, MV64x60_GPP_VALUE_SET,(BIT2 | BIT19));
  while(i -- >0);

  printk("ppmc280_reset_board: reset did not happen?\n");

  return;
}

static void
ppmc280_restart(char *cmd)
{
  volatile ulong	i = 10000000;

  ppmc280_reset_board();

  while (i-- > 0);
  panic("restart failed\n");
}

static void
ppmc280_halt(void)
{
  local_irq_disable();
  while (1);
  /* NOTREACHED */
}

static void
ppmc280_power_off(void)
{
  ppmc280_halt();
  /* NOTREACHED */
}

static int
ppmc280_show_cpuinfo(struct seq_file *m)
{
  uint pvid;

  pvid = mfspr(SPRN_PVR);
  seq_printf(m, "vendor\t\t: Marvell/Galileo\n");
  seq_printf(m, "machine\t\t: PPMC280\n");
  seq_printf(m, "PVID\t\t: 0x%x, vendor: %s\n",
	     pvid, (pvid & (1<<15) ? "IBM" : "Motorola"));

  return 0;
}

/* Not using RTC for calibration */
static void __init
ppmc280_calibrate_decr(void)
{
  ulong freq;

  freq = PPMC280_SYSTEM_TCLK / 4;
	
  printk("time_init: decrementer frequency = %lu.%.6lu MHz\n",
	 freq/1000000, freq%1000000);

  tb_ticks_per_jiffy = freq / HZ;
  tb_to_us = mulhwu_scale_factor(freq, 1000000);

  return;
}

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
#if 0
static void
ppmc280_ide_request_region(unsigned long from,
			   unsigned int extent,
			   const char *name)
{
  request_region(from, extent, name);
  return;
}

static void
ppmc280_ide_release_region(unsigned long from,
			   unsigned int extent)
{
  release_region(from, extent);
  return;
}
#endif
static void __init
ppmc280_ide_pci_init_hwif_ports(hw_regs_t *hw, unsigned long data_port,
				unsigned long ctrl_port, int *irq)
{

//  struct pci_dev	*dev;
  unsigned long reg = data_port;
  int i;
  
  /*
  while ((dev = pci_find_device(PCI_ANY_ID, PCI_ANY_ID, dev)) != NULL) {
    if (((dev->class >> 8) == PCI_CLASS_STORAGE_IDE) ||
	((dev->class >> 8) == PCI_CLASS_STORAGE_RAID)) {
      hw->irq = dev->irq;
      
      if (irq != NULL) {
	*irq = dev->irq;
      }
    }
  }
  */
  
  for (i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++) {
    hw->io_ports[i] = reg;
    reg += 1;
    
  }
  if (ctrl_port)
    hw->io_ports[IDE_CONTROL_OFFSET] = ctrl_port;
  else
    hw->io_ports[IDE_CONTROL_OFFSET] = hw->io_ports[IDE_DATA_OFFSET] + 0x206;
  
  if (irq != NULL)
    *irq = 0;
}
#endif

void ppmc280_heartbeat(void) {
  /* Use LED at GPP 23 */
  ppc_md.heartbeat_count = ppc_md.heartbeat_reset;

  /* Turn on and off debug led 0 */
  mv64x60_write(&bh, MV64x60_GPP_VALUE, mv64x60_read(&bh, MV64x60_GPP_VALUE) ^ BIT23);
}


static void __init ppmc280_map_io(void) {
  if( ppc_md.progress) ppc_md.progress( "Calling ppmc280_map_io", 0 );
#ifdef CONFIG_MV64360_SRAM_CACHEABLE
  io_block_mapping(PPMC280_INTERNAL_SRAM_BASE, PPMC280_INTERNAL_SRAM_BASE,
		   PPMC280_INTERNAL_SRAM_SIZE,_PAGE_KERNEL | _PAGE_GUARDED);
#else
#ifdef CONFIG_MV64360_SRAM_CACHE_COHERENT
  io_block_mapping(PPMC280_INTERNAL_SRAM_BASE, PPMC280_INTERNAL_SRAM_BASE,
		   PPMC280_INTERNAL_SRAM_SIZE,_PAGE_KERNEL | _PAGE_GUARDED | _PAGE_COHERENT);
#else
  io_block_mapping(PPMC280_INTERNAL_SRAM_BASE, PPMC280_INTERNAL_SRAM_BASE,
		   PPMC280_INTERNAL_SRAM_SIZE, _PAGE_IO);
#endif
#endif
  io_block_mapping(0xf1000000, 0xf1000000, 0x00100000, _PAGE_IO);
  io_block_mapping(0xff800000, 0xff800000, 0x00800000, _PAGE_IO);
}



#ifdef CONFIG_SMP
char ppmc280_smp0[] = "SMP Cpu #0";
char ppmc280_smp1[] = "SMP Cpu #1";

static void ppmc280_smp_cpu0_int_handler(int irq , void *dev_id , struct pt_regs *regs) {
  volatile unsigned int doorbell;
  
  doorbell = mv64x60_read(&bh, MV64360_SMP_CPU0_DOORBELL);
  /* printk ("Interrupt in CPU #0 doorbell (%08x)\n",doorbell);*/
  /*__asm__ ("sync");
    __asm__ ("eieio");*/

  /* Ack the doorbell interrupts */
  mv64x60_write(&bh, MV64360_SMP_CPU0_DOORBELL_CLEAR, doorbell);
  /*  printk ("Mordi: Interrupt in CPU #0 doorbell cleared (%08x)\n",mv64x60_read(&bh, MV64360_SMP_CPU0_DOORBELL));*/
  /*__asm__ ("sync");
    __asm__ ("eieio");  */

  if (doorbell & BIT0) {
    smp_message_recv(0, regs);
  }
  if (doorbell & BIT1) {
    smp_message_recv(1, regs);
  }
  if (doorbell & BIT2) {
    smp_message_recv(2, regs);
  }
  if (doorbell & BIT3) {
    smp_message_recv(3, regs);
  }
}

static void ppmc280_smp_cpu1_int_handler(int irq , void *dev_id , struct pt_regs *regs) {
  volatile unsigned int doorbell;

  doorbell= mv64x60_read(&bh, MV64360_SMP_CPU1_DOORBELL);
  /*  printk ("Interrupt in CPU #1 doorbell (%08x)\n",doorbell);*/
  /* __asm__ ("sync");
     __asm__ ("eieio");  */

  /* Ack the doorbell interrupts */
  mv64x60_write(&bh, MV64360_SMP_CPU1_DOORBELL_CLEAR, doorbell);
  /*    printk ("Mordi: Interrupt in CPU #1 doorbell cleared (%08x)\n",mv64x60_read(&bh, MV64360_SMP_CPU1_DOORBELL)); */
  /*__asm__ ("sync");
    __asm__ ("eieio");  */

  if (doorbell & BIT0) {
    smp_message_recv(0, regs);
  }
  if (doorbell & BIT1) {
    smp_message_recv(1, regs);
  }
  if (doorbell & BIT2) {
    smp_message_recv(2, regs);
  }
  if (doorbell & BIT3) {
    smp_message_recv(3, regs);
  }
}

extern void __secondary_start(void);

#if 0
static void
smp_ppmc280_CPU_two(void)
{
  register u32 temp;
  *(volatile unsigned int*) 0xf1000828 = 0x11111111;
  /*  for (temp = 0 ; temp < 100000 ; temp ++) {}*/
  __secondary_start();
}
#endif

static int
smp_ppmc280_probe(void)
{
  /*
    int i, nr;
    
    nr = (readb(PPMC280_CPUSTAT) & PPMC280_CPU_COUNT_MASK) >> 2;
    if (nr == 0)
    nr = 4;
    
    if (nr > 1) {
    openpic_request_IPIs();
    for (i = 1; i < nr; ++i)
    smp_hw_index[i] = i;
    }
  */
  /* The following cannot be done */
  /*  mv64x60_write(&bh, MV64360_SMP_CPU0_DOORBELL_CLEAR, 0x0);
      mv64x60_write(&bh, MV64360_SMP_CPU1_DOORBELL_CLEAR, 0x0);*/
 
  return 2;
}

static void
smp_ppmc280_message_pass(int target, int msg, unsigned long data, int wait)
{
  
  int smp_id = smp_processor_id();
  /*  printk ("Mordi : Someone trying to send message to target %d, msg %08x, data %08x, wait %d\n",target,msg,data,wait);*/
  if (msg > 0x3) {
    printk("SMP %d: smp_message_pass: unknown msg %d\n",
	   smp_processor_id(), msg);
    return;
  }
  switch (target) {
  case MSG_ALL:
    mv64x60_write(&bh, MV64360_SMP_CPU0_DOORBELL, 1 << msg);
    mv64x60_write(&bh, MV64360_SMP_CPU1_DOORBELL, 1 << msg);
    break;
  case MSG_ALL_BUT_SELF:
    if (smp_processor_id()) 
      mv64x60_write(&bh, MV64360_SMP_CPU0_DOORBELL, 1 << msg);
    else 
      mv64x60_write(&bh, MV64360_SMP_CPU1_DOORBELL, 1 << msg);
    break;
  default:
    if (target == 0)
      mv64x60_write(&bh, MV64360_SMP_CPU0_DOORBELL, 1 << msg);
    else 
      mv64x60_write(&bh, MV64360_SMP_CPU1_DOORBELL, 1 << msg);
    break;
  }
}
static void
smp_ppmc280_kick_cpu(int nr)
{
  u32 temp, internal_sram_base,ret_addr;
  u32 *sram_address, *flash_address;
  
  /* Mordi - DBG */
  /*
    __asm__ __volatile__ ("mfspr %0,1014\n\t"::"r" (ret_addr));
    __asm__ __volatile__ ("mtspr 1014,%0\n\t"::"r" (ret_addr & 0xff7fffff));
  */

  mv64x60_write(&bh, 0x828,0x0); /* Mordi - DBG */

#if 0 /* Rabeeh - Return to 1 */
  mv64x60_write(&bh, 0x82c,(u32) &smp_ppmc280_CPU_two);
#else
  mv64x60_write(&bh, 0x82c,(u32) &__secondary_start);
#endif

  /* map SRAM to 0xfff00000 */
  /* As we cannot boot from BOOT flash now Copy Boot Flash to SRAM again 
   * SRAM may have changed during Loads */
  if( ppc_md.progress) ppc_md.progress( "Copying FLASH to SRAM" , 0 );
  sram_address = (u32 *)PPMC280_INTERNAL_SRAM_BASE ;
  flash_address = (u32 *)PPMC280_BOOT_FLASH_BASE; 
  while( (u32)sram_address < PPMC280_INTERNAL_SRAM_BASE+PPMC280_INTERNAL_SRAM_SIZE){
    *(sram_address++) = *(flash_address++);
  }
  /* TODO:Make sure SRAM is not cached */
  if( ppc_md.progress) ppc_md.progress( "Mapping SRAM to boot address..." , 0 );

	   
  internal_sram_base = mv64x60_read(&bh, MV64360_INTEGRATED_SRAM_BASE_ADDR);
  mv64x60_write(&bh, MV64360_BASE_ADDR_ENABLE, mv64x60_read(&bh, MV64360_BASE_ADDR_ENABLE) | BIT8 | BIT19);
  mv64x60_write(&bh, MV64360_INTEGRATED_SRAM_BASE_ADDR, 0xFFF00000 >> 16);
  mv64x60_write(&bh, MV64360_BASE_ADDR_ENABLE, mv64x60_read(&bh, MV64360_BASE_ADDR_ENABLE) & ~BIT19);
  __asm__ ("sync");
  __asm__ ("eieio");
  
  /* Enable CPU1 arbitration */
  printk ("smp_ppmc280_CPU_two : Kicking CPU 1...");
  /* write cpu_nr at KERNELBASE */
  *(unsigned long *)KERNELBASE = nr;
  flush_dcache_range(KERNELBASE, KERNELBASE+0x100);
  temp =  mv64x60_read(&bh, MV64x60_CPU_MASTER_CNTL);
  mv64x60_write(&bh, MV64x60_CPU_MASTER_CNTL, temp & (~BIT9)); 
  printk("done\n"); 

  /*
   * Wait 100mSecond until other CPU has reached __secondary_start.
   * When it reaches, it is permittable to rever the SRAM mapping etc...
   */
  mdelay (100);
#if 0
  /* For testing with one CPU disable bus grant to CPU1 */
  temp =  mv64x60_read(&bh, MV64x60_CPU_MASTER_CNTL);
  mv64x60_write(&bh, MV64x60_CPU_MASTER_CNTL, temp | BIT9); 
  printk ("done\n"); 
#endif
  if( ppc_md.progress) ppc_md.progress( "Remapping SRAM and Boot Flash" , 0 );
  /* map SRAM to default address */
  mv64x60_write(&bh, MV64360_BASE_ADDR_ENABLE, mv64x60_read(&bh, MV64360_BASE_ADDR_ENABLE) | BIT19);
  mv64x60_write(&bh, MV64360_INTEGRATED_SRAM_BASE_ADDR,internal_sram_base);
  mv64x60_write(&bh, MV64360_BASE_ADDR_ENABLE, mv64x60_read(&bh, MV64360_BASE_ADDR_ENABLE) & ~(BIT19 | BIT8));
}
extern volatile unsigned long cpu_callin_map[NR_CPUS];

static void
smp_ppmc280_setup_cpu(int cpu_nr)
{
  /*	if (OpenPIC_Addr)
	do_openpic_setup_cpu();
	if (cpu_nr > 0)
	ppmc280_init_l2();
  */
  unsigned int ret_addr;
  unsigned int temp;
  static atomic_t ready = ATOMIC_INIT(1);
  static volatile int frozen = 0;
  u32 val;
#if 1
  /* Enable L2 Cache */
  if( cpu_nr > 0 ){
    val = _get_L2CR();
    val |= L2CR_L2E;
    _set_L2CR(val);
  }
#endif
  /* HID1 */
  /* Enable EBA and EBD PAR SyncBE and ABE in HID1 register */
  /* Note that this might be valid only for 7455 */
  /*  temp = mfspr (SPRN_HID1);
      temp |= BIT28 | BIT29 | BIT11 | BIT10;
      mtspr (SPRN_HID1, temp);*/

  /* ICTRL */
  /* Enable EIEC, EDCE, EICP */
  /*  temp = mfspr (SPRN_ICTRL);
      temp |= BIT27 | BIT26 | BIT8;
      mtspr (SPRN_ICTRL, temp);*/
  
  if (cpu_nr == 0) {
    /* udelay(100);  Mordi - TBD */
    mv64x60_write(&bh, MV64360_SMP_CPU0_DOORBELL_CLEAR, 0xff);
    mv64x60_write(&bh, MV64360_SMP_CPU0_DOORBELL_MASK, 0xff);
    /* move to arch_initcall if early request_irq becomes a problem */
    request_irq (60, ppmc280_smp_cpu0_int_handler,
		 SA_INTERRUPT , ppmc280_smp0, 0);
#if 0/* Stolen from CHRP */
    /* wait for all the others */
    while (atomic_read(&ready) < smp_num_cpus)
      barrier();
    atomic_set(&ready, 1);
    /* freeze the timebase */
    /*		call_rtas("freeze-time-base", 0, 1, NULL);*/
    mv64x60_set_bits(&bh, MV64x60_GPP_VALUE_CLEAR, BIT23);
    mb();
    frozen = 1;
    /* XXX assumes this is not a 601 */
    set_tb(0, 0);
    last_jiffy_stamp(0) = 0;
    while (atomic_read(&ready) < smp_num_cpus)
      barrier();
    /* thaw the timebase again */
    /*    call_rtas("thaw-time-base", 0, 1, NULL);*/
    mv64x60_set_bits(&bh, MV64x60_GPP_VALUE_SET, BIT23);
    mb();
    set_dec(tb_ticks_per_jiffy);
    frozen = 0;
    smp_tb_synchronized = 1;
    printk ("\n\n\n\n\n\n\n\nGetting OUT OF HERE\n");
#endif 
  }
  
  if (cpu_nr == 1) {
    cpu_callin_map[cpu_nr] = 2;//pass;
    /* *(volatile unsigned int*) 0xfbe00828 = 0x11111111;*/
    mv64x60_write(&bh, MV64360_SMP_CPU1_DOORBELL_CLEAR, 0x0);
    mv64x60_write(&bh, MV64360_SMP_CPU1_DOORBELL_MASK, 0xff);
    /* move to arch_initcall if early request_irq becomes a problem */
    request_irq (28, ppmc280_smp_cpu1_int_handler,
		 SA_INTERRUPT , ppmc280_smp1, 0);
    /* Release CPU #0 from polling */
    /*
     *(volatile unsigned int*) 0xf1000828 = 0x11111111;
     */

#if 0 /* Stolen from CHRP */
    atomic_inc(&ready);
    while (!frozen)
      barrier();
    set_tb(0, 0);
    last_jiffy_stamp(0) = 0;
    mb();
    atomic_inc(&ready);
    while (frozen)
      barrier();
    
    set_dec(tb_ticks_per_jiffy);
    printk ("\n\n\n\n\n\n\n\nGetting OUT OF HERE - II\n");
#endif
    
  }
#if 0
  printk ("Bats cpu%d:\n",cpu_nr);
  printk ("%08x\n",mfspr (SPRN_DBAT0L));
  printk ("%08x\n",mfspr (SPRN_DBAT0U));
  printk ("%08x\n",mfspr (SPRN_DBAT1L));
  printk ("%08x\n",mfspr (SPRN_DBAT1U));
  printk ("%08x\n",mfspr (SPRN_DBAT2L));
  printk ("%08x\n",mfspr (SPRN_DBAT2U));
  printk ("%08x\n",mfspr (SPRN_DBAT3L));
  printk ("%08x\n",mfspr (SPRN_DBAT3U));

  printk ("cpu%d- SPRN_L2CR %08x\n",cpu_nr,mfspr (SPRN_L2CR));
  printk ("cpu%d- SPRN_L3CR %08x\n",cpu_nr,mfspr (SPRN_L3CR));
  printk ("cpu%d- SPRN_HID0 %08x\n",cpu_nr,mfspr (SPRN_HID0));
  printk ("cpu%d- SPRN_HID1 %08x\n",cpu_nr,mfspr (SPRN_HID1));
  printk ("cpu%d- SPRN_MSSCR0 %08x\n",cpu_nr,mfspr (SPRN_MSSCR0));
  printk ("cpu%d- SPRN_MSSSR0 %08x\n",cpu_nr,mfspr (SPRN_MSSSR0));
  printk ("cpu%d- SPRN_ICTRL %08x\n",cpu_nr,mfspr (SPRN_ICTRL));
  printk ("cpu%d- SPRN_LDSTR %08x\n",cpu_nr,mfspr (SPRN_LDSTCR));
  printk ("cpu%d- SPRN_ICTC %08x\n",cpu_nr,mfspr (SPRN_ICTC));
  printk ("cpu%d- SPRN_SDR1 %08x\n",cpu_nr,mfspr (SPRN_SDR1));

  printk ("Ticks per Jifa = %d\n",tb_ticks_per_jiffy);
  /*__asm__ __volatile__ ("mfmsr %0\n\t"::"r" (ret_addr));
    printk("\n\n\n\nMordi: In smp_ppmc280_setup_cpu %d msr = (%08x)\n\n\n\n",cpu_nr,ret_addr);*/
  /*printk ("Mordi : %d - SPRN_MSSCR0 = %08x\n",cpu_nr, mfspr (SPRN_MSSCR0));*/
#endif

#if 0 /* Testing cache coherency between two CPUs */
  {
    register unsigned int temp, addr;
    static unsigned int first = 0;
    if (first == 0) {
      memset (0xc0100000, _4K, 0);
      first = 1;
    }
    while (1) {
      if (cpu_nr == 0) mdelay (1000);
      else mdelay (10);
      printk("CPU %d iteration\n",cpu_nr);
      for (addr = 0xc0100000 ; addr < (0xc0100000 + _4K); addr += 4) {
	while (((* (unsigned int *) addr) % 2) != cpu_nr) {
	  * (unsigned int *) addr ++;
	}
      }
    }
  }
#endif
#if 0 /* Testing cache coherency */
  {
    register unsigned int temp, addr;
    * (unsigned int *)0xc0100000 = 0;
    while (1) {
      if (cpu_nr == 0) mdelay (1000);
      else mdelay (10);
      /*      printk ("CPU %d iteration\n",cpu_nr);*/
      while (((* (volatile unsigned int *) 0xc0100000) % 2) == cpu_nr) {}
      printk ("%d: %08x --> %08x\n",cpu_nr, * (unsigned int *) 0xc0100000, (* (unsigned int *) 0xc0100000) +1);
      * (unsigned int *) 0xc0100000 = * (unsigned int *) 0xc0100000 + 1;
    }
  }
#endif
}

static struct smp_ops_t ppmc280_smp_ops = {
  smp_ppmc280_message_pass,
  smp_ppmc280_probe,
  smp_ppmc280_kick_cpu,
  smp_ppmc280_setup_cpu,
};
#endif /* CONFIG_SMP */

extern void i2c_rtc_read( unsigned char *);
extern void i2c_rtc_write( unsigned char *);

unsigned long todc_i2c_get_rtc_time( void)
{
  u_char buffer[9] ;
  struct rtc_time dt;

  i2c_rtc_read( buffer ) ;
  dt.tm_sec =    BCD_TO_BIN( buffer[2] );
  dt.tm_min =    BCD_TO_BIN( buffer[1] );
  dt.tm_hour =   BCD_TO_BIN( buffer[0] );
  dt.tm_mday =   BCD_TO_BIN( buffer[3] );
  dt.tm_mon =    BCD_TO_BIN( buffer[4] );

  dt.tm_year =   ( BCD_TO_BIN( buffer[5])) +
    (( BCD_TO_BIN( buffer[7])) * 100 );

  dt.tm_wday = buffer[6];

  /* Good chance the time is invalid, since clock has only a 2 day battery */
  if (dt.tm_year < 1970) {
	dt.tm_year = 1970;
	dt.tm_mon = 1;
	dt.tm_mday = 1;
	dt.tm_hour = 0;
	dt.tm_min = 0;
	dt.tm_sec = 0;
  }

  return mktime( dt.tm_year, dt.tm_mon, dt.tm_mday, dt.tm_hour,
		 dt.tm_min, dt.tm_sec);

}
static int todc_i2c_set_rtc_time( unsigned long nowtime)
{
  struct rtc_time dt;
  unsigned char buffer[9];
  unsigned char temp;

  to_tm( nowtime, &dt);

  buffer[0] = BIN_TO_BCD( dt.tm_hour );
  buffer[1] = BIN_TO_BCD( dt.tm_min );
  buffer[2] = BIN_TO_BCD( dt.tm_sec );
  buffer[3] = BIN_TO_BCD( dt.tm_mday );
  buffer[4] = BIN_TO_BCD( dt.tm_mon );

  temp = dt.tm_year % 100 ;

  buffer[5] =  BIN_TO_BCD( temp );

  temp = dt.tm_year / 100 ;
  buffer[7] = BIN_TO_BCD( temp );

  buffer[6] = BIN_TO_BCD( dt.tm_wday );	

  i2c_rtc_write(buffer);
	
  return 0;
}

void __init
platform_init(unsigned long r3, unsigned long r4, unsigned long r5,
	      unsigned long r6, unsigned long r7)
{
  parse_bootinfo(find_bootinfo());

  isa_mem_base = 0;

  ppc_md.setup_arch = ppmc280_setup_arch;
  ppc_md.show_cpuinfo = ppmc280_show_cpuinfo;
#if 0
  ppc_md.irq_cannonicalize = NULL;
#endif
  ppc_md.init_IRQ = mv64360_init_irq;
  ppc_md.get_irq = mv64360_get_irq;
  ppc_md.init = NULL;

  ppc_md.restart = ppmc280_restart;
  ppc_md.power_off = ppmc280_power_off;
  ppc_md.halt = ppmc280_halt;

  ppc_md.find_end_of_memory = ppmc280_find_end_of_memory;

#if 0
  ppc_md.time_init = todc_time_init;
  ppc_md.set_rtc_time = todc_set_rtc_time;
  ppc_md.get_rtc_time = todc_get_rtc_time;
#else
  ppc_md.set_rtc_time = todc_i2c_set_rtc_time;
  ppc_md.get_rtc_time = todc_i2c_get_rtc_time;
#endif
  ppc_md.calibrate_decr = ppmc280_calibrate_decr;

#if 0
  ppc_md.nvram_read_val = todc_direct_read_val;
  ppc_md.nvram_write_val = todc_direct_write_val;
#endif
  ppc_md.heartbeat = ppmc280_heartbeat;
  ppc_md.heartbeat_reset = HZ/2;
  ppc_md.heartbeat_count = 1;

//  ppc_md.pcibios_fixup = ppmc280_pcibios_fixup;
#ifdef	CONFIG_SERIAL_TEXT_DEBUG
  ppmc280_set_bat();
  ppc_md.setup_io_mappings = ppmc280_map_io;
#ifdef	CONFIG_SERIAL_MPSC_CONSOLE
  ppc_md.progress = mv64x60_mpsc_progress; /* embedded UART */
  mv64x60_progress_init(CONFIG_MV64X60_NEW_BASE);
#endif
#else	/* !CONFIG_SERIAL_TEXT_DEBUG */
  ppc_md.progress = NULL;
#endif	/* CONFIG_SERIAL_TEXT_DEBUG */

#if defined(CONFIG_SERIAL_MPSC) || defined(CONFIG_MV643XX_ETH)
	platform_notify = ppmc280_platform_notify;
#endif


#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
#if 0
  ppc_ide_md.ide_check_region = ppmc280_ide_check_region;
  ppc_ide_md.ide_request_region = ppmc280_ide_request_region;
  ppc_ide_md.ide_release_region = ppmc280_ide_release_region;
#endif
  ppc_ide_md.ide_init_hwif = ppmc280_ide_pci_init_hwif_ports;
#endif

#ifdef CONFIG_SPECIAL_CONSISTENT_MEMORY
  ppc_md.platform_pci_alloc_consistent = ppmc280_pci_alloc_consistent;
  ppc_md.platform_pci_free_consistent = ppmc280_pci_free_consistent;
#endif

#ifdef CONFIG_SMP
  ppc_md.smp_ops = &ppmc280_smp_ops;
#endif /* CONFIG_SMP */

#ifdef CONFIG_BLK_DEV_INITRD
  if( r4 )  {
    initrd_start = r4 + KERNELBASE;
    initrd_end = r5 + KERNELBASE;
  }
#endif /* CONFIG_BLK_DEV_INITRD */

  if( (r3 - r6) == 0x1000 ){
    /* Cmdline is valid if it does not start with NULL */
    char * new_cmdline = (char *)(r6 + KERNELBASE);
    if( new_cmdline[0] != '\0'){
      int i;
      for( i=0; new_cmdline[i] != '\0' && i < 511; i++)
	cmd_line[i] = new_cmdline[i];
      cmd_line[i] = '\0';
    }
  }

  return;
}


#if 0 

/* RESOURCE DUMP UTILITY - see gt_shell user space application for activation */



/* global variables from 'regdump' */
static struct proc_dir_entry *evb_resource_dump;
static u32 evb_resource_dump_request , evb_resource_dump_result;

/* Some service routines */

static int ishex (char ch) {
  if (((ch>='0') && (ch <='9')) || ((ch>='a') && (ch <='f')) || ((ch >='A') && (ch <='F'))) return 1;
  return 0;
}

static int hex_value (char ch) {
  if ((ch >= '0') && (ch <= '9')) return ch-'0';
  if ((ch >= 'a') && (ch <= 'f')) return ch-'a'+10;
  if ((ch >= 'A') && (ch <= 'F')) return ch-'A'+10;
  return 0;
}

static int atoh(char *s , int len) {
  int i=0;
  while (ishex(*s) && len--) {
    i = i*0x10 + hex_value(*s);
    s++;
  }  
  return i;
}

/* The format of writing to this module is as follows -
   char 0 - r/w (Reading from register or Writing to register/memory)
   char 1 - space
   char 2 - register/mem_addr offset 7
   char 3 - register/mem_addr offset 6
   char 4 - register/mem_addr offset 5
   char 5 - register/mem_addr offset 4
   char 6 - register/mem_addr offset 3
   char 7 - register/mem_addr offset 2
   char 8 - register/mem_addr offset 1
   char 9 - register/mem_addr offset 0
   // The following is valid only if write request
   char 10 - space
   char 11 - register/mem_addr value 7
   char 12 - register/mem_addr value 6
   char 13 - register/mem_addr value 5
   char 14 - register/mem_addr value 4
   char 15 - register/mem_addr value 3
   char 16 - register/mem_addr value 2
   char 17 - register/mem_addr value 1
   char 18 - register/mem_addr value 0

*/

/********************************************************************
 * evb_resource_dump_write -
 *
 * When written to the /proc/resource_dump file this function is called
 *
 * Inputs: file / data are not used. Buffer and count are the pointer
 *         and length of the input string
 * Returns: Read from GT register
 * Outputs: count
 *********************************************************************/
int evb_resource_dump_write (struct file *file, const char *buffer,
			     unsigned long count, void *data) {
  /* Reading / Writing from system controller internal registers */
  if (!strncmp (buffer , "register" , 8)) {
    if (buffer[10] == 'r') {
      evb_resource_dump_request = atoh((char *)((unsigned int)buffer + 12),8);
      evb_resource_dump_result = mv64x60_read(&bh, evb_resource_dump_request);
    };
    if (buffer[10] == 'w') {
      evb_resource_dump_request = atoh((char *)((unsigned int)buffer + 12), 8);
      evb_resource_dump_result = atoh ((char *)((unsigned int)buffer + 12 + 8 + 1) , 8);
      mv64x60_write(&bh, evb_resource_dump_request , evb_resource_dump_result);
    }
  }

  /* Reading / Writing from 32bit address - mostly usable for memory */
  if (!strncmp (buffer , "memory  " , 8)) {
    if (buffer[10] == 'r') {
      evb_resource_dump_request = atoh((char *)((unsigned int)buffer + 12),8);
      evb_resource_dump_result = * (unsigned int *) evb_resource_dump_request;
    };
    if (buffer[10] == 'w') {
      evb_resource_dump_request = atoh((char *)((unsigned int)buffer + 12), 8);
      evb_resource_dump_result = atoh ((char *)((unsigned int)buffer + 12 + 8 + 1) , 8);
      * (unsigned int *) evb_resource_dump_request = evb_resource_dump_result;
    }
  }
  
  /* Reading / Writing from internal CPU L2CR register */
  if (!strncmp (buffer , "cpu l2cr" , 8)) {
    if (buffer[10] == 'r') {
      evb_resource_dump_result = mfspr (SPRN_L2CR);
    };
    if (buffer[10] == 'w') {
      evb_resource_dump_result = atoh ((char *)((unsigned int)buffer + 12) , 8);
      mb();
      mtspr (SPRN_L2CR , evb_resource_dump_result);
      mb();
    }
  }

  /* Reading / Writing from internal CPU L2CR register */
  if (!strncmp (buffer , "cpu hid0" , 8)) {
    if (buffer[10] == 'r') {
      evb_resource_dump_result = mfspr (SPRN_HID0);
    };
    if (buffer[10] == 'w') {
      evb_resource_dump_result = atoh ((char *)((unsigned int)buffer + 12) , 8);
      mtspr (SPRN_HID0 , evb_resource_dump_result);
      mb();
    }
  }
  return count;
}

/********************************************************************
 * evb_resource_dump_read -
 *
 * When read from the /proc/resource_dump file this function is called
 *
 * Inputs: buffer_location and buffer_length and zero are not used.
 *         buffer is the pointer where to post the result
 * Returns: N/A
 * Outputs: length of string posted
 *********************************************************************/
int evb_resource_dump_read (char *buffer, char **buffer_location, off_t offset, 
			    int buffer_length, int *zero, void *ptr) {
  
  if (offset > 0)
    return 0;
  return sprintf(buffer, "%08x\n", evb_resource_dump_result);
}

/********************************************************************
 * start_regdump_memdump -
 *
 * Register the /proc/regdump file at the /proc filesystem
 * Register the /proc/memdump file at the /proc filesystem
 *
 * Inputs: N/A
 * Returns: N/A
 * Outputs: N/A
 *********************************************************************/
int __init start_resource_dump(void)
{
  evb_resource_dump = create_proc_entry ("resource_dump" , 0666 , &proc_root);
  evb_resource_dump->read_proc = evb_resource_dump_read;
  evb_resource_dump->write_proc = evb_resource_dump_write;
  evb_resource_dump->nlink = 1;
  return 0;
}

module_init(start_resource_dump);

#endif   /* resource dump module */
