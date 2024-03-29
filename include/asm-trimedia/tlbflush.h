/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_TLBFLUSH_H
#define _ASM_TRIMEDIA_TLBFLUSH_H

/*
 * Copyright (C) 2000 Lineo, David McCullough <davidm@uclinux.org>
 * Copyright (C) 2000-2002, Greg Ungerer <gerg@snapgear.com>
 */

#include <asm/setup.h>

/*
 * flush all user-space atc entries.
 */
static inline void __flush_tlb(void)
{
	BUG();
}

static inline void __flush_tlb_one(unsigned long addr)
{
	BUG();
}

#define flush_tlb() __flush_tlb()

/*
 * flush all atc entries (both kernel and user-space entries).
 */
static inline void flush_tlb_all(void)
{
	BUG();
}

static inline void flush_tlb_mm(struct mm_struct *mm)
{
	BUG();
}

static inline void flush_tlb_page(struct vm_area_struct *vma, unsigned long addr)
{
	BUG();
}

static inline void flush_tlb_range(struct mm_struct *mm,
				   unsigned long start, unsigned long end)
{
	BUG();
}

extern inline void flush_tlb_kernel_page(unsigned long addr)
{
	BUG();
}

extern inline void flush_tlb_pgtables(struct mm_struct *mm,
				      unsigned long start, unsigned long end)
{
	BUG();
}

#endif /* _ASM_TRIMEDIA_TLBFLUSH_H */

