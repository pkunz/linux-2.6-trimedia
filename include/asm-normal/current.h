#ifndef _NORMAL_CURRENT_H
#define _NORMAL_CURRENT_H
/*
 *	current.h
 *	(C) Copyright 2000, Lineo, David McCullough <davidm@lineo.com>
 *	(C) Copyright 2002, Greg Ungerer (gerg@snapgear.com)
 *
 *	rather than dedicate a register (as the m68k source does), we
 *	just keep a global,  we should probably just change it all to be
 *	current and lose _current_task.
 */

#include <linux/thread_info.h>

struct task_struct;

extern struct task_struct *get_current(void);

#define	current	get_current()

#endif /* _NORMAL_CURRENT_H */

