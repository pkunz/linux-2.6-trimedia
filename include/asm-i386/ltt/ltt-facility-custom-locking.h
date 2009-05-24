/*
 * ltt-facility-custom-locking.h
 *
 * (C) Copyright  2006 -
 *              Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 */
#ifndef _LTT_FACILITY_CUSTOM_LOCKING_H
#define _LTT_FACILITY_CUSTOM_LOCKING_H

#ifdef CONFIG_LTT_FACILITY_LOCKING
extern void _trace_locking_irq_save(const void * lttng_param_EIP,
		unsigned long lttng_param_flags);
extern void _trace_locking_irq_restore(const void * lttng_param_EIP,
		unsigned long lttng_param_flags);
extern void _trace_locking_irq_disable(const void * lttng_param_EIP);
extern void _trace_locking_irq_enable(const void * lttng_param_EIP);
#endif //CONFIG_LTT_FACILITY_LOCKING

#endif //_LTT_FACILITY_CUSTOM_LOCKING_H
