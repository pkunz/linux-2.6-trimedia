/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/kernel.h>
#include <linux/module.h>

#include <asm/io.h>

/*
 * Copy data from IO memory space to 'real' memory space.
 * TODO: this need to be optimized
 */
extern void
memcpy_fromio(void* dst,const volatile void __iomem* src,unsigned long count)
{
	char* p=(char*)dst;
	while(count){
		count--;
		*p=readb(src);
		p++;
		src=(volatile void __iomem*)((unsigned long)src+1);	
	}
	return;
}

/*
 * Copy data from 'real' memory space to IO memory space.
 * TODO: this needs to be optimized
 */
extern void 
memcpy_toio(volatile void __iomem* dst,void* src,unsigned long count)
{
	const char* p=(const char*)src;
	while(count){
		count--;
		writeb(*p,dst);
		p++;
		dst=(volatile void __iomem*)((unsigned long)dst+1);	
	}
	return;
}

/*
 * memset on IO memory space
 * TODO: this needs to be optimized
 */
extern void 
memset_io(volatile void __iomem* dst,__u8 c,unsigned long count)
{
	while(count){
		count--;
		writeb(c,dst);
		dst=(volatile void __iomem*)((unsigned long)dst+1);	
	}
}

EXPORT_SYMBOL(memcpy_fromio);
EXPORT_SYMBOL(memcpy_toio);
EXPORT_SYMBOL(memset_io);

