/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 * Derived (i.e. mostly copied) from arch/i386/kernel/irq.c:
 *	Copyright (C) 1992, 1998 Linus Torvalds, Ingo Molnar
 */

#include "linux/config.h"
#include "linux/kernel.h"
#include "linux/module.h"
#include "linux/smp.h"
#include "linux/kernel_stat.h"
#include "linux/interrupt.h"
#include "linux/random.h"
#include "linux/slab.h"
#include "linux/file.h"
#include "linux/proc_fs.h"
#include "linux/init.h"
#include "linux/seq_file.h"
#include "linux/profile.h"
#include "linux/hardirq.h"
#include "asm/irq.h"
#include "asm/hw_irq.h"
#include "asm/atomic.h"
#include "asm/signal.h"
#include "asm/system.h"
#include "asm/errno.h"
#include "asm/uaccess.h"
#include "user_util.h"
#include "kern_util.h"
#include "irq_user.h"
#include "irq_kern.h"
#include "os.h"
#include "sigio.h"
#include "misc_constants.h"

/*
 * Generic, controller-independent functions:
 */

int show_interrupts(struct seq_file *p, void *v)
{
	int i = *(loff_t *) v, j;
	struct irqaction * action;
	unsigned long flags;

	if (i == 0) {
		seq_printf(p, "           ");
		for_each_online_cpu(j)
			seq_printf(p, "CPU%d       ",j);
		seq_putc(p, '\n');
	}

	if (i < NR_IRQS) {
		spin_lock_irqsave(&irq_desc[i].lock, flags);
		action = irq_desc[i].action;
		if (!action) 
			goto skip;
		seq_printf(p, "%3d: ",i);
#ifndef CONFIG_SMP
		seq_printf(p, "%10u ", kstat_irqs(i));
#else
		for_each_online_cpu(j)
			seq_printf(p, "%10u ", kstat_cpu(j).irqs[i]);
#endif
		seq_printf(p, " %14s", irq_desc[i].handler->typename);
		seq_printf(p, "  %s", action->name);

		for (action=action->next; action; action = action->next)
			seq_printf(p, ", %s", action->name);

		seq_putc(p, '\n');
skip:
		spin_unlock_irqrestore(&irq_desc[i].lock, flags);
	} else if (i == NR_IRQS) {
		seq_putc(p, '\n');
	}

	return 0;
}

struct irq_fd *active_fds = NULL;
static struct irq_fd **last_irq_ptr = &active_fds;

extern void free_irqs(void);

void sigio_handler(int sig, union uml_pt_regs *regs)
{
	struct irq_fd *irq_fd;
	int n;

	if (smp_sigio_handler())
		return;

	while (1) {
		n = os_waiting_for_events(active_fds);
		if (n <= 0) {
			if(n == -EINTR) continue;
			else break;
		}

		for (irq_fd = active_fds; irq_fd != NULL; irq_fd = irq_fd->next) {
			if (irq_fd->current_events != 0) {
				irq_fd->current_events = 0;
				do_IRQ(irq_fd->irq, regs);
			}
		}
	}

	free_irqs();
}

static DEFINE_SPINLOCK(irq_lock);

int activate_fd(int irq, int fd, int type, void *dev_id)
{
	struct pollfd *tmp_pfd;
	struct irq_fd *new_fd, *irq_fd;
	unsigned long flags;
	int pid, events, err, n;

	pid = os_getpid();
	err = os_set_fd_async(fd, pid);
	if (err < 0)
		goto out;

	new_fd = um_kmalloc(sizeof(*new_fd));
	err = -ENOMEM;
	if (new_fd == NULL)
		goto out;

	if (type == IRQ_READ)
		events = UM_POLLIN | UM_POLLPRI;
	else
		events = UM_POLLOUT;
	*new_fd = ((struct irq_fd) { .next  		= NULL,
				     .id 		= dev_id,
				     .fd 		= fd,
				     .type 		= type,
				     .irq 		= irq,
				     .pid  		= pid,
				     .events 		= events,
				     .current_events 	= 0 } );

	/* Critical section - locked by a spinlock because this stuff can
	 * be changed from interrupt handlers.  The stuff above is done
	 * outside the lock because it allocates memory.
	 */

	/* Actually, it only looks like it can be called from interrupt
	 * context.  The culprit is reactivate_fd, which calls
	 * maybe_sigio_broken, which calls write_sigio_workaround,
	 * which calls activate_fd.  However, write_sigio_workaround should
	 * only be called once, at boot time.  That would make it clear that
	 * this is called only from process context, and can be locked with
	 * a semaphore.
	 */
	spin_lock_irqsave(&irq_lock, flags);
	for (irq_fd = active_fds; irq_fd != NULL; irq_fd = irq_fd->next) {
		if ((irq_fd->fd == fd) && (irq_fd->type == type)) {
			printk("Registering fd %d twice\n", fd);
			printk("Irqs : %d, %d\n", irq_fd->irq, irq);
			printk("Ids : 0x%p, 0x%p\n", irq_fd->id, dev_id);
			goto out_unlock;
		}
	}

	/*-------------*/
	if (type == IRQ_WRITE)
		fd = -1;

	tmp_pfd = NULL;
	n = 0;

	while (1) {
		n = os_create_pollfd(fd, events, tmp_pfd, n);
		if (n == 0)
			break;

		/* n > 0
		 * It means we couldn't put new pollfd to current pollfds
		 * and tmp_fds is NULL or too small for new pollfds array.
		 * Needed size is equal to n as minimum.
		 *
		 * Here we have to drop the lock in order to call
		 * kmalloc, which might sleep.
		 * If something else came in and changed the pollfds array
		 * so we will not be able to put new pollfd struct to pollfds
		 * then we free the buffer tmp_fds and try again.
		 */
		spin_unlock_irqrestore(&irq_lock, flags);
		kfree(tmp_pfd);
		tmp_pfd = NULL;

		tmp_pfd = um_kmalloc(n);
		if (tmp_pfd == NULL)
			goto out_kfree;

		spin_lock_irqsave(&irq_lock, flags);
	}
	/*-------------*/

	*last_irq_ptr = new_fd;
	last_irq_ptr = &new_fd->next;

	spin_unlock_irqrestore(&irq_lock, flags);

	/* This calls activate_fd, so it has to be outside the critical
	 * section.
	 */
	maybe_sigio_broken(fd, (type == IRQ_READ));

	return(0);

 out_unlock:
	spin_unlock_irqrestore(&irq_lock, flags);
 out_kfree:
	kfree(new_fd);
 out:
	return(err);
}

static void free_irq_by_cb(int (*test)(struct irq_fd *, void *), void *arg)
{
	unsigned long flags;

	spin_lock_irqsave(&irq_lock, flags);
	os_free_irq_by_cb(test, arg, active_fds, &last_irq_ptr);
	spin_unlock_irqrestore(&irq_lock, flags);
}

struct irq_and_dev {
	int irq;
	void *dev;
};

static int same_irq_and_dev(struct irq_fd *irq, void *d)
{
	struct irq_and_dev *data = d;

	return ((irq->irq == data->irq) && (irq->id == data->dev));
}

void free_irq_by_irq_and_dev(unsigned int irq, void *dev)
{
	struct irq_and_dev data = ((struct irq_and_dev) { .irq  = irq,
							  .dev  = dev });

	free_irq_by_cb(same_irq_and_dev, &data);
}

static int same_fd(struct irq_fd *irq, void *fd)
{
	return (irq->fd == *((int *)fd));
}

void free_irq_by_fd(int fd)
{
	free_irq_by_cb(same_fd, &fd);
}

static struct irq_fd *find_irq_by_fd(int fd, int irqnum, int *index_out)
{
	struct irq_fd *irq;
	int i = 0;
	int fdi;

	for (irq = active_fds; irq != NULL; irq = irq->next) {
		if ((irq->fd == fd) && (irq->irq == irqnum))
			break;
		i++;
	}
	if (irq == NULL) {
		printk("find_irq_by_fd doesn't have descriptor %d\n", fd);
		goto out;
	}
	fdi = os_get_pollfd(i);
	if ((fdi != -1) && (fdi != fd)) {
		printk("find_irq_by_fd - mismatch between active_fds and "
		       "pollfds, fd %d vs %d, need %d\n", irq->fd,
		       fdi, fd);
		irq = NULL;
		goto out;
	}
	*index_out = i;
 out:
	return irq;
}

void reactivate_fd(int fd, int irqnum)
{
	struct irq_fd *irq;
	unsigned long flags;
	int i;

	spin_lock_irqsave(&irq_lock, flags);
	irq = find_irq_by_fd(fd, irqnum, &i);
	if (irq == NULL) {
		spin_unlock_irqrestore(&irq_lock, flags);
		return;
	}
	os_set_pollfd(i, irq->fd);
	spin_unlock_irqrestore(&irq_lock, flags);

	/* This calls activate_fd, so it has to be outside the critical
	 * section.
	 */
	maybe_sigio_broken(fd, (irq->type == IRQ_READ));
}

void deactivate_fd(int fd, int irqnum)
{
	struct irq_fd *irq;
	unsigned long flags;
	int i;

	spin_lock_irqsave(&irq_lock, flags);
	irq = find_irq_by_fd(fd, irqnum, &i);
	if (irq == NULL)
		goto out;
	os_set_pollfd(i, -1);
 out:
	spin_unlock_irqrestore(&irq_lock, flags);
}

int deactivate_all_fds(void)
{
	struct irq_fd *irq;
	int err;

	for (irq = active_fds; irq != NULL; irq = irq->next) {
		err = os_clear_fd_async(irq->fd);
		if (err)
			return err;
	}
	/* If there is a signal already queued, after unblocking ignore it */
	os_set_ioignore();

	return 0;
}

#ifdef CONFIG_MODE_TT
void forward_interrupts(int pid)
{
	struct irq_fd *irq;
	unsigned long flags;
	int err;

	spin_lock_irqsave(&irq_lock, flags);
	for (irq = active_fds; irq != NULL; irq = irq->next) {
		err = os_set_owner(irq->fd, pid);
		if (err < 0) {
			/* XXX Just remove the irq rather than
			 * print out an infinite stream of these
			 */
			printk("Failed to forward %d to pid %d, err = %d\n",
			       irq->fd, pid, -err);
		}

		irq->pid = pid;
	}
	spin_unlock_irqrestore(&irq_lock, flags);
}
#endif

/*
 * do_IRQ handles all normal device IRQ's (the special
 * SMP cross-CPU interrupts have their own specific
 * handlers).
 */
unsigned int do_IRQ(int irq, union uml_pt_regs *regs)
{
       irq_enter();
       __do_IRQ(irq, (struct pt_regs *)regs);
       irq_exit();
       return 1;
}

int um_request_irq(unsigned int irq, int fd, int type,
		   irqreturn_t (*handler)(int, void *, struct pt_regs *),
		   unsigned long irqflags, const char * devname,
		   void *dev_id)
{
	int err;

	err = request_irq(irq, handler, irqflags, devname, dev_id);
	if (err)
		return err;

	if (fd != -1)
		err = activate_fd(irq, fd, type, dev_id);
	return err;
}
EXPORT_SYMBOL(um_request_irq);
EXPORT_SYMBOL(reactivate_fd);

/* hw_interrupt_type must define (startup || enable) &&
 * (shutdown || disable) && end */
static void dummy(unsigned int irq)
{
}

/* This is used for everything else than the timer. */
static struct hw_interrupt_type normal_irq_type = {
	.typename = "SIGIO",
	.release = free_irq_by_irq_and_dev,
	.disable = dummy,
	.enable = dummy,
	.ack = dummy,
	.end = dummy
};

static struct hw_interrupt_type SIGVTALRM_irq_type = {
	.typename = "SIGVTALRM",
	.release = free_irq_by_irq_and_dev,
	.shutdown = dummy, /* never called */
	.disable = dummy,
	.enable = dummy,
	.ack = dummy,
	.end = dummy
};

void __init init_IRQ(void)
{
	int i;

	irq_desc[TIMER_IRQ].status = IRQ_DISABLED;
	irq_desc[TIMER_IRQ].action = NULL;
	irq_desc[TIMER_IRQ].depth = 1;
	irq_desc[TIMER_IRQ].handler = &SIGVTALRM_irq_type;
	enable_irq(TIMER_IRQ);
	for (i = 1; i < NR_IRQS; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = NULL;
		irq_desc[i].depth = 1;
		irq_desc[i].handler = &normal_irq_type;
		enable_irq(i);
	}
}

int init_aio_irq(int irq, char *name, irqreturn_t (*handler)(int, void *,
							     struct pt_regs *))
{
	int fds[2], err;

	err = os_pipe(fds, 1, 1);
	if (err) {
		printk("init_aio_irq - os_pipe failed, err = %d\n", -err);
		goto out;
	}

	err = um_request_irq(irq, fds[0], IRQ_READ, handler,
#if 0 /* 2.6.18 */
			     IRQF_DISABLED | IRQF_SAMPLE_RANDOM, name,
#else
			     SA_INTERRUPT | SA_SAMPLE_RANDOM, name,
#endif
			     (void *) (long) fds[0]);
	if (err) {
		printk("init_aio_irq - : um_request_irq failed, err = %d\n",
		       err);
		goto out_close;
	}

	err = fds[1];
	goto out;

 out_close:
	os_close_file(fds[0]);
	os_close_file(fds[1]);
 out:
	return err;
}
