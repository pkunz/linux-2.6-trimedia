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

#ifndef _ASM_TRIMEDIA_CACHEFLUSH_H
#define _ASM_TRIMEDIA_CACHEFLUSH_H

/*
 * Cache handling functions
 * No Cache memory all dummy functions
 */

/*
 *FIXME: I am not sure whether or not it is suitable.
 */
#define flush_cache_all()
#define	flush_cache_mm(mm)
#define	flush_cache_range(vma,a,b)
#define	flush_cache_page(vma,p,pfn)
#define flush_cache_vmap(start, end)
#define flush_cache_vunmap(start, end)

#define	flush_dcache_page(page)
#define	flush_dcache_mmap_lock(mapping)
#define	flush_dcache_mmap_unlock(mapping)
#define	flush_dcache_range(a,b)

#define	flush_icache()
#define	flush_icache_page(vma,page)
#define	flush_icache_range(start,len)
#define	flush_icache_user_range(vma,page,addr,len)

#define copy_to_user_page(vma, page, vaddr, dst, src, len) \
	memcpy(dst, src, len)
#define copy_from_user_page(vma, page, vaddr, dst, src, len) \
	memcpy(dst, src, len)

#endif /* _ASM_TRIMEDIA_CACHEFLUSH_H */

