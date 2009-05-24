#ifndef _LINUX_DEVLIST_H
#define _LINUX_DEVLIST_H

int register_dev(dev_t a, dev_t b, int size, const char * name,
		 struct list_head *list, dev_t *r);

int unregister_dev(dev_t begin, struct list_head *l);

int get_dev_list(char *p, struct list_head *l);

#endif
