/* 
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_PCI_H
#define _ASM_TRIMEDIA_PCI_H

#ifdef __KERNEL__

#include <linux/config.h>

#if defined(CONFIG_LITTLE_ENDIAN)
#define PCI_BIG_ENDIAN       0  
#define PCI_LITTLE_ENDIAN    1 
#elif defined(CONFIG_BIG_ENDIAN)
#define PCI_BIG_ENDIAN       1  
#define PCI_LITTLE_ENDIAN    0 
#else
#error "I don't know the endianess of the platform"
#endif 

struct pci_bus;
struct pci_dev;
struct pci_bus_region;
struct resource;
struct pci_dev;
struct scatterlist;
struct page;

/* note pci_set_dma_mask isn't here, since it's a public function
 * exported from drivers/pci, use dma_supported instead */

extern int pci_dma_supported(struct pci_dev *hwdev, u64 mask);
extern void * pci_alloc_consistent(struct pci_dev *hwdev, size_t size, dma_addr_t *dma_handle);
extern void pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr, dma_addr_t dma_handle);
extern dma_addr_t pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size, int direction);
extern void pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, size_t size, int direction);
extern dma_addr_t pci_map_page(struct pci_dev *hwdev, struct page *page, 
		unsigned long offset, size_t size, int direction);
extern void pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address, size_t size, int direction);
extern int pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction);
extern void pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction);
extern void pci_dma_sync_single_for_cpu(struct pci_dev *hwdev, dma_addr_t dma_handle, size_t size, int direction);
extern void pci_dma_sync_single_for_device(struct pci_dev *hwdev, dma_addr_t dma_handle, size_t size, int direction);
extern void pci_dma_sync_sg_for_cpu(struct pci_dev *hwdev, struct scatterlist *sg, int nelems, int direction);
extern void pci_dma_sync_sg_for_device(struct pci_dev *hwdev, struct scatterlist *sg, int nelems, int direction);
extern int pci_dma_mapping_error(dma_addr_t dma_addr);

/*
 * export for pci core driver. and implemented in pcibios
 */
extern int pcibios_scan_all_fns(struct pci_bus* bus,int devfn);
extern int pcibios_assign_all_busses(void);
extern void pcibios_set_master(struct pci_dev *dev);
extern void pcibios_penalize_isa_irq(int irq, int active);
extern void pcibios_add_platform_entries(struct pci_dev *dev);
extern void pcibios_resource_to_bus(struct pci_dev *dev, struct pci_bus_region *region, struct resource *res);
extern void pcibios_bus_to_resource(struct pci_dev *dev, struct resource *res, struct pci_bus_region *region);
extern struct resource * pcibios_select_root(struct pci_dev *pdev, struct resource *res);

#define PCIBIOS_MIN_IO 		0x18000000
#define PCIBIOS_MAX_IO 		0x18FFFFFF

#define PCIBIOS_MIN_MEM 	0x19000000
#define PCIBIOS_MEM_SIZE 	0x01E00000 
#define PCIBIOS_MAX_MEM 	(PCIBIOS_MIN_MEM+PCIBIOS_MEM_SIZE-1)

static inline int
is_pci_mem(const volatile void* addr)
{
	return ((unsigned long)addr >= PCIBIOS_MIN_MEM) && ((unsigned long)addr <= PCIBIOS_MAX_MEM);
}

static inline int
is_pci_ioport(unsigned long port)
{
	return (port >= PCIBIOS_MIN_IO) && (port <= PCIBIOS_MAX_IO);
}

extern unsigned long xio_region_start;
extern unsigned long xio_region_end;

static inline int
is_pcixio_port(const volatile void* addr)
{
	return ((unsigned long)addr >= xio_region_start) && ((unsigned long)addr <= xio_region_end);
}

#define PCI_DMA_BUS_IS_PHYS 	(1)

#endif 

#endif /* _ASM_TRIMEDIA_PCI_H */

