/*
 *  linux/fs/char_dev.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/string.h>

#include <linux/major.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/smp_lock.h>
#include <linux/devfs_fs_kernel.h>

#include <linux/kobject.h>
#include <linux/kobj_map.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/devlist.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

static struct kobj_map *cdev_map;

#define MAX_PROBE_HASH 255	/* random */

static DECLARE_MUTEX(chrdevs_lock);
static LIST_HEAD(chrdev_names);

#ifdef CONFIG_PROC_FS
int get_chrdev_list(char *page)
{
	int len;

	len = sprintf(page, "Character devices:\n");

	down(&chrdevs_lock);
	len += get_dev_list(page + len, &chrdev_names);
	up(&chrdevs_lock);

	return len;
}
#endif

int register_chrdev_region(dev_t from, unsigned count, const char *name)
{
	int ret;

	down(&chrdevs_lock);
	ret = register_dev(from, from, count, name, &chrdev_names, NULL);
	up(&chrdevs_lock);

	return ret;
}

int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,
			const char *name)
{
	int ret;

	down(&chrdevs_lock);
	ret = register_dev(MKDEV(1, 0), MKDEV(254, 0),
			    MKDEV(1, 0), name, &chrdev_names, dev);
	up(&chrdevs_lock);

	return ret;
}

int register_chrdev(unsigned int major, const char *name,
		    struct file_operations *fops)
{
	dev_t r;
	int ret = 0, err;
	struct cdev *cdev;
	char *s;

	down(&chrdevs_lock);
	if (!major) {
		err = register_dev(MKDEV(1, 0), MKDEV(254, 0),
				   MKDEV(1, 0), name, &chrdev_names, &r);
		ret = MAJOR(r);
	} else
		err = register_dev(MKDEV(major, 0), MKDEV(major, 0),
				   MKDEV(1, 0), name, &chrdev_names, &r);
	up(&chrdevs_lock);



	if (err)
		return err;

	cdev = cdev_alloc();
	if (!cdev)
		goto unreg;

	cdev->owner = fops->owner;
	cdev->ops = fops;
	kobject_set_name(&cdev->kobj, "%s", name);
	for (s = strchr(kobject_name(&cdev->kobj),'/'); s; s = strchr(s, '/'))
		*s = '!';

	err = cdev_add(cdev, r, 256);
	if (!err)
		return ret;

	kobject_put(&cdev->kobj);
unreg:
	unregister_dev(r, &chrdev_names);
	return err;
}

void unregister_chrdev_region(dev_t from, unsigned count)
{
	down(&chrdevs_lock);
	unregister_dev(from, &chrdev_names);
	up(&chrdevs_lock);
}

int unregister_chrdev(unsigned int major, const char *name)
{
	struct kobject *kobj;
	int idx;

	down(&chrdevs_lock);
	unregister_dev(MKDEV(major, 0), &chrdev_names);
	up(&chrdevs_lock);

	kobj = kobj_lookup(cdev_map, MKDEV(major, 0), &idx);
	if (kobj)
		cdev_del(container_of(kobj, struct cdev, kobj));

	return 0;
}

static DEFINE_SPINLOCK(cdev_lock);

static struct kobject *cdev_get(struct cdev *p)
{
	struct module *owner = p->owner;
	struct kobject *kobj;

	if (owner && !try_module_get(owner))
		return NULL;
	kobj = kobject_get(&p->kobj);
	if (!kobj)
		module_put(owner);
	return kobj;
}

void cdev_put(struct cdev *p)
{
	if (p) {
		struct module *owner = p->owner;
		kobject_put(&p->kobj);
		module_put(owner);
	}
}

/*
 * Called every time a character special file is opened
 */
int chrdev_open(struct inode * inode, struct file * filp)
{
	struct cdev *p;
	struct cdev *new = NULL;
	int ret = 0;

	spin_lock(&cdev_lock);
	p = inode->i_cdev;
	if (!p) {
		struct kobject *kobj;
		int idx;
		spin_unlock(&cdev_lock);
		kobj = kobj_lookup(cdev_map, inode->i_rdev, &idx);
		if (!kobj)
			return -ENXIO;
		new = container_of(kobj, struct cdev, kobj);
		spin_lock(&cdev_lock);
		p = inode->i_cdev;
		if (!p) {
			inode->i_cdev = p = new;
			inode->i_cindex = idx;
			list_add(&inode->i_devices, &p->list);
			new = NULL;
		} else if (!cdev_get(p))
			ret = -ENXIO;
	} else if (!cdev_get(p))
		ret = -ENXIO;
	spin_unlock(&cdev_lock);
	cdev_put(new);
	if (ret)
		return ret;
	filp->f_op = fops_get(p->ops);
	if (!filp->f_op) {
		cdev_put(p);
		return -ENXIO;
	}
	if (filp->f_op->open) {
		lock_kernel();
		ret = filp->f_op->open(inode,filp);
		unlock_kernel();
	}
	if (ret)
		cdev_put(p);
	return ret;
}

void cd_forget(struct inode *inode)
{
	spin_lock(&cdev_lock);
	list_del_init(&inode->i_devices);
	inode->i_cdev = NULL;
	spin_unlock(&cdev_lock);
}

static void cdev_purge(struct cdev *cdev)
{
	spin_lock(&cdev_lock);
	while (!list_empty(&cdev->list)) {
		struct inode *inode;
		inode = container_of(cdev->list.next, struct inode, i_devices);
		list_del_init(&inode->i_devices);
		inode->i_cdev = NULL;
	}
	spin_unlock(&cdev_lock);
}

/*
 * Dummy default file-operations: the only thing this does
 * is contain the open that then fills in the correct operations
 * depending on the special file...
 */
struct file_operations def_chr_fops = {
	.open = chrdev_open,
};

static struct kobject *exact_match(dev_t dev, int *part, void *data)
{
	struct cdev *p = data;
	return &p->kobj;
}

static int exact_lock(dev_t dev, void *data)
{
	struct cdev *p = data;
	return cdev_get(p) ? 0 : -1;
}

int cdev_add(struct cdev *p, dev_t dev, unsigned count)
{
	p->dev = dev;
	p->count = count;
	return kobj_map(cdev_map, dev, count, NULL, exact_match, exact_lock, p);
}

static void cdev_unmap(dev_t dev, unsigned count)
{
	kobj_unmap(cdev_map, dev, count);
}

void cdev_del(struct cdev *p)
{
	cdev_unmap(p->dev, p->count);
	kobject_put(&p->kobj);
}


static void cdev_default_release(struct kobject *kobj)
{
	struct cdev *p = container_of(kobj, struct cdev, kobj);
	cdev_purge(p);
}

static void cdev_dynamic_release(struct kobject *kobj)
{
	struct cdev *p = container_of(kobj, struct cdev, kobj);
	cdev_purge(p);
	kfree(p);
}

static struct kobj_type ktype_cdev_default = {
	.release	= cdev_default_release,
};

static struct kobj_type ktype_cdev_dynamic = {
	.release	= cdev_dynamic_release,
};

struct cdev *cdev_alloc(void)
{
	struct cdev *p = kmalloc(sizeof(struct cdev), GFP_KERNEL);
	if (p) {
		memset(p, 0, sizeof(struct cdev));
		p->kobj.ktype = &ktype_cdev_dynamic;
		INIT_LIST_HEAD(&p->list);
		kobject_init(&p->kobj);
	}
	return p;
}

void cdev_init(struct cdev *cdev, struct file_operations *fops)
{
	memset(cdev, 0, sizeof *cdev);
	INIT_LIST_HEAD(&cdev->list);
	cdev->kobj.ktype = &ktype_cdev_default;
	kobject_init(&cdev->kobj);
	cdev->ops = fops;
}

static struct kobject *base_probe(dev_t dev, int *part, void *data)
{
	if (request_module("char-major-%d-%d", MAJOR(dev), MINOR(dev)) > 0)
		/* Make old-style 2.4 aliases work */
		request_module("char-major-%d", MAJOR(dev));
	return NULL;
}

void __init chrdev_init(void)
{
	cdev_map = kobj_map_init(base_probe, &chrdevs_lock);
}


/* Let modules do char dev stuff */
EXPORT_SYMBOL(register_chrdev_region);
EXPORT_SYMBOL(unregister_chrdev_region);
EXPORT_SYMBOL(alloc_chrdev_region);
EXPORT_SYMBOL(cdev_init);
EXPORT_SYMBOL(cdev_alloc);
EXPORT_SYMBOL(cdev_del);
EXPORT_SYMBOL(cdev_add);
EXPORT_SYMBOL(register_chrdev);
EXPORT_SYMBOL(unregister_chrdev);
