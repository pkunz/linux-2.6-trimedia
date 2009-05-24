/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_TIMEX_H
#define _ASM_TRIMEDIA_TIMEX_H

#include <ops/custom_ops.h>

#define CLOCK_TICK_RATE   CONFIG_CPU_FREQ 

typedef unsigned long cycles_t;

static inline cycles_t get_cycles(void)
{
	return (cycles_t)CYCLES();
}

#endif
