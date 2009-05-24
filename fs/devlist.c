/*
 * A simple device node space allocator for the Linux kernel
 *
 * This uses a simple linked list of dev_t ranges for reserving device
 * areas. The io resource tree scheme would almost work here except we
 * have backwards compatibility requirements for dynamically allocated
 * ranges from the top of the legacy device space downwards.
 *
 * Feb 27 2005  Matt Mackall <mpm@selenic.com>
 */

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/errno.h>

struct device_name {
	struct list_head list;
	dev_t begin, end;
	char name[16];
};

/*
 * register_dev - reserve a range of devices for a given device name
 * @a: first allowed base address
 * @b: last allowed base address
 * @size: size of minor devices device range to reserve
 * @name: name to associate with the range
 * @list: device name list to add to
 * @r: allocated base address
 *
 * This function reserves a range of %size minor devices starting
 * somewhere between a and b of the given size. To allocate at a fixed
 * base address, use a = b. For dynamic allocation in a range,
 * allocation occurs from the top down for backward compatibility.
 * There are no restrictions on the size of the range allocated, so
 * multiple contiguous majors can be allocated with a single call.
 *
 * This function returns 0 on success, -EBUSY if the requested range
 * could not be allocated, and -ENOMEM if the device name node could
 * not be allocated.
 */

int register_dev(dev_t a, dev_t b, int size, const char * name,
		 struct list_head *list, dev_t *r)
{
	struct device_name *d;
	struct list_head *l;

	/* search for insertion point in reverse for dynamic allocation */
	list_for_each_prev(l, list) {
		d = list_entry(l, struct device_name, list);
		/* have we found an insertion point? */
		if (b >= d->end)
			break;
		/* does the current entry force us to shrink the range? */
		if (b + size > d->begin)
			b = d->begin - size;
		/* have we searched outside of or collapsed the range? */
		if (a > d->begin || a > b)
			return -EBUSY;
	}

	d = kmalloc(sizeof(struct device_name), GFP_KERNEL);
	if (!d)
		return -ENOMEM;

	strlcpy(d->name, name, sizeof(d->name));
	d->begin = b;
	d->end = b + size;
	list_add(&d->list, l);

	if (r)
		*r = b;

	return 0;
}

/*
 * unregister_dev - drop a device reservation range
 * @begin: beginning of allocated range
 * @l: device name list
 *
 * This function removes the range starting with begin from the given
 * list and returns 0, or returns -EINVAL if the range is not found.
 */

int unregister_dev(dev_t begin, struct list_head *l)
{
	struct device_name *n;

	list_for_each_entry(n, l, list)
		if (n->begin == begin) {
			list_del(&n->list);
			kfree(n);
			return 0;
		}

	return -EINVAL;
}

#ifdef CONFIG_PROC_FS
int get_dev_list(char *p, struct list_head *l)
{
	struct device_name *n;
	int len = 0, m;

	list_for_each_entry(n, l, list)
		for (m = MAJOR(n->begin); m <= MAJOR(n->end - 1); m++)
			len += sprintf(p + len, "%3d %s\n", m, n->name);

	return len;
}
#endif
