#ifndef _LTT_FACILITY_NETWORK_IP_INTERFACE_H_
#define _LTT_FACILITY_NETWORK_IP_INTERFACE_H_

#include <linux/types.h>
#include <linux/ltt/ltt-facility-id-network_ip_interface.h>
#include <linux/ltt-core.h>

/* Named types */

/* Event dev_up structures */
static inline void lttng_write_string_network_ip_interface_dev_up_name(
		void *buffer,
		size_t *to_base,
		size_t *to,
		const void **from,
		size_t *len,
		const char * obj)
{
	size_t size;
	size_t align;

	/* Flush pending memcpy */
	if(*len != 0) {
		if(buffer != NULL)
			memcpy(buffer+*to_base+*to, *from, *len);
	}
	*to += *len;
	*len = 0;

	align = sizeof(char);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	/* Contains variable sized fields : must explode the structure */

	size = strlen(obj) + 1; /* Include final NULL char. */
	if(buffer != NULL)
		memcpy(buffer+*to_base+*to, obj, size);
	*to += size;

	/* Realign the *to_base on arch size, set *to to 0 */
	*to += ltt_align(*to, sizeof(void *));
	*to_base = *to_base+*to;
	*to = 0;

	/* Put source *from just after the C string */
	*from += size;
}


/* Event dev_up logging function */
static inline void trace_network_ip_interface_dev_up(
		const char * lttng_param_name,
		unsigned int lttng_param_addr)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_NETWORK_IP_INTERFACE))
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

	*from = lttng_param_name;
	lttng_write_string_network_ip_interface_dev_up_name(buffer, to_base, to, from, len, lttng_param_name);

	*from = &lttng_param_addr;
	align = sizeof(unsigned int);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	*len += sizeof(unsigned int);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_network_ip_interface_7A3120EF,
						event_network_ip_interface_dev_up);

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
			ltt_facility_network_ip_interface_7A3120EF, event_network_ip_interface_dev_up,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = lttng_param_name;
		lttng_write_string_network_ip_interface_dev_up_name(buffer, to_base, to, from, len, lttng_param_name);

		/* Flush pending memcpy */
		if(*len != 0) {
			memcpy(buffer+*to_base+*to, *from, *len);
			*to += *len;
			*len = 0;
		}

		*from = &lttng_param_addr;
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

		ltt_commit_slot(relayfs_buf, buffer, slot_size);

	}

	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
}
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_NETWORK_IP_INTERFACE))


/* Event dev_down structures */
static inline void lttng_write_string_network_ip_interface_dev_down_name(
		void *buffer,
		size_t *to_base,
		size_t *to,
		const void **from,
		size_t *len,
		const char * obj)
{
	size_t size;
	size_t align;

	/* Flush pending memcpy */
	if(*len != 0) {
		if(buffer != NULL)
			memcpy(buffer+*to_base+*to, *from, *len);
	}
	*to += *len;
	*len = 0;

	align = sizeof(char);

	if(*len == 0) {
		*to += ltt_align(*to, align); /* align output */
	} else {
		*len += ltt_align(*to+*len, align); /* alignment, ok to do a memcpy of it */
	}

	/* Contains variable sized fields : must explode the structure */

	size = strlen(obj) + 1; /* Include final NULL char. */
	if(buffer != NULL)
		memcpy(buffer+*to_base+*to, obj, size);
	*to += size;

	/* Realign the *to_base on arch size, set *to to 0 */
	*to += ltt_align(*to, sizeof(void *));
	*to_base = *to_base+*to;
	*to = 0;

	/* Put source *from just after the C string */
	*from += size;
}


/* Event dev_down logging function */
static inline void trace_network_ip_interface_dev_down(
		const char * lttng_param_name)
#if (!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_NETWORK_IP_INTERFACE))
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
	const void *real_from;
	const void **from = &real_from;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;

	if(ltt_traces.num_active_traces == 0) return;

	/* For each field, calculate the field size. */
	/* size = *to_base + *to + *len */
	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	*from = lttng_param_name;
	lttng_write_string_network_ip_interface_dev_down_name(buffer, to_base, to, from, len, lttng_param_name);

	reserve_size = *to_base + *to + *len;
	preempt_disable();
	ltt_nesting[smp_processor_id()]++;
	index = ltt_get_index_from_facility(ltt_facility_network_ip_interface_7A3120EF,
						event_network_ip_interface_dev_down);

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
			ltt_facility_network_ip_interface_7A3120EF, event_network_ip_interface_dev_down,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;

		*from = lttng_param_name;
		lttng_write_string_network_ip_interface_dev_down_name(buffer, to_base, to, from, len, lttng_param_name);

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
#endif //(!defined(CONFIG_LTT) || !defined(CONFIG_LTT_FACILITY_NETWORK_IP_INTERFACE))


#endif //_LTT_FACILITY_NETWORK_IP_INTERFACE_H_
