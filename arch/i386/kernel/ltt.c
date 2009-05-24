/*
 * ltt.c
 *
 * (C) Copyright  2006 -
 * 		Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 * We need to fallback on calling functions instead of inlining only because of
 * headers re-inclusion in critical kernel headers. This is necessary for irq,
 * spinlock, ...
 */

#include <linux/module.h>
#include <linux/ltt/ltt-facility-locking.h>
#include <asm/ltt/ltt-facility-custom-locking.h>

void _trace_locking_irq_save(const void * lttng_param_EIP,
		unsigned long lttng_param_flags)
{
	trace_locking_irq_save(lttng_param_EIP, lttng_param_flags);
}

void _trace_locking_irq_restore(const void * lttng_param_EIP,
		unsigned long lttng_param_flags)
{
	trace_locking_irq_restore(lttng_param_EIP, lttng_param_flags);
}

void _trace_locking_irq_disable(const void * lttng_param_EIP)
{
	trace_locking_irq_disable(lttng_param_EIP);
}

void _trace_locking_irq_enable(const void * lttng_param_EIP)
{
	trace_locking_irq_enable(lttng_param_EIP);
}


EXPORT_SYMBOL(_trace_locking_irq_save);
EXPORT_SYMBOL(_trace_locking_irq_restore);
EXPORT_SYMBOL(_trace_locking_irq_disable);
EXPORT_SYMBOL(_trace_locking_irq_enable);

