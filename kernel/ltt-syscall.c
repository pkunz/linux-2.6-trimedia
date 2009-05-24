/******************************************************************************
 * ltt-syscall.c
 *
 * Mathieu Desnoyers (mathieu.desnoyers@polymtl.ca)
 * March 2006
 *
 * LTT userspace tracing syscalls
 */

#include <linux/errno.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/ltt-core.h>
#include <linux/ltt-facilities.h>

#include <asm/uaccess.h>

/* User event logging function */
static inline int trace_user_event(unsigned int facility_id,
		unsigned int event_id,
		void __user *data, size_t data_size, int blocking)
{
	int ret = 0;
	unsigned int index;
	struct ltt_channel_struct *channel;
	struct ltt_trace_struct *trace;
	struct ltt_channel_buf_struct *ltt_buf;
	struct rchan_buf *relayfs_buf;
	void *buffer = NULL;
	size_t real_to_base = 0; /* buffer allocated on arch_size alignment */
	size_t *to_base = &real_to_base;
	size_t real_to = 0;
	size_t *to = &real_to;
	size_t real_len = 0;
	size_t *len = &real_len;
	size_t reserve_size;
	size_t slot_size;
	u64 tsc;
	size_t before_hdr_pad, after_hdr_pad, header_size;
	DECLARE_WAITQUEUE(wait, current);
	unsigned long dbg_avail_size = 0;
	unsigned long dbg_write = 0;
	unsigned long dbg_read = 0;

	if(ltt_traces.num_active_traces == 0) return 0;

	/* Assume that the padding for alignment starts at a
	 * sizeof(void *) address. */

	reserve_size = data_size;
	index = ltt_get_index_from_facility(facility_id, event_id);

	preempt_disable();

	if(blocking) {
		/* User space requested blocking mode :
		 * If one of the active traces has free space below a specific
		 * threshold value, we reenable preemption and block. */
block_test_begin:
		list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
			if(!trace->active) continue;

			channel = ltt_get_channel_from_index(trace, index);
			relayfs_buf = channel->rchan->buf[smp_processor_id()];
			ltt_buf = &channel->buf[smp_processor_id()];
			/* Check if data is too big for the channel : do not
			 * block for it */
			if(LTT_RESERVE_CRITICAL + data_size
					> relayfs_buf->chan->subbuf_size)
				continue;

			/* If free space too low, we block. We restart from the
			 * beginning after we resume (cpu id may have changed
			 * while preemption is active).
			 */
			spin_lock(&ltt_buf->full_lock);
			if(!relayfs_buf->chan->overwrite &&
				(dbg_avail_size = (dbg_write=atomic_read(
					&channel->buf[relayfs_buf->cpu].offset))
				+ LTT_RESERVE_CRITICAL + data_size
				 - SUBBUF_TRUNC((dbg_read = atomic_read(
				&channel->buf[relayfs_buf->cpu].consumed)),
					 		relayfs_buf->chan))
					>= channel->rchan->alloc_size) {
				__set_current_state(TASK_INTERRUPTIBLE);
				add_wait_queue(&ltt_buf->write_wait, &wait);
				spin_unlock(&ltt_buf->full_lock);
				preempt_enable();
				schedule();
				__set_current_state(TASK_RUNNING);
				remove_wait_queue(&ltt_buf->write_wait, &wait);
				if (signal_pending(current))
					return -ERESTARTSYS;
				preempt_disable();
				goto block_test_begin;
			}
			spin_unlock(&ltt_buf->full_lock);
		}
	}
	ltt_nesting[smp_processor_id()]++;
	list_for_each_entry_rcu(trace, &ltt_traces.head, list) {
		if(!trace->active) continue;
		channel = ltt_get_channel_from_index(trace, index);
		relayfs_buf = channel->rchan->buf[smp_processor_id()];
		slot_size = 0;
		buffer = ltt_reserve_slot(trace, relayfs_buf,
			reserve_size, &slot_size, &tsc,
			&before_hdr_pad, &after_hdr_pad, &header_size);
		if(!buffer) {
			if(blocking) {
				printk(KERN_ERR "Error in LTT usertrace : "
				"buffer full : event lost in blocking "
				"mode. Increase LTT_RESERVE_CRITICAL.\n");
				printk(KERN_ERR "LTT nesting level is %u.\n",
					ltt_nesting[smp_processor_id()]);
				printk(KERN_ERR "LTT avail size %lu.\n",
					dbg_avail_size);
				printk(KERN_ERR "avai write : %lu, read : %lu\n",
						dbg_write, dbg_read);
				printk(KERN_ERR "LTT cur size %lu.\n",
					(dbg_write = atomic_read(
                                        &channel->buf[relayfs_buf->cpu].offset))
                                + LTT_RESERVE_CRITICAL + data_size
                                 - SUBBUF_TRUNC((dbg_read = atomic_read(
                                &channel->buf[relayfs_buf->cpu].consumed)),
                                                        relayfs_buf->chan));
				printk(KERN_ERR "cur write : %lu, read : %lu\n",
						dbg_write, dbg_read);
			}
			continue; /* buffer full */
		}
		*to_base = *to = *len = 0;
		ltt_write_event_header(trace, channel, buffer,
			facility_id, event_id,
			reserve_size, before_hdr_pad, tsc);
		*to_base += before_hdr_pad + after_hdr_pad + header_size;
		/* Hope the user pages are not swapped out. In the rare case
		 * where it is, the slot will be zeroed and EFAULT returned. */
		if(__copy_from_user_inatomic(buffer+*to_base+*to, data,
					data_size)) {
			/* Data is garbage in the slot */
			ret = -EFAULT;
		}
		ltt_commit_slot(relayfs_buf, buffer, slot_size);
		if(ret != 0) break;
	}
	ltt_nesting[smp_processor_id()]--;
	preempt_enable_no_resched();
	return ret;
}

asmlinkage long sys_ltt_trace_generic(unsigned int facility_id,
		unsigned int event_id,
		void __user *data,
		size_t data_size,
		int blocking)
{
	if(!ltt_facility_user_access_ok(facility_id)) return -EPERM;
	if(!access_ok(VERIFY_READ, data, data_size))
			return -EFAULT;

	return trace_user_event(facility_id, event_id, data, data_size,
			blocking);
}

asmlinkage long sys_ltt_register_generic(unsigned int __user *facility_id,
		const struct user_facility_info __user *info)
{
	struct user_facility_info kinfo;
	int fac_id;
	unsigned int i;

	/* Check if the process has already registered the maximum number of
	 * allowed facilities */
	if(current->ltt_facilities[LTT_FAC_PER_PROCESS-1] != 0)
		return -EPERM;

	if(copy_from_user(&kinfo, info, sizeof(*info))) return -EFAULT;

	/* Verify if facility is already registered */
	printk(KERN_DEBUG "LTT register generic for %s\n", kinfo.name);
	fac_id = ltt_facility_verify(LTT_FACILITY_TYPE_USER,
				kinfo.name,
				kinfo.num_events,
				kinfo.checksum,
				kinfo.int_size,
				kinfo.long_size,
				kinfo.pointer_size,
				kinfo.size_t_size,
				kinfo.alignment);

	printk(KERN_DEBUG "LTT verify return %d\n", fac_id);
	if(fac_id > 0) goto found;

	fac_id = ltt_facility_register(LTT_FACILITY_TYPE_USER,
				kinfo.name,
				kinfo.num_events,
				kinfo.checksum,
				kinfo.int_size,
				kinfo.long_size,
				kinfo.pointer_size,
				kinfo.size_t_size,
				kinfo.alignment);

	printk(KERN_DEBUG "LTT register return %d\n", fac_id);
	if(fac_id == 0)	return -EPERM;
	if(fac_id < 0) return fac_id;	/* Error */
found:
	spin_lock(&current->group_leader->proc_lock);
	for(i=0; i<LTT_FAC_PER_PROCESS; i++) {
		if(current->group_leader->ltt_facilities[i] == 0) {
			current->group_leader->ltt_facilities[i] =
				(ltt_facility_t)fac_id;
			break;
		}
	}
	spin_unlock(&current->group_leader->proc_lock);
	/* Write facility_id */
	put_user((unsigned int)fac_id, facility_id);
	return 0;
}
