/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <asm/regdef.h>
#include <asm/linkage.h>
#include <asm/asm-offsets.h>

#include "switch.h"

/*
* extern struct thread_info* __switch_to(struct thread_info* prev,struct thread_info* next);
*/
ENTRY(__switch_to)
	UPDATE_CURRENT_THREAD_INFO
	GET_PREV_KSP
	SAVE_PREV_SWITCH_STACK
	SAVE_PREV_KSP
	LOAD_NEXT_KSP
	RESTORE_NEXT_SWITCH_STACK
	JUMP_TO_R2

