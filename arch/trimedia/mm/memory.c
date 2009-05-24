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
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/slab.h>

#include <asm/setup.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/io.h>

/*
 * FIXME: Implement it according TriMedia Cache Routine.
 */

/*
 * cache_clear() semantics: Clear any cache entries for the area in question,
 * without writing back dirty entries first. This is useful if the data will
 * be overwritten anyway, e.g. by DMA to memory. The range is defined by a
 * _physical_ address.
 */

void cache_clear (unsigned long paddr, int len)
{
}

/*
 * cache_push() semantics: Write back any dirty cache data in the given area,
 * and invalidate the range in the instruction cache. It needs not (but may)
 * invalidate those entries also in the data cache. The range is defined by a
 * _physical_ address.
 */

void cache_push (unsigned long paddr, int len)
{

}


/*
 * cache_push_v() semantics: Write back any dirty cache data in the given
 * area, and invalidate those entries at least in the instruction cache. This
 * is intended to be used after data has been written that can be executed as
 * code later. The range is defined by a _user_mode_ _virtual_ address  (or,
 * more exactly, the space is defined by the %sfc/%dfc register.)
 */

void cache_push_v (unsigned long vaddr, int len)
{

}

/* Map some physical address range into the kernel address space. The
 * code is copied and adapted from map_chunk().
 */

unsigned long kernel_map(unsigned long paddr, unsigned long size,
			 int nocacheflag, unsigned long *memavailp )
{
	return paddr;
}


int is_in_rom(unsigned long addr)
{
	extern unsigned long _ramstart, _ramend;

	/*
	 *	What we are really trying to do is determine if addr is
	 *	in an allocated kernel memory region. If not then assume
	 *	we cannot free it or otherwise de-allocate it. Ideally
	 *	we could restrict this to really being in a ROM or flash,
	 *	but that would need to be done on a board by board basis,
	 *	not globally.
	 */
	if ((addr < _ramstart) || (addr >= _ramend))
		return(1);

	/* Default case, not in ROM */
	return(0);
}

