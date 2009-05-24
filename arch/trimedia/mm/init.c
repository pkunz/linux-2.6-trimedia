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
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/bootmem.h>
#include <linux/slab.h>

#include <asm/setup.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>

#undef DEBUG

extern void die_if_kernel(char *,struct pt_regs *,long);
extern void free_initmem(void);

void show_mem(void)
{
    unsigned long i;
    int free = 0, total = 0, reserved = 0, shared = 0;
    int cached = 0;

    printk(KERN_INFO "\nMem-info:\n");
    show_free_areas();
    i = max_mapnr;
    while (i-- > 0) {
	total++;
	if (PageReserved(mem_map+i))
	    reserved++;
	else if (PageSwapCache(mem_map+i))
	    cached++;
	else if (!page_count(mem_map+i))
	    free++;
	else
	    shared += page_count(mem_map+i) - 1;
    }
    printk(KERN_INFO "%d pages of RAM\n",total);
    printk(KERN_INFO "%d free pages\n",free);
    printk(KERN_INFO "%d reserved pages\n",reserved);
    printk(KERN_INFO "%d pages shared\n",shared);
    printk(KERN_INFO "%d pages swap cached\n",cached);
}

extern unsigned long memory_start;
extern unsigned long memory_end;

/*
 * paging_init() continues the virtual memory environment setup which
 * was begun by the code in arch/head.S.
 * The parameters are pointers to where to stick the starting and ending
 * addresses of available kernel virtual memory.
 */
void paging_init(void)
{
	/*
	 * Make sure start_mem is page aligned, otherwise bootmem and
	 * page_alloc get different views of the world.
	 */
#ifdef DEBUG
	unsigned long start_mem = PAGE_ALIGN(memory_start);
#endif
	unsigned long end_mem   = memory_end & PAGE_MASK;

#ifdef DEBUG
	printk (KERN_DEBUG "start_mem is %#lx\nvirtual_end is %#lx\n",
		start_mem, end_mem);
#endif

#ifdef DEBUG
	printk (KERN_DEBUG "before free_area_init\n");

	printk (KERN_DEBUG "free_area_init -> start_mem is %#lx\nvirtual_end is %#lx\n",
		start_mem, end_mem);
#endif

	{
		unsigned long zones_size[MAX_NR_ZONES] = {0, 0, 0};

		zones_size[ZONE_DMA] = 0 >> PAGE_SHIFT;
		zones_size[ZONE_NORMAL] = (end_mem - PAGE_OFFSET) >> PAGE_SHIFT;
#ifdef CONFIG_HIGHMEM
		zones_size[ZONE_HIGHMEM] = 0;
#endif
		free_area_init(zones_size);
	}
}

void mem_init(void)
{
	unsigned long tmp;
	unsigned long len = memory_end-memory_start;
	unsigned long start_mem = memory_start; 
	unsigned long end_mem   = memory_end; 

#ifdef DEBUG
	printk(KERN_DEBUG "Mem_init: start=%lx, end=%lx\n", start_mem, end_mem);
#endif

	end_mem &= PAGE_MASK;
	high_memory = (void *) end_mem;

	start_mem = PAGE_ALIGN(start_mem);
	max_mapnr = num_physpages = (((unsigned long) high_memory) - PAGE_OFFSET) >> PAGE_SHIFT;

	/* this will put all memory onto the freelists */
	totalram_pages = free_all_bootmem();

	tmp = nr_free_pages() << PAGE_SHIFT;

	printk(KERN_INFO "Memory available: %luk/%luk RAM.\n", tmp >> 10, len >> 10);
}

/*
 * FIXME: I don't know how to implement it in TriMedia
 */
#ifdef CONFIG_BLK_DEV_INITRD
void free_initrd_mem(unsigned long start, unsigned long end)
{
}
#endif

void
free_initmem()
{
}

