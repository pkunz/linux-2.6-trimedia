/*
 * Copyright (C) 2002 - 2003 Jeff Dike (jdike@addtoit.com)
 * Licensed under the GPL
 */

#include "linux/kernel.h"
#include "linux/list.h"
#include "linux/slab.h"
#include "linux/signal.h"
#include "linux/interrupt.h"
#include "init.h"
#include "sigio.h"
#include "irq_user.h"
#include "irq_kern.h"
#include "os.h"

/* Protected by sigio_lock() called from write_sigio_workaround */
static int sigio_irq_fd = -1;

static irqreturn_t sigio_interrupt(int irq, void *data, struct pt_regs *unused)
{
	char c;

	os_read_file(sigio_irq_fd, &c, sizeof(c));
	reactivate_fd(sigio_irq_fd, SIGIO_WRITE_IRQ);
	return(IRQ_HANDLED);
}

int write_sigio_irq(int fd)
{
	int err;

	err = um_request_irq(SIGIO_WRITE_IRQ, fd, IRQ_READ, sigio_interrupt,
#if 0 /* 2.6.18 */
			     IRQF_DISABLED|IRQF_SAMPLE_RANDOM, "write sigio",
#else
			     SA_INTERRUPT | SA_SAMPLE_RANDOM, "write sigio",
#endif
			     NULL);
	if(err){
		printk("write_sigio_irq : um_request_irq failed, err = %d\n",
		       err);
		return(-1);
	}
	sigio_irq_fd = fd;
	return(0);
}

static DEFINE_SPINLOCK(sigio_spinlock);

void sigio_lock(void)
{
	spin_lock(&sigio_spinlock);
}

void sigio_unlock(void)
{
	spin_unlock(&sigio_spinlock);
}
