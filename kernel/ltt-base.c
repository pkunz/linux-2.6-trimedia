/*
 * ltt-base.c
 *
 * (C) Copyright  2005 -
 * 		Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * Contains the kernel core code for Linux Trace Toolkit.
 *
 * This base ltt file is used when LTT is configured as a module. Otherwise,
 * ltt-core defines the ltt_log_event directly. The cost of this modularisation
 * is a pointer to dereference in the LTT tracing critical path.
 *
 * Author:
 *	Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 */

#include <linux/ltt-core.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/atomic.h>

/* Traces structures */
struct ltt_traces ltt_traces = {
	.head = LIST_HEAD_INIT(ltt_traces.head),
	.num_active_traces = 0
};

EXPORT_SYMBOL(ltt_traces);

volatile unsigned int ltt_nesting[NR_CPUS] = { [ 0 ... NR_CPUS-1 ] = 0 } ;

EXPORT_SYMBOL(ltt_nesting);

atomic_t lttng_logical_clock = ATOMIC_INIT(0);
EXPORT_SYMBOL(lttng_logical_clock);

void __init ltt_init(void)
{
	printk(KERN_INFO "LTT : ltt-base init\n");
}

