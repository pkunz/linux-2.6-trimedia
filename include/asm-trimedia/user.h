/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_USER_H__
#define _ASM_TRIMEDIA_USER_H__

#ifdef __KERNEL__

#include <linux/ptrace.h>

/*
 * FIXME: used in coredump, so maybe it is not useful for trimedia
 */
struct user {
	char		u_comm[32];		/* user command name */
};


#endif /* __KERNEL__ */

#endif /* _ASM_TRIMEDIA_USER_H__ */

