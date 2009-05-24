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

#include <linux/config.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>

extern int pci_dma_supported(struct pci_dev *hwdev, u64 mask)
{
	return dma_supported(hwdev == NULL ? NULL : &hwdev->dev, mask);
}

extern void * pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
		dma_addr_t *dma_handle)
{
	return dma_alloc_coherent(hwdev == NULL ? NULL : &hwdev->dev, size, dma_handle, GFP_ATOMIC);
}

extern void pci_free_consistent(struct pci_dev *hwdev, size_t size,
		void *vaddr, dma_addr_t dma_handle)
{
	dma_free_coherent(hwdev == NULL ? NULL : &hwdev->dev, size, vaddr, dma_handle);
}

extern dma_addr_t pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size, int direction)
{
	return dma_map_single(hwdev == NULL ? NULL : &hwdev->dev, ptr, size, (enum dma_data_direction)direction);
}

extern void pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr,
		size_t size, int direction)
{
	dma_unmap_single(hwdev == NULL ? NULL : &hwdev->dev, dma_addr, size, (enum dma_data_direction)direction);
}

extern dma_addr_t pci_map_page(struct pci_dev *hwdev, struct page *page,
		unsigned long offset, size_t size, int direction)
{
	return dma_map_page(hwdev == NULL ? NULL : &hwdev->dev, page, offset, size, (enum dma_data_direction)direction);
}

extern void pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address,
		size_t size, int direction)
{
	dma_unmap_page(hwdev == NULL ? NULL : &hwdev->dev, dma_address, size, (enum dma_data_direction)direction);
}

extern int pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg,
		int nents, int direction)
{
	return dma_map_sg(hwdev == NULL ? NULL : &hwdev->dev, sg, nents, (enum dma_data_direction)direction);
}

extern void pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg,
		int nents, int direction)
{
	dma_unmap_sg(hwdev == NULL ? NULL : &hwdev->dev, sg, nents, (enum dma_data_direction)direction);
}

extern void pci_dma_sync_single_for_cpu(struct pci_dev *hwdev, dma_addr_t dma_handle,
		size_t size, int direction)
{
	dma_sync_single_for_cpu(hwdev == NULL ? NULL : &hwdev->dev, dma_handle, size, (enum dma_data_direction)direction);
}

extern void pci_dma_sync_single_for_device(struct pci_dev *hwdev, dma_addr_t dma_handle,
		size_t size, int direction)
{
	dma_sync_single_for_device(hwdev == NULL ? NULL : &hwdev->dev, dma_handle, size, (enum dma_data_direction)direction);
}

extern void pci_dma_sync_sg_for_cpu(struct pci_dev *hwdev, struct scatterlist *sg,
		int nelems, int direction)
{
	dma_sync_sg_for_cpu(hwdev == NULL ? NULL : &hwdev->dev, sg, nelems, (enum dma_data_direction)direction);
}

extern void pci_dma_sync_sg_for_device(struct pci_dev *hwdev, struct scatterlist *sg,
		int nelems, int direction)
{
	dma_sync_sg_for_device(hwdev == NULL ? NULL : &hwdev->dev, sg, nelems, (enum dma_data_direction)direction);
}

extern int pci_dma_mapping_error(dma_addr_t dma_addr)
{
	return dma_mapping_error(dma_addr);
}
