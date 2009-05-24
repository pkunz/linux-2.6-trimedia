/*
 * mcp905_setup.c
 *
 * Board setup routines for the Motorola MCP905 Board.
 *
 * Based on code done by Ajit Prem <Ajit.Prem@motorola.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/pci.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/initrd.h>
#include <linux/blkdev.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/ide.h>
#include <linux/seq_file.h>
#include <linux/root_dev.h>
#include <linux/mv643xx.h>
#if !defined(CONFIG_SERIAL_MPSC_CONSOLE)
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/serial_core.h>
#endif

#include <asm/page.h>
#include <asm/smp.h>
#include <asm/bootinfo.h>
#include <asm/machdep.h>
#include <asm/mv64x60.h>
#include <asm/todc.h>
#include <asm/time.h>

#include <platforms/mcp905.h>

static struct mv64x60_handle    bh;

extern void _set_L3CR(unsigned long);
extern char cmd_line[];
unsigned long mcp905_find_end_of_memory(void);

#if !defined(CONFIG_SERIAL_MPSC_CONSOLE)
extern void gen550_progress(char *, unsigned short);
extern void gen550_init(int, struct uart_port *);
#endif

TODC_ALLOC();

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
 * Set BAT 3 to map 0xf0000000 to end of physical memory space.
 */
static __inline__ void mcp905_set_bat(void)
{
	unsigned long bat3u, bat3l;
	static int mapping_set = 0;

	if (!mapping_set) {

		__asm__ __volatile__(" lis %0,0xf000\n \
		  ori %1,%0,0x002a\n \
		  ori %0,%0,0x1ffe\n \
		  mtspr 0x21e,%0\n \
		  mtspr 0x21f,%1\n \
		  isync\n \
		  sync ":"=r"(bat3u), "=r"(bat3l));

		mapping_set = 1;
	}

	return;
}

unsigned long __init mcp905_find_end_of_memory(void)
{
	static int mem_size = 0;

	if (mem_size == 0) {
		mem_size = mv64x60_get_mem_size(CONFIG_MV64X60_NEW_BASE,
				MV64x60_TYPE_MV64360);
	}
	return mem_size;
}

/*
 * Motorola MCP905 Board PCI interrupt routing.
 */
static int __init
mcp905_map_irq(struct pci_dev *dev, unsigned char idsel, unsigned char pin)
{
	struct pci_controller *hose = pci_bus_to_hose(dev->bus->number);

	if (hose->index == 0) {
		static char pci_irq_table[][4] =
		    /*
		     *      PCI IDSEL/INTPIN->INTLINE 
		     *         A   B   C   D
		     */
		{
			{82, 83, 82, 83},	/* IDSEL 4 - PMC 2 IDSEL */
			{83, 82, 83, 82},	/* IDSEL 5 - PMC 2 IDSELB */

		};

		const long min_idsel = 4, max_idsel = 5, irqs_per_slot = 4;
		return PCI_IRQ_TABLE_LOOKUP;
	} else {
		static char pci_irq_table[][4] =
		    /*
		     *      PCI IDSEL/INTPIN->INTLINE 
		     *         A   B   C   D
		     */
		{
			{80, 81, 80, 81},	/* IDSEL 4 - 16 - PMC 1 IDSEL */
			{80, 81, 80, 81},	/* IDSEL 5 - PMC 1 IDSELB */
			{81, 0, 0, 0},	/* IDSEL 6 - PCI646U2 IDE */
			{80, 0, 0, 0},	/* IDSEL 7 - 21555 Bridge */
		};

		const long min_idsel = 4, max_idsel = 7, irqs_per_slot = 4;
		return PCI_IRQ_TABLE_LOOKUP;
	}
}

#define SET_PCI_COMMAND_INVALIDATE

#ifdef SET_PCI_COMMAND_INVALIDATE
static void __init set_pci_command_invalidate(void)
{
	struct pci_dev *dev = NULL;
	u16 val;

	while ((dev = pci_find_device(PCI_ANY_ID, PCI_ANY_ID, dev)) != NULL) {
		pci_read_config_word(dev, PCI_COMMAND, &val);
		val |= PCI_COMMAND_INVALIDATE;
		pci_write_config_word(dev, PCI_COMMAND, val);
	}
}
#endif

/* Set IDE controllers into Native mode */
#undef SET_PCI_IDE_NATIVE

#ifdef SET_PCI_IDE_NATIVE
static void __init set_pci_native_mode(void)
{
	struct pci_dev *dev = NULL;

	while ((dev = pci_find_device(PCI_ANY_ID, PCI_ANY_ID, dev)) != NULL) {
		int class = dev->class >> 8;

		/* enable pci native mode */
		if (class == PCI_CLASS_STORAGE_IDE) {
			u8 reg;

			pci_read_config_byte(dev, 0x9, &reg);
			if (reg == 0x8a) {
				printk
				    ("PCI: Enabling PCI IDE native mode on %sn",
				     dev->slot_name);
				pci_write_config_byte(dev, 0x9, 0x8f);

				/* let the pci code set this device up after we
				   change it */
				pci_setup_device(dev);
			} else if (reg != 0x8f) {
				printk
				    ("PCI: IDE chip in unknown mode 0x%02x on%s",
				     reg, dev->slot_name);
			}
		}
	}
}
#endif

static void __init mcp905_pci_fixups(void)
{
#ifdef SET_PCI_IDE_NATIVE
	set_pci_native_mode();
#endif
#ifdef SET_PCI_COMMAND_INVALIDATE
	set_pci_command_invalidate();
#endif
}

#define MV64360_PCI_ACC_CNTL_CACHE_COHERENT MV64360_PCI_ACC_CNTL_ENABLE | MV64360_PCI_ACC_CNTL_SNOOP_WB | MV64360_PCI_ACC_CNTL_MBURST_32_BYTES | MV64360_PCI_ACC_CNTL_RDSIZE_128_BYTES

#define MV64360_PCI_ACC_CNTL_NONE_CACHE_COHERENT MV64360_PCI_ACC_CNTL_ENABLE | MV64360_PCI_ACC_CNTL_MBURST_128_BYTES | MV64360_PCI_ACC_CNTL_RDSIZE_256_BYTES

static void __init mcp905_setup_bridge(void)
{
	struct mv64x60_setup_info si;
	int i;

	memset(&si, 0, sizeof(si));

	si.phys_reg_base = CONFIG_MV64X60_NEW_BASE;

	si.pci_0.enable_bus = 1;
	si.pci_0.pci_io.cpu_base = MCP905_PCI_0_IO_START_PROC;
	si.pci_0.pci_io.pci_base_hi = 0;
	si.pci_0.pci_io.pci_base_lo = MCP905_PCI_0_IO_START;
	si.pci_0.pci_io.size = MCP905_PCI_0_IO_SIZE;
	si.pci_0.pci_io.swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_0.pci_mem[0].cpu_base = MCP905_PCI_0_MEM_START_PROC;
	si.pci_0.pci_mem[0].pci_base_hi = 0;
	si.pci_0.pci_mem[0].pci_base_lo = MCP905_PCI_0_MEM_START;
	si.pci_0.pci_mem[0].size = MCP905_PCI_0_MEM_SIZE;
	si.pci_0.pci_mem[0].swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_0.pci_cmd_bits = 0;
	si.pci_0.latency_timer = 0x80;

	si.pci_1.enable_bus = 1;
	si.pci_1.pci_io.cpu_base = MCP905_PCI_1_IO_START_PROC;
	si.pci_1.pci_io.pci_base_hi = 0;
	si.pci_1.pci_io.pci_base_lo = MCP905_PCI_1_IO_START;
	si.pci_1.pci_io.size = MCP905_PCI_1_IO_SIZE;
	si.pci_1.pci_io.swap = MV64x60_CPU2PCI_SWAP_NONE;
	si.pci_1.pci_mem[0].cpu_base = MCP905_PCI_1_MEM_START_PROC;
	si.pci_1.pci_mem[0].pci_base_hi = 0;
	si.pci_1.pci_mem[0].pci_base_lo = MCP905_PCI_1_MEM_START;
	si.pci_1.pci_mem[0].size = MCP905_PCI_1_MEM_SIZE;
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
		si.pci_1.acc_cntl_options[i] = MV64360_PCI_ACC_CNTL_NONE_CACHE_COHERENT;
#else
		si.pci_0.acc_cntl_options[i] = MV64360_PCI_ACC_CNTL_CACHE_COHERENT;
		si.pci_1.acc_cntl_options[i] = MV64360_PCI_ACC_CNTL_CACHE_COHERENT;
#endif
	}

	/* Lookup PCI host bridges */
	if (mv64x60_init(&bh, &si))
		printk(KERN_WARNING "Bridge initialization failed.\n");

	pci_dram_offset = 0;
	ppc_md.pci_swizzle = common_swizzle;
	ppc_md.pci_map_irq = mcp905_map_irq;
	ppc_md.pci_exclude_device = mv64x60_pci_exclude_device;

	mv64x60_set_bus(&bh, 0, 0);
	bh.hose_a->first_busno = 0;
	bh.hose_a->last_busno = 0xff;
	bh.hose_a->last_busno = pciauto_bus_scan(bh.hose_a, 0);

	bh.hose_b->first_busno = bh.hose_a->last_busno + 1;
	mv64x60_set_bus(&bh, 1, bh.hose_b->first_busno);
	bh.hose_b->last_busno = 0xff;
	bh.hose_b->last_busno = pciauto_bus_scan(bh.hose_b, bh.hose_b->first_busno);

	return;
}

void __init mcp905_setup_peripherals(void)
{
	u32 val;
	int bank_b_boot;
	int status_reg_2;
	u32 boot_bank_base;
	u32 boot_bank_size;
	u32 cs0_bank_base;
	u32 cs0_bank_size;

#ifdef CONFIG_SMP
#error "mcp905_setup.c: No SMP support"
#else
	mv64x60_set_bits(&bh, MV64x60_CPU_MASTER_CNTL, (1<<9)); /* Only 1 cpu */
#endif
	mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_1_WIN,
		MCP905_DEVICE_CS1_BASE, MCP905_DEVICE_CS1_SIZE, 0);
	bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_1_WIN);

	/*
	 * Set up windows for boot FLASH (using boot CS window),
	 * and for the TODC, UARTs, and BMC  on the device module.
	 */

	/* Disable unused windows */
	bh.ci->disable_window_32bit(&bh, MV64x60_CPU2DEV_2_WIN);
	bh.ci->disable_window_32bit(&bh, MV64x60_CPU2DEV_3_WIN);

	bank_b_boot =
	    readb(MCP905_BOARD_STATUS_REG_1) & MCP905_BOARD_BANK_SEL_MASK;
	if (bank_b_boot) {
		cs0_bank_size = MCP905_BANK_A_FLASH_SIZE;
		cs0_bank_base = MCP905_BANK_A_FLASH_BASE;
		boot_bank_base = MCP905_BANK_B_FLASH_BASE;
		boot_bank_size = MCP905_BANK_B_FLASH_SIZE;
	} else {
		cs0_bank_size = MCP905_BANK_B_FLASH_SIZE;
		cs0_bank_base = MCP905_BANK_B_FLASH_BASE;
		boot_bank_base = MCP905_BANK_A_FLASH_BASE;
		boot_bank_size = MCP905_BANK_A_FLASH_SIZE;
	}

	/* Setup the 2 flash windows */
	mv64x60_set_32bit_window(&bh, MV64x60_CPU2BOOT_WIN,
		boot_bank_base, boot_bank_size, 0);
	bh.ci->enable_window_32bit(&bh, MV64x60_CPU2BOOT_WIN);
	mv64x60_set_32bit_window(&bh, MV64x60_CPU2DEV_0_WIN,
		cs0_bank_base, cs0_bank_size, 0);
	bh.ci->enable_window_32bit(&bh, MV64x60_CPU2DEV_0_WIN);

	/* Set up window for internal sram (256KByte size) */
	mv64x60_set_32bit_window(&bh, MV64x60_CPU2SRAM_WIN,
		MCP905_INTERNAL_SRAM_BASE, _256K, 0);
	bh.ci->enable_window_32bit(&bh, MV64x60_CPU2SRAM_WIN);

	/* clear boardfail indicator; allow flash write */
	status_reg_2 = readb(MCP905_BOARD_STATUS_REG_2);
	status_reg_2 &= ~MCP905_BOARD_FAIL_MASK;
 /* don't do this if flash write was disabled on the board to prevent accidental scribbling of motload */
#if 0
	status_reg_2 &= ~MCP905_BOARD_FLASH_WP_MASK;
#endif
	writeb(status_reg_2, MCP905_BOARD_STATUS_REG_2);

	/*
	 * Configure internal SRAM
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

	/*
	 * Set integrated SRAM to 0. 
	 */
	memset((void *)MCP905_INTERNAL_SRAM_BASE, 0, MCP905_INTERNAL_SRAM_SIZE);

	/* MPP 7 */
	mv64x60_clr_bits(&bh, MV64x60_MPP_CNTL_0, BIT28 | BIT29 | BIT30 | BIT31);
	/* MPP 16-23 */
	mv64x60_write(&bh, MV64x60_MPP_CNTL_2, 0);

#define GPP_EXTERNAL_INTERRUPTS \
	(BIT7 | BIT16 | BIT17 | BIT18 | BIT19 | BIT20 | BIT21 | BIT22 | BIT23)

	/*
	 * Define GPP 7, 16-23 interrupt polarity as active low
	 * input signals and level triggered
	 */
	mv64x60_set_bits(&bh, MV64x60_GPP_LEVEL_CNTL, GPP_EXTERNAL_INTERRUPTS);
	mv64x60_clr_bits(&bh, MV64x60_GPP_IO_CNTL, GPP_EXTERNAL_INTERRUPTS);

	/* Config GPP interrupt controller to respond to level trigger */
	mv64x60_set_bits(&bh, MV64x60_COMM_ARBITER_CNTL, (1<<10));

	/*
	 * Dismiss and then enable interrupt on GPP interrupt cause for CPU #0
	 */
	mv64x60_write(&bh, MV64x60_GPP_INTR_CAUSE, ~GPP_EXTERNAL_INTERRUPTS);
	mv64x60_set_bits(&bh, MV64x60_GPP_INTR_MASK, GPP_EXTERNAL_INTERRUPTS);

	/*
	 * Dismiss and then enable interrupt on CPU #0 high cause register
	 * BIT26 summarizes GPP interrupts 16-23 (Need MPP 16-23)
	 * BIT24 summarizes GPP interrupts 0-7 (Need MPP 7)
	 */
	mv64x60_set_bits(&bh, MV64360_IC_CPU0_INTR_MASK_HI, (1<<26)||(1<<24));

	/*
	 * Change DRAM read buffer assignment.
	 * CPU assigned read buffer 0, the others assigned read buffer 1.
	 */
	val = mv64x60_read(&bh, MV64360_SDRAM_CONFIG);
	val = val & 0x03ffffff;
	val = val | 0xf8000000;
	mv64x60_write(&bh, MV64360_SDRAM_CONFIG, val);

	return;
}

static void __init mcp905_map_io(void)
{
	io_block_mapping(0xf1000000, 0xf1000000, 0x01000000, _PAGE_IO);
#ifdef CONFIG_MV64360_SRAM_CACHEABLE 
	io_block_mapping(MCP905_INTERNAL_SRAM_BASE, MCP905_INTERNAL_SRAM_BASE,
			 MCP905_INTERNAL_SRAM_SIZE,
                         _PAGE_KERNEL | _PAGE_GUARDED);
#else
#ifdef CONFIG_MV64360_SRAM_CACHE_COHERENT
	io_block_mapping(MCP905_INTERNAL_SRAM_BASE, MCP905_INTERNAL_SRAM_BASE,
                         MCP905_INTERNAL_SRAM_SIZE,
                         _PAGE_KERNEL | _PAGE_GUARDED | _PAGE_COHERENT);
#else
	io_block_mapping(MCP905_INTERNAL_SRAM_BASE, MCP905_INTERNAL_SRAM_BASE,
                        MCP905_INTERNAL_SRAM_SIZE, _PAGE_IO);
#endif
#endif
	io_block_mapping(0xff800000, 0xff800000, 0x00800000, _PAGE_IO);
}

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
static void __init
mcp905_ide_init_hwif_ports(hw_regs_t * hw, unsigned long data_port,
			   unsigned long ctrl_port, int *irq)
{
	unsigned long reg = data_port;
	int i;

	for (i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++) {
		hw->io_ports[i] = reg;
		reg += 1;

	}
	if (ctrl_port)
		hw->io_ports[IDE_CONTROL_OFFSET] = ctrl_port;
	else
		hw->io_ports[IDE_CONTROL_OFFSET] =
		    hw->io_ports[IDE_DATA_OFFSET] + 0x206;

	if (irq != NULL)
		*irq = 0;
}

#endif


#if defined(CONFIG_SERIAL_TEXT_DEBUG)
#if !defined(CONFIG_SERIAL_MPSC_CONSOLE)
#include <linux/serialP.h>
#include <linux/serial_reg.h>
#include <asm/serial.h>

static struct serial_state rs_table[RS_TABLE_SIZE] = {
	SERIAL_PORT_DFNS	/* Defined in <asm/serial.h> */
};

static void mcp905_16550_progress(char *s, unsigned short hex)
{
	volatile char c;
	volatile unsigned long com_port;
	u16 shift;

	com_port = rs_table[0].port;
	shift = rs_table[0].iomem_reg_shift;

	while ((c = *s++) != 0) {
		while ((*((volatile unsigned char *)com_port +
			  (UART_LSR << shift)) & UART_LSR_THRE) == 0) ;
		*(volatile unsigned char *)com_port = c;

		if (c == '\n') {
			while ((*((volatile unsigned char *)com_port +
				  (UART_LSR << shift)) & UART_LSR_THRE) == 0) ;
			*(volatile unsigned char *)com_port = '\r';
		}
	}

	/* Move to next line on */
	while ((*((volatile unsigned char *)com_port +
		  (UART_LSR << shift)) & UART_LSR_THRE) == 0) ;
	*(volatile unsigned char *)com_port = '\n';
	while ((*((volatile unsigned char *)com_port +
		  (UART_LSR << shift)) & UART_LSR_THRE) == 0) ;
	*(volatile unsigned char *)com_port = '\r';

	return;
}

static void __init mcp905_early_serial_map(void)
{
	struct uart_port serial_req;

	memset(&serial_req, 0, sizeof(serial_req));
	serial_req.uartclk = MCP905_BASE_BAUD;
	serial_req.irq = MCP905_UART_0_IRQ;
	serial_req.flags = STD_COM_FLAGS;
	serial_req.iotype = SERIAL_IO_MEM;
	serial_req.membase = (u_char *) MCP905_SERIAL_0;
	serial_req.regshift = 0;

	gen550_init(0, &serial_req);

	if (early_serial_setup(&serial_req) != 0) {
		printk("Early serial init of port 0 failed\n");
	}

	/* Assume early_serial_setup() doesn't modify serial_req */
	serial_req.line = 1;
	serial_req.irq = MCP905_UART_1_IRQ;
	serial_req.membase = (u_char *) MCP905_SERIAL_1;

	gen550_init(1, &serial_req);

	if (early_serial_setup(&serial_req) != 0) {
		printk("Early serial init of port 1 failed\n");
	}
}
#endif				/* !CONFIG_SERIAL_MPSC_CONSOLE */
#endif				/* CONFIG_SERIAL_TEXT_DEBUG */

static void __init mcp905_init_caches(void)
{
	uint val;

	/* Enable L2 and L3 caches (if 745x) */
	val = _get_L2CR();
	val |= L2CR_L2E;
#if 0
	_set_L2CR(val);
#else
	_set_L2CR(0xc0000000);
#endif

	if (PVR_VER(mfspr(SPRN_PVR)) == PVR_VER(PVR_7450)
	    || PVR_VER(mfspr(SPRN_PVR)) == PVR_VER(PVR_7455)
	    || PVR_VER(mfspr(SPRN_PVR)) == PVR_VER(PVR_7457)) {
		val = _get_L3CR();
		val |= L3CR_L3E;
#if 1
		_set_L3CR(val);
#else
		_set_L3CR(0xdf826000);
#endif
	}
#if 0
	val = mfspr(HID1);
	val &= ~HID0_EBD;
	mtspr(HID1, val);
	asm volatile ("isync");
	asm volatile ("sync");
#endif
}

static void __init mcp905_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("mcp905_setup_arch: enter", 0);

	loops_per_jiffy = 50000000 / HZ;

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

#ifdef  CONFIG_DUMMY_CONSOLE
	conswitchp = &dummy_con;
#endif

	mcp905_init_caches();

	if (ppc_md.progress)
		ppc_md.progress("mcp905_setup_arch: find_bridges", 0);

	mcp905_setup_bridge();
	mcp905_setup_peripherals();

	if (ppc_md.progress)
		ppc_md.progress("mcp905_setup_arch: find_bridges done", 0);

	TODC_INIT(TODC_TYPE_MK48T37, 0, 0,
		  ioremap(MCP905_TODC_BASE, MCP905_TODC_SIZE), 8);

	printk("Motorola Computer Group MCP905 Board\n");
	printk("MCP905 port (C) 2002 Motorola, Inc.\n");

	if (ppc_md.progress)
		ppc_md.progress("mcp905_setup_arch: exit", 0);

	return;
}

/* Platform device data fixup routines. */
#if defined(CONFIG_SERIAL_MPSC)
static void __init
mcp905_fixup_mpsc_pdata(struct platform_device *pdev)
{
	struct mpsc_pdata *pdata;

	pdata = (struct mpsc_pdata *)pdev->dev.platform_data;

	pdata->max_idle = 40;
	pdata->default_baud = MCP905_DEFAULT_BAUD;
	pdata->brg_clk_src = MCP905_MPSC_CLK_SRC;
	pdata->brg_clk_freq = MCP905_MPSC_CLK_FREQ;

	return;
}
#endif

#if defined(CONFIG_MV643XX_ETH)
static void __init
mcp905_fixup_eth_pdata(struct platform_device *pdev)
{
	struct mv643xx_eth_platform_data *eth_pd;
	static u16 phy_addr[] = {
		MCP905_ETH0_PHY_ADDR,
		MCP905_ETH1_PHY_ADDR,
		MCP905_ETH2_PHY_ADDR,
	};

	eth_pd = pdev->dev.platform_data;
	eth_pd->force_phy_addr = 1;
	eth_pd->phy_addr = phy_addr[pdev->id];
	eth_pd->tx_queue_size = MCP905_ETH_TX_QUEUE_SIZE;
	eth_pd->rx_queue_size = MCP905_ETH_RX_QUEUE_SIZE;

#define MCP905_MV643XXX_PSCR \
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

	eth_pd->port_serial_control = MCP905_MV643XXX_PSCR;
}
#endif

static int __init
mcp905_platform_notify(struct device *dev)
{
	static struct {
		char	*bus_id;
		void	((*rtn)(struct platform_device *pdev));
	} dev_map[] = {
#if defined(CONFIG_SERIAL_MPSC)
		{ MPSC_CTLR_NAME ".0", mcp905_fixup_mpsc_pdata },
		{ MPSC_CTLR_NAME ".1", mcp905_fixup_mpsc_pdata },
#endif
#if defined(CONFIG_MV643XX_ETH)
		{ MV643XX_ETH_NAME ".0", mcp905_fixup_eth_pdata },
		{ MV643XX_ETH_NAME ".1", mcp905_fixup_eth_pdata },
		{ MV643XX_ETH_NAME ".2", mcp905_fixup_eth_pdata },
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

#if 1
static int __init
mcp905_fixup_eth_mtu(void)
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

late_initcall(mcp905_fixup_eth_mtu);
#endif

static void mcp905_restart(char *cmd)
{
	volatile ulong i = 10000000;
	u8 *status_reg_3_addr;

	local_irq_disable();
	status_reg_3_addr = ioremap(MCP905_BOARD_STATUS_REG_3, 1);
	writeb(MCP905_BOARD_RESET_MASK, status_reg_3_addr);

	while (i-- > 0) ;
	panic("restart failed\n");
}

static void mcp905_halt(void)
{
	local_irq_disable();
	while (1) ;
	/* NOT REACHED */
}

static void mcp905_power_off(void)
{
	mcp905_halt();
	/* NOT REACHED */
}

static int mcp905_show_cpuinfo(struct seq_file *m)
{
	uint pvid;
	unsigned char mcp905_geo_bits;

	pvid = mfspr(SPRN_PVR);
	seq_printf(m, "machine\t\t: MCP905\n");
	seq_printf(m, "PVID\t\t: 0x%x, vendor: %s\n",
		   pvid, (pvid & (1 << 15) ? "IBM" : "Motorola"));
	mcp905_geo_bits =
	    readb(MCP905_BOARD_GEO_ADDR_REG) & MCP905_BOARD_GEO_ADDR_MASK;
	seq_printf(m, "slot\t\t: %d\n", mcp905_geo_bits);
	return 0;
}

void __init
platform_init(unsigned long r3, unsigned long r4, unsigned long r5,
	      unsigned long r6, unsigned long r7)
{
	parse_bootinfo(find_bootinfo());

#ifdef CONFIG_BLK_DEV_INITRD
	if (r4) {
		initrd_start = r4 + KERNELBASE;
		initrd_end = r5 + KERNELBASE;
	}
#endif

	/* Copy cmd_line parameters */
	if (r6 && (((char *)r6) != '\0')) {
		*(char *)(r7 + KERNELBASE) = 0;
		strcpy(cmd_line, (char *)(r6 + KERNELBASE));
	}

	isa_io_base = MCP905_PCI_0_IO_BASE_ADDR_PROC;
	isa_mem_base = MCP905_ISA_MEM_BASE;
	pci_dram_offset = MCP905_PCI_DRAM_OFFSET;

	ppc_md.setup_arch = mcp905_setup_arch;
	ppc_md.show_cpuinfo = mcp905_show_cpuinfo;
	ppc_md.init_IRQ = mv64360_init_irq;
	ppc_md.get_irq = mv64360_get_irq;
	ppc_md.init = NULL;

	ppc_md.restart = mcp905_restart;
	ppc_md.power_off = mcp905_power_off;
	ppc_md.halt = mcp905_halt;

	ppc_md.find_end_of_memory = mcp905_find_end_of_memory;

	ppc_md.time_init = todc_time_init;
	ppc_md.set_rtc_time = todc_set_rtc_time;
	ppc_md.get_rtc_time = todc_get_rtc_time;
	ppc_md.calibrate_decr = todc_calibrate_decr;

	ppc_md.nvram_read_val = todc_direct_read_val;
	ppc_md.nvram_write_val = todc_direct_write_val;

	ppc_md.pcibios_fixup = mcp905_pci_fixups;

	bh.p_base = CONFIG_MV64X60_NEW_BASE;

	mcp905_set_bat();
	ppc_md.setup_io_mappings = mcp905_map_io;

#if !defined(CONFIG_SERIAL_MPSC_CONSOLE) && defined(CONFIG_SERIAL_8250) && \
        (defined(CONFIG_KGDB) || defined(CONFIG_SERIAL_TEXT_DEBUG))
	mcp905_early_serial_map();
#ifdef  CONFIG_SERIAL_TEXT_DEBUG
	ppc_md.progress = gen550_progress;	/* Device module UART */
#else				/* !CONFIG_SERIAL_TEXT_DEBUG */
	ppc_md.progress = NULL;
#endif
#endif

#if defined(CONFIG_SERIAL_TEXT_DEBUG) && defined(CONFIG_SERIAL_MPSC_CONSOLE)
	ppc_md.progress = mv64x60_mpsc_progress;
	mv64x60_progress_init(CONFIG_MV64X60_NEW_BASE);
#endif

#if defined(CONFIG_SERIAL_MPSC) || defined(CONFIG_MV643XX_ETH)
	platform_notify = mcp905_platform_notify;
#endif

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
	ppc_ide_md.ide_init_hwif = mcp905_ide_init_hwif_ports;
#endif

	if (ppc_md.progress)
		ppc_md.progress("mcp905_init(): exit", 0);

	return;
}
