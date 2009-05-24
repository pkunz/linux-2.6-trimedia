#ifdef __KERNEL__
#include <linux/types.h>
#endif

#include <linux/errno.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#include <linux/oom-protect.h>

/*
 * Protect the process whose pid is given from selection by the OOM killer
 * when the system is out of memory.  If the total virtual memory allocated
 * by the process (in pages) is less than the max_vm_bytes value passed,
 * it will be exempt, otherwise it will be treated normally.
 *
 * On success we return the previous protection limit, on failure we return
 * an appropriate error code.
 *
 * We take the read lock on tasklist to ensure the task doesn't go away
 * between finding the task and setting oom protection on it.
 *
 * We take the task_lock to protect against others racing to  modify p->mm.
 */
int do_oom_protect(unsigned long arg)
{
	int ret = 0;
	struct task_struct *p = NULL;
	struct set_oom_protect_info args;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (copy_from_user(&args, (struct set_oom_protect_info __user *)arg,
				sizeof(args)))
		return -EFAULT;

	ret = -ESRCH;
	read_lock(&tasklist_lock);
	p = args.pid ? find_task_by_pid(args.pid) : current;

	if (!p)
		goto tasklist_unlock_out;

	ret = -EINVAL;
	task_lock(p);
	if (!p->mm)
	{
		printk(KERN_EMERG "do_oom_protect():!p->mm\n");
		goto task_unlock_out;
	}

	switch (args.cmd) {
		case SET_OOM_PROTECT:
			printk(KERN_EMERG "do_oom_protect():SET_OOM_PROTECT\n");
			
			if (p->mm->oom_protect_bytes > args.max_vm_bytes)
			{
				/* The standard setrlimit() code seems to choose to barf if you
				 * try to reduce your limits, so we should do the same here. */
				ret = -EINVAL;
				goto task_unlock_out;
			}

			p->mm->oom_protect_bytes = args.max_vm_bytes;
			ret = 0;
			break;

		case QUERY_OOM_PROTECT:
			printk(KERN_EMERG "do_oom_protect():QUERY_OOM_PROTECT\n");
			
			args.max_vm_bytes = p->mm->oom_protect_bytes;

			if (copy_to_user((struct set_oom_protect_info __user *) arg,
						&args, sizeof(struct set_oom_protect_info))) {
				ret = -EFAULT;
				goto task_unlock_out;
			}

			ret = 0;
			break;

		default:
			ret = -EINVAL;
			break;
	}

task_unlock_out:
	task_unlock(p);

tasklist_unlock_out:
	read_unlock(&tasklist_lock);
	return ret;
}
