/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_MMU_CONTEXT_H__
#define _ASM_TRIMEDIA_MMU_CONTEXT_H__

#define destroy_context(mm)		((void)0)
#define init_new_context(tsk,mm)	0
#define switch_mm(prev,next,tsk)	((void)0)
#define deactivate_mm(tsk,mm)		do { } while (0)
#define activate_mm(prev,next)		((void)0)
#define enter_lazy_tlb(mm,tsk)		((void)0)

#endif /* _ASM_TRIMEDIA_MMU_CONTEXT_H__ */

