/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_CACHE_H
#define _ASM_TRIMEDIA_CACHE_H

#if defined(__TCS_tm3260__)
#define L1_CACHE_SHIFT  	8
#elif defined(__TCS_tm3282__)
#define L1_CACHE_SHIFT  	8
#else
#error "Unknow cache shift for this machine"
#endif 

#define L1_CACHE_BYTES      	(1<<L1_CACHE_SHIFT) 

/*
 * FIXME: I don't know how to define them, this will cause poor performance.
 */
#define __cacheline_aligned
#define ____cacheline_aligned

#define L1_CACHE_SHIFT_MAX   	L1_CACHE_SHIFT

#endif /*_ASM_TRIMEDIA_CACHE_H*/
