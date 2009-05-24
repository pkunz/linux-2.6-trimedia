#ifndef _I386_CURRENT_H
#define _I386_CURRENT_H

#include <linux/thread_info.h>

struct task_struct;

#ifdef CONFIG_INLINE_THREADINFO
static inline struct task_struct * get_current(void)
{
	return current_thread_info()->task;
}
#else
extern struct task_struct *__current_task;
static inline struct task_struct * get_current(void)
{
	return __current_task;
}
#endif
#define current get_current()

#endif /* !(_I386_CURRENT_H) */
