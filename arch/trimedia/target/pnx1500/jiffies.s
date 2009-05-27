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

	.data
	.global	_jiffies_64
	.global _jiffies
	.align	4
#ifdef __BIG_ENDIAN__
_jiffies_64:
	.word	0xbbbbbbbb 	
_jiffies:
	.word	0xaaaaaaaa 
#else 
# ifdef __LITTLE_ENDIAN__
_jiffies_64:
_jiffies:
	.word	0xaaaaaaaa	
	.word	0xbbbbbbbb
# else
error invalid endian
#endif 
#endif 
