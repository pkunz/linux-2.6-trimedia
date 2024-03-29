#ifndef _ASM_NORMAL_PCI_H
#define _ASM_NORMAL_PCI_H

#define pcibios_assign_all_busses()	0
#define pcibios_scan_all_fns(a, b)	0

extern inline void pcibios_set_master(struct pci_dev *dev)
{
	/* No special bus mastering setup handling */
}

extern inline void pcibios_penalize_isa_irq(int irq, int active)
{
	/* We don't do dynamic PCI IRQ allocation */
}

#define PCI_DMA_BUS_IS_PHYS	(1)

static inline void pcibios_add_platform_entries(struct pci_dev *dev)
{
}

#endif /* _ASM_NORMAL_PCI_H */

