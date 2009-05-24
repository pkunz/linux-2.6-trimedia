#ifndef __NORMAL_USER_H__
#define __NORMAL_USER_H__

#ifdef __KERNEL__

#include <linux/ptrace.h>

struct user {
	char		u_comm[32];		/* user command name */
};


#endif /* __KERNEL__ */

#endif /* __NORMAL_USER_H__ */

