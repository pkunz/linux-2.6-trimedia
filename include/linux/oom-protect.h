#ifndef _LINUX_OOM_PROTECT_H
#define _LINUX_OOM_PROTECT_H

/* The following commands may be specified for oom protection */
#define SET_OOM_PROTECT 1
#define QUERY_OOM_PROTECT 2

/* This is the data structure for setting oom protection.
 * The caller fills in the pid of the process to protect
 * and the number of bytes for which to request protection.
 */
struct set_oom_protect_info {
	unsigned int cmd;
	int pid;
	unsigned int max_vm_bytes;
};

#ifdef __KERNEL__
int do_oom_protect(unsigned long);
#endif /* __KERNEL__ */

#endif /* _LINUX_OOM_PROTECT_H */
