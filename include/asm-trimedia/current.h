/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_CURRENT_H
#define _ASM_TRIMEDIA_CURRENT_H

#include <linux/thread_info.h>

struct task_struct;

static inline struct task_struct *get_current(void)
{
	return (current_thread_info()->task);
}

#define	current	get_current()

#endif /* _ASM_TRIMEDIA_CURRENT_H */
