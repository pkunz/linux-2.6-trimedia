
#ifndef __NORMAL_SCATTERLIST_H__
#define __NORMAL_SCATTERLIST_H__

struct scatterlist {
	struct page	*page;
	unsigned	offset;
	dma_addr_t	dma_address;
	unsigned	length;
};

#define ISA_DMA_THRESHOLD	(~0UL)

#endif /* __NORMAL_SCATTERLIST_H__ */

