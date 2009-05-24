/*
 * linux/include/linux/ltt-core.h
 *
 * Copyright (C) 2005,2006 Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * This contains the core definitions for the Linux Trace Toolkit.
 */

#ifndef _LTT_CORE_H
#define _LTT_CORE_H

#include <linux/config.h>
#include <linux/types.h>
#include <linux/limits.h>
#include <linux/list.h>
#include <linux/cache.h>
#include <linux/kernel.h>
#include <linux/timex.h>
#include <linux/wait.h>
#include <linux/relayfs_fs.h>
#include <linux/ltt-facilities.h>
#include <linux/ltt/ltt-facility-id-core.h>
#include <linux/ltt/ltt-facility-id-process.h>
#include <linux/ltt/ltt-facility-id-network_ip_interface.h>

#include <asm/semaphore.h>
#include <asm/atomic.h>
#include <asm/ltt.h>

/* Number of bytes to log with a read/write event */
#define LTT_LOG_RW_SIZE			32

#include <asm/system.h>

static inline int atomic_cmpxchg(atomic_t * p, int a, int b)
   {
   return cmpxchg(&p->counter,a,b);
   }

#ifdef CONFIG_LTT_ALIGNMENT

/* Calculate the offset needed to align the type */
static inline unsigned int ltt_align(size_t align_drift,
		 size_t size_of_type)
{
	size_t alignment = min(sizeof(void*), size_of_type);
	return ((alignment - align_drift) & (alignment-1));
}
/* Default arch alignment */
#define LTT_ALIGN

#else
static inline unsigned int ltt_align(size_t align_drift,
		 size_t size_of_type)
{
	return 0;
}

#define LTT_ALIGN __attribute__((packed))

#endif //CONFIG_LTT_ALIGNMENT


#ifdef CONFIG_LTT

extern void ltt_init(void);

/* All modifications of ltt_traces must be done by ltt-core.c, while holding the
 * semaphore. Only reading of this information can be done elsewhere, with the
 * RCU mechanism : the preemption must be disabled while reading the list. */
struct ltt_traces {
	struct list_head head;		/* Traces list */
	unsigned int num_active_traces;	/* Number of active traces */
} ____cacheline_aligned;

extern struct ltt_traces ltt_traces;
extern struct file_operations ltt_file_operations;
/* Keep track of traps nesting inside LTT */
extern volatile unsigned int ltt_nesting[];
struct ltt_trace_struct;

/* LTTng lockless logging buffer info */
struct ltt_channel_buf_struct {
	/* Use the relayfs void *start as buffer start pointer */
	atomic_t offset;		/* Current offset in the buffer */
	atomic_t consumed;		/* Current offset in the buffer */
	atomic_t active_readers;	/* Active readers count */
	atomic_t wakeup_readers;	/* Boolean : wakeup readers waiting ? */
	atomic_t *reserve_count;	/* Final per sub-buffer reserve count */
	atomic_t *commit_count;		/* Commit count per sub-buffer */
	spinlock_t full_lock;		/* buffer full condition spinlock, only
					 * for userspace tracing blocking mode
					 * synchronisation with reader. */
	atomic_t events_lost;
	atomic_t corrupted_subbuffers;
	struct timeval	current_subbuffer_start_time;
	wait_queue_head_t write_wait;	/* Wait queue for blocking user space
					 * writers */
	struct work_struct wake_writers;/* Writers wake-up work struct */
} ____cacheline_aligned;

struct ltt_channel_struct {
	char channel_name[PATH_MAX];
	struct rchan *rchan;
	struct ltt_trace_struct	*trace;
	struct ltt_channel_buf_struct buf[NR_CPUS];
	/*
	 * subbuf_header_len - called on buffer-switch to a new sub-buffer
	 * @buf: the channel buffer containing the new sub-buffer
	 *
	 * returns the client header size at the beginning of the buffer.
	 */
	unsigned int (*subbuf_header_len) (struct rchan_buf *buf);
	/*
	 * buffer_begin - called on buffer-switch to a new sub-buffer
	 * @buf: the channel buffer containing the new sub-buffer
	 */
	void (*buffer_begin) (struct rchan_buf *buf,
			u64 tsc, unsigned int subbuf_idx);
	/*
	 * buffer_end - called on buffer-switch to a new sub-buffer
	 * @buf: the channel buffer containing the previous sub-buffer
	 */
	void (*buffer_end) (struct rchan_buf *buf,
			u64 tsc, unsigned int offset, unsigned int subbuf_idx);
};


enum trace_mode { LTT_TRACE_NORMAL, LTT_TRACE_FLIGHT };

/* Per-trace information - each trace/flight recorder represented by one */
struct ltt_trace_struct {
	struct list_head list;
	int active;
	char trace_name[NAME_MAX];
	int paused;
	enum trace_mode mode;
	u32 freq_scale;
	u64 start_freq;
	u64 start_tsc;
	unsigned long long start_monotonic;
	struct timeval		start_time;
	struct {
		struct dentry			*trace_root;
		struct dentry			*control_root;
	} dentry;
	struct {
		struct ltt_channel_struct	*facilities;
		struct ltt_channel_struct	*interrupts;
		struct ltt_channel_struct	*processes;
		struct ltt_channel_struct	*modules;
		struct ltt_channel_struct	*cpu;
		struct ltt_channel_struct	*network;
	} channel;
	struct rchan_callbacks callbacks;
} ____cacheline_aligned;


/* Hardcoded event headers */

/* event header for a trace with active heartbeat : 32 bits timestamps */

/* headers are 8 bytes aligned : that means members are aligned on memory
 * boundaries *if* structure starts on a 8 bytes boundary. In order to insure
 * such alignment, a dynamic per trace alignment value must be set.
 *
 * Remeber that the C compiler does align each member on the boundary equivalent
 * to their own size.
 *
 * As relayfs subbuffers are aligned on pages, we are sure that they are 8 bytes
 * aligned, so the buffer header and trace header are aligned.
 *
 * Event headers are aligned depending on the trace alignment option. */

struct ltt_event_header_hb {
	uint32_t timestamp;
	unsigned char facility_id;
	unsigned char event_id;
	uint16_t event_size;
} __attribute((packed));

struct ltt_event_header_nohb {
	uint64_t timestamp;
	unsigned char facility_id;
	unsigned char event_id;
	uint16_t event_size;
} __attribute((packed));

struct ltt_trace_header {
	uint32_t magic_number;
	uint32_t arch_type;
	uint32_t arch_variant;
	uint32_t float_word_order;	 /* Only useful for user space traces */
	uint8_t arch_size;
	uint8_t major_version;
	uint8_t minor_version;
	uint8_t flight_recorder;
	uint8_t has_heartbeat;
	uint8_t has_alignment;		/* Event header alignment */
	uint32_t freq_scale;
	uint64_t start_freq;
	uint64_t start_tsc;
	uint64_t start_monotonic;
	uint64_t start_time_sec;
	uint64_t start_time_usec;
} __attribute((packed));


/* We use asm/timex.h : cpu_khz/HZ variable in here : we might have to deal
 * specifically with CPU frequency scaling someday, so using an interpolation
 * between the start and end of buffer values is not flexible enough. Using an
 * immediate frequency value permits to calculate directly the times for parts
 * of a buffer that would be before a frequency change. */
struct ltt_block_start_header {
	struct {
		uint64_t cycle_count;
		uint64_t freq; /* khz */
	} begin;
	struct {
		uint64_t cycle_count;
		uint64_t freq; /* khz */
	} end;
	uint32_t lost_size;	/* Size unused at the end of the buffer */
	uint32_t buf_size;	/* The size of this sub-buffer */
	struct ltt_trace_header	trace;
} __attribute((packed));

/* Get the offset of the channel in the ltt_trace_struct */
#define GET_CHANNEL_INDEX(chan)	\
	(unsigned int)&((struct ltt_trace_struct*)NULL)->channel.chan

/* ltt_get_index_from_facility
 *
 * Get channel index from facility and event id.
 *
 * @fID : facility ID
 * @eID : event number
 *
 * Get the channel index into which events must be written for the given
 * facility and event number. We get this structure offset as soon as possible
 * and remember it so we pass through this logic only once per trace call (not
 * for every trace).
 */
static inline unsigned int ltt_get_index_from_facility(ltt_facility_t fID,
		u8 eID)
{
	if(fID == ltt_facility_core) {
		switch(eID) {
			case event_core_facility_load:
			case event_core_facility_unload:
			case event_core_state_dump_facility_load:
				return GET_CHANNEL_INDEX(facilities);
			default:
				return GET_CHANNEL_INDEX(cpu);
		}
	}
#ifdef CONFIG_LTT_FACILITY_PROCESS
	else if(fID == ltt_facility_process) {
		switch(eID) {
			case event_process_fork:
			case event_process_kernel_thread:
			case event_process_exit:
			case event_process_wait:
			case event_process_free:
				return GET_CHANNEL_INDEX(processes);
			default:
				return GET_CHANNEL_INDEX(cpu);
		}
	}
#endif //CONFIG_LTT_FACILITY_PROCESS
#ifdef CONFIG_LTT_FACILITY_NETWORK_IP_INTERFACE
	else if(fID == ltt_facility_network_ip_interface) {
		switch(eID) {
			case event_network_ip_interface_dev_up:
			case event_network_ip_interface_dev_down:
				return GET_CHANNEL_INDEX(network);
			default:
				return GET_CHANNEL_INDEX(cpu);
		}
	}
#endif //CONFIG_LTT_FACILITY_NETWORK_IP_INTERFACE
	//TODO
	// add modules events
	// add interrupts register/unregister events.

	/* Default channel */
	return GET_CHANNEL_INDEX(cpu);
}

static inline struct ltt_channel_struct *ltt_get_channel_from_index(
		struct ltt_trace_struct *trace, unsigned int index)
{
	return *(struct ltt_channel_struct **)((void*)trace+index);
}


/*
 * ltt_get_header_size
 *
 * Calculate alignment offset for arch size void*. This is the
 * alignment offset of the event header.
 *
 * Important note :
 * The event header must be a size multiple of the void* size. This is necessary
 * to be able to calculate statically the alignment offset of the variable
 * length data fields that follows. The total offset calculated here :
 *
 *	 Alignment of header struct on arch size
 * + sizeof(header struct)
 * + padding added to end of struct to align on arch size.
 * */
static inline unsigned char ltt_get_header_size(struct ltt_trace_struct *trace,
		void *address,
		size_t *before_hdr_pad,
		size_t *after_hdr_pad,
		size_t *header_size)
{
	unsigned int padding;
	unsigned int header;

#ifdef CONFIG_LTT_HEARTBEAT_EVENT
	header = sizeof(struct ltt_event_header_hb);
#else
	header = sizeof(struct ltt_event_header_nohb);
#endif // CONFIG_LTT_HEARTBEAT_EVENT

	/* Padding before the header. Calculated dynamically */
	*before_hdr_pad = ltt_align((unsigned long)address, header);
	padding = *before_hdr_pad;

	/* Padding after header, considering header aligned on ltt_align.
	 * Calculated statically if header size if known. */
	*after_hdr_pad = ltt_align(header, sizeof(void*));
	padding += *after_hdr_pad;

	*header_size = header;

	return header+padding;
}


/* ltt_write_event_header
 *
 * Writes the event header to the pointer.
 *
 * @channel : pointer to the channel structure
 * @ptr : buffer pointer
 * @fID : facility ID
 * @eID : event ID
 * @event_size : size of the event, excluding the event header.
 * @offset : offset of the beginning of the header, for alignment.
 *           Calculated by ltt_get_event_header_size.
 * @tsc : time stamp counter.
 */
static inline void ltt_write_event_header(struct ltt_trace_struct *trace,
		struct ltt_channel_struct *channel,
		void *ptr, ltt_facility_t fID,
		uint32_t eID, size_t event_size,
		size_t offset, u64 tsc)
{
#ifdef CONFIG_LTT_HEARTBEAT_EVENT
	struct ltt_event_header_hb *hb;

	event_size = min(event_size, (size_t)0xFFFFU);
	hb = (struct ltt_event_header_hb *)(ptr+offset);
	hb->timestamp = (u32)tsc;
	hb->facility_id = fID;
	hb->event_id = eID;
	hb->event_size = (uint16_t)event_size;
#else
	struct ltt_event_header_nohb *nohb;

	event_size = min(event_size, (size_t)0xFFFFU);
	nohb = (struct ltt_event_header_nohb *)(ptr+offset);
	nohb->timestamp = (u64)tsc;
	nohb->facility_id = fID;
	nohb->event_id = eID;
	nohb->event_size = (uint16_t)event_size;
#endif //CONFIG_LTT_HEARTBEAT_EVENT
}

/* for flight recording. must be called after relay_commit.
 * This function does not protect from corruption resulting from writing non
 * sequentially in the buffer (and trying to read this buffer after a crash
 * which occured at the wrong moment).
 * That's why sequential writes are good!
 *
 * This function does nothing if trace is in normal mode. */
#if 0
static inline void ltt_write_commit_counter(struct rchan_buf *buf,
		void *reserved)
{
	struct ltt_channel_struct *channel =
		(struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_block_start_header *header =
		(struct ltt_block_start_header*)buf->data;
	unsigned offset, subbuf_idx;

	offset = reserved - buf->start;
	subbuf_idx = offset / buf->chan->subbuf_size;

	if(channel->trace->mode == LTT_TRACE_FLIGHT)
		header->lost_size = buf->chan->subbuf_size -
													buf->commit[subbuf_idx];

}
#endif //0

/* Lockless LTTng */

/* Buffer offset macros */

#define BUFFER_OFFSET(offset, chan) (offset & (chan->alloc_size-1))
#define SUBBUF_OFFSET(offset, chan) (offset & (chan->subbuf_size-1))
#define SUBBUF_ALIGN(offset, chan) \
	(((offset) + chan->subbuf_size) & (~(chan->subbuf_size-1)))
#define SUBBUF_TRUNC(offset, chan) \
	((offset) & (~(chan->subbuf_size-1)))
#define SUBBUF_INDEX(offset, chan) \
	(BUFFER_OFFSET(offset,chan)/chan->subbuf_size)


/* ltt_reserve_slot
 *
 * Atomic slot reservation in a LTTng buffer. It will take care of
 * sub-buffer switching.
 *
 * Parameters:
 *
 * @trace : the trace structure to log to.
 * @buf : the buffer to reserve space into.
 * @data_size : size of the variable length data to log.
 * @slot_size : pointer to total size of the slot (out)
 * @tsc : pointer to the tsc at the slot reservation (out)
 * @before_hdr_pad : dynamic padding before the event header.
 * @after_hdr_pad : dynamic padding after the event header.
 *
 * Return : NULL if not enough space, else returns the pointer
 * 					to the beginning of the reserved slot. */
static inline void *ltt_reserve_slot(
		struct ltt_trace_struct *trace,
		struct rchan_buf *buf,
		unsigned int data_size,
		size_t *slot_size,
		u64 *tsc,
		size_t *before_hdr_pad,
		size_t *after_hdr_pad,
		size_t *header_size)
{
	struct ltt_channel_struct *ltt_channel =
		(struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_channel_buf_struct *ltt_buf = &ltt_channel->buf[buf->cpu];

	int offset_begin, offset_end, offset_old;
	int begin_switch, end_switch_current, end_switch_old;
	int reserve_commit_diff = 0;
	unsigned int size;
	int consumed_old, consumed_new;
	int commit_count, reserve_count;

	if(ltt_nesting[smp_processor_id()] > 1) return NULL;

	do {
		offset_old = atomic_read(&ltt_buf->offset);
		offset_begin = offset_old;
		begin_switch = 0;
		end_switch_current = 0;
		end_switch_old = 0;

		if(SUBBUF_OFFSET(offset_begin, buf->chan) == 0) {
			begin_switch = 1; /* For offset_begin */
		} else {
			size = ltt_get_header_size(trace,
					buf->start + offset_begin,
					before_hdr_pad, after_hdr_pad,
					header_size) + data_size;
			if((SUBBUF_OFFSET(offset_begin, buf->chan)+size)
					> buf->chan->subbuf_size) {
				end_switch_old = 1;	/* For offset_old */
				begin_switch = 1;	/* For offset_begin */
			}
		}
		if(begin_switch) {
			if(end_switch_old) {
				offset_begin =
					SUBBUF_ALIGN(offset_begin, buf->chan);
			}
			offset_begin = offset_begin +
				ltt_channel->subbuf_header_len(buf);
			/* Test new buffer integrity */
			reserve_commit_diff =
				atomic_read(
					&ltt_buf->reserve_count[SUBBUF_INDEX(
								offset_begin,
								buf->chan)])
				- atomic_read(
					&ltt_buf->commit_count[SUBBUF_INDEX(
								offset_begin,
								buf->chan)]);
			if(reserve_commit_diff == 0) {
				/* Next buffer not corrupted. */
				if(!buf->chan->overwrite &&
					(SUBBUF_TRUNC(offset_begin, buf->chan)
					- SUBBUF_TRUNC(
						atomic_read(&ltt_buf->consumed),
						buf->chan))
					>= ltt_channel->rchan->alloc_size) {
					/* We do not overwrite non consumed
					 * buffers and we are full : event
					 * is lost. */
					atomic_inc(&ltt_buf->events_lost);
					return NULL;
				} else {
					/* next buffer not corrupted, we are
					 * either in overwrite mode or the
					 * buffer is not full. It's safe to
					 * write in this new subbuffer.*/
				}
			} else {
				/* Next subbuffer corrupted. Force pushing
				 * reader even in normal mode. It's safe to
				 * write in this new subbuffer. */
			}
			size = ltt_get_header_size(trace,
					buf->start + offset_begin,
					before_hdr_pad, after_hdr_pad,
					header_size) + data_size;
			if((SUBBUF_OFFSET(offset_begin,buf->chan) + size)
					> buf->chan->subbuf_size) {
				/* Event too big for subbuffers, report error,
				 * don't complete the sub-buffer switch. */
				atomic_inc(&ltt_buf->events_lost);
				return NULL;
			} else {
				/* We just made a successful buffer switch and
				 * the event fits in the new subbuffer. Let's
				 * write. */
			}
		} else {
			/* Event fits in the current buffer and we are not on a
			 * switch boundary. It's safe to write */
		}
		offset_end = offset_begin + size;

		if((SUBBUF_OFFSET(offset_end, buf->chan)) == 0) {
			/* The offset_end will fall at the very beginning of
			 * the next subbuffer. */
			end_switch_current = 1;	/* For offset_begin */
		}
#ifdef CONFIG_LTT_HEARTBEAT_EVENT
		if(begin_switch || end_switch_old || end_switch_current) {
			*tsc = ltt_get_timestamp64();
		} else {
			*tsc = ltt_get_timestamp32();
		}
#else
		*tsc = ltt_get_timestamp64();
#endif //CONFIG_LTT_HEARTBEAT_EVENT
		if(*tsc == 0) {
			/* Error in getting the timestamp, event lost */
			atomic_inc(&ltt_buf->events_lost);
			return NULL;
		}

	} while(atomic_cmpxchg(&ltt_buf->offset, offset_old, offset_end)
							!= offset_old);


	/* Push the reader if necessary */
	do {
		consumed_old = atomic_read(&ltt_buf->consumed);
		/* If buffer is in overwrite mode, push the reader consumed
		 * count if the write position has reached it and we are not
		 * at the first iteration (don't push the reader farther than
		 * the writer). This operation can be done concurrently by many
		 * writers in the same buffer, the writer being at the fartest
		 * write position sub-buffer index in the buffer being the one
		 * which will win this loop. */
		/* If the buffer is not in overwrite mode, pushing the reader
		 * only happen if a sub-buffer is corrupted */
		if((SUBBUF_TRUNC(offset_end-1, buf->chan)
					- SUBBUF_TRUNC(consumed_old, buf->chan))
					>= ltt_channel->rchan->alloc_size)
			consumed_new = SUBBUF_ALIGN(consumed_old, buf->chan);
		else {
			consumed_new = consumed_old;
			break;
		}
	} while(atomic_cmpxchg(&ltt_buf->consumed, consumed_old, consumed_new)
						!= consumed_old);

	if(consumed_old != consumed_new) {
		/* Reader pushed : we are the winner of the push, we can
		 * therefore reequilibrate reserve and commit. Atomic increment
		 * of the commit count permits other writers to play around
		 * with this variable before us. We keep track of
		 * corrupted_subbuffers even in overwrite mode :
		 * we never want to write over a non completely committed
		 * sub-buffer : possible causes : the buffer size is too low
		 * compared to the unordered data input, or there is a writer
		 * who died between the reserve and the commit. */
		if(reserve_commit_diff) {
			/* We have to alter the sub-buffer commit count : a
			 * sub-buffer is corrupted. We do not deliver it. */
			atomic_add(
				reserve_commit_diff,
				&ltt_buf->commit_count[
					SUBBUF_INDEX(offset_begin, buf->chan)]);
			atomic_inc(&ltt_buf->corrupted_subbuffers);
		}
	}

	if(end_switch_old) {
		/* old subbuffer */
		/* Concurrency safe because we are the last and only thread to
		 * alter this sub-buffer. As long as it is not delivered and
		 * read, no other thread can alter the offset, alter the
		 * reserve_count or call the client_buffer_end_callback on
		 * this sub-buffer.
		 * The only remaining threads could be the ones with pending
		 * commits. They will have to do the deliver themself.
		 * Not concurrency safe in overwrite mode. We detect corrupted
		 * subbuffers with commit and reserve counts. We keep a
		 * corrupted sub-buffers count and push the readers across
		 * these sub-buffers.
		 * Not concurrency safe if a writer is stalled in a subbuffer
		 * and another writer switches in, finding out it's corrupted.
		 * The result will be than the old (uncommited) subbuffer will
		 * be declared corrupted, and that the new subbuffer will be
		 * declared corrupted too because of the commit count
		 * adjustment.
		 * Note : offset_old should never be 0 here.*/
		ltt_channel->buffer_end(buf, *tsc, offset_old,
			SUBBUF_INDEX((offset_old-1), buf->chan));
		/* Setting this reserve_count will allow the sub-buffer to be
		 * delivered by the last committer. */
		reserve_count =
			 atomic_add_return(
				(SUBBUF_OFFSET((offset_old-1), buf->chan)+1),
				 &ltt_buf->reserve_count[
				 SUBBUF_INDEX((offset_old-1), buf->chan)]);
		if(reserve_count == atomic_read(
			&ltt_buf->commit_count[SUBBUF_INDEX((offset_old-1),
						buf->chan)])) {
			buf->chan->cb->deliver(buf, SUBBUF_INDEX((offset_old-1),
						buf->chan), NULL);
		}
	}

	if(begin_switch) {
		/* New sub-buffer */
		/* This code can be executed unordered : writers may already
		 * have written to the sub-buffer before this code gets
		 * executed, caution. */
		/* The commit makes sure that this code is executed before the
		 * deliver of this sub-buffer */
		ltt_channel->buffer_begin(buf, *tsc, SUBBUF_INDEX(offset_begin,
					buf->chan));
		commit_count = atomic_add_return(
				ltt_channel->subbuf_header_len(buf),
				&ltt_buf->commit_count[
					SUBBUF_INDEX(offset_begin, buf->chan)]);
		/* Check if the written buffer has to be delivered */
		if(commit_count == atomic_read(
			&ltt_buf->reserve_count[SUBBUF_INDEX(offset_begin,
							buf->chan)])) {
			buf->chan->cb->deliver(buf,
				SUBBUF_INDEX(offset_begin, buf->chan), NULL);
		}
	}

	if(end_switch_current) {
		/* current subbuffer */
		/* Concurrency safe because we are the last and only thread to
		 * alter this sub-buffer. As long as it is not delivered and
		 * read, no other thread can alter the offset, alter the
		 * reserve_count or call the client_buffer_end_callback on this
		 * sub-buffer.
		 * The only remaining threads could be the ones with pending
		 * commits. They will have to do the deliver themself.
		 * Not concurrency safe in overwrite mode. We detect corrupted
		 * subbuffers with commit and reserve counts. We keep a
		 * corrupted sub-buffers count and push the readers across
		 * these sub-buffers.
		 * Not concurrency safe if a writer is stalled in a subbuffer
		 * and another writer switches in, finding out it's corrupted.
		 * The result will be than the old (uncommited) subbuffer will
		 * be declared corrupted, and that the new subbuffer will be
		 * declared corrupted too because of the commit count
		 * adjustment. */
		ltt_channel->buffer_end(buf, *tsc, offset_end,
			SUBBUF_INDEX((offset_end-1), buf->chan));
		/* Setting this reserve_count will allow the sub-buffer to be
		 * delivered by the last committer. */
		reserve_count = atomic_add_return(
			(SUBBUF_OFFSET((offset_end-1), buf->chan)+1),
			&ltt_buf->reserve_count[SUBBUF_INDEX((offset_end-1),
							buf->chan)]);
		if(reserve_count == atomic_read(
			&ltt_buf->commit_count[SUBBUF_INDEX((offset_end-1),
							buf->chan)])) {
			buf->chan->cb->deliver(buf,
				SUBBUF_INDEX((offset_end-1), buf->chan), NULL);
		}
	}

	*slot_size = size;

	//BUG_ON(*slot_size != (data_size + *before_hdr_pad + *after_hdr_pad + *header_size));
	//BUG_ON(*slot_size != (offset_end - offset_begin));

	return buf->start + BUFFER_OFFSET(offset_begin, buf->chan);
}


/* ltt_commit_slot
 *
 * Atomic unordered slot commit. Increments the commit count in the
 * specified sub-buffer, and delivers it if necessary.
 *
 * Parameters:
 *
 * @buf : the buffer to commit to.
 * @reserved : address of the beginnig of the reserved slot.
 * @slot_size : size of the reserved slot.
 *
 */
static inline void ltt_commit_slot(
		struct rchan_buf *buf,
		void *reserved,
		unsigned int slot_size)
{
	struct ltt_channel_struct *ltt_channel =
		(struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_channel_buf_struct *ltt_buf = &ltt_channel->buf[buf->cpu];
	unsigned int offset_begin = reserved - buf->start;
	int commit_count;

	commit_count = atomic_add_return(slot_size,
			&ltt_buf->commit_count[SUBBUF_INDEX(offset_begin,
							buf->chan)]);
	/* Check if all commits have been done */
	if(commit_count	== atomic_read(
		&ltt_buf->reserve_count[
			SUBBUF_INDEX(offset_begin, buf->chan)])) {
		buf->chan->cb->deliver(buf,
			SUBBUF_INDEX(offset_begin, buf->chan), NULL);
	}
}

#endif //CONFIG_LTT


/* Is kernel tracer enabled */
#if defined(CONFIG_LTT_TRACER) || defined(CONFIG_LTT_TRACER_MODULE)

/* 4 control channels :
 * ltt/control/facilities
 * ltt/control/interrupts
 * ltt/control/processes
 * ltt/control/network
 *
 * 1 cpu channel :
 * ltt/cpu
 */
#define LTT_RELAYFS_ROOT	"ltt"
#define LTT_CONTROL_ROOT	"control"
#define LTT_FACILITIES_CHANNEL	"facilities"
#define LTT_INTERRUPTS_CHANNEL	"interrupts"
#define LTT_PROCESSES_CHANNEL	"processes"
#define LTT_MODULES_CHANNEL	"modules"
#define LTT_NETWORK_CHANNEL	"network"
#define LTT_CPU_CHANNEL		"cpu"

/* System types */
#define LTT_SYS_TYPE_VANILLA_LINUX	1

/* Architecture types */
#define LTT_ARCH_TYPE_I386		1
#define LTT_ARCH_TYPE_PPC		2
#define LTT_ARCH_TYPE_SH		3
#define LTT_ARCH_TYPE_S390		4
#define LTT_ARCH_TYPE_MIPS		5
#define LTT_ARCH_TYPE_ARM		6
#define LTT_ARCH_TYPE_PPC64		7
#define LTT_ARCH_TYPE_X86_64		8
#define LTT_ARCH_TYPE_C2		9
#define LTT_ARCH_TYPE_POWERPC		10

/* Standard definitions for variants */
#define LTT_ARCH_VARIANT_NONE		0

/* Tracer properties */
#define LTT_DEFAULT_SUBBUF_SIZE		1048576
#define LTT_DEFAULT_N_SUBBUFS		2
#define LTT_TRACER_MAGIC_NUMBER		0x00D6B7ED
#define LTT_TRACER_VERSION_MAJOR	0
#define LTT_TRACER_VERSION_MINOR	7

/* Size reserved for high priority events (interrupts, NMI, BH) at the end of a
 * nearly full buffer. User space won't use this last amount of space when in
 * blocking mode. This space also includes the event header that would be
 * written by this user space event. */
#define LTT_RESERVE_CRITICAL		4096

/* Register and unregister function pointers */

enum ltt_module_function {
	LTT_FUNCTION_RUN_FILTER,
	LTT_FUNCTION_FILTER_CONTROL,
	LTT_FUNCTION_STATEDUMP
};

extern int ltt_module_register(enum ltt_module_function name, void *function,
		struct module *owner);
extern void ltt_module_unregister(enum ltt_module_function name);

/* Exported control function */

enum ltt_heartbeat_functor_msg { LTT_HEARTBEAT_START, LTT_HEARTBEAT_STOP };

enum ltt_control_msg {
	LTT_CONTROL_START,
	LTT_CONTROL_STOP,
	LTT_CONTROL_CREATE_TRACE,
	LTT_CONTROL_DESTROY_TRACE
};

union ltt_control_args {
	struct {
		enum trace_mode mode;
		unsigned subbuf_size;
		unsigned n_subbufs;
	} new_trace;
};

extern int ltt_control(enum ltt_control_msg msg, char *trace_name,
		union ltt_control_args args);

enum ltt_filter_control_msg {
	LTT_FILTER_DEFAULT_ACCEPT,
	LTT_FILTER_DEFAULT_REJECT };

extern int ltt_filter_control(enum ltt_filter_control_msg msg,
		char *trace_name);


void ltt_core_register(int (*function)(u8, void*));

void ltt_core_unregister(void);

#ifdef CONFIG_LTT_HEARTBEAT
int ltt_heartbeat_trigger(enum ltt_heartbeat_functor_msg msg);
#endif //CONFIG_LTT_HEARTBEAT

/* Relayfs IOCTL */

/* Get the next sub buffer that can be read. */
#define RELAYFS_GET_SUBBUF		_IOR(0xF4, 0x00,__u32)
/* Release the oldest reserved (by "get") sub buffer. */
#define RELAYFS_PUT_SUBBUF		_IOW(0xF4, 0x01,__u32)
/* returns the number of sub buffers in the per cpu channel. */
#define RELAYFS_GET_N_SUBBUFS		_IOR(0xF4, 0x02,__u32)
/* returns the size of the sub buffers. */
#define RELAYFS_GET_SUBBUF_SIZE		_IOR(0xF4, 0x03,__u32)

#endif /* defined(CONFIG_LTT_TRACER) || defined(CONFIG_LTT_TRACER_MODULE) */
#endif /* _LTT_CORE_H */
