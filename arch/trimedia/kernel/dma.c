/* 
 * arch/trimedia/kernel/dma.c: Dynamic DMA mapping support for trimedia 
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/types.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/mm.h>

#include <asm/io.h>
#include <asm/page.h>

#include <tmMemUncached.h>
#include <tmCache.h>

struct dma_alloc_record {
	struct list_head	list;
	unsigned long 		phy_addr;
	void* 			virt_addr;
	unsigned long 		MemHandle;
};

static DEFINE_SPINLOCK(dma_alloc_lock);
static LIST_HEAD(dma_alloc_list);

extern void *dma_alloc_coherent(struct device *hwdev, size_t size, dma_addr_t *dma_handle, gfp_t gfp)
{
	struct dma_alloc_record *new=NULL;
	unsigned long flags;
	unsigned long alloc_size=0;
	unsigned long got_size=0;
	tmErrorCode_t ret=TM_OK;

	new = kmalloc(sizeof (*new), GFP_ATOMIC);
	if (!new){
		*dma_handle=0;
		return NULL;
	}
	memset(new,0,sizeof(struct dma_alloc_record));

	/* Round up to a page alignment*/
	alloc_size = (size+PAGE_SIZE-1) & PAGE_MASK;

	spin_lock_irqsave(&dma_alloc_lock, flags);

	ret=tmMemUncached_Malloc(alloc_size,&new->MemHandle,&new->phy_addr,&got_size);
	if(ret!=TM_OK){
		goto free_record;
	}

	new->virt_addr=phys_to_virt(new->phy_addr);
	list_add(&new->list,&dma_alloc_list);
	spin_unlock_irqrestore(&dma_alloc_lock, flags);

	*dma_handle = new->phy_addr;
	return (void *)new->virt_addr;

free_record:
	spin_unlock_irqrestore(&dma_alloc_lock, flags);
	kfree(new);
	*dma_handle=0;
	return NULL;
}

extern void dma_free_coherent(struct device *hwdev, size_t size, void *vaddr, dma_addr_t dma_handle)
{
	struct dma_alloc_record *rec;
	unsigned long flags;

	spin_lock_irqsave(&dma_alloc_lock, flags);

	list_for_each_entry(rec, &dma_alloc_list, list) {
		if((rec->virt_addr==vaddr) && (rec->phy_addr==dma_handle)){
			list_del(&rec->list);
			kfree(rec);
			spin_unlock_irqrestore(&dma_alloc_lock, flags);
			return;
		}
	}
	spin_unlock_irqrestore(&dma_alloc_lock, flags);
	BUG();
}

static inline void __dma_map_sync(void* start, size_t len, enum dma_data_direction direction)
{
	switch (direction) {
		case DMA_TO_DEVICE:
			tmCacheDataCopyBack((unsigned long)start,len);
			break;

		case DMA_FROM_DEVICE:
			break;

		case DMA_BIDIRECTIONAL:
			tmCacheDataCopyBack((unsigned long)start,len);
			break;

		default:
			BUG();
	}
}

static inline void __dma_unmap_sync(void* start, size_t len, enum dma_data_direction direction)
{
	switch (direction) {
		case DMA_TO_DEVICE:
			break;

		case DMA_FROM_DEVICE:
			tmCacheInvalidateData((unsigned long)start,len);
			break;

		case DMA_BIDIRECTIONAL:
			tmCacheInvalidateData((unsigned long)start,len);
			break;

		default:
			BUG();
	}
}

extern dma_addr_t dma_map_single(struct device *dev, void *ptr, size_t size,
		enum dma_data_direction direction)
{
	__dma_map_sync(ptr,size,direction);
	return virt_to_phys(ptr);
}

extern void dma_unmap_single(struct device* dev, dma_addr_t dma_addr, size_t size,
		enum dma_data_direction direction)
{
	__dma_unmap_sync(phys_to_virt(dma_addr),size,direction);
}

extern int dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
		enum dma_data_direction direction)
{
	int i;

	BUG_ON(direction == DMA_NONE);

	for (i = 0; i < nents; i++, sg++) {
		void* cpu_addr=NULL;

		cpu_addr= (void*)((unsigned long) page_address(sg->page)+sg->offset);
		__dma_map_sync(cpu_addr,sg->length,direction);
		sg->dma_address=(dma_addr_t)virt_to_phys(cpu_addr);
	}

	return nents;
}

extern void dma_unmap_sg(struct device *dev, struct scatterlist *sg, int nhwentries,
		enum dma_data_direction direction)
{
	unsigned long addr;
	int i;

	BUG_ON(direction == DMA_NONE);

	for (i = 0; i < nhwentries; i++, sg++) {
		__dma_unmap_sync(virt_to_phys(sg->dma_address),sg->length,direction);
	}
}

extern dma_addr_t dma_map_page(struct device *dev, struct page *page,
		unsigned long offset, size_t size, enum dma_data_direction direction)
{
	void* cpu_addr=NULL;

	BUG_ON(direction == DMA_NONE);

	cpu_addr = (void*)((unsigned long) page_address(page) + offset);
	__dma_map_sync(cpu_addr,size,direction);
	return (dma_addr_t)phys_to_virt(cpu_addr);
}

extern void dma_unmap_page(struct device *dev, dma_addr_t dma_address, size_t size,
		enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);

	__dma_unmap_sync(phys_to_virt(dma_address),size,direction);
}

extern void dma_sync_single_for_cpu(struct device *dev, dma_addr_t dma_handle,
		size_t size, enum dma_data_direction direction)
{
	unsigned long addr;

	BUG_ON(direction == DMA_NONE);

	__dma_map_sync(phys_to_virt(dma_handle),size,direction);
}

extern void dma_sync_single_for_device(struct device *dev, dma_addr_t dma_handle,
		size_t size, enum dma_data_direction direction)
{
	unsigned long addr;

	BUG_ON(direction == DMA_NONE);

	__dma_unmap_sync(phys_to_virt(dma_handle),size,direction);
}

extern void dma_sync_single_range_for_cpu(struct device *dev, dma_addr_t dma_handle,
		unsigned long offset, size_t size, enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);

	__dma_map_sync(phys_to_virt(dma_handle+offset),size,direction);
}

extern void dma_sync_single_range_for_device(struct device *dev, dma_addr_t dma_handle,
		unsigned long offset, size_t size, enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);

	__dma_unmap_sync(phys_to_virt(dma_handle+offset),size,direction);
}

extern void dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg, int nelems,
		enum dma_data_direction direction)
{
	int i;

	BUG_ON(direction == DMA_NONE);

	for (i = 0; i < nelems; i++, sg++)
		__dma_map_sync(phys_to_virt(sg->dma_address),sg->length,direction);
}

extern void dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg, int nelems,
		enum dma_data_direction direction)
{
	int i;

	BUG_ON(direction == DMA_NONE);

	for (i = 0; i < nelems; i++, sg++)
		__dma_unmap_sync(phys_to_virt(sg->dma_address),sg->length,direction);
}

extern int dma_mapping_error(dma_addr_t dma_addr)
{
	return dma_addr==0;
}

