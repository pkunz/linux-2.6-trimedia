/*
 * ltt-core.c
 *
 * (C) Copyright	2005-2006 -
 * 		Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * Contains the kernel code for the Linux Trace Toolkit.
 *
 * Author:
 *	Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 *
 * Inspired from LTT :
 *	Karim Yaghmour (karim@opersys.com)
 *	Tom Zanussi (zanussi@us.ibm.com)
 *	Bob Wisniewski (bob@watson.ibm.com)
 * And from K42 :
 *  Bob Wisniewski (bob@watson.ibm.com)
 *
 * Changelog:
 *  19/10/05, Complete lockless mechanism. (Mathieu Desnoyers)
 *	27/05/05, Modular redesign and rewrite. (Mathieu Desnoyers)

 * Comments :
 * num_active_traces protects the functors. Changing the pointer is an atomic
 * operation, but the functions can only be called when in tracing. It is then
 * safe to unload a module in which sits a functor when no tracing is active.
 *
 * filter_control functor is protected by incrementing its module refcount.
 *
 */

#include <linux/config.h>
#include <linux/time.h>
#include <linux/ltt-core.h>
#include <linux/relayfs_fs.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/ltt-facilities.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/bitops.h>
#include <linux/fs.h>
#include <asm/atomic.h>

static struct dentry *ltt_root_dentry;

/* How a force_switch must be done ?
 *
 * Is it done during tracing or as a final flush after tracing
 * (so it won't write in the new sub-buffer).
 */
enum force_switch_mode { FORCE_ACTIVE, FORCE_FLUSH };

/* Traces list writer locking */
DECLARE_MUTEX(ltt_traces_sem);

static struct timer_list ltt_async_wakeup_timer;

/* Default callbacks for modules */
int ltt_run_filter_default(void)
{
	return 1;
}

int ltt_filter_control_default
	(enum ltt_filter_control_msg msg, struct ltt_trace_struct *trace)
{
	return 0;
}

int ltt_statedump_default(struct ltt_trace_struct *trace)
{
	return 0;
}



/* Callbacks for registered modules */

int (*ltt_filter_control_functor)
	(enum ltt_filter_control_msg msg, struct ltt_trace_struct *trace) =
					ltt_filter_control_default;
struct module *ltt_filter_control_owner = NULL;

/* These function pointers are protected by trace activation check */

int (*ltt_run_filter_functor)(void) = ltt_run_filter_default;
struct module *ltt_run_filter_owner = NULL;

// FIXME : integrate the filter in the logging chain.
//
int (*ltt_statedump_functor)(struct ltt_trace_struct *trace) =
					ltt_statedump_default;
struct module *ltt_statedump_owner = NULL;

/* Module registration methods */

int ltt_module_register(enum ltt_module_function name, void *function,
		struct module *owner)
{
	int ret = 0;

	/* Protect these operations by disallowing them when tracing is
	 * active */
	if(ltt_traces.num_active_traces) {
		ret = -EBUSY;
		goto end;
	}

	switch(name) {
		case LTT_FUNCTION_RUN_FILTER:
			if(ltt_run_filter_owner != NULL) {
				ret = -EEXIST;
				goto end;
			}
			ltt_run_filter_functor = (int (*)(void))function;
			ltt_run_filter_owner = owner;
			break;
		case LTT_FUNCTION_FILTER_CONTROL:
			if(ltt_filter_control_owner != NULL) {
				ret = -EEXIST;
				goto end;
			}
			ltt_filter_control_functor =
				(int (*)(enum ltt_filter_control_msg,
				struct ltt_trace_struct *))function;
			break;
		case LTT_FUNCTION_STATEDUMP:
			if(ltt_statedump_owner != NULL) {
				ret = -EEXIST;
				goto end;
			}
			ltt_statedump_functor =
				(int (*)(struct ltt_trace_struct *))function;
			ltt_statedump_owner = owner;
			break;
	}

end:

	return ret;
}


void ltt_module_unregister(enum ltt_module_function name)
{
	/* (BUG_ON) It should NEVER happen : modules has their reference count
	 * incremented when tracing is active. */
	BUG_ON(ltt_traces.num_active_traces);

	switch(name) {
		case LTT_FUNCTION_RUN_FILTER:
			ltt_run_filter_functor = ltt_run_filter_default;
			ltt_run_filter_owner = NULL;
			break;
		case LTT_FUNCTION_FILTER_CONTROL:
			ltt_filter_control_functor = ltt_filter_control_default;
			break;
		case LTT_FUNCTION_STATEDUMP:
			ltt_statedump_functor = ltt_statedump_default;
			ltt_statedump_owner = NULL;
			break;
	}

}

EXPORT_SYMBOL_GPL(ltt_module_register);
EXPORT_SYMBOL_GPL(ltt_module_unregister);

static void ltt_write_trace_header(struct ltt_trace_struct *trace,
		struct ltt_trace_header *header)
{
	header->magic_number = LTT_TRACER_MAGIC_NUMBER;
	header->major_version = LTT_TRACER_VERSION_MAJOR;
	header->minor_version = LTT_TRACER_VERSION_MINOR;
	header->float_word_order = 0;	 /* Kernel : no floating point */
	header->arch_type = LTT_ARCH_TYPE;
	header->arch_size = sizeof(void*);
	header->arch_variant = LTT_ARCH_VARIANT;
	header->flight_recorder = (trace->mode == LTT_TRACE_FLIGHT);
#ifdef CONFIG_LTT_HEARTBEAT_EVENT
	header->has_heartbeat = 1;
#else
	header->has_heartbeat = 0;
#endif //CONFIG_LTT_HEARTBEAT_EVENT

#ifdef CONFIG_LTT_ALIGNMENT
	header->has_alignment = sizeof(void*);
#else
	header->has_alignment = 0;
#endif

	header->freq_scale = trace->freq_scale;
	header->start_freq = trace->start_freq;
	header->start_tsc = trace->start_tsc;
	header->start_monotonic = trace->start_monotonic;
	header->start_time_sec = trace->start_time.tv_sec;
	header->start_time_usec = trace->start_time.tv_usec;
}

/* Trace callbacks */

static unsigned int ltt_subbuf_header_len_callback(struct rchan_buf *buf)
{
	return sizeof(struct ltt_block_start_header);
}


static void ltt_buffer_begin_callback(struct rchan_buf *buf,
		      u64 tsc, unsigned int subbuf_idx)
{
	struct ltt_channel_struct *channel =
		(struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_block_start_header *header =
		(struct ltt_block_start_header*)
			(buf->start + (subbuf_idx*buf->chan->subbuf_size));

	header->begin.cycle_count = tsc;
	header->begin.freq = ltt_frequency();
	header->lost_size = 0xFFFFFFFF; // for debugging...
	header->buf_size = buf->chan->subbuf_size;
	ltt_write_trace_header(channel->trace, &header->trace);

}

static void ltt_buffer_end_callback(struct rchan_buf *buf,
		      u64 tsc, unsigned int offset, unsigned int subbuf_idx)
{
	struct ltt_block_start_header *header =
		(struct ltt_block_start_header*)
			(buf->start + (subbuf_idx*buf->chan->subbuf_size));

	/* offset is assumed to never be 0 here : never deliver a completely
	 * empty subbuffer.
	 * The lost size is between 0 and subbuf_size-1 */
	header->lost_size = SUBBUF_OFFSET((buf->chan->subbuf_size - offset),
				buf->chan);
	header->end.cycle_count = tsc;
	header->end.freq = ltt_frequency();
}

static void ltt_deliver_callback(struct rchan_buf *buf,
		unsigned subbuf_idx,
		void *subbuf)
{
	struct ltt_channel_struct *channel =
		(struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_channel_buf_struct *ltt_buf = &channel->buf[buf->cpu];

	atomic_set(&ltt_buf->wakeup_readers, 1);
}

static void async_wakeup_chan(struct ltt_channel_struct *ltt_chan)
{
	unsigned int i;

	for(i=0; i<NR_CPUS; i++) {
		if(!ltt_chan->rchan->buf[i])
			continue;
		if(atomic_read(&ltt_chan->buf[i].wakeup_readers) == 1) {
			atomic_set(&ltt_chan->buf[i].wakeup_readers, 0);
			wake_up_interruptible(
					&ltt_chan->rchan->buf[i]->read_wait);
		}
	}
}

static void trace_async_wakeup(struct ltt_trace_struct *trace)
{
	/* Must check each channel for pending read wakeup */
	async_wakeup_chan(trace->channel.facilities);
	async_wakeup_chan(trace->channel.interrupts);
	async_wakeup_chan(trace->channel.processes);
	async_wakeup_chan(trace->channel.modules);
	async_wakeup_chan(trace->channel.cpu);
	async_wakeup_chan(trace->channel.network);
}

/* Timer to send async wakeups to the readers */
static void async_wakeup(unsigned long data)
{
	struct ltt_trace_struct *trace;
	rcu_read_lock();
	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		trace_async_wakeup(trace);
	}
	rcu_read_unlock();

	del_timer(&ltt_async_wakeup_timer);
	ltt_async_wakeup_timer.expires = jiffies + 1;
	add_timer(&ltt_async_wakeup_timer);
}


static void ltt_buf_mapped_callback(struct rchan_buf *buf,
		struct file *filp)
{
}

static void ltt_buf_unmapped_callback(struct rchan_buf *buf,
		struct file *filp)
{
}

static void ltt_buf_full_callback(struct rchan_buf *buf,
		unsigned subbuf_idx,
		void *subbuf)
{
}

static void wakeup_writers(void *private)
{
	struct ltt_channel_buf_struct *ltt_buf = private;
	wake_up_interruptible(&ltt_buf->write_wait);
}

/* This callback should not be called from NMI interrupt context */
static void ltt_buf_unfull_callback(struct rchan_buf *buf,
		unsigned subbuf_idx,
		void *subbuf)
{
	struct ltt_channel_struct *ltt_channel =
		(struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_channel_buf_struct *ltt_buf = &ltt_channel->buf[buf->cpu];
	if (waitqueue_active(&ltt_buf->write_wait)) {
		schedule_work(&ltt_buf->wake_writers);
	}
}

/* _ltt_trace_find :
 * find a trace by given name.
 *
 * Returns a pointer to the trace structure, NULL if not found. */
static struct ltt_trace_struct *_ltt_trace_find(char *trace_name)
{
	int compare;
	struct ltt_trace_struct *trace, *found=NULL;

	list_for_each_entry(trace, &ltt_traces.head, list) {
		compare = strncmp(trace->trace_name, trace_name, NAME_MAX);

		if(compare == 0) {
			found = trace;
			break;
		}
	}

	return found;
}

/* This function must be called with traces semaphore held. */
static int _ltt_trace_create(char *trace_name,	enum trace_mode mode,
				unsigned subbuf_size, unsigned n_subbufs,
				struct ltt_trace_struct *new_trace)
{
	int err = EPERM;

	if(_ltt_trace_find(trace_name) != NULL) {
		printk(KERN_ERR "LTT : Trace %s already exists\n", trace_name);
		err = EEXIST;
		goto traces_error;
	}
	list_add_rcu(&new_trace->list, &ltt_traces.head);
	synchronize_rcu();
	/* Everything went fine, finish creation */
	return 0;

	/* Error handling */
traces_error:
	return err;
}



/**
 *	ltt_poll - poll file op for ltt files
 *	@filp: the file
 *	@wait: poll table
 *
 *	Poll implemention.
 */
unsigned int ltt_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct inode *inode;
	struct rchan_buf *buf;
	struct ltt_channel_struct *ltt_channel;
	struct ltt_channel_buf_struct *ltt_buf;
#if 0
	printk(KERN_EMERG "In LTT poll filp = %p\n", filp);
#endif
	inode = filp->f_dentry->d_inode;
#if 0	/* 2.6.17 */
	buf = inode->u.generic_ip;
#else
	buf = RELAYFS_I(inode)->buf;
#endif
#if 0
	printk(KERN_EMERG " inode = %p  buf = %p\n", inode, buf);
#endif
	ltt_channel = (struct ltt_channel_struct*)buf->chan->client_data;
	ltt_buf = &ltt_channel->buf[buf->cpu];
#if 0
	printk(KERN_EMERG " ltt_channel = %p  ltt_buf = %p\n", ltt_channel, ltt_buf);
#endif
	if (filp->f_mode & FMODE_READ) {
		poll_wait(filp, &buf->read_wait, wait);

		if(atomic_read(&ltt_buf->active_readers) != 0) {
			return 0;
		} else {
			if(SUBBUF_TRUNC(
				atomic_read(&ltt_buf->offset), buf->chan)
			- SUBBUF_TRUNC(
				atomic_read(&ltt_buf->consumed), buf->chan)
			== 0) {
				if(buf->finalized) return POLLHUP;
				else return 0;
			} else {
				if(SUBBUF_TRUNC(
					atomic_read(&ltt_buf->offset), buf->chan)
				- SUBBUF_TRUNC(
					atomic_read(&ltt_buf->consumed),
							buf->chan)
				>= ltt_channel->rchan->alloc_size)
					return POLLPRI | POLLRDBAND;
				else
					return POLLIN | POLLRDNORM;
			}
		}
	}
	return mask;
}


/**
 *	ltt_ioctl - ioctl control on the relayfs file
 *
 *	@inode: the inode
 *	@filp: the file
 *	@cmd: the command
 *	@arg: command arg
 *
 *	This ioctl implements three commands necessary for a minimal
 *	producer/consumer implementation :
 *	RELAYFS_GET_SUBBUF
 *		Get the next sub buffer that can be read. It never blocks.
 *	RELAYFS_PUT_SUBBUF
 *		Release the currently read sub-buffer. Parameter is the last
 *		put subbuffer (returned by GET_SUBBUF).
 *	RELAYFS_GET_N_BUBBUFS
 *		returns the number of sub buffers in the per cpu channel.
 *	RELAYFS_GET_SUBBUF_SIZE
 *		returns the size of the sub buffers.
 *
 */
int ltt_ioctl(struct inode *inode, struct file *filp,
		unsigned int cmd, unsigned long arg)
{
#if 0	/* 2.6.17 */
	struct rchan_buf *buf = inode->u.generic_ip;
#else
	struct rchan_buf *buf = RELAYFS_I(inode)->buf;
#endif
	struct ltt_channel_struct *ltt_channel =
            (struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_channel_buf_struct *ltt_buf = &ltt_channel->buf[buf->cpu];
	u32 __user *argp = (u32 __user *)arg;

	switch(cmd) {
		case RELAYFS_GET_SUBBUF:
		{
			unsigned int consumed_old, consumed_idx;
			atomic_inc(&ltt_buf->active_readers);
			consumed_old = atomic_read(&ltt_buf->consumed);
			consumed_idx = SUBBUF_INDEX(consumed_old, buf->chan);
			if(atomic_read(&ltt_buf->commit_count[consumed_idx])
			!= atomic_read(&ltt_buf->reserve_count[consumed_idx])) {
				atomic_dec(&ltt_buf->active_readers);
				return -EAGAIN;
			}
			if((SUBBUF_TRUNC(
				atomic_read(&ltt_buf->offset), buf->chan)
			- SUBBUF_TRUNC(consumed_old, buf->chan))
			== 0) {
				atomic_dec(&ltt_buf->active_readers);
				return -EAGAIN;
			}
			printk(KERN_DEBUG "LTT ioctl get subbuf %d\n",
					consumed_old);
			return put_user((u32)consumed_old, argp);
			break;
		}
		case RELAYFS_PUT_SUBBUF:
		{
			u32 consumed_old;
			int ret;
			unsigned int consumed_new;

			ret = get_user(consumed_old, argp);
			if(ret) return ret; /* will return -EFAULT */

			printk(KERN_DEBUG "LTT ioctl put subbuf %d\n",
					consumed_old);
			consumed_new = SUBBUF_ALIGN(consumed_old, buf->chan);
			spin_lock(&ltt_buf->full_lock);
			if(atomic_cmpxchg(
				&ltt_buf->consumed, consumed_old, consumed_new)
					!= consumed_old) {
				/* We have been pushed by the writer : the last
				 * buffer read _is_ corrupted! It can also
				 * happen if this is a buffer we never got. */
				atomic_dec(&ltt_buf->active_readers);
				spin_unlock(&ltt_buf->full_lock);
				return -EIO;
			} else {
				/* tell the client that buffer is now unfull */
				int index;
				void *data;
				index = SUBBUF_INDEX(consumed_old, buf->chan);
				data = buf->start +
					BUFFER_OFFSET(consumed_old, buf->chan);
				buf->chan->cb->buf_unfull(buf, index, data);
				atomic_dec(&ltt_buf->active_readers);
				spin_unlock(&ltt_buf->full_lock);
			}
			break;
		}
		case RELAYFS_GET_N_SUBBUFS:
			printk(KERN_DEBUG "LTT ioctl get n subbufs\n");
			return put_user((u32)buf->chan->n_subbufs, argp);
			break;
		case RELAYFS_GET_SUBBUF_SIZE:
			printk(KERN_DEBUG "LTT ioctl get subbuf size\n");
			return put_user((u32)buf->chan->subbuf_size, argp);
			break;
		default:
			return -ENOIOCTLCMD;
	}
	return 0;
}

#ifdef CONFIG_COMPAT

static long ltt_compat_ioctl(struct file *file, unsigned cmd, unsigned long arg)
{
	long ret = -ENOIOCTLCMD;

	lock_kernel();
	ret = ltt_ioctl(file->f_dentry->d_inode, file, cmd, arg);
	unlock_kernel();

	return ret;
}

#endif //CONFIG_COMPAT

struct file_operations ltt_file_operations;

EXPORT_SYMBOL(ltt_file_operations);

static void print_subbuffer_errors(struct ltt_channel_struct *ltt_chan,
		int cons_off, unsigned int i)
{
  int cons_idx;

	printk(KERN_WARNING
		"LTT : unread channel %s offset is %d "
		"and cons_off : %d (cpu %u)\n",
		ltt_chan->channel_name,
		atomic_read(&ltt_chan->buf[i].offset), cons_off, i);
	/* Check each sub-buffer for unequal reserve and commit count */
	cons_idx = SUBBUF_INDEX(cons_off, ltt_chan->rchan);
	if(atomic_read(&ltt_chan->buf[i].reserve_count[cons_idx])
		!= atomic_read(&ltt_chan->buf[i].commit_count[cons_idx]))
		printk(KERN_ALERT
			"LTT : %s : subbuffer %u has unequal "
			"reserve and commit counts.\n",
			ltt_chan->channel_name, cons_idx);
	printk(KERN_ALERT "LTT : %s : reserve count : %u, commit count : %u\n",
			ltt_chan->channel_name,
			atomic_read(&ltt_chan->buf[i].reserve_count[cons_idx]),
			atomic_read(&ltt_chan->buf[i].commit_count[cons_idx]));
}

static void print_channel_errors(struct ltt_channel_struct *ltt_chan)
{
	unsigned int i;
	int cons_off;

	for(i=0;i<NR_CPUS;i++) {
		if(atomic_read(&ltt_chan->buf[i].events_lost))
			printk(KERN_ALERT
				"LTT : %s : %d events lost "
				"in %s channel (cpu %u).\n",
				ltt_chan->channel_name,
				atomic_read(&ltt_chan->buf[i].events_lost),
				ltt_chan->channel_name, i);
		if(atomic_read(&ltt_chan->buf[i].corrupted_subbuffers))
			printk(KERN_ALERT
				"LTT : %s : %d corrupted subbuffers "
				"in %s channel (cpu %u).\n",
				ltt_chan->channel_name,
				atomic_read(
					&ltt_chan->buf[i].corrupted_subbuffers),
				ltt_chan->channel_name, i);
		for(cons_off = atomic_read(&ltt_chan->buf[i].consumed);
			(SUBBUF_TRUNC(atomic_read(&ltt_chan->buf[i].offset),
					ltt_chan->rchan)
				- cons_off) > 0;
			cons_off = SUBBUF_ALIGN(cons_off, ltt_chan->rchan)) {
			print_subbuffer_errors(ltt_chan, cons_off, i);
		}
	}
}


static void ltt_channel_destroy(void *object)
{
	struct ltt_channel_struct *ltt_chan =
		(struct ltt_channel_struct *)object;
	unsigned int i;

	/* Note : we check that every channel has equal reserve/commit count.
	 * It's ok if reserve and commit are surrounded by disable_preempt (see
	 * synchronize_rcu()), but it won't be the case for user space
	 * tracing... be warned. FIXME */
	print_channel_errors(ltt_chan);

	for(i=0;i<NR_CPUS;i++) {
		kfree(ltt_chan->buf[i].reserve_count);
		kfree(ltt_chan->buf[i].commit_count);
	}
	kfree(ltt_chan);
}

/* Create channel.
 */
static int ltt_channel_create(char *trace_name,
		struct ltt_trace_struct *trace,
		struct dentry *dir,
		char *channel_name,
		struct ltt_channel_struct **ltt_chan,
		unsigned int subbuf_size, unsigned int n_subbufs,
		int overwrite)
{
	unsigned int i, j;

	/* This channel info will be freed by relayfs if the relay_open
	 * succeed.
	 * FIXME : in one error path, it will be freed by the kref_put */
	*ltt_chan = kmalloc(sizeof(struct ltt_channel_struct), GFP_KERNEL);

	(*ltt_chan)->trace = trace;
	(*ltt_chan)->rchan = relay_open(channel_name,
			dir,
			subbuf_size,
			n_subbufs,
			overwrite,
			THIS_MODULE,
			&trace->callbacks,
			&ltt_file_operations,
			*ltt_chan,
			ltt_channel_destroy);

	if((*ltt_chan)->rchan == NULL) {
		printk(KERN_ERR "LTT : Can't open %s channel for trace %s\n",
											channel_name, trace_name);
		goto error;
	}

	(*ltt_chan)->subbuf_header_len = ltt_subbuf_header_len_callback;
	(*ltt_chan)->buffer_begin = ltt_buffer_begin_callback;
	(*ltt_chan)->buffer_end = ltt_buffer_end_callback;

	strncpy((*ltt_chan)->channel_name, channel_name, PATH_MAX);

	for(i=0;i<NR_CPUS;i++) {
		atomic_set(&(*ltt_chan)->buf[i].offset,
			ltt_subbuf_header_len_callback(
				(*ltt_chan)->rchan->buf[i]));
		atomic_set(&(*ltt_chan)->buf[i].consumed, 0);
		atomic_set(&(*ltt_chan)->buf[i].active_readers, 0);
		(*ltt_chan)->buf[i].reserve_count =
			kmalloc(sizeof(atomic_t) * n_subbufs, GFP_KERNEL);
		for(j=0; j<n_subbufs; j++)
			atomic_set(&(*ltt_chan)->buf[i].reserve_count[j], 0);
		(*ltt_chan)->buf[i].commit_count =
			kmalloc(sizeof(atomic_t) * n_subbufs, GFP_KERNEL);
		for(j=0; j<n_subbufs; j++)
			atomic_set(&(*ltt_chan)->buf[i].commit_count[j], 0);
		init_waitqueue_head(&(*ltt_chan)->buf[i].write_wait);
		atomic_set(&(*ltt_chan)->buf[i].wakeup_readers, 0);
		INIT_WORK(&(*ltt_chan)->buf[i].wake_writers,
				wakeup_writers, &(*ltt_chan)->buf[i]);
		spin_lock_init(&(*ltt_chan)->buf[i].full_lock);
		/* Tracing not started yet :
		 * we don't deal with concurrency (no delivery) */
		if(!(*ltt_chan)->rchan->buf[i])
			continue;

		ltt_buffer_begin_callback((*ltt_chan)->rchan->buf[i],
				trace->start_tsc, 0);
		atomic_add((*ltt_chan)->subbuf_header_len(
					(*ltt_chan)->rchan->buf[i]),
			&(*ltt_chan)->buf[i].commit_count[0]);
	}
	return 0;

error:
	ltt_channel_destroy(*ltt_chan);
	return EPERM;
}


static void init_error_count(struct ltt_channel_struct *ltt_chan)
{
	unsigned int i;
	for(i=0;i<NR_CPUS;i++) {
		atomic_set(&ltt_chan->buf[i].events_lost, 0);
		atomic_set(&ltt_chan->buf[i].corrupted_subbuffers, 0);
	}
}

static int ltt_trace_create(char *trace_name, enum trace_mode mode,
		unsigned subbuf_size, unsigned n_subbufs)
{
	int overwrite;
	int err = 0;
	struct ltt_trace_struct *new_trace;
	unsigned long flags;
	unsigned int subbuf_size_align;

	if(subbuf_size == 0) subbuf_size = LTT_DEFAULT_SUBBUF_SIZE;
	if(n_subbufs == 0) n_subbufs = LTT_DEFAULT_N_SUBBUFS;
	subbuf_size_align = (subbuf_size + PAGE_SIZE-1)&PAGE_MASK;

	/* Subbuf size and number must both be power of two */
	if(hweight32(subbuf_size_align) != 1) return EINVAL;
	if(hweight32(n_subbufs) != 1) return EINVAL;

	new_trace = kcalloc(1, sizeof(struct ltt_trace_struct), GFP_KERNEL);
	if(!new_trace) {
		printk(KERN_ERR
			"LTT : Unable to allocate memory for trace %s\n",
			trace_name);
		err = ENOMEM;
		goto traces_error;
	}

	new_trace->dentry.trace_root = relayfs_create_dir(trace_name,
			ltt_root_dentry);
	if(new_trace->dentry.trace_root == NULL) {
		printk(KERN_ERR "LTT : Trace directory name %s already taken\n",
			trace_name);
		err = EEXIST;
		goto trace_error;
	}

	new_trace->dentry.control_root = relayfs_create_dir(LTT_CONTROL_ROOT,
			new_trace->dentry.trace_root);
	if(new_trace->dentry.control_root == NULL) {
		printk(KERN_ERR "LTT : Trace control subdirectory name "
			"%s/%s already taken\n",
			trace_name, LTT_CONTROL_ROOT);
		err = EEXIST;
		goto control_error;
	}

	new_trace->active = 0;
	strncpy(new_trace->trace_name, trace_name, NAME_MAX);
	new_trace->paused = 0;
	new_trace->mode = mode;
	new_trace->freq_scale = ltt_freq_scale();

	/* Allocate the relayfs channels */

	switch(mode) {
		case LTT_TRACE_NORMAL:
			overwrite = 0;
			break;
		case LTT_TRACE_FLIGHT:
			overwrite = 1;
			break;
		default:
			overwrite = 0;
	}

	local_irq_save(flags);
	new_trace->start_freq = ltt_frequency();
	new_trace->start_tsc = ltt_get_timestamp64();
	do_gettimeofday(&new_trace->start_time);
	local_irq_restore(flags);

	//new_trace->callbacks.subbuf_start = ltt_subbuf_start_callback;
	new_trace->callbacks.deliver = ltt_deliver_callback;
	new_trace->callbacks.buf_mapped = ltt_buf_mapped_callback;
	new_trace->callbacks.buf_unmapped = ltt_buf_unmapped_callback;
	new_trace->callbacks.buf_full = ltt_buf_full_callback;
	new_trace->callbacks.buf_unfull = ltt_buf_unfull_callback;

	/* Always put the facilities channel in non-overwrite mode :
	 * This is a very low traffic channel and it can't afford to have its
	 * data overwritten : this data (facilities info) is necessary to be
	 * able to read the trace.
	 *
	 * WARNING : The heartbeat time _shouldn't_ write events in the
	 * facilities channel as it would make the traffic too high. This is a
	 * problematic case with flight recorder mode. FIXME
	 */
	err = ltt_channel_create(trace_name, new_trace,
			new_trace->dentry.control_root, LTT_FACILITIES_CHANNEL,
			&new_trace->channel.facilities, subbuf_size_align,
			n_subbufs, 0);
	if(err != 0) {
		goto facilities_error;
	}
	err = ltt_channel_create(trace_name, new_trace,
			new_trace->dentry.control_root, LTT_INTERRUPTS_CHANNEL,
			&new_trace->channel.interrupts, subbuf_size_align,
			n_subbufs, overwrite);
	if(err != 0) {
		goto interrupts_error;
	}
	err = ltt_channel_create(trace_name, new_trace,
			new_trace->dentry.control_root, LTT_PROCESSES_CHANNEL,
			&new_trace->channel.processes, subbuf_size_align,
			n_subbufs, overwrite);
	if(err != 0) {
		goto processes_error;
	}
	err = ltt_channel_create(trace_name, new_trace,
			new_trace->dentry.control_root, LTT_MODULES_CHANNEL,
			&new_trace->channel.modules, subbuf_size_align,
			n_subbufs, overwrite);
	if(err != 0) {
		goto modules_error;
	}
	err = ltt_channel_create(trace_name, new_trace,
			new_trace->dentry.trace_root, LTT_CPU_CHANNEL,
			&new_trace->channel.cpu, subbuf_size_align, n_subbufs,
			overwrite);
	if(err != 0) {
		goto cpu_error;
	}
	err = ltt_channel_create(trace_name, new_trace,
			new_trace->dentry.control_root, LTT_NETWORK_CHANNEL,
			&new_trace->channel.network, subbuf_size_align,
			n_subbufs, overwrite);
	if(err != 0) {
		goto network_error;
	}

	init_error_count(new_trace->channel.facilities);
	init_error_count(new_trace->channel.interrupts);
	init_error_count(new_trace->channel.processes);
	init_error_count(new_trace->channel.modules);
	init_error_count(new_trace->channel.cpu);
	init_error_count(new_trace->channel.network);

	down(&ltt_traces_sem);

	err = _ltt_trace_create(trace_name, mode, subbuf_size_align, n_subbufs,
			new_trace);

	up(&ltt_traces_sem);
	if(err != 0)
		goto lock_create_error;
	return err;

lock_create_error:
	relay_close(new_trace->channel.network->rchan);
network_error:
	relay_close(new_trace->channel.cpu->rchan);
cpu_error:
	relay_close(new_trace->channel.modules->rchan);
modules_error:
	relay_close(new_trace->channel.processes->rchan);
processes_error:
	relay_close(new_trace->channel.interrupts->rchan);
interrupts_error:
	relay_close(new_trace->channel.facilities->rchan);
facilities_error:
	relayfs_remove_dir(new_trace->dentry.control_root);
control_error:
	relayfs_remove_dir(new_trace->dentry.trace_root);
trace_error:
	kfree(new_trace);
traces_error:
	return err;
}

/* Must be called while sure that trace is in the list. */
static int _ltt_trace_destroy(struct ltt_trace_struct	*trace)
{
	int err = EPERM;

	if(trace == NULL) {
		err = ENOENT;
		goto traces_error;
	}
	if(trace->active) {
		printk(KERN_ERR
			"LTT : Can't destroy trace %s : tracer is active\n",
			trace->trace_name);
		err = EBUSY;
		goto active_error;
	}
	/* Everything went fine */
	list_del_rcu(&trace->list);
	synchronize_rcu();
	/* If no more trace in the list, we can free the unused facilities */
	if(list_empty(&ltt_traces.head))
		ltt_facility_free_unused();
	return 0;

	/* error handling */
active_error:
traces_error:
	return err;
}

/* Force a sub-buffer switch for a per-cpu buffer. This operation is
 * completely reentrant : can be called while tracing is active with
 * absolutely no lock held.
 */
static void ltt_force_switch(struct rchan_buf *buf, enum force_switch_mode mode)
{
	struct ltt_channel_struct *ltt_channel =
			(struct ltt_channel_struct*)buf->chan->client_data;
	struct ltt_channel_buf_struct *ltt_buf = &ltt_channel->buf[buf->cpu];
	u64 tsc;
	int offset_begin, offset_end, offset_old;
	int reserve_commit_diff;
	int consumed_old, consumed_new;
	int commit_count, reserve_count;
	int end_switch_old;

	do {
		offset_old = atomic_read(&ltt_buf->offset);
		offset_begin = offset_old;
		end_switch_old = 0;

		if(SUBBUF_OFFSET(offset_begin, buf->chan) != 0) {
			offset_begin = SUBBUF_ALIGN(offset_begin, buf->chan);
			end_switch_old = 1;
		} else {
			/* we do not have to switch : buffer is empty */
			return;
		}
		if(mode == FORCE_ACTIVE)
			offset_begin += ltt_channel->subbuf_header_len(buf);
		/* Always begin_switch in FORCE_ACTIVE mode */
		/* Test new buffer integrity */
		reserve_commit_diff = atomic_read(
			&ltt_buf->reserve_count[SUBBUF_INDEX(offset_begin,
							buf->chan)])
			- atomic_read(
			&ltt_buf->commit_count[SUBBUF_INDEX(offset_begin,
							buf->chan)]);
		if(reserve_commit_diff == 0) {
			/* Next buffer not corrupted. */
			if(mode == FORCE_ACTIVE && !buf->chan->overwrite &&
				(offset_begin-atomic_read(&ltt_buf->consumed))
				>= ltt_channel->rchan->alloc_size) {
	  			/* We do not overwrite non consumed buffers
				 * and we are full :
				 * ignore switch while tracing is active. */
				return;
			}
		} else {
			/* Next subbuffer corrupted. Force pushing reader even
			 * in normal mode */
		}
		offset_end = offset_begin;
		tsc = ltt_get_timestamp64();
		if(tsc == 0) {
			/* Error in getting the timestamp : should not happen :
			 * it would mean we are called from an NMI during a
			 * write seqlock on xtime. */
			return;
		}

	} while(atomic_cmpxchg(&ltt_buf->offset, offset_old, offset_end)
							!= offset_old);

	if(mode == FORCE_ACTIVE) {
		/* Push the reader if necessary */
		do {
			consumed_old = atomic_read(&ltt_buf->consumed);
			/* If buffer is in overwrite mode, push the reader
			 * consumed count if the write position has reached it
			 * and we are not at the first iteration (don't push
			 * the reader farther than the writer). This operation
			 * can be done concurrently by many writers in the same
			 * buffer, the writer being at the fartest write
			 * position sub-buffer index in the buffer being the
			 * one which will win this loop.
			 * If the buffer is not in overwrite mode, pushing the
			 * reader only happen if a sub-buffer is corrupted */
			if((SUBBUF_TRUNC(offset_end-1, buf->chan)
					- SUBBUF_TRUNC(consumed_old,
						buf->chan))
					>= ltt_channel->rchan->alloc_size)
				consumed_new =
					SUBBUF_ALIGN(consumed_old, buf->chan);
			else {
				consumed_new = consumed_old;
				break;
			}
		} while(atomic_cmpxchg(&ltt_buf->consumed, consumed_old,
					consumed_new)
				!= consumed_old);

		if(consumed_old != consumed_new) {
			/* Reader pushed : we are the winner of the push, we
			 * can therefore reequilibrate reserve and commit.
			 * Atomic increment of the commit count permits other
			 * writers to play around with this variable before us.
			 * We keep track of corrupted_subbuffers even in
			 * overwrite mode :
			 * we never want to write over a non completely
			 * committed sub-buffer : possible causes : the buffer
			 * size is too low compared to the unordered data input,
			 * or there is a writer who died between the reserve
			 * and the commit. */
			if(reserve_commit_diff) {
				/* We have to alter the sub-buffer commit
				 * count : a sub-buffer is corrupted */
				atomic_add(reserve_commit_diff,
					&ltt_buf->commit_count[SUBBUF_INDEX(
						offset_begin, buf->chan)]);
				atomic_inc(&ltt_buf->corrupted_subbuffers);
			}
		}
	}

	/* Always switch */
	if(end_switch_old) {
		/* old subbuffer */
		/* Concurrency safe because we are the last and only thread to
		 * alter this sub-buffer. As long as it is not delivered and
		 * read, no other thread can alter the offset, alter the
		 * reserve_count or call the client_buffer_end_callback on this
		 * sub-buffer. The only remaining threads could be the ones
		 * with pending commits. They will have to do the deliver
		 * themself.
		 * Not concurrency safe in overwrite mode.
		 * We detect corrupted subbuffers with commit and reserve
		 * counts. We keep a corrupted sub-buffers count and push the
		 * readers across these sub-buffers. Not concurrency safe if a
		 * writer is stalled in a subbuffer and another writer switches
		 * in, finding out it's corrupted. The result will be than the
		 * old (uncommited) subbuffer will be declared corrupted, and
		 * that the new subbuffer will be declared corrupted too because
		 * of the commit count adjustment.
		 * Offset old should never be 0. */
		ltt_channel->buffer_end(buf, tsc, offset_old,
				SUBBUF_INDEX((offset_old), buf->chan));
		/* Setting this reserve_count will allow the sub-buffer to be
		 * delivered by the last committer. */
		reserve_count = atomic_add_return((SUBBUF_OFFSET((offset_old-1),
                                                      buf->chan) + 1),
			&ltt_buf->reserve_count[SUBBUF_INDEX((offset_old),
                                                          buf->chan)]);
		if(reserve_count == atomic_read(
			&ltt_buf->commit_count[SUBBUF_INDEX(
				(offset_old), buf->chan)])) {
			buf->chan->cb->deliver(buf,
				SUBBUF_INDEX((offset_old), buf->chan), NULL);
		}
	}

	if(mode == FORCE_ACTIVE) {
		/* New sub-buffer */
		/* This code can be executed unordered : writers may already
		 * have written to the sub-buffer before this code gets
		 * executed, caution. */
		/* The commit makes sure that this code is executed before the
		 * deliver of this sub-buffer */
		ltt_channel->buffer_begin(buf, tsc,
				SUBBUF_INDEX(offset_begin, buf->chan));
		commit_count =
			atomic_add_return(ltt_channel->subbuf_header_len(buf),
			 &ltt_buf->commit_count[SUBBUF_INDEX(offset_begin,
				 buf->chan)]);
		/* Check if the written buffer has to be delivered */
		if(commit_count	== atomic_read(
			&ltt_buf->reserve_count[SUBBUF_INDEX(offset_begin,
				buf->chan)])) {
			buf->chan->cb->deliver(buf,
				SUBBUF_INDEX(offset_begin, buf->chan), NULL);
		}
	}
}


/* LTTng channel flush function.
 *
 * Must be called when no tracing is active in the channel, because of
 * accesses across CPUs. */
static void ltt_buffer_flush(struct rchan *chan)
{
	unsigned int i;
	for(i=0;i<NR_CPUS;i++) {
		if(!chan->buf[i]) continue;
		chan->buf[i]->finalized = 1;
		ltt_force_switch(chan->buf[i], FORCE_FLUSH);
	}
}

/* Wake writers :
 *
 * This must be done after the trace is removed from the RCU list so that there
 * are no stalled writers. */
static void ltt_wake_writers(struct rchan *chan)
{
	struct ltt_channel_struct *ltt_channel =
		(struct ltt_channel_struct*)chan->client_data;
	struct ltt_channel_buf_struct *ltt_buf;
	unsigned int i;

	for(i=0;i<NR_CPUS;i++) {
		if(!chan->buf[i]) continue;
		ltt_buf = &ltt_channel->buf[i];
		if (waitqueue_active(&ltt_buf->write_wait)) {
			schedule_work(&ltt_buf->wake_writers);
		}
	}
}

/* Sleepable part of the destroy */
static void __ltt_trace_destroy(struct ltt_trace_struct	*trace)
{
	int err;

	ltt_buffer_flush(trace->channel.facilities->rchan);
	ltt_buffer_flush(trace->channel.interrupts->rchan);
	ltt_buffer_flush(trace->channel.processes->rchan);
	ltt_buffer_flush(trace->channel.modules->rchan);
	ltt_buffer_flush(trace->channel.cpu->rchan);
	ltt_buffer_flush(trace->channel.network->rchan);

	ltt_wake_writers(trace->channel.facilities->rchan);
	ltt_wake_writers(trace->channel.interrupts->rchan);
	ltt_wake_writers(trace->channel.processes->rchan);
	ltt_wake_writers(trace->channel.modules->rchan);
	ltt_wake_writers(trace->channel.cpu->rchan);
	flush_scheduled_work();

	if(ltt_traces.num_active_traces == 0) {
		/* We stop the asynchronous delivery of reader wakeup, but
		 * we must make one last check for reader wakeups pending. */
		del_timer(&ltt_async_wakeup_timer);
	}
	/* The currently destroyed trace is not in the trace list anymore,
	 * so it's safe to call the async wakeup ourself. It will deliver
	 * the last subbuffers. */
	trace_async_wakeup(trace);

	relay_close(trace->channel.facilities->rchan);
	relay_close(trace->channel.interrupts->rchan);
	relay_close(trace->channel.processes->rchan);
	relay_close(trace->channel.modules->rchan);
	relay_close(trace->channel.cpu->rchan);
	relay_close(trace->channel.network->rchan);

	err = relayfs_remove_dir(trace->dentry.control_root);
	WARN_ON(err);
	err = relayfs_remove_dir(trace->dentry.trace_root);
	WARN_ON(err);

	kfree(trace);
}

static int ltt_trace_destroy(char *trace_name)
{
	int err = 0;
	struct ltt_trace_struct* trace;

	down(&ltt_traces_sem);
	trace = _ltt_trace_find(trace_name);
	err = _ltt_trace_destroy(trace);
	if(err) goto error;
	up(&ltt_traces_sem);
	__ltt_trace_destroy(trace);
	return err;

	/* Error handling */
error:
	up(&ltt_traces_sem);
	return err;
}

/* must be called from within a traces lock. */
static int _ltt_trace_start(struct ltt_trace_struct* trace)
{
	int err = 0;

	if(trace == NULL) {
		err = ENOENT;
		goto traces_error;
	}
	if(trace->active)
		printk(KERN_INFO "LTT : Tracing already active for trace %s\n",
				trace->trace_name);
	if(ltt_traces.num_active_traces == 0) {
		if(!try_module_get(ltt_run_filter_owner)) {
			err = ENODEV;
			printk(KERN_ERR "LTT : Can't lock filter module.\n");
			goto get_ltt_run_filter_error;
		}
		if(!try_module_get(ltt_statedump_owner)) {
			err = ENODEV;
			printk(KERN_ERR
				"LTT : Can't lock state dump module.\n");
			goto get_ltt_statedump_error;
		}
#ifdef CONFIG_LTT_HEARTBEAT
		if(ltt_heartbeat_trigger(LTT_HEARTBEAT_START)) {
			err = ENODEV;
			printk(KERN_ERR
				"LTT : Heartbeat timer module no present.\n");
			goto ltt_heartbeat_error;
		}
#endif //CONFIG_LTT_HEARTBEAT
		init_timer(&ltt_async_wakeup_timer);
		ltt_async_wakeup_timer.function = async_wakeup;
		ltt_async_wakeup_timer.expires = jiffies + 1;
		add_timer(&ltt_async_wakeup_timer);
	}
	trace->active = 1;
	ltt_traces.num_active_traces++;	/* Read by trace points without
					 * protection : be careful */
	return err;

	/* error handling */
#ifdef CONFIG_LTT_HEARTBEAT
ltt_heartbeat_error:
	module_put(ltt_statedump_owner);
#endif //CONFIG_LTT_HEARTBEAT
get_ltt_statedump_error:
	module_put(ltt_run_filter_owner);
get_ltt_run_filter_error:
traces_error:
	return err;
}

static int ltt_trace_start(char *trace_name)
{
	int err = 0;
	struct ltt_trace_struct* trace;

	down(&ltt_traces_sem);

	trace = _ltt_trace_find(trace_name);
	if(trace == NULL) goto no_trace;
	err = _ltt_trace_start(trace);

	up(&ltt_traces_sem);

	/* Call the kernel state dump.
	 * Events will be mixed with real kernel events, it's ok.
	 * Notice that there is no protection on the trace : that's exactly
	 * why we iterate on the list and check for trace equality instead of
	 * directly using this trace handle inside the logging function. */

	/* State dump begin...
	 * (must have send a tracing stop event if is the case...) */ //FIXME
	ltt_facility_state_dump(trace);
	ltt_statedump_functor(trace);

	/* State dump finalize... tells state is coherent */ //FIXME
	return err;

	/* Error handling */
no_trace:
	up(&ltt_traces_sem);
	return err;
}


/* must be called from within traces lock */
static int _ltt_trace_stop(struct ltt_trace_struct* trace)
{
	int err = EPERM;

	if(trace == NULL) {
		err = ENOENT;
		goto traces_error;
	}
	if(!trace->active)
		printk(KERN_INFO "LTT : Tracing not active for trace %s\n",
				trace->trace_name);
	if(trace->active) {
		trace->active = 0;
		ltt_traces.num_active_traces--;
		synchronize_rcu(); /* Wait for each tracing to be finished */
	}
	if(ltt_traces.num_active_traces == 0) {
#ifdef CONFIG_LTT_HEARTBEAT
	/* stop the heartbeat if we are the last active trace */
		ltt_heartbeat_trigger(LTT_HEARTBEAT_STOP);
#endif //CONFIG_LTT_HEARTBEAT
	/* Release the modules if we are the last active trace */
		module_put(ltt_run_filter_owner);
		module_put(ltt_statedump_owner);
	}
	/* Everything went fine */
	return 0;

	/* Error handling */
traces_error:
	return err;
}

static int ltt_trace_stop(char *trace_name)
{
	int err = 0;
	struct ltt_trace_struct* trace;

	down(&ltt_traces_sem);
	trace = _ltt_trace_find(trace_name);
	err = _ltt_trace_stop(trace);
	up(&ltt_traces_sem);
	return err;
}


/* Exported functions */

int ltt_control(enum ltt_control_msg msg, char *trace_name,
		union ltt_control_args args)
{
	int err = EPERM;

	printk(KERN_ALERT "ltt_control : trace %s\n", trace_name);
	switch(msg) {
		case LTT_CONTROL_START:
			printk(KERN_DEBUG "Start tracing %s\n", trace_name);
			err = ltt_trace_start(trace_name);
			break;
		case LTT_CONTROL_STOP:
			printk(KERN_DEBUG "Stop tracing %s\n", trace_name);
			err = ltt_trace_stop(trace_name);
			break;
		case LTT_CONTROL_CREATE_TRACE:
			printk(KERN_DEBUG "Creating trace %s\n", trace_name);
			err = ltt_trace_create(trace_name,args.new_trace.mode,
				args.new_trace.subbuf_size,
				args.new_trace.n_subbufs);
			break;
		case LTT_CONTROL_DESTROY_TRACE:
			printk(KERN_DEBUG "Destroying trace %s\n", trace_name);
			err = ltt_trace_destroy(trace_name);
			break;
	}
	return err;
}
EXPORT_SYMBOL_GPL(ltt_control);


int ltt_filter_control(enum ltt_filter_control_msg msg, char *trace_name)
{
	int err;
	struct ltt_trace_struct *trace;

	printk(KERN_DEBUG "ltt_filter_control : trace %s\n", trace_name);
	down(&ltt_traces_sem);
	trace = _ltt_trace_find(trace_name);
	if(trace == NULL) {
		printk(KERN_ALERT
			"Trace does not exist. Cannot proxy control request\n");
		err = ENOENT;
		goto trace_error;
	}
	if(!try_module_get(ltt_filter_control_owner)) {
		err = ENODEV;
		goto get_module_error;
	}
	switch(msg) {
		case LTT_FILTER_DEFAULT_ACCEPT:
			printk(KERN_DEBUG
				"Proxy filter default accept %s\n", trace_name);
			err = (*ltt_filter_control_functor)(msg, trace);
			break;
		case LTT_FILTER_DEFAULT_REJECT:
			printk(KERN_DEBUG
				"Proxy filter default reject %s\n", trace_name);
			err = (*ltt_filter_control_functor)(msg, trace);
			break;
		default:
			err = EPERM;
	}
	module_put(ltt_filter_control_owner);

get_module_error:
trace_error:
	up(&ltt_traces_sem);
	return err;
}
EXPORT_SYMBOL_GPL(ltt_filter_control);

static int __init ltt_core_init(void)
{
	extern struct file_operations relay_file_operations;  /* MUST BE FIXED!! */

	printk(KERN_INFO "LTT : ltt-core init as module\n");
	ltt_root_dentry = relayfs_create_dir(LTT_RELAYFS_ROOT, NULL);
	if(ltt_root_dentry == NULL) return -EEXIST;

	ltt_file_operations = relay_file_operations;
	ltt_file_operations.owner = THIS_MODULE;
	ltt_file_operations.poll = ltt_poll;
	ltt_file_operations.ioctl = ltt_ioctl;
#ifdef CONFIG_COMPAT
	ltt_file_operations.compat_ioctl = ltt_compat_ioctl;
#endif //CONFIG_COMPAT

	return 0;
}

static void __exit ltt_exit(void)
{
	int err;
	struct ltt_trace_struct *trace;

	printk(KERN_INFO "LTT : ltt-core exit\n");
	down(&ltt_traces_sem);
	/* Stop each trace and destroy them */
	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		_ltt_trace_stop(trace);
		_ltt_trace_destroy(trace); /* it's doing a synchronize_rcu() */
		__ltt_trace_destroy(trace);
	}
	up(&ltt_traces_sem);
	err = relayfs_remove_dir(ltt_root_dentry);
	if(err != 0)
		printk(KERN_INFO "LTT : stale ltt root relayfs entry\n");
}

module_init(ltt_core_init)
module_exit(ltt_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Next Generation Tracer");

