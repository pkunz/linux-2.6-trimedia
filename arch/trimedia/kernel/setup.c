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
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/fb.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/genhd.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/major.h>
#include <linux/bootmem.h>
#include <linux/seq_file.h>
#include <linux/root_dev.h>
#include <linux/init.h>

#include <asm/setup.h>
#include <asm/irq.h>

#ifdef CONFIG_BLK_DEV_INITRD
#include <asm/pgtable.h>
#endif

#undef DEBUG

/*
 *The following symbole was defined by bootloader.
 */
extern int _ramstart, _ramend;

unsigned long memory_start;
unsigned long memory_end;

EXPORT_SYMBOL(memory_start);
EXPORT_SYMBOL(memory_end);

char command_line[COMMAND_LINE_SIZE];

#define AUTHOR "guo.hongruan@gulessoft.com"

#ifdef CONFIG_TM1300
#define TARGET  "tm1300"
#endif
#ifdef CONFIG_PNX1500
#define TARGET  "pnx1500"
#endif
#ifdef CONFIG_PNX1700
#define TARGET "pnx1700"
#endif 
#ifdef CONFIG_PNX1005
#define TARGET "pnx1005"
#endif 

#ifndef TARGET
#define TARGET  "Unknown"
#endif 

#ifdef CONFIG_NOHOST
#define HOST "nohost"
#endif 
#ifdef CONFIG_OLD_TMSIM
#define HOST "tmsim"
#endif
#ifdef CONFIG_TTISIM
#define HOST "ttisim"
#endif 
#ifdef CONFIG_WINNT
#define HOST "winnt"
#endif 

#ifndef HOST
#define HOST "Unknown"
#endif 

#ifdef CONFIG_FP32
#define FP  "32"
#endif 
#ifdef CONFIG_FP64
#define FP  "64"
#endif

#ifndef FP
#define FP "Unknown"
#endif 

#ifdef CONFIG_LITTLE_ENDIAN
#define ENDIAN "Little endian"
#endif 
#ifdef CONFIG_BIG_ENDIAN
#define ENDIAN "Big endian"
#endif 

#ifndef ENDIAN
#define ENDIAN "Unknown"
#endif 

extern int tmboard_activate(void); 

static long trimedia_panic(long time)
{
	local_irq_disable();
	while(1)
		exit(11223344);	
}

void setup_arch(char **cmdline_p)
{
	int bootmap_size;
	int ret=0;

	ret=tmboard_activate();
	if(ret!=0){
		panic("tmboard_activate failed!\n");
	}

	panic_blink=trimedia_panic;

	memory_start = PAGE_ALIGN(_ramstart);
	memory_end = _ramend & PAGE_MASK; /* by now the stack is part of the init task */

	/*
	 *FIXME: I don't know the object format of trimedia executable.
	 */
#if 0
	init_mm.start_code = (unsigned long) &_stext;
	init_mm.end_code = (unsigned long) &_etext;
	init_mm.end_data = (unsigned long) &_edata;
	init_mm.brk = (unsigned long) 0;
#else
	init_mm.start_code = 0;
	init_mm.end_code =  0;
	init_mm.end_data =  0;
	init_mm.brk = 0;
	
#endif 
	
	printk(KERN_INFO "tmlinux "TARGET" on "HOST" with "FP" bit float point ("ENDIAN") supported by "AUTHOR"\n");

	/* Keep a copy of command line */
	*cmdline_p = &command_line[0];
	memcpy(saved_command_line, command_line, COMMAND_LINE_SIZE);
	saved_command_line[COMMAND_LINE_SIZE-1] = 0;

#ifdef DEBUG
	if (strlen(*cmdline_p))
		printk(KERN_DEBUG "Command line: '%s'\n", *cmdline_p);
#endif 
	
	/*
	 * Give all the memory to the bootmap allocator, tell it to put the
	 * boot mem_map at the start of memory.
	 */
	bootmap_size = init_bootmem_node(
			NODE_DATA(0),
			memory_start >> PAGE_SHIFT, /* map goes here */
			PAGE_OFFSET >> PAGE_SHIFT,      /* 0 on coldfire */
			memory_end >> PAGE_SHIFT);
	/*
	 * Free the usable memory, we have to make sure we do not free
	 * the bootmem bitmap so we then reserve it after freeing it :-)
	 */
	free_bootmem(memory_start, memory_end - memory_start);
	reserve_bootmem(memory_start, bootmap_size);

	/*
	 * Get kmalloc into gear.
	 */
	paging_init();
}

/*
 *	Get CPU information for use by the procfs.
 */

static int show_cpuinfo(struct seq_file *m, void *v)
{
	char* target;
	char* host;
	char* fp;
	char* endian;

	target=TARGET;
	host=HOST;
	fp=FP;
	endian=ENDIAN;

	seq_printf(m, "TARGET:\t\t%s\n"
			"HOST:\t\t%s\n"
			"Float point:\t\t%s"
			"Endian:\t\t%s",
			target,host,fp,endian);	

	return 0;
}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	return *pos < NR_CPUS ? ((void *) 0x12345678) : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return c_start(m, pos);
}

static void c_stop(struct seq_file *m, void *v)
{
}

struct seq_operations cpuinfo_op = {
	.start	= c_start,
	.next	= c_next,
	.stop	= c_stop,
	.show	= show_cpuinfo,
};

