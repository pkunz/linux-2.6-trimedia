/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_LINKAGE_H
#define _ASM_TRIMEDIA_LINKAGE_H

#undef SYMBOL_NAME
#undef ENTRY
#undef GLOBAL_DATA

#define SYMBOL_NAME(name) 	_##name

#define ENTRY(name)         		\
		.##text  		\
	        .##global _##name       \
        _##name:

#define GLOBAL_DATA(name) 		\
		.##data 		\
		.##align 4 		\
		.##global _##name    	\
	_##name:

#endif /* _ASM_TRIMEDIA_LINKAGE_H */
