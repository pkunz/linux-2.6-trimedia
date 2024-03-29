/*
 * linux/include/linux/relayfs_fs.h
 *
 * Copyright (C) 2005, Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 * Copyright (C) 2002, 2003 - Tom Zanussi (zanussi@us.ibm.com), IBM Corp
 * Copyright (C) 1999, 2000, 2001, 2002 - Karim Yaghmour (karim@opersys.com)
 *
 * RelayFS definitions and declarations
 */

#ifndef _LINUX_RELAYFS_FS_H
#define _LINUX_RELAYFS_FS_H

#include <linux/config.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/kref.h>
//#include <linux/percpu.h>
//#include <linux/preempt.h>
//#include <linux/err.h>
//#include <asm/system.h> //for cmpxchg

/*
 * Tracks changes to rchan_buf struct
 */
#define RELAYFS_CHANNEL_VERSION		5

/*
 * Per-cpu relay channel buffer
 */
struct rchan_buf
{
	void *start;			/* start of channel buffer */
	void *data;			/* start of current sub-buffer */
	size_t offset;			/* current offset into sub-buffer */
	size_t subbufs_produced;	/* count of sub-buffers produced */
	size_t subbufs_consumed;	/* count of sub-buffers consumed */
	atomic_t full;		/* buffer is in full state */
	struct rchan *chan;		/* associated channel */
	wait_queue_head_t read_wait;	/* reader wait queue */
	struct work_struct wake_readers; /* reader wake-up work struct */
	struct dentry *dentry;		/* channel file dentry */
	struct kref kref;		/* channel buffer refcount */
	atomic_t open_count;			/* file open count */
	struct page **page_array;	/* array of current buffer pages */
	unsigned int page_count;	/* number of current buffer pages */
	unsigned int finalized;		/* buffer has been finalized */
	size_t *padding;		/* padding counts per sub-buffer */
	size_t prev_padding;		/* temporary variable */
	size_t bytes_consumed;		/* bytes consumed in cur read subbuf */
	unsigned int cpu;		/* this buf's cpu */
} ____cacheline_aligned;

/*
 * Relay channel data structure
 */
struct rchan
{
	u32 version;			/* the version of this struct */
	size_t subbuf_size;		/* sub-buffer size */
	size_t n_subbufs;		/* number of sub-buffers per buffer */
	size_t alloc_size;		/* total buffer size allocated */
 	int overwrite;			/* overwrite buffer when full? */
  	struct rchan_callbacks *cb;	/* client callbacks */
	struct file_operations fops; /* file operations */
 	void *client_data; /* client data associated with the client callbacks */
 	void (*free_client_data_cb)(void*); /* Callback function to free the client
 																				 data */
	struct kref kref;		/* channel refcount */
	void *private_data;		/* for user-defined data */
	struct rchan_buf *buf[NR_CPUS]; /* per-cpu channel buffers */
 	struct module *client_module;	/* Client module (for reference count) */
};

/*
 * Relayfs inode
 */
struct relayfs_inode_info
{
	struct inode vfs_inode;
	struct rchan_buf *buf;
};

static inline struct relayfs_inode_info *RELAYFS_I(struct inode *inode)
{
	return container_of(inode, struct relayfs_inode_info, vfs_inode);
}

/*
 * Relay channel client callbacks
 */
struct rchan_callbacks
{
	/*
	 * subbuf_start - called on buffer-switch to a new sub-buffer
	 * @buf: the channel buffer containing the new sub-buffer
	 * @subbuf: the start of the new sub-buffer
	 * @prev_subbuf_idx: the previous sub-buffer's index
	 * @prev_subbuf: the start of the previous sub-buffer
	 * @prev_padding: unused space at the end of previous sub-buffer
	 *
	 * The client should return 1 to continue logging, 0 to stop
	 * logging.
	 *
	 * NOTE: subbuf_start will also be invoked when the buffer is
	 *       created, so that the first sub-buffer can be initialized
	 *       if necessary.  In this case, prev_subbuf will be NULL.
	 *
	 * NOTE: the client can reserve bytes at the beginning of the new
	 *       sub-buffer by calling subbuf_start_reserve() in this callback.
	 */
	int (*subbuf_start) (struct rchan_buf *buf,
			     void *subbuf,
			     void *prev_subbuf,
			     size_t prev_padding);

	/*
	 * deliver - deliver a guaranteed full sub-buffer to client
	 * @buf: the channel buffer containing the sub-buffer
	 * @subbuf_idx: the sub-buffer's index
	 * @subbuf: the start of the new sub-buffer
	 *
	 * Only works if relay_commit is also used
	 */
	void (*deliver) (struct rchan_buf *buf,
			 unsigned subbuf_idx,
			 void *subbuf);

	/*
	 * buf_mapped - relayfs buffer mmap notification
	 * @buf: the channel buffer
	 * @filp: relayfs file pointer
	 *
	 * Called when a relayfs file is successfully mmapped
	 */
        void (*buf_mapped)(struct rchan_buf *buf,
			   struct file *filp);

	/*
	 * buf_unmapped - relayfs buffer unmap notification
	 * @buf: the channel buffer
	 * @filp: relayfs file pointer
	 *
	 * Called when a relayfs file is successfully unmapped
	 */
        void (*buf_unmapped)(struct rchan_buf *buf,
			     struct file *filp);

	/*
	 * buf_full - relayfs buffer full notification
	 * @buf: the channel channel buffer
	 * @subbuf_idx: the current sub-buffer's index
	 * @subbuf: the start of the current sub-buffer
	 *
	 * Called when a relayfs buffer becomes full
	 */
	void (*buf_full)(struct rchan_buf *buf,
			unsigned subbuf_idx,
			void *subbuf);

	/*
	 * buf_unfull - relayfs buffer unfull notification
	 * @buf: the channel channel buffer
	 * @subbuf_idx: the current sub-buffer's index
	 * @subbuf: the start of the current sub-buffer
	 *
	 * Called when a relayfs buffer passes from full to not full
	 */
	void (*buf_unfull)(struct rchan_buf *buf,
			unsigned subbuf_idx,
			void *subbuf);

};

/*
 * relayfs kernel API, fs/relayfs/relay.c
 */

struct rchan *relay_open(const char *base_filename,
			 struct dentry *parent,
			 size_t subbuf_size,
			 size_t n_subbufs,
			 int overwrite,
			 struct module *client_module,
			 struct rchan_callbacks *cb,
			 struct file_operations *fops,
			 void *client_data,
			 void (*free_client_data_cb)(void*));
extern void relay_close(struct rchan *chan);
extern void relay_flush(struct rchan *chan);
extern void relay_subbufs_consumed(struct rchan *chan,
				   unsigned int cpu,
				   size_t consumed);
extern void relay_reset(struct rchan *chan);
extern int relay_buf_full(struct rchan_buf *buf);

extern size_t relay_switch_subbuf(struct rchan_buf *buf,
				  size_t length);
extern struct dentry *relayfs_create_dir(const char *name,
					 struct dentry *parent);
extern int relayfs_remove_dir(struct dentry *dentry);

#if 0  /* added in ltt 2.6.17 patch */
extern struct dentry *relayfs_create_file(const char *name,
					  struct dentry *parent,
					  int mode,
					  struct file_operations *fops,
					  void *data);
extern int relayfs_remove_file(struct dentry *dentry);
#endif

/**
 *	relay_write - write data into the channel
 *	@chan: relay channel
 *	@data: data to be written
 *	@length: number of bytes to write
 *
 *	Writes data into the current cpu's channel buffer.
 *
 *	Protects the buffer by disabling interrupts.  Use this
 *	if you might be logging from interrupt context.  Try
 *	__relay_write() if you know you	won't be logging from
 *	interrupt context.
 */
static inline void relay_write(struct rchan *chan,
			       const void *data,
			       size_t length)
{
	unsigned long flags;
	struct rchan_buf *buf;

	local_irq_save(flags);
	buf = chan->buf[smp_processor_id()];
	if (unlikely(buf->offset + length > chan->subbuf_size))
		length = relay_switch_subbuf(buf, length);
	memcpy(buf->data + buf->offset, data, length);
	buf->offset += length;
	local_irq_restore(flags);
}

/**
 *	__relay_write - write data into the channel
 *	@chan: relay channel
 *	@data: data to be written
 *	@length: number of bytes to write
 *
 *	Writes data into the current cpu's channel buffer.
 *
 *	Protects the buffer by disabling preemption.  Use
 *	relay_write() if you might be logging from interrupt
 *	context.
 */
static inline void __relay_write(struct rchan *chan,
				 const void *data,
				 size_t length)
{
	struct rchan_buf *buf;

	buf = chan->buf[get_cpu()];
	if (unlikely(buf->offset + length > buf->chan->subbuf_size))
		length = relay_switch_subbuf(buf, length);
	memcpy(buf->data + buf->offset, data, length);
	buf->offset += length;
	put_cpu();
}

/**
 *	relay_reserve - reserve slot in channel buffer
 *	@chan: relay channel
 *	@length: number of bytes to reserve
 *
 *	Returns pointer to reserved slot, NULL if full.
 *
 *	Reserves a slot in the current cpu's channel buffer.
 *	Does not protect the buffer at all - caller must provide
 *	appropriate synchronization.
 */
static inline void *relay_reserve(struct rchan *chan, size_t length)
{
	void *reserved;
	struct rchan_buf *buf = chan->buf[smp_processor_id()];

	if (unlikely(buf->offset + length > buf->chan->subbuf_size)) {
		length = relay_switch_subbuf(buf, length);
		if (!length)
			return NULL;
	}

	reserved = buf->data + buf->offset;
	buf->offset += length;

	return reserved;
}

/**
 *	subbuf_start_reserve - reserve bytes at the start of a sub-buffer
 *	@buf: relay channel buffer
 *	@length: number of bytes to reserve
 *
 *	Helper function used to reserve bytes at the beginning of
 *	a sub-buffer in the subbuf_start() callback.
 */
static inline void subbuf_start_reserve(struct rchan_buf *buf,
					size_t length)
{
	BUG_ON(length >= buf->chan->subbuf_size - 1);
	buf->offset = length;
}

#endif /* _LINUX_RELAYFS_FS_H */

