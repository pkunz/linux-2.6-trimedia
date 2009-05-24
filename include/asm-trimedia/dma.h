/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_DMA_H__
#define _ASM_TRIMEDIA_DMA_H__

/* What should this be?  */
#define MAX_DMA_ADDRESS	0xFFFFFFFF

/* reserve a DMA channel */
static inline int request_dma (unsigned int dmanr, const char * device_id)
{
	return -EINVAL;
}

/* release it again */
static inline void free_dma (unsigned int dmanr)
{
	return ;
}

#ifdef CONFIG_PCI
extern int isa_dma_bridge_buggy;
#else
#define isa_dma_bridge_buggy    (0)
#endif

#endif /* _ASM_TRIMEDIA_DMA_H__ */

