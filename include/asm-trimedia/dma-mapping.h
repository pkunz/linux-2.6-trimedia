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

#ifndef _ASM_TRIMEDIA_DMA_MAPPING_H__
#define _ASM_TRIMEDIA_DMA_MAPPING_H__

#include <linux/types.h>
#include <asm/scatterlist.h>

static inline int dma_supported(struct device* dev,u64 mask)
{
	return 1;
}
extern int dma_set_mask(struct device* dev,u64 dma_mask);
extern void* dma_alloc_coherent(struct device* dev, size_t size, dma_addr_t* dma_handle, gfp_t flag);
extern void dma_free_coherent(struct device* dev,size_t size,void* cpu_addr,dma_addr_t dma_handle);
extern dma_addr_t dma_map_single(struct device* dev, void* cpu_addr, size_t size, 
		enum dma_data_direction direction);
extern void dma_unmap_single(struct device* dev, dma_addr_t dma_addr, size_t size, 
		enum dma_data_direction direction);
extern dma_addr_t dma_map_page(struct device *dev, struct page *page, unsigned long offset, size_t size,
	       	enum dma_data_direction direction);
extern void dma_unmap_page(struct device *dev, dma_addr_t dma_address, size_t size,
	       	enum dma_data_direction direction);
extern int dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
	       	enum dma_data_direction direction);
extern void dma_unmap_sg(struct device *dev, struct scatterlist *sg, int nhwentries,
		enum dma_data_direction direction);
extern void dma_sync_single_for_cpu(struct device *dev, dma_addr_t dma_handle, size_t size,
		enum dma_data_direction direction);
extern void dma_sync_single_for_device(struct device *dev, dma_addr_t dma_handle, size_t size,
		enum dma_data_direction direction);
extern void dma_sync_single_range_for_cpu(struct device *dev, dma_addr_t dma_handle,
		unsigned long offset, size_t size, enum dma_data_direction direction);
extern void dma_sync_single_range_for_device(struct device *dev, dma_addr_t dma_handle,
		unsigned long offset, size_t size, enum dma_data_direction direction);
extern void dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg, int nelems,
		    enum dma_data_direction direction);
extern void dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg, int nelems,
		       enum dma_data_direction direction);
extern int dma_mapping_error(dma_addr_t dma_addr);

#endif /* _ASM_TRIMEDIA_DMA_MAPPING_H__ */

