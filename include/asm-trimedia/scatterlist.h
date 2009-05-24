/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_SCATTERLIST_H__
#define _ASM_TRIMEDIA_SCATTERLIST_H__

#include <asm/types.h>
#include <asm/page.h>

struct scatterlist {
	struct page	*page;
	unsigned	offset;
	dma_addr_t	dma_address;
	unsigned	length;
};

#define ISA_DMA_THRESHOLD	(~0UL)

#define sg_dma_address(sg)      ((sg)->dma_address)
#define sg_dma_len(sg)          ((sg)->length)

#endif /* _ASM_TRIMEDIA_SCATTERLIST_H__ */

