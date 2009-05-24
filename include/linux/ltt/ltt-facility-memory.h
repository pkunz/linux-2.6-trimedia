#ifndef _LTT_FACILITY_MEMORY_H_
#define _LTT_FACILITY_MEMORY_H_

#include <linux/types.h>
#include <linux/ltt/ltt-facility-id-memory.h>
#include <linux/ltt-core.h>

/* Named types */

/* Event page_alloc structures */

/* Event page_alloc logging function */
static inline void trace_memory_page_alloc(
		unsigned int lttng_param_order,
		const void * lttng_param_address)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	size_t align;
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = &lttng_param_order;
	align = sizeof(unsigned int);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(unsigned int);

	*from = &lttng_param_address;
	align = sizeof(const void *);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(const void *);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_memory_D63D41C7,
						event_memory_page_alloc);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_memory_D63D41C7, event_memory_page_alloc,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = &lttng_param_order;
		align = sizeof(unsigned int);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(unsigned int);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		*from = &lttng_param_address;
		align = sizeof(const void *);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(const void *);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))


/* Event page_free structures */

/* Event page_free logging function */
static inline void trace_memory_page_free(
		unsigned int lttng_param_order,
		const void * lttng_param_address)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	size_t align;
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = &lttng_param_order;
	align = sizeof(unsigned int);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(unsigned int);

	*from = &lttng_param_address;
	align = sizeof(const void *);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(const void *);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_memory_D63D41C7,
						event_memory_page_free);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_memory_D63D41C7, event_memory_page_free,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = &lttng_param_order;
		align = sizeof(unsigned int);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(unsigned int);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		*from = &lttng_param_address;
		align = sizeof(const void *);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(const void *);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))


/* Event swap_in structures */

/* Event swap_in logging function */
static inline void trace_memory_swap_in(
		const void * lttng_param_address)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	size_t align;
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = &lttng_param_address;
	align = sizeof(const void *);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(const void *);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_memory_D63D41C7,
						event_memory_swap_in);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_memory_D63D41C7, event_memory_swap_in,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = &lttng_param_address;
		align = sizeof(const void *);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(const void *);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))


/* Event swap_out structures */

/* Event swap_out logging function */
static inline void trace_memory_swap_out(
		const void * lttng_param_address)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	size_t align;
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = &lttng_param_address;
	align = sizeof(const void *);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(const void *);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_memory_D63D41C7,
						event_memory_swap_out);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_memory_D63D41C7, event_memory_swap_out,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = &lttng_param_address;
		align = sizeof(const void *);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(const void *);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))


/* Event page_wait_start structures */

/* Event page_wait_start logging function */
static inline void trace_memory_page_wait_start(
		const void * lttng_param_address)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	size_t align;
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = &lttng_param_address;
	align = sizeof(const void *);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(const void *);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_memory_D63D41C7,
						event_memory_page_wait_start);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_memory_D63D41C7, event_memory_page_wait_start,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = &lttng_param_address;
		align = sizeof(const void *);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(const void *);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))


/* Event page_wait_end structures */

/* Event page_wait_end logging function */
static inline void trace_memory_page_wait_end(
		const void * lttng_param_address)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))
{
}
#else
{
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* The buffer is allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	size_t align;
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = &lttng_param_address;
	align = sizeof(const void *);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(const void *);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_memory_D63D41C7,
						event_memory_page_wait_end);

	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;

		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];

		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) continue; /* buffer full */

		*to_base = *to = *len = 0;

		ltt_write_event_header(trace, channel, buffer,
			ltt_facility_memory_D63D41C7, event_memory_page_wait_end,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = &lttng_param_address;
		align = sizeof(const void *);

		if(*len == 0) {
			*to += ltt_align(*to, align); /* align output */
		} else {
			*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
		}

		*len += sizeof(const void *);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_MEMORY))


#endif //_LTT_FACILITY_MEMORY_H_
