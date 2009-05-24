/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997, 1998, 1999, 2000 Ralf Baechle ralf@gnu.org
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2002 MIPS Technologies, Inc.  All rights reserved.
 * Copyright (C) 2004,2005 Broadcom Corp
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/mmu_context.h>
#include <asm/pgtable.h>
#include <asm/system.h>

extern void build_tlb_refill_handler(void);

/*
 * Make sure all entries differ.  If they're not different
 * MIPS32 will take revenge ...
 */
#define UNIQUE_ENTRYHI(idx) (CKSEG0 + ((idx) << (PAGE_SHIFT + 1)))

/* CP0 hazard avoidance. */
#define BARRIER __asm__ __volatile__(".set noreorder\n\t" \
				     "nop; nop; nop; nop; nop; nop;\n\t" \
				     ".set reorder\n\t")

#if defined( CONFIG_MIPS_BCM7038 ) || defined( CONFIG_MIPS_BCM3560 ) \
	|| defined( CONFIG_MIPS_BCM7401 )

#ifdef CONFIG_MIPS_BCM7038A0
#include <asm/brcmstb/brcm97038/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97038/boardmap.h>

#elif defined(CONFIG_MIPS_BCM7038B0)
#include <asm/brcmstb/brcm97038b0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97038b0/boardmap.h>

#elif defined(CONFIG_MIPS_BCM7038C0)
#include <asm/brcmstb/brcm97038c0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97038c0/boardmap.h>

#elif defined(CONFIG_MIPS_BCM3560)
#include <asm/brcmstb/brcm93560/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm93560/boardmap.h>

#elif defined(CONFIG_MIPS_BCM7401A0)
#include <asm/brcmstb/brcm97401a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97401a0/boardmap.h>


#else
#error "Unsupported Chip revision"
#endif

//#define PCI_MEM_WIN_BASE 0xd0000000
//#define CPU2PCI_CPU_PHYS_MEM_WIN_BASE 0xd0000000

#define OFFSET_16MBYTES       0x1000000
#define OFFSET_32MBYTES       0x2000000

#define TLBLO_OFFSET_32MBYTES (0x2000000>>(4+2))

#define PCI_BUS_MASTER  BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_MASK
#define PCI_IO_ENABLE   BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_MASK
#define PCI_MEM_ENABLE  BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_MASK

//#define PCI_DEVICE_ID_SATA      0x0e
//#define PCI_DEVICE_ID_EXT       0x0d
//#define PCI_DEVICE_ID_7041      0x04
//#define PCI_DEVICE_ID_3250      0x01

//#define PCI_IDSEL_EXT           (0x10000 << PCI_DEVICE_ID_EXT)
//#define PCI_IDSEL_7041          (0x10000 << PCI_DEVICE_ID_7041)
//#define PCI_IDSEL_3250          (0x10000 << PCI_DEVICE_ID_3250)
//#define PCI_IDSEL_SATA          (0x10000 << PCI_DEVICE_ID_SATA)

//#define PCI_DEV_NUM_EXT         (PCI_DEVICE_ID_EXT  << 11)
//#define PCI_DEV_NUM_7041        (PCI_DEVICE_ID_7041 << 11)
//#define PCI_DEV_NUM_3250        (PCI_DEVICE_ID_3250 << 11)
//#define PCI_DEV_NUM_SATA        (PCI_DEVICE_ID_SATA << 11)

void local_init_tlb(void)
{
	unsigned long flags;
	unsigned long old_ctx;
	unsigned long lo0, lo1, hi;
	int entry, wired;

#ifdef DEBUG_TLB
	printk("[tlball]");
#endif
	write_c0_pagemask(PM_16M); /*max we can get. */

	local_irq_save(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = (read_c0_entryhi() & 0xff);
	hi = (PCI_MEM_WIN_BASE&0xffffe000);
	lo0 = (((CPU2PCI_CPU_PHYS_MEM_WIN_BASE>>(4+2))&0x3fffffc0)|0x17);
	lo1 = ((((CPU2PCI_CPU_PHYS_MEM_WIN_BASE+OFFSET_16MBYTES)>>(4+2))&0x3fffffc0)|0x17);

	// Save the start entry presumably starting at 0, but we never know
	entry = wired = read_c0_wired();

	/* Blast 'em all away. */
	while (entry < 17+wired) {
		/*
		 * Make sure all entries differ.  If they're not different
		 * MIPS32 will take revenge ...
		 */
		write_c0_entrylo0(lo0);
		write_c0_entrylo1(lo1);
		BARRIER;
		write_c0_entryhi(hi);
		write_c0_index(entry);
		BARRIER;
		tlb_write_indexed();
		BARRIER;
		hi += OFFSET_32MBYTES;
		lo0 += TLBLO_OFFSET_32MBYTES;
		lo1 += TLBLO_OFFSET_32MBYTES;
		entry++;
	}
// THT: May be we should reduce the last wired entry to just 2x4K page, as 32MB is a waste
	BARRIER;
	write_c0_entryhi(old_ctx);
	// THT: Write it the wired entries here, before releasing the lock
	write_c0_wired(wired+17);
		write_c0_pagemask(PM_4K);
	local_irq_restore(flags);
}
#endif    /* CONFIG_MIPS_BCM7038/3560/7401 */

void local_flush_tlb_all(void)
{
	unsigned long flags;
	unsigned long old_ctx;
	int entry;

#ifdef DEBUG_TLB
	printk("[tlball]");
#endif

	local_irq_save(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = read_c0_entryhi();
	write_c0_entrylo0(0);
	write_c0_entrylo1(0);

	entry = read_c0_wired();

	/* Blast 'em all away. */
	while (entry < current_cpu_data.tlbsize) {
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(entry));
		write_c0_index(entry);
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		entry++;
	}
	tlbw_use_hazard();
	write_c0_entryhi(old_ctx);
	local_irq_restore(flags);
}

/* All entries common to a mm share an asid.  To effectively flush
   these entries, we just bump the asid. */
void local_flush_tlb_mm(struct mm_struct *mm)
{
	int cpu;

	preempt_disable();

	cpu = smp_processor_id();

	if (cpu_context(cpu, mm) != 0) {
#ifdef DEBUG_TLB
		printk("[tlbmm<%d>]", cpu_context(cpu, mm));
#endif
		drop_mmu_context(mm, cpu);
	}

	preempt_enable();
}

void local_flush_tlb_range(struct vm_area_struct *vma, unsigned long start,
	unsigned long end)
{
	struct mm_struct *mm = vma->vm_mm;
	int cpu = smp_processor_id();

	if (cpu_context(cpu, mm) != 0) {
		unsigned long flags;
		int size;

#ifdef DEBUG_TLB
		printk("[tlbrange<%02x,%08lx,%08lx>]",
		       cpu_asid(cpu, mm), start, end);
#endif
		size = (end - start + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
		size = (size + 1) >> 1;
		local_irq_save(flags);
		if (size <= current_cpu_data.tlbsize/2) {
			int oldpid = read_c0_entryhi();
			int newpid = cpu_asid(cpu, mm);

			start &= (PAGE_MASK << 1);
			end += ((PAGE_SIZE << 1) - 1);
			end &= (PAGE_MASK << 1);
			while (start < end) {
				int idx;

				write_c0_entryhi(start | newpid);
				start += (PAGE_SIZE << 1);
				mtc0_tlbw_hazard();
				tlb_probe();
				BARRIER;
				idx = read_c0_index();
				write_c0_entrylo0(0);
				write_c0_entrylo1(0);
				if (idx < 0)
					continue;
				/* Make sure all entries differ. */
				write_c0_entryhi(UNIQUE_ENTRYHI(idx));
				mtc0_tlbw_hazard();
				tlb_write_indexed();
			}
			tlbw_use_hazard();
			write_c0_entryhi(oldpid);
		} else {
			drop_mmu_context(mm, cpu);
		}
		local_irq_restore(flags);
	}
}

void local_flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	unsigned long flags;
	int size;

	size = (end - start + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
	size = (size + 1) >> 1;
	local_irq_save(flags);
	if (size <= current_cpu_data.tlbsize / 2) {
		int pid = read_c0_entryhi();

		start &= (PAGE_MASK << 1);
		end += ((PAGE_SIZE << 1) - 1);
		end &= (PAGE_MASK << 1);

		while (start < end) {
			int idx;

			write_c0_entryhi(start);
			start += (PAGE_SIZE << 1);
			mtc0_tlbw_hazard();
			tlb_probe();
			BARRIER;
			idx = read_c0_index();
			write_c0_entrylo0(0);
			write_c0_entrylo1(0);
			if (idx < 0)
				continue;
			/* Make sure all entries differ. */
			write_c0_entryhi(UNIQUE_ENTRYHI(idx));
			mtc0_tlbw_hazard();
			tlb_write_indexed();
		}
		tlbw_use_hazard();
		write_c0_entryhi(pid);
	} else {
		local_flush_tlb_all();
	}
	local_irq_restore(flags);
}

void local_flush_tlb_page(struct vm_area_struct *vma, unsigned long page)
{
	int cpu = smp_processor_id();

	if (cpu_context(cpu, vma->vm_mm) != 0) {
		unsigned long flags;
		int oldpid, newpid, idx;

#ifdef DEBUG_TLB
		printk("[tlbpage<%d,%08lx>]", cpu_context(cpu, vma->vm_mm),
		       page);
#endif
		newpid = cpu_asid(cpu, vma->vm_mm);
		page &= (PAGE_MASK << 1);
		local_irq_save(flags);
		oldpid = read_c0_entryhi();
		write_c0_entryhi(page | newpid);
		mtc0_tlbw_hazard();
		tlb_probe();
		BARRIER;
		idx = read_c0_index();
		write_c0_entrylo0(0);
		write_c0_entrylo1(0);
		if (idx < 0)
			goto finish;
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(idx));
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		tlbw_use_hazard();

	finish:
		write_c0_entryhi(oldpid);
		local_irq_restore(flags);
	}
}

/*
 * This one is only used for pages with the global bit set so we don't care
 * much about the ASID.
 */
void local_flush_tlb_one(unsigned long page)
{
	unsigned long flags;
	int oldpid, idx;

	local_irq_save(flags);
	oldpid = read_c0_entryhi();
	page &= (PAGE_MASK << 1);
	write_c0_entryhi(page);
	mtc0_tlbw_hazard();
	tlb_probe();
	BARRIER;
	idx = read_c0_index();
	write_c0_entrylo0(0);
	write_c0_entrylo1(0);
	if (idx >= 0) {
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(idx));
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		tlbw_use_hazard();
	}
	write_c0_entryhi(oldpid);

	local_irq_restore(flags);
}

/*
 * We will need multiple versions of update_mmu_cache(), one that just
 * updates the TLB with the new pte(s), and another which also checks
 * for the R4k "end of page" hardware bug and does the needy.
 */
void __update_tlb(struct vm_area_struct * vma, unsigned long address, pte_t pte)
{
	unsigned long flags;
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;
	int idx, pid;

	/*
	 * Handle debugger faulting in for debugee.
	 */
	if (current->active_mm != vma->vm_mm)
		return;

	local_irq_save(flags);

	pid = read_c0_entryhi() & ASID_MASK;
	address &= (PAGE_MASK << 1);
	write_c0_entryhi(address | pid);
	pgdp = pgd_offset(vma->vm_mm, address);
	mtc0_tlbw_hazard();
	tlb_probe();
	BARRIER;
	pudp = pud_offset(pgdp, address);
	pmdp = pmd_offset(pudp, address);
	idx = read_c0_index();
	ptep = pte_offset_map(pmdp, address);

#if defined(CONFIG_64BIT_PHYS_ADDR) && defined(CONFIG_CPU_MIPS32_R1)
	write_c0_entrylo0(ptep->pte_high);
	ptep++;
	write_c0_entrylo1(ptep->pte_high);
#else
	write_c0_entrylo0(pte_val(*ptep++) >> 6);
	write_c0_entrylo1(pte_val(*ptep) >> 6);
#endif
	mtc0_tlbw_hazard();
	if (idx < 0)
		tlb_write_random();
	else
		tlb_write_indexed();
	tlbw_use_hazard();
	local_irq_restore(flags);
}

#if 0
static void r4k_update_mmu_cache_hwbug(struct vm_area_struct * vma,
				       unsigned long address, pte_t pte)
{
	unsigned long flags;
	unsigned int asid;
	pgd_t *pgdp;
	pmd_t *pmdp;
	pte_t *ptep;
	int idx;

	local_irq_save(flags);
	address &= (PAGE_MASK << 1);
	asid = read_c0_entryhi() & ASID_MASK;
	write_c0_entryhi(address | asid);
	pgdp = pgd_offset(vma->vm_mm, address);
	mtc0_tlbw_hazard();
	tlb_probe();
	BARRIER;
	pmdp = pmd_offset(pgdp, address);
	idx = read_c0_index();
	ptep = pte_offset_map(pmdp, address);
	write_c0_entrylo0(pte_val(*ptep++) >> 6);
	write_c0_entrylo1(pte_val(*ptep) >> 6);
	mtc0_tlbw_hazard();
	if (idx < 0)
		tlb_write_random();
	else
		tlb_write_indexed();
	tlbw_use_hazard();
	local_irq_restore(flags);
}
#endif

void __init add_wired_entry(unsigned long entrylo0, unsigned long entrylo1,
	unsigned long entryhi, unsigned long pagemask)
{
	unsigned long flags;
	unsigned long wired;
	unsigned long old_pagemask;
	unsigned long old_ctx;

	local_irq_save(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = read_c0_entryhi();
	old_pagemask = read_c0_pagemask();
	wired = read_c0_wired();
	write_c0_wired(wired + 1);
	write_c0_index(wired);
	BARRIER;
	write_c0_pagemask(pagemask);
	write_c0_entryhi(entryhi);
	write_c0_entrylo0(entrylo0);
	write_c0_entrylo1(entrylo1);
	mtc0_tlbw_hazard();
	tlb_write_indexed();
	tlbw_use_hazard();

	write_c0_entryhi(old_ctx);
	BARRIER;
	write_c0_pagemask(old_pagemask);
	local_flush_tlb_all();
	local_irq_restore(flags);
}

/*
 * Used for loading TLB entries before trap_init() has started, when we
 * don't actually want to add a wired entry which remains throughout the
 * lifetime of the system
 */

static int temp_tlb_entry __initdata;

__init int add_temporary_entry(unsigned long entrylo0, unsigned long entrylo1,
			       unsigned long entryhi, unsigned long pagemask)
{
	int ret = 0;
	unsigned long flags;
	unsigned long wired;
	unsigned long old_pagemask;
	unsigned long old_ctx;

	local_irq_save(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = read_c0_entryhi();
	old_pagemask = read_c0_pagemask();
	wired = read_c0_wired();
	if (--temp_tlb_entry < wired) {
		printk(KERN_WARNING
		       "No TLB space left for add_temporary_entry\n");
		ret = -ENOSPC;
		goto out;
	}

	write_c0_index(temp_tlb_entry);
	write_c0_pagemask(pagemask);
	write_c0_entryhi(entryhi);
	write_c0_entrylo0(entrylo0);
	write_c0_entrylo1(entrylo1);
	mtc0_tlbw_hazard();
	tlb_write_indexed();
	tlbw_use_hazard();

	write_c0_entryhi(old_ctx);
	write_c0_pagemask(old_pagemask);
out:
	local_irq_restore(flags);
	return ret;
}

static void __init probe_tlb(unsigned long config)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int reg;

	/*
	 * If this isn't a MIPS32 / MIPS64 compliant CPU.  Config 1 register
	 * is not supported, we assume R4k style.  Cpu probing already figured
	 * out the number of tlb entries.
	 */
	if ((c->processor_id & 0xff0000) == PRID_COMP_LEGACY)
		return;

	reg = read_c0_config1();
	if (!((config >> 7) & 3))
		panic("No TLB present");

	c->tlbsize = ((reg >> 25) & 0x3f) + 1;
}

void __init tlb_init(void)
{
	unsigned int config = read_c0_config();

	/*
	 * You should never change this register:
	 *   - On R4600 1.7 the tlbp never hits for pages smaller than
	 *     the value in the c0_pagemask register.
	 *   - The entire mm handling assumes the c0_pagemask register to
	 *     be set for 4kb pages.
	 */
	probe_tlb(config);
	write_c0_pagemask(PM_DEFAULT_MASK);
	write_c0_wired(0);
	temp_tlb_entry = current_cpu_data.tlbsize - 1;
	local_flush_tlb_all();


#if defined( CONFIG_MIPS_BCM3560)
	local_init_tlb();
	// THT Done in local_init_tlb write_c0_wired(17);

// QY & THT: 11/13/03 This is here because CFE does not do it
	// first set up pci host.
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_STATUS_COMMAND)) |= (PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0)) = CPU2PCI_PCI_PHYS_MEM_WIN0_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1)) = CPU2PCI_PCI_PHYS_MEM_WIN1_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2)) = CPU2PCI_PCI_PHYS_MEM_WIN2_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3)) = CPU2PCI_PCI_PHYS_MEM_WIN3_BASE;

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400000;
#else
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400002;
#endif

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_GISB_BASE_W)) = 0x10000000;

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) = 0x00000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W1)) = 0x02000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W2)) = 0x04000000;

#ifndef CONFIG_CPU_LITTLE_ENDIAN
	// TDT - Swap memory base w0 when running big endian
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		&= ~BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		|= 2<<BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT;
#endif

{
#define PCI_ENABLE              0x80000000
#define PCI_IDSEL(x)		(((x)&0x1f)<<11)
#define PCI_FNCSEL(x)		(((x)&0x7)<<8)

	// do a pci config read.
	*((volatile unsigned long *)0xf0600004) = PCI_ENABLE+PCI_IDSEL(PCI_DEVICE_ID_USB)+PCI_FNCSEL(0); 
	printk("$$$$$$$$$$usb fn0 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_ENABLE+PCI_IDSEL(PCI_DEVICE_ID_USB)+PCI_FNCSEL(1); 
	printk("$$$$$$$$$$usb fn1 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_ENABLE+PCI_IDSEL(PCI_DEVICE_ID_USB)+PCI_FNCSEL(2); 
	printk("$$$$$$$$$$usb fn2 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	
	*((volatile unsigned long *)0xf0600004) = PCI_ENABLE+PCI_IDSEL(PCI_DEVICE_ID_1394)+PCI_FNCSEL(0); 
	printk("$$$$$$$$$$1394 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_ENABLE+PCI_IDSEL(PCI_DEVICE_ID_EXT)+PCI_FNCSEL(0);
	printk("$$$$$$$$$$external dev id %08x\n", *((volatile unsigned long *)0xf0600008));
}

#elif defined( CONFIG_MIPS_BCM7038A0 ) 
	local_init_tlb();
	// THT DOne in local_init_tlb write_c0_wired(17);

// QY & THT: 11/13/03 This is here because CFE does not do it
	// first set up pci host.
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_STATUS_COMMAND)) |= (PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2)) = 0xd0000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3)) = 0xd8000000;

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400000;
#else
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400002;
#endif

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_GISB_BASE_W)) = 0x10000000;

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) = 0x00000000;
	// QY: PR10078 Enable only 1 window, and use board strapping to enable the entire 128MB range.
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W1)) = 0x02000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W2)) = 0x04000000;

#ifndef CONFIG_CPU_LITTLE_ENDIAN
	// TDT - Swap memory base w0 when running big endian
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL)) 
		&= ~BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL)) 
		|= 2<<BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT;
#endif

	// do a pci config read.
	*((volatile unsigned long *)0xe0600004) = PCI_DEV_NUM_7041;
	printk("$$$$$$$$$$7041 dev id %08x\n", *((volatile unsigned long *)0xe0600008));
	*((volatile unsigned long *)0xe0600004) = PCI_DEV_NUM_3250;
	printk("$$$$$$$$$$3250 dev id %08x\n", *((volatile unsigned long *)0xe0600008));
	*((volatile unsigned long *)0xe0600004) = PCI_DEV_NUM_SATA;
	printk("$$$$$$$$$$SATA dev id %08x\n", *((volatile unsigned long *)0xe0600008));

#elif defined( CONFIG_MIPS_BCM7038B0)
	local_init_tlb();
	// THT Done in local_init_tlb write_c0_wired(17);

// QY & THT: 11/13/03 This is here because CFE does not do it
	// first set up pci host.
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_STATUS_COMMAND)) |= (PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0)) = CPU2PCI_PCI_PHYS_MEM_WIN0_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1)) = CPU2PCI_PCI_PHYS_MEM_WIN1_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2)) = CPU2PCI_PCI_PHYS_MEM_WIN2_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3)) = CPU2PCI_PCI_PHYS_MEM_WIN3_BASE;

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400000;
#else
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400002;
#endif

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_GISB_BASE_W)) = 0x10000000;

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) = 0x00000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W1)) = 0x02000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W2)) = 0x04000000;

#ifndef CONFIG_CPU_LITTLE_ENDIAN
	// TDT - Swap memory base w0 when running big endian
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		&= ~BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		|= 2<<BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT;
#endif

	// do a pci config read.
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_7041;
	printk("$$$$$$$$$$7041 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_3250;
	printk("$$$$$$$$$$3250 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_SATA;
	printk("$$$$$$$$$$SATA dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_EXT;
	printk("$$$$$$$$$$external dev id %08x\n", *((volatile unsigned long *)0xf0600008));

#elif defined( CONFIG_MIPS_BCM7038C0)
	local_init_tlb();
	// THT Done in local_init_tlb write_c0_wired(17);
    
// QY & THT: 11/13/03 This is here because CFE does not do it
	// first set up pci host.
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_STATUS_COMMAND)) |= (PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0)) = CPU2PCI_PCI_PHYS_MEM_WIN0_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1)) = CPU2PCI_PCI_PHYS_MEM_WIN1_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2)) = CPU2PCI_PCI_PHYS_MEM_WIN2_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3)) = CPU2PCI_PCI_PHYS_MEM_WIN3_BASE;

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400000;
#else
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400002;
#endif

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_GISB_BASE_W)) = 0x10000000;

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) = 0x00000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W1)) = 0x02000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W2)) = 0x04000000;

#ifndef CONFIG_CPU_LITTLE_ENDIAN
	// TDT - Swap memory base w0 when running big endian
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		&= ~BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		|= 2<<BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT;
#endif

	// do a pci config read.
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_7041;
	printk("$$$$$$$$$$7041 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_3250;
	printk("$$$$$$$$$$3250 dev id %08x\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_EXT;
	printk("$$$$$$$$$$external dev id %08x\n", *((volatile unsigned long *)0xf0600008));

/* 2nd PCI Bridge for SATA on 7038C0 */
/*
 * Set BCM7038 PCI Bus Bridge Command
 */

//#define PCI_BUS_MASTER  BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_MASK
//#define PCI_IO_ENABLE   BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_MASK
//#define PCI_MEM_ENABLE  BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_MASK

#define BCHP_SATA_PHYSICAL_OFFSET         0x10500000
#define BCHP_SATA_KSEG1_OFFSET			KSEG1ADDR(BCHP_SATA_PHYSICAL_OFFSET)

#define BCHP_SATA_PCI_BRIDGE_PCI_CTRL     0x200
#define PCI_SATA_MEM_ENABLE               1
#define PCI_SATA_BUS_MASTER_ENABLE        2
#define PCI_SATA_PERR_ENABLE              0x10
#define PCI_SATA_SERR_ENABLE              0x20

	// first set up pci host.
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_SATA_PCI_BRIDGE_PCI_CTRL)) |=
		(PCI_SATA_MEM_ENABLE|PCI_SATA_BUS_MASTER_ENABLE|PCI_SATA_PERR_ENABLE|PCI_SATA_SERR_ENABLE);

	 /************************************************
	  * Configure 7038C0 PCI config registers
	  ************************************************/

#define  BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0 0x20c

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0)) 
		= PCI_7038_PHYS_MEM_WIN0_BASE;
#else
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0)) 
		= PCI_7038_PHYS_MEM_WIN0_BASE | 2;
#endif

	/************************************************
    * Configure MIPS to PCI bridge.  
    ************************************************/
#define BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0 0x210
#define BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0  0x214

#define CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE  0x10510000
#define CPU2PCI_PCI_SATA_PHYS_IO_WIN0_BASE   0x10520000

#ifdef CONFIG_CPU_LITTLE_ENDIAN
 	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0)) 
 		= CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE;
 	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0)) 
 		= 0;

#else

/* Using little endian byte order on disk */
	*((volatile unsigned long *) (BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0))
		= CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE | 0;
	


 /* DW swap */
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0))
		= 2;

#endif // ifdef Little Endian

		// do a pci config read.
	*((volatile unsigned long *)0xb0500204) = PCI_DEV_NUM_SATA;
	printk("$$$$$$$$$$SATA dev id %08x\n", *((volatile unsigned long *)0xb0500208));


#elif defined (CONFIG_MIPS_BCM7329)
#define PCI_CFG_MEMORY_BASE_W 0xbafe9010
#define PCI_CFG_STATUS_COMMAND 0xbafe9004
#define PCI_CFG_CPU_2_PCI_MEM_WIN 0xbafe9050
#define PCI_CFG_CPU_2_PCI_IO_WIN 0xbafe9060
#define CPU2PCI_PCI_PHYS_IO_WIN0_BASE 0x1af90000
#define CPU2PCI_PCI_PHYS_MEM_WIN0_BASE 0x1afa0000
#define MIPS_PCI_XCFG_INDEX     0xbafe9100
#define MIPS_PCI_XCFG_DATA      0xbafe9104
#define PCI_REG_PCI_CTRL		0xbafe9110
#define PCI_REG_PCI_INTR_STA_CLR 0xbafe910c


#define MTN_RB_MMC_CLIENT_INDEX	0xbae0000c
#define MTN_RB_MMC_INDX_DATA_WR	0xbae00010
#define MTN_RB_MMC_INDX_DATA_RD	0xbae00014
#define SATA_PCI_BRIDGE_CLIENT_ID	17
	
{
	char msg[256];

#if 1
/* Attempt to give SATA higher memory bandwidth */
	volatile unsigned long temp;
/* Debugging Memory priority */

#if 0
	int i;

	for (i=0; i <=31; i++) {
		if (i == 2 || i == 28) continue; // Unused
		*((volatile unsigned long *)MTN_RB_MMC_CLIENT_INDEX) = i;
		printk("priority[%d] = %08x\n", i, *((volatile unsigned long *)MTN_RB_MMC_INDX_DATA_RD));
	}

#endif // if 2

*((volatile unsigned long *)MTN_RB_MMC_CLIENT_INDEX) = SATA_PCI_BRIDGE_CLIENT_ID;
temp = *((volatile unsigned long *)MTN_RB_MMC_INDX_DATA_RD);
printk("SATA/PCI Memory cfg = %08x\n", temp);

// Enable Round Robin bit

// Old value of 0x40d0 yield a "hdparm -t" score of 8.5MB/s
temp |= 0x20; /* Enable Round Robin bit 5, yield 13.9 MB/s */

// Write it back
*((volatile unsigned long *)MTN_RB_MMC_CLIENT_INDEX) = SATA_PCI_BRIDGE_CLIENT_ID;
*((volatile unsigned long *)MTN_RB_MMC_INDX_DATA_WR) = temp;

#endif

//TM_SOFT_RESET_2 - Soft Reset Control 2 Register, reset everything except SATA
*((volatile unsigned char *)0xbafe8026) &= 0xfb;
//TM_SATA_CTRL - SATA CTRL Register
*((volatile unsigned char *)0xbafe8044) |= 0x05;//0x01; 
sprintf(msg, "$$$TM_SATA_CTRL after %02x\n", *((volatile unsigned char *)0xbafe8044));
uart_puts(msg);
// Now reset SATA
*((volatile unsigned char *)0xbafe8026) |= 0x04;
sprintf(msg, "$$$TM_reset_2 %02x\n", *((volatile unsigned char *)0xbafe8026));
uart_puts(msg);
}

printk("$$$PBus stat %08x\n", *((volatile unsigned long *)0xbafe0900));
*((volatile unsigned long *)0xbafe0900) |= 0x7fff;
printk("$$$PBus stat after %08x\n", *((volatile unsigned long *)0xbafe0900));


	*((volatile unsigned long *)PCI_CFG_STATUS_COMMAND) |= 0x07;
	*((volatile unsigned long *)PCI_CFG_MEMORY_BASE_W) = 0x00000000;
	*((volatile unsigned long *)PCI_CFG_CPU_2_PCI_MEM_WIN) = CPU2PCI_PCI_PHYS_MEM_WIN0_BASE;
	*((volatile unsigned long *)PCI_CFG_CPU_2_PCI_IO_WIN) = 0x00000000;
	//*((volatile unsigned long *)PCI_REG_PCI_CTRL) |= 0x04;

#elif defined( CONFIG_MIPS_BCM7401A0)
	local_init_tlb();
	// THT Done in local_init_tlb write_c0_wired(17);
    
// QY & THT: 11/13/03 This is here because CFE does not do it
	// first set up pci host.
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_STATUS_COMMAND)) |= (PCI_BUS_MASTER|PCI_IO_ENABLE|PCI_MEM_ENABLE);
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN0)) = CPU2PCI_PCI_PHYS_MEM_WIN0_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN1)) = CPU2PCI_PCI_PHYS_MEM_WIN1_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN2)) = CPU2PCI_PCI_PHYS_MEM_WIN2_BASE;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_MEM_WIN3)) = CPU2PCI_PCI_PHYS_MEM_WIN3_BASE;

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400000;
#else
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN0)) = 0x00000002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN1)) = 0x00200002;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_CPU_2_PCI_IO_WIN2)) = 0x00400002;
#endif

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_GISB_BASE_W)) = 0x10000000;

	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W0)) = 0x00000000;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W1)) = 0x08000000;
	//*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_MEMORY_BASE_W2)) = 0x04000000;

#ifndef CONFIG_CPU_LITTLE_ENDIAN
	// TDT - Swap memory base w0 when running big endian
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		&= ~BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_MASK;
	*((volatile unsigned long *)(0xb0000000+BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL))
		|= 2<<BCHP_PCI_CFG_PCI_SDRAM_ENDIAN_CTRL_ENDIAN_MODE_MWIN0_SHIFT;
#endif

	// do a pci config read.
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_1394;
	printk("$$$$$$$$$$ 1394 dev id %08lx\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_MINI;
	printk("$$$$$$$$$$ mini slot dev id %08lx\n", *((volatile unsigned long *)0xf0600008));
	*((volatile unsigned long *)0xf0600004) = PCI_DEV_NUM_EXT;
	printk("$$$$$$$$$$ external dev id %08lx\n", *((volatile unsigned long *)0xf0600008));

/* 2nd PCI Bridge for SATA on 7038C0 */
/*
 * Set BCM7038 PCI Bus Bridge Command
 */

//#define PCI_BUS_MASTER  BCHP_PCI_CFG_STATUS_COMMAND_BUS_MASTER_MASK
//#define PCI_IO_ENABLE   BCHP_PCI_CFG_STATUS_COMMAND_MEMORY_SPACE_MASK
//#define PCI_MEM_ENABLE  BCHP_PCI_CFG_STATUS_COMMAND_IO_SPACE_MASK

#define BCHP_SATA_PHYSICAL_OFFSET         0x10500000
#define BCHP_SATA_KSEG1_OFFSET			KSEG1ADDR(BCHP_SATA_PHYSICAL_OFFSET)

#define BCHP_SATA_PCI_BRIDGE_PCI_CTRL     0x200
#define PCI_SATA_MEM_ENABLE               1
#define PCI_SATA_BUS_MASTER_ENABLE        2
#define PCI_SATA_PERR_ENABLE              0x10
#define PCI_SATA_SERR_ENABLE              0x20

	// first set up pci host.
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_SATA_PCI_BRIDGE_PCI_CTRL)) |=
		(PCI_SATA_MEM_ENABLE|PCI_SATA_BUS_MASTER_ENABLE|PCI_SATA_PERR_ENABLE|PCI_SATA_SERR_ENABLE);

	 /************************************************
	  * Configure 7038C0 PCI config registers
	  ************************************************/

#define  BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0 0x20c

#ifdef CONFIG_CPU_LITTLE_ENDIAN
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0)) 
		= PCI_7401_PHYS_MEM_WIN0_BASE;
#else
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_SLV_MEMORY_BASE_W0)) 
		= PCI_7401_PHYS_MEM_WIN0_BASE | 2;
#endif

	/************************************************
    * Configure MIPS to PCI bridge.  
    ************************************************/
#define BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0 0x210
#define BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0  0x214

#define CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE  0x10510000

#ifdef CONFIG_CPU_LITTLE_ENDIAN
 	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0)) 
 		= CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE;
 	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0)) 
 		= 0;

#else

/* Using little endian byte order on disk */
	*((volatile unsigned long *) (BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_MEM_WIN0))
		= CPU2PCI_PCI_SATA_PHYS_MEM_WIN0_BASE | 0;
	


 /* DW swap */
	*((volatile unsigned long *)(BCHP_SATA_KSEG1_OFFSET+BCHP_PCI_SATA_CFG_CPU_2_PCI_IO_WIN0))
		= 2;

#endif // ifdef Little Endian

		// do a pci config read.
	*((volatile unsigned long *)0xb0500204) = PCI_DEV_NUM_SATA;
	printk("$$$$$$$$$$SATA dev id %08lx\n", *((volatile unsigned long *)0xb0500208));
	
#endif

	build_tlb_refill_handler();
}
