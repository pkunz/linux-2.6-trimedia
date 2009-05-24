/* 
 * arch/trimedia/lib/atomic.c: TCS_atomic version atomic operation
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <asm/atomic.h>

extern int
atomic_add_return(int i,volatile atomic_t *v)
{
#pragma TCS_atomic
	return v->counter+=i;	
}

extern int
atomic_sub_return(int i,volatile atomic_t* v)
{
#pragma TCS_atomic
	return v->counter-=i;
}

extern void
atomic_set_mask(unsigned long mask,unsigned long* addr)
{
#pragma TCS_atomic
	*addr |= mask;
}

extern void
atomic_clear_mask(unsigned long mask,unsigned long* addr)
{
#pragma TCS_atomic
	*addr &= ~mask;
}

