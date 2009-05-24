/*
 *	Linux INET6 implementation
 *	FIB front-end.
 *
 *	Authors:
 *	Pedro Roque		<roque@di.fc.ul.pt>	
 *
 *	$Id: route.c,v 1.56 2001/10/31 21:55:55 davem Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

/*	Changes:
 *
 *	YOSHIFUJI Hideaki @USAGI
 *		reworked default router selection.
 *		- respect outgoing interface
 *		- select from (probably) reachable routers (i.e.
 *		routers in REACHABLE, STALE, DELAY or PROBE states).
 *		- always select the same router if it is (probably)
 *		reachable.  otherwise, round-robin the list.
 * 	Ville Nuorvala
 *		Fixed routing subtrees.
 *		Moved source address selection to routing code.
 *		Implemented policy based routing.
 */

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/times.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <linux/route.h>
#include <linux/netdevice.h>
#include <linux/in6.h>
#include <linux/init.h>
#include <linux/netlink.h>
#include <linux/if_arp.h>

#ifdef 	CONFIG_PROC_FS
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#endif

#include <net/snmp.h>
#include <net/ipv6.h>
#include <net/ip6_fib.h>
#include <net/ip6_route.h>
#include <net/ndisc.h>
#include <net/addrconf.h>
#include <net/tcp.h>
#include <linux/rtnetlink.h>
#include <net/dst.h>
#include <net/xfrm.h>

#include <asm/uaccess.h>

#ifdef CONFIG_SYSCTL
#include <linux/sysctl.h>
#endif

/* Set to 3 to get tracing. */
#define RT6_DEBUG 2

#if RT6_DEBUG >= 3
#define RDBG(x) printk x
#define RT6_TRACE(x...) printk(KERN_DEBUG x)
#else
#define RDBG(x)
#define RT6_TRACE(x...) do { ; } while (0)
#endif


static int ip6_rt_max_size = 4096;
static int ip6_rt_gc_min_interval = HZ / 2;
static int ip6_rt_gc_timeout = 60*HZ;
int ip6_rt_gc_interval = 30*HZ;
static int ip6_rt_gc_elasticity = 9;
static int ip6_rt_mtu_expires = 10*60*HZ;
static int ip6_rt_min_advmss = IPV6_MIN_MTU - 20 - 40;

static struct rt6_info * ip6_rt_copy(struct rt6_info *ort);
static struct dst_entry	*ip6_dst_check(struct dst_entry *dst, u32 cookie);
static struct dst_entry *ip6_negative_advice(struct dst_entry *);
static void		ip6_dst_destroy(struct dst_entry *);
static void		ip6_dst_ifdown(struct dst_entry *,
				       struct net_device *dev, int how);
static int		 ip6_dst_gc(void);

static int		ip6_pkt_discard(struct sk_buff *skb);
static int		ip6_pkt_discard_out(struct sk_buff *skb);
static void		ip6_link_failure(struct sk_buff *skb);
static void		ip6_rt_update_pmtu(struct dst_entry *dst, u32 mtu);

static struct dst_ops ip6_dst_ops = {
	.family			=	AF_INET6,
	.protocol		=	__constant_htons(ETH_P_IPV6),
	.gc			=	ip6_dst_gc,
	.gc_thresh		=	1024,
	.check			=	ip6_dst_check,
	.destroy		=	ip6_dst_destroy,
	.ifdown			=	ip6_dst_ifdown,
	.negative_advice	=	ip6_negative_advice,
	.link_failure		=	ip6_link_failure,
	.update_pmtu		=	ip6_rt_update_pmtu,
	.entry_size		=	sizeof(struct rt6_info),
};

struct rt6_info ip6_null_entry = {
	.u = {
		.dst = {
			.__refcnt	= ATOMIC_INIT(1),
			.__use		= 1,
			.dev		= &loopback_dev,
			.obsolete	= -1,
			.error		= -ENETUNREACH,
			.metrics	= { [RTAX_HOPLIMIT - 1] = 255, },
			.input		= ip6_pkt_discard,
			.output		= ip6_pkt_discard_out,
			.ops		= &ip6_dst_ops,
			.path		= (struct dst_entry*)&ip6_null_entry,
		}
	},
	.rt6i_flags	= (RTF_REJECT | RTF_NONEXTHOP),
	.rt6i_metric	= ~(u32) 0,
	.rt6i_ref	= ATOMIC_INIT(1),
};

#ifdef CONFIG_IPV6_MULTIPLE_TABLES

struct rt6_info ip6_prohibit_entry = {
	.u = {
		.dst = {
			.__refcnt	= ATOMIC_INIT(1),
			.__use		= 1,
			.dev		= &loopback_dev,
			.obsolete	= -1,
			.error		= -EACCES,
			.metrics	= { [RTAX_HOPLIMIT - 1] = 255, },
			.input		= ip6_pkt_discard,
			.output		= ip6_pkt_discard_out,
			.ops		= &ip6_dst_ops,
			.path		= (struct dst_entry*)&ip6_prohibit_entry,
		}
	},
	.rt6i_flags	= (RTF_REJECT | RTF_NONEXTHOP),
	.rt6i_metric	= ~(u32) 0,
	.rt6i_ref	= ATOMIC_INIT(1),
};

struct rt6_info ip6_blk_hole_entry = {
	.u = {
		.dst = {
			.__refcnt	= ATOMIC_INIT(1),
			.__use		= 1,
			.dev		= &loopback_dev,
			.obsolete	= -1,
			.error		= -EINVAL,
			.metrics	= { [RTAX_HOPLIMIT - 1] = 255, },
			.input		= ip6_pkt_discard,
			.output		= ip6_pkt_discard_out,
			.ops		= &ip6_dst_ops,
			.path		= (struct dst_entry*)&ip6_blk_hole_entry,
		}
	},
	.rt6i_flags	= (RTF_REJECT | RTF_NONEXTHOP),
	.rt6i_metric	= ~(u32) 0,
	.rt6i_ref	= ATOMIC_INIT(1),
};

#endif

struct rt6_table ip6_routing_table = {
	.table_id	= RT6_TABLE_MAIN,
	.refcnt		= ATOMIC_INIT(1),
	.lock		= RW_LOCK_UNLOCKED,
	.root		= { .leaf	= &ip6_null_entry,
			    .fn_flags 	= RTN_ROOT | RTN_TL_ROOT | RTN_RTINFO,
	},
	.dflt_lock	= SPIN_LOCK_UNLOCKED,
};

#ifdef CONFIG_IPV6_MULTIPLE_TABLES

struct rt6_table ip6_local_table = {
	.table_id	= RT6_TABLE_LOCAL,
	.refcnt		= ATOMIC_INIT(1),
	.lock		= RW_LOCK_UNLOCKED,
	.root		= { .leaf	= &ip6_null_entry,
			    .fn_flags 	= RTN_ROOT | RTN_TL_ROOT | RTN_RTINFO,
	},
	.dflt_lock	= SPIN_LOCK_UNLOCKED,
};

struct rt6_table *rt6_tables[RT6_TABLE_MAX + 1];

DEFINE_RWLOCK(rt6_table_lock);

struct rt6_table *rt6_get_table(unsigned char id)
{
	struct rt6_table *table;
	if (id == RT6_TABLE_UNSPEC)
		id = RT6_TABLE_MAIN;

	read_lock_bh(&rt6_table_lock);
	if ((table = rt6_tables[id]) != NULL) {
		atomic_inc(&table->refcnt);
	}
	read_unlock_bh(&rt6_table_lock);
	return table;
}

void rt6_put_table(struct rt6_table *table)
{
	if (atomic_dec_and_test(&table->refcnt)) {
		if (table->dead)
			kfree(table);
		else
			printk("Freeing alive routing table %p\n", table);
	}
}

static void rt6_tables_init(void)
{
	write_lock_bh(&rt6_table_lock);
	rt6_tables[RT6_TABLE_MAIN] = rt6_hold_table(&ip6_routing_table);
	rt6_tables[RT6_TABLE_LOCAL] = rt6_hold_table(&ip6_local_table);
	write_unlock_bh(&rt6_table_lock);
}

static void rt6_tables_cleanup(void)
{
	int i;
	write_lock_bh(&rt6_table_lock);
	for (i = RT6_TABLE_MIN; i <= RT6_TABLE_MAX; i++) {
		struct rt6_table *table = rt6_tables[i];
		if (table != NULL) {
			rt6_tables[i] = NULL;
			table->dead = 1;
			rt6_put_table(table);
		}
	}
	write_unlock_bh(&rt6_table_lock);
}

static struct rt6_table *rt6_tree_init(unsigned char id)
{
	struct rt6_table *table;

	if ((table = kmalloc(sizeof(struct rt6_table), GFP_ATOMIC))) {
		memset(table, 0, sizeof(struct rt6_table));
		table->table_id = id;
		atomic_set(&table->refcnt, 1);
		table->lock = RW_LOCK_UNLOCKED;
		table->root.leaf = &ip6_null_entry;
		table->root.fn_flags = RTN_ROOT | RTN_TL_ROOT | RTN_RTINFO;
		table->dflt_lock = SPIN_LOCK_UNLOCKED;
	}
	return table;
}

struct rt6_table *__rt6_new_table(unsigned char id)
{
	struct rt6_table *table = rt6_tree_init(id);
	rt6_tables[id] = table;
	return table;
}

struct rt6_table *rt6_new_table(unsigned char id)
{
	struct rt6_table *table;

	if (id == RT6_TABLE_UNSPEC)
		id = RT6_TABLE_MAIN;

	write_lock_bh(&rt6_table_lock);
	if ((table = rt6_tables[id]) != NULL ||
	    (table = __rt6_new_table(id)) != NULL)
		atomic_inc(&table->refcnt);
	write_unlock_bh(&rt6_table_lock);
	return table;
}
  
#endif

DEFINE_SPINLOCK(ndisc_lock);

/* allocate dst with ip6_dst_ops */
static __inline__ struct rt6_info *ip6_dst_alloc(void)
{
	return (struct rt6_info *)dst_alloc(&ip6_dst_ops);
}

static void ip6_dst_destroy(struct dst_entry *dst)
{
	struct rt6_info *rt = (struct rt6_info *)dst;
	struct inet6_dev *idev = rt->rt6i_idev;
	struct rt6_table *table = rt->rt6i_table;

	if (idev != NULL) {
		rt->rt6i_idev = NULL;
		in6_dev_put(idev);
	}
	if (table != NULL) {
		rt->rt6i_table = NULL;
		rt6_put_table(table);
	}
}

static void ip6_dst_ifdown(struct dst_entry *dst, struct net_device *dev,
			   int how)
{
	struct rt6_info *rt = (struct rt6_info *)dst;
	struct inet6_dev *idev = rt->rt6i_idev;

	if (dev != &loopback_dev && idev != NULL && idev->dev == dev) {
		struct inet6_dev *loopback_idev = in6_dev_get(&loopback_dev);
		if (loopback_idev != NULL) {
			rt->rt6i_idev = loopback_idev;
			in6_dev_put(idev);
		}
	}
}

static __inline__ int rt6_check_expired(const struct rt6_info *rt)
{
	return (rt->rt6i_flags & RTF_EXPIRES &&
		time_after(jiffies, rt->rt6i_expires));
}

/*
 *	Route lookup. Any table->lock is implied.
 */

static __inline__ struct rt6_info *rt6_device_match(struct rt6_info *rt,
						    int oif,
						    int strict)
{
	struct rt6_info *local = NULL;
	struct rt6_info *sprt;

	if (oif) {
		for (sprt = rt; sprt; sprt = sprt->u.next) {
			struct net_device *dev = sprt->rt6i_dev;
			if (dev->ifindex == oif)
				return sprt;
			if (dev->flags & IFF_LOOPBACK) {
				if (sprt->rt6i_idev == NULL ||
				    sprt->rt6i_idev->dev->ifindex != oif) {
					if (strict && oif)
						continue;
					if (local && (!oif ||
						      local->rt6i_idev->dev->ifindex == oif))
						continue;
				}
				local = sprt;
			}
		}

		if (local)
			return local;

		if (strict)
			return &ip6_null_entry;
	}
	return rt;
}

/*
 *	pointer to the last default router chosen. BH is disabled locally.
 */
#if !defined(CONFIG_IPV6_NEW_ROUNDROBIN)
/* struct rt6_info *rt6_dflt_pointer;
 * DEFINE_SPINLOCK(rt6_dflt_lock);
 */
void rt6_reset_dflt_pointer(struct rt6_info *rt)
{
	struct rt6_table *table = rt->rt6i_table;
	spin_lock_bh(&table->dflt_lock);
	if (rt == table->dflt_pointer) {
		RT6_TRACE("reset default router: %p->NULL\n", 
			  table->dflt_pointer);
		table->dflt_pointer = NULL;
	}
	spin_unlock_bh(&table->dflt_lock);
}
#endif

/* Default Router Selection (RFC 2461 6.3.6) */
static int __rt6_score_dflt(struct rt6_info *sprt, struct rt6_info *dflt, int oif)
{
	struct neighbour *neigh = sprt->rt6i_nexthop;
	int m = oif ? 0 : 8;

	if (!neigh)
		return -1;

	if (rt6_check_expired(sprt))
		return -1;

	if (oif && sprt->rt6i_dev &&
	    sprt->rt6i_dev->ifindex == oif)
		m += 8;

#if !defined(CONFIG_IPV6_ROUTER_PREF)
	if (sprt == dflt) 
		m += 4;
#endif

	read_lock_bh(&neigh->lock);
	switch (neigh->nud_state) {
	case NUD_REACHABLE:
		m += 3;
		break;

	case NUD_STALE:
	case NUD_DELAY:
	case NUD_PROBE:
		m += 2;
		break;

	case NUD_NOARP:
	case NUD_PERMANENT:
		m += 1;
		break;

	case NUD_INCOMPLETE:
	default:
		m = -1;
	}
	read_unlock_bh(&neigh->lock);

	return m;
}

static struct rt6_info *rt6_best_dflt(struct rt6_info *rt, struct rt6_info **head, int oif)
{
	struct rt6_table *table;
	struct rt6_info *match = NULL;
	struct rt6_info *sprt;
#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
	struct rt6_info *last = NULL;
#endif
	int mpri = 0;
#if defined(CONFIG_IPV6_ROUTER_PREF)
	u32 metric = 0;
	int pref = -3;
#else
	static const int okpri = 12;	/* device match, prob. reachable */
#endif

	table = rt6_hold_table(rt->rt6i_table);

	if (head != NULL && *head != rt)
		head = NULL;	/*XXX*/

#if !defined(CONFIG_IPV6_NEW_ROUNDROBIN)
	spin_lock(&table->dflt_lock);
#endif
#if !defined(CONFIG_IPV6_NEW_ROUNDROBIN)
	if (table->dflt_pointer) {
		for (sprt = rt; sprt; sprt = sprt->u.next) {
			if (sprt == table->dflt_pointer)
				break;
		}
		if (!sprt)
			table->dflt_pointer = NULL;	/* for sure */
	}
#endif

#if defined(CONFIG_IPV6_ROUTER_PREF)
#if !defined(CONFIG_IPV6_NEW_ROUNDROBIN)
	if (table->dflt_pointer) {
		for (sprt = table->dflt_pointer->u.next; sprt; sprt = sprt->u.next) {
			int m, p;

			if ((metric != 0 && sprt->rt6i_metric > metric) ||
			    sprt->u.dst.obsolete > 0 ||
			    sprt->u.dst.error != 0)
				continue;

#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
			m = __rt6_score_dflt(sprt, rt, oif);
#else
			m = __rt6_score_dflt(sprt, table->dflt_pointer, oif);
#endif
			if (m < mpri)
				continue;
			p = IPV6_SIGNEDPREF(IPV6_UNSHIFT_PREF(sprt->rt6i_flags));
			if (sprt->rt6i_metric < metric || m > mpri || p > pref) {
				match = sprt;
				metric = sprt->rt6i_metric;
				mpri = m;
				pref = p;
			}
		}
	}
#endif
#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
	metric = rt->rt6i_metric;
#endif
	for (sprt = rt; sprt; sprt = sprt->u.next) {
		int m, p;
#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
		if (sprt->rt6i_metric > metric)
			break;
#else
		if ((metric != 0 && sprt->rt6i_metric > metric) ||
		    sprt->u.dst.obsolete > 0 ||
		    sprt->u.dst.error != 0)
			continue;

#endif
#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
		m = __rt6_score_dflt(sprt, rt, oif);
#else
		m = __rt6_score_dflt(sprt, table->dflt_pointer, oif);
#endif
		if (m < mpri)
			continue;
		p = IPV6_SIGNEDPREF(IPV6_UNSHIFT_PREF(sprt->rt6i_flags));
		if (
#if !defined(CONFIG_IPV6_NEW_ROUNDROBIN)
		    sprt->rt6i_metric < metric || 
#endif
		    m > mpri || p > pref) {
			match = sprt;
			metric = sprt->rt6i_metric;
			mpri = m;
			pref = p;
		}
#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
		last = sprt;
#else
		if (sprt == table->dflt_pointer)
			break;
#endif
	}
#else	/* CONFIG_IPV6_ROUTER_PREF / !CONFIG_IPV6_ROUTER_PREF */
	for (sprt = rt; sprt; sprt = sprt->u.next) {
		int m;
#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
		m = __rt6_score_dflt(sprt, rt, oif);
#else
		m = __rt6_score_dflt(sprt, table->dflt_pointer, oif);
#endif

		if (m > mpri || m >= okpri) {
			match = sprt;
			mpri = m;
			if (m >= okpri) {
				/* we choose the last default router if it
				 * is in (probably) reachable state.
				 * If route changed, we should do pmtu
				 * discovery. --yoshfuji
				 */
				break;
			}
		}
	}

	if (!match) {
		/*
		 *	No default routers are known to be reachable.
		 *	SHOULD round robin
		 */
		if (table->dflt_pointer) {
			for (sprt = table->dflt_pointer->u.next;
			     sprt; sprt = sprt->u.next) {
				if (sprt->u.dst.obsolete <= 0 &&
				    sprt->u.dst.error == 0 &&
				    !rt6_check_expired(sprt)) {
					match = sprt;
					break;
				}
			}
			for (sprt = rt;
			     !match && sprt;
			     sprt = sprt->u.next) {
				if (sprt->u.dst.obsolete <= 0 &&
				    sprt->u.dst.error == 0 &&
				    !rt6_check_expired(sprt)) {
					match = sprt;
					break;
				}
				if (sprt == table->dflt_pointer)
					break;
			}
		}
	}
#endif /* !CONFIG_IPV6_ROUTER_PREF */

	if (match) {
#if defined(CONFIG_IPV6_NEW_ROUNDROBIN)
		if (rt != last && last) {
			*head = rt->u.next;
			rt->u.next = last->u.next;
			last->u.next = rt;
		}
#else
		if (table->dflt_pointer != match)
			RT6_TRACE("changed default router: %p->%p\n",
				  table->dflt_pointer, match);
		table->dflt_pointer = match;
#endif
	}
#if !defined(CONFIG_IPV6_NEW_ROUNDROBIN)
	spin_unlock(&table->dflt_lock);
#endif

	if (!match) {
		/*
		 * Last Resort: if no default routers found, 
		 * use addrconf default route.
		 * We don't record this route.
		 */
		for (sprt = table->root.leaf;
		     sprt; sprt = sprt->u.next) {
			if (!rt6_check_expired(sprt) &&
			    (sprt->rt6i_flags & RTF_DEFAULT) &&
			    (!oif ||
			     (sprt->rt6i_dev &&
			      sprt->rt6i_dev->ifindex == oif))) {
				match = sprt;
				break;
			}
		}
		if (!match) {
			/* no default route.  give up. */
			match = &ip6_null_entry;
		}
	}

	rt6_put_table(table);
	return match;
}

#define BACKTRACK(has_saddr, saddr) \
if (rt == &ip6_null_entry && flags & RT6_F_STRICT) { \
	while (1) { \
		if (fn->fn_flags & RTN_TL_ROOT) { \
			dst_hold(&rt->u.dst); \
			goto out; \
		} \
		pn = fn->parent; \
		if ((has_saddr) && \
		    FIB6_SUBTREE(pn) != NULL && pn->subtree != fn) { \
			fn = fib6_subtree_lookup(pn, (saddr)); \
		} else { \
			fn = pn; \
		} \
		if (fn->fn_flags & RTN_RTINFO) \
			goto restart; \
	} \
}

struct rt6_info *ip6_pol_route_lookup(struct rt6_table *table,
				      struct flowi *fl,
				      int flags,
				      struct fib6_rule *rule)
{
	struct fib6_node *fn, *pn;
	struct rt6_info *rt;

	read_lock_bh(&table->lock);
	fn = fib6_lookup(&table->root, &fl->fl6_dst, &fl->fl6_src);
restart:
	rt = fn->leaf;

	rt = rt6_device_match(rt, fl->oif, flags & RT6_F_STRICT);
	BACKTRACK(flags & RT6_F_HAS_SADDR, &fl->fl6_src);
	dst_hold(&rt->u.dst);
out:
	read_unlock_bh(&table->lock);

	rt->u.dst.lastuse = jiffies;
	rt->u.dst.__use++;
	return rt;
}

/* ip6_ins_rt is called with FREE table->lock.
   It takes new route entry, the addition fails by any reason the
   route is freed. In any case, if caller does not hold it, it may
   be destroyed.
 */

int ip6_ins_rt(struct rt6_info *rt, struct nlmsghdr *nlh,
		void *_rtattr, struct netlink_skb_parms *req)
{
	int err;
	struct rt6_table *table = rt6_hold_table(rt->rt6i_table);
	write_lock_bh(&table->lock);
	err = fib6_add(&table->root, rt, nlh, _rtattr, req);
	write_unlock_bh(&table->lock);
	rt6_put_table(table);
	return err;
}

/* No table->lock! If COW failed, the function returns dead route entry
   with dst->error set to errno value.
 */

static struct rt6_info *rt6_cow(struct rt6_info *ort, struct in6_addr *daddr,
				struct in6_addr *saddr, struct netlink_skb_parms *req)
{
	int err;
	struct rt6_info *rt;

	/*
	 *	Clone the route.
	 */

	rt = ip6_rt_copy(ort);

	if (rt) {
		ipv6_addr_copy(&rt->rt6i_dst.addr, daddr);

		if (!(rt->rt6i_flags&RTF_GATEWAY))
			ipv6_addr_copy(&rt->rt6i_gateway, daddr);

		rt->rt6i_dst.plen = 128;
		rt->rt6i_flags |= RTF_CACHE;
		rt->u.dst.flags |= DST_HOST;

#ifdef CONFIG_IPV6_SUBTREES
		ipv6_addr_copy(&rt->rt6i_src.addr, saddr);
		rt->rt6i_src.plen = 128;
#endif

		rt->rt6i_nexthop = ndisc_get_neigh(rt->rt6i_dev, &rt->rt6i_gateway);

		dst_hold(&rt->u.dst);
		/*
		 * An unspecified source address is allowed on special occasions
		 * such as in DAD, RS etc. As kernel generated packets of this
		 * type don't generate any cache routes, make sure user
		 * generated packets behave the same way.
		 */
		if (!ipv6_addr_any(&rt->rt6i_src.addr)) {
			err = ip6_ins_rt(rt, NULL, NULL, req);
			if (err == 0)
				return rt;

			rt->u.dst.error = err;
		}
		return rt;
	}
	dst_hold(&ip6_null_entry.u.dst);
	return &ip6_null_entry;
}

struct rt6_info *ip6_pol_route_input(struct rt6_table *table, struct flowi *fl,
				     int flags, struct fib6_rule *rule)
{
	struct fib6_node *fn, *pn;
	struct rt6_info *rt;
	int attempts = 3;

relookup:
	read_lock_bh(&table->lock);

	fn = fib6_lookup(&table->root, &fl->fl6_dst, &fl->fl6_src);

restart:
	rt = fn->leaf;

	if ((rt->rt6i_flags & RTF_CACHE)) {
		rt = rt6_device_match(rt, fl->iif, flags & RT6_F_STRICT);
		BACKTRACK(1, &fl->fl6_src);
		dst_hold(&rt->u.dst);
		goto out;
	}

	rt = rt6_device_match(rt, fl->iif, 0);
	BACKTRACK(1, &fl->fl6_src);

	if (!rt->rt6i_nexthop && !(rt->rt6i_flags & RTF_NONEXTHOP)) {
		struct rt6_info *nrt;
		dst_hold(&rt->u.dst);
		read_unlock_bh(&table->lock);

		nrt = rt6_cow(rt, &fl->fl6_dst, &fl->fl6_src, NULL);

		dst_release(&rt->u.dst);
		rt = nrt;

		if (rt->u.dst.error != -EEXIST || --attempts <= 0)
			goto out2;

		/* Race condition! In the gap, when table->lock was
		   released someone could insert this route.  Relookup.
		*/
		dst_release(&rt->u.dst);
		goto relookup;
	}
	dst_hold(&rt->u.dst);

out:
	read_unlock_bh(&table->lock);
out2:
	rt->u.dst.lastuse = jiffies;
	rt->u.dst.__use++;
	return rt;
}

struct rt6_info *ip6_pol_route_output(struct rt6_table *table,
				      struct flowi *fl,
				      int flags,
				      struct fib6_rule *rule)
{
	struct fib6_node *fn, *pn;
	struct rt6_info *rt;
	int attempts = 3;
	int has_saddr = flags & RT6_F_HAS_SADDR;
	struct in6_addr saddr;
	int oif = fl->oif;
  
	ipv6_addr_copy(&saddr, &fl->fl6_src);

#ifdef CONFIG_IPV6_SUBTREES
	/* a source prefix specific policy rule overrides the normal source
	   address selection process */
	if (!has_saddr && rule->r_src.plen > 0) {
		struct net_device *dev = NULL;
		if (oif)
			dev = dev_get_by_index(oif);
		oif = ipv6_dev_get_saddr(dev, &fl->fl6_dst,
					 &rule->r_src, &saddr);
		if (dev)
			dev_put(dev);
		if (oif < 0) {
			/* no matching source addres found; exit */
			rt = &ip6_null_entry;
			dst_hold(&rt->u.dst);
			goto out2;
		}
		ipv6_addr_copy(&fl->fl6_src, &saddr);
		has_saddr = 1;
	}
#endif

relookup:
	read_lock_bh(&table->lock);

	fn = fib6_lookup(&table->root, &fl->fl6_dst, &saddr);

restart:
	rt = fn->leaf;

	if ((rt->rt6i_flags & RTF_CACHE)) {
		rt = rt6_device_match(rt, oif, flags & RT6_F_STRICT);
		BACKTRACK(has_saddr, &saddr);
		dst_hold(&rt->u.dst);
		goto out;
	}
	if (rt->rt6i_flags & RTF_DEFAULT) {
		if (rt->rt6i_metric >= IP6_RT_PRIO_ADDRCONF)
			rt = rt6_best_dflt(rt, &fn->leaf, oif);
	} else {
		rt = rt6_device_match(rt, oif, flags & RT6_F_STRICT);
		BACKTRACK(has_saddr, &saddr);
	}

	if (!has_saddr) {
		if (ipv6_get_saddr(&rt->u.dst, &fl->fl6_dst, &saddr) < 0) {
			/* no matching source addres found; exit */
			rt = &ip6_null_entry;
			dst_hold(&rt->u.dst);
			goto out;
		}
		ipv6_addr_copy(&fl->fl6_src, &saddr);
		has_saddr = 1;
	}

	if (!rt->rt6i_nexthop && !(rt->rt6i_flags & RTF_NONEXTHOP)) {
		struct rt6_info *nrt;
		dst_hold(&rt->u.dst);
		read_unlock_bh(&table->lock);
  
		nrt = rt6_cow(rt, &fl->fl6_dst, &saddr, NULL);

		dst_release(&rt->u.dst);
		rt = nrt;

		if (rt->u.dst.error != -EEXIST || --attempts <= 0)
			goto out2;

		/* Race condition! In the gap, when table->lock was
		   released someone could insert this route.  Relookup.
		*/
		dst_release(&rt->u.dst);
		goto relookup;
	}
	dst_hold(&rt->u.dst);

out:
	read_unlock_bh(&table->lock);
out2:
	rt->u.dst.lastuse = jiffies;
	rt->u.dst.__use++;
	return rt;
}

/*
 *	Destination cache support functions
 */

static struct dst_entry *ip6_dst_check(struct dst_entry *dst, u32 cookie)
{
	if (dst->obsolete <= 0 && cookie == atomic_read(&flow_cache_genid))
		return dst;

	return NULL;
}

static struct dst_entry *ip6_negative_advice(struct dst_entry *dst)
{
	struct rt6_info *rt = (struct rt6_info *) dst;

	if (rt) {
		if (rt->rt6i_flags & RTF_CACHE)
			ip6_del_rt(rt, NULL, NULL, NULL);
		else
			dst_release(dst);
	}
	return NULL;
}

static void ip6_link_failure(struct sk_buff *skb)
{
	struct rt6_info *rt;

	icmpv6_send(skb, ICMPV6_DEST_UNREACH, ICMPV6_ADDR_UNREACH, 0, skb->dev);

	rt = (struct rt6_info *) skb->dst;
	if (rt) {
		if (rt->rt6i_flags & RTF_CACHE) {
			dst_set_expires(&rt->u.dst, 0);
			rt->rt6i_flags |= RTF_EXPIRES;
		}
	}
}

static void ip6_rt_update_pmtu(struct dst_entry *dst, u32 mtu)
{
	struct rt6_info *rt6 = (struct rt6_info*)dst;

	if (mtu < dst_mtu(dst) && rt6->rt6i_dst.plen == 128) {
		rt6->rt6i_flags |= RTF_MODIFIED;
		if (mtu < IPV6_MIN_MTU) {
			mtu = IPV6_MIN_MTU;
			dst->metrics[RTAX_FEATURES-1] |= RTAX_FEATURE_ALLFRAG;
		}
		dst->metrics[RTAX_MTU-1] = mtu;
	}
}

/* Protected by ndisc_lock.  */
static struct dst_entry *ndisc_dst_gc_list;
static int ipv6_get_mtu(struct net_device *dev);

static inline unsigned int ipv6_advmss(unsigned int mtu)
{
	mtu -= sizeof(struct ipv6hdr) + sizeof(struct tcphdr);

	if (mtu < ip6_rt_min_advmss)
		mtu = ip6_rt_min_advmss;

	/*
	 * Maximal non-jumbo IPv6 payload is IPV6_MAXPLEN and
	 * corresponding MSS is IPV6_MAXPLEN - tcp_header_size.
	 * IPV6_MAXPLEN is also valid and means: "any MSS,
	 * rely only on pmtu discovery"
	 */
	if (mtu > IPV6_MAXPLEN - sizeof(struct tcphdr))
		mtu = IPV6_MAXPLEN;
	return mtu;
}

struct dst_entry *ndisc_dst_alloc(struct net_device *dev,
				  struct neighbour *neigh,
				  struct in6_addr *addr,
				  int (*output)(struct sk_buff *))
{
	struct rt6_info *rt;
	struct inet6_dev *idev = in6_dev_get(dev);

	if (unlikely(idev == NULL))
		return NULL;

	rt = ip6_dst_alloc();
	if (unlikely(rt == NULL)) {
		in6_dev_put(idev);
		goto out;
	}

	dev_hold(dev);
	if (neigh)
		neigh_hold(neigh);
	else
		neigh = ndisc_get_neigh(dev, addr);

	rt->rt6i_dev	  = dev;
	rt->rt6i_idev     = idev;
	rt->rt6i_nexthop  = neigh;
	atomic_set(&rt->u.dst.__refcnt, 1);
	rt->u.dst.metrics[RTAX_HOPLIMIT-1] = 255;
	rt->u.dst.metrics[RTAX_MTU-1] = ipv6_get_mtu(rt->rt6i_dev);
	rt->u.dst.metrics[RTAX_ADVMSS-1] = ipv6_advmss(dst_mtu(&rt->u.dst));
	rt->u.dst.output  = output;
	rt->u.dst.obsolete = -1;

#if 0	/* there's no chance to use these for ndisc */
	rt->u.dst.flags   = ipv6_addr_type(addr) & IPV6_ADDR_UNICAST
				? DST_HOST
				: 0;
	ipv6_addr_copy(&rt->rt6i_dst.addr, addr);
	rt->rt6i_dst.plen = 128;
#endif

	spin_lock_bh(&ndisc_lock);
	rt->u.dst.next = ndisc_dst_gc_list;
	ndisc_dst_gc_list = &rt->u.dst;
	spin_unlock_bh(&ndisc_lock);

	fib6_force_start_gc();
	atomic_inc(&flow_cache_genid);
out:
	return (struct dst_entry *)rt;
}

int ndisc_dst_gc(int *more)
{
	struct dst_entry *dst, *next, **pprev;
	int freed;

	next = NULL;
	freed = 0;

	spin_lock_bh(&ndisc_lock);

	pprev = &ndisc_dst_gc_list;
	while ((dst = *pprev) != NULL) {
		if (!atomic_read(&dst->__refcnt)) {
			*pprev = dst->next;
			dst_free(dst);
			freed++;
		} else {
			pprev = &dst->next;
			(*more)++;
		}
	}

	spin_unlock_bh(&ndisc_lock);

	return freed;
}

static int ip6_dst_gc(void)
{
	static unsigned expire = 30*HZ;
	static unsigned long last_gc;
	unsigned long now = jiffies;

	if (time_after(last_gc + ip6_rt_gc_min_interval, now) &&
	    atomic_read(&ip6_dst_ops.entries) <= ip6_rt_max_size)
		goto out;

	expire++;
	fib6_run_gc(expire);
	last_gc = now;
	if (atomic_read(&ip6_dst_ops.entries) < ip6_dst_ops.gc_thresh)
		expire = ip6_rt_gc_timeout>>1;

out:
	expire -= expire>>ip6_rt_gc_elasticity;
	return (atomic_read(&ip6_dst_ops.entries) > ip6_rt_max_size);
}

/* Clean host part of a prefix. Not necessary in radix tree,
   but results in cleaner routing tables.

   Remove it only when all the things will work!
 */

static int ipv6_get_mtu(struct net_device *dev)
{
	int mtu = IPV6_MIN_MTU;
	struct inet6_dev *idev;

	idev = in6_dev_get(dev);
	if (idev) {
		mtu = idev->cnf.mtu6;
		in6_dev_put(idev);
	}
	return mtu;
}

int ipv6_get_hoplimit(struct net_device *dev)
{
	int hoplimit = ipv6_devconf.hop_limit;
	struct inet6_dev *idev;

	idev = in6_dev_get(dev);
	if (idev) {
		hoplimit = idev->cnf.hop_limit;
		in6_dev_put(idev);
	}
	return hoplimit;
}

/*
 *
 */

int ip6_route_add(struct in6_rtmsg *rtmsg, struct nlmsghdr *nlh, void *_rtattr,
		  struct netlink_skb_parms *req, unsigned char table_id)
{
	int err;
	struct rtmsg *r;
	struct rtattr **rta;
	struct rt6_table *table = NULL;
	struct rt6_info *rt = NULL;
	struct net_device *dev = NULL;
	struct inet6_dev *idev = NULL;
	int addr_type;

	rta = (struct rtattr **) _rtattr;

	if (rtmsg->rtmsg_dst_len > 128 || rtmsg->rtmsg_src_len > 128)
		return -EINVAL;
#ifndef CONFIG_IPV6_SUBTREES
	if (rtmsg->rtmsg_src_len)
		return -EINVAL;
#endif
	if (rtmsg->rtmsg_ifindex) {
		err = -ENODEV;
		dev = dev_get_by_index(rtmsg->rtmsg_ifindex);
		if (!dev)
			goto out;
		idev = in6_dev_get(dev);
		if (!idev)
			goto out;
	}

	if (rtmsg->rtmsg_metric == 0)
		rtmsg->rtmsg_metric = IP6_RT_PRIO_USER;

	table = rt6_new_table(table_id);

	if (unlikely(table == NULL)) {
		err = -ENOBUFS;
		goto out;
	}

	rt = ip6_dst_alloc();

	if (rt == NULL) {
		err = -ENOMEM;
		goto out;
	}

	rt->u.dst.obsolete = -1;
	rt->rt6i_expires = jiffies + clock_t_to_jiffies(rtmsg->rtmsg_info);
	if (nlh && (r = NLMSG_DATA(nlh))) {
		rt->rt6i_protocol = r->rtm_protocol;
	} else {
		rt->rt6i_protocol = RTPROT_BOOT;
	}

	addr_type = ipv6_addr_type(&rtmsg->rtmsg_dst);

	if (addr_type & IPV6_ADDR_MULTICAST)
		rt->u.dst.input = ip6_mc_input;
	else
		rt->u.dst.input = ip6_forward;

	rt->u.dst.output = ip6_output;

	ipv6_addr_prefix(&rt->rt6i_dst.addr,
			 &rtmsg->rtmsg_dst, rtmsg->rtmsg_dst_len);
	rt->rt6i_dst.plen = rtmsg->rtmsg_dst_len;
	if (rt->rt6i_dst.plen == 128)
	       rt->u.dst.flags = DST_HOST;

#ifdef CONFIG_IPV6_SUBTREES
	ipv6_addr_prefix(&rt->rt6i_src.addr,
			 &rtmsg->rtmsg_src, rtmsg->rtmsg_src_len);
	rt->rt6i_src.plen = rtmsg->rtmsg_src_len;
#endif

	rt->rt6i_metric = rtmsg->rtmsg_metric;

	/* We cannot add true routes via loopback here,
	   they would result in kernel looping; promote them to reject routes
	 */
	if ((rtmsg->rtmsg_flags&RTF_REJECT) ||
	    (dev && (dev->flags&IFF_LOOPBACK) && !(addr_type&IPV6_ADDR_LOOPBACK))) {
		/* hold loopback dev/idev if we haven't done so. */
		if (dev != &loopback_dev) {
			if (dev) {
				dev_put(dev);
				in6_dev_put(idev);
			}
			dev = &loopback_dev;
			dev_hold(dev);
			idev = in6_dev_get(dev);
			if (!idev) {
				err = -ENODEV;
				goto out;
			}
		}
		rt->u.dst.output = ip6_pkt_discard_out;
		rt->u.dst.input = ip6_pkt_discard;
		rt->u.dst.error = -ENETUNREACH;
		rt->rt6i_flags = RTF_REJECT|RTF_NONEXTHOP;
		goto install_route;
	}

	if (rtmsg->rtmsg_flags & RTF_GATEWAY) {
		struct in6_addr *gw_addr;
		int gwa_type;

		gw_addr = &rtmsg->rtmsg_gateway;
		ipv6_addr_copy(&rt->rt6i_gateway, &rtmsg->rtmsg_gateway);
		gwa_type = ipv6_addr_type(gw_addr);

		if (gwa_type != (IPV6_ADDR_LINKLOCAL|IPV6_ADDR_UNICAST)) {
			struct rt6_info *grt;

			/* IPv6 strictly inhibits using not link-local
			   addresses as nexthop address.
			   Otherwise, router will not able to send redirects.
			   It is very good, but in some (rare!) circumstances
			   (SIT, PtP, NBMA NOARP links) it is handy to allow
			   some exceptions. --ANK
			 */
			err = -EINVAL;
			if (!(gwa_type&IPV6_ADDR_UNICAST))
				goto out;

			grt = rt6_lookup(gw_addr, &rt->rt6i_src.addr, rtmsg->rtmsg_ifindex, 1);

			err = -EHOSTUNREACH;
			if (grt == NULL)
				goto out;
			if (dev) {
				if (dev != grt->rt6i_dev) {
					dst_release(&grt->u.dst);
					goto out;
				}
			} else {
				dev = grt->rt6i_dev;
				idev = grt->rt6i_idev;
				dev_hold(dev);
				in6_dev_hold(grt->rt6i_idev);
			}
			if (!(grt->rt6i_flags&RTF_GATEWAY))
				err = 0;
			dst_release(&grt->u.dst);

			if (err)
				goto out;
		}
		err = -EINVAL;
		if (dev == NULL || (dev->flags&IFF_LOOPBACK))
			goto out;
	}

	err = -ENODEV;
	if (dev == NULL)
		goto out;

	if (rtmsg->rtmsg_flags & (RTF_GATEWAY|RTF_NONEXTHOP)) {
		rt->rt6i_nexthop = __neigh_lookup_errno(&nd_tbl, &rt->rt6i_gateway, dev);
		if (IS_ERR(rt->rt6i_nexthop)) {
			err = PTR_ERR(rt->rt6i_nexthop);
			rt->rt6i_nexthop = NULL;
			goto out;
		}
	}

	rt->rt6i_flags = rtmsg->rtmsg_flags;

install_route:
	if (rta && rta[RTA_METRICS-1]) {
		int attrlen = RTA_PAYLOAD(rta[RTA_METRICS-1]);
		struct rtattr *attr = RTA_DATA(rta[RTA_METRICS-1]);

		while (RTA_OK(attr, attrlen)) {
			unsigned flavor = attr->rta_type;
			if (flavor) {
				if (flavor > RTAX_MAX) {
					err = -EINVAL;
					goto out;
				}
				rt->u.dst.metrics[flavor-1] =
					*(u32 *)RTA_DATA(attr);
			}
			attr = RTA_NEXT(attr, attrlen);
		}
	}

	if (rt->u.dst.metrics[RTAX_HOPLIMIT-1] == 0)
		rt->u.dst.metrics[RTAX_HOPLIMIT-1] = -1;
	if (!rt->u.dst.metrics[RTAX_MTU-1])
		rt->u.dst.metrics[RTAX_MTU-1] = ipv6_get_mtu(dev);
	if (!rt->u.dst.metrics[RTAX_ADVMSS-1])
		rt->u.dst.metrics[RTAX_ADVMSS-1] = ipv6_advmss(dst_mtu(&rt->u.dst));
	rt->u.dst.dev = dev;
	rt->rt6i_idev = idev;
	rt->rt6i_table = table;
	return ip6_ins_rt(rt, nlh, _rtattr, req);

out:
	if (table)
		rt6_put_table(table);
	if (dev)
		dev_put(dev);
	if (idev)
		in6_dev_put(idev);
	if (rt)
		dst_free((struct dst_entry *) rt);
	return err;
}

int ip6_del_rt(struct rt6_info *rt, struct nlmsghdr *nlh, void *_rtattr, struct netlink_skb_parms *req)
{
	int err;

	struct rt6_table *table = rt6_hold_table(rt->rt6i_table);

	write_lock_bh(&table->lock);

#ifndef CONFIG_IPV6_NEW_ROUNDROBIN
	rt6_reset_dflt_pointer(rt);
#endif

	err = fib6_del(rt, nlh, _rtattr, req);
	dst_release(&rt->u.dst);

	write_unlock_bh(&table->lock);
	rt6_put_table(table);

	return err;
}

static int ip6_route_del(struct in6_rtmsg *rtmsg, struct nlmsghdr *nlh,
			 void *_rtattr, struct netlink_skb_parms *req,
			 unsigned char table_id)
{
	struct rt6_table *table;
	struct fib6_node *fn;
	struct rt6_info *rt;
	int err = -ESRCH;

	table = rt6_get_table(table_id);

	if (unlikely(table == NULL))
		return err;

	read_lock_bh(&table->lock);

	fn = fib6_locate(&table->root,
			 &rtmsg->rtmsg_dst, rtmsg->rtmsg_dst_len,
			 &rtmsg->rtmsg_src, rtmsg->rtmsg_src_len);
	
	if (fn) {
		for (rt = fn->leaf; rt; rt = rt->u.next) {
			if (rtmsg->rtmsg_ifindex &&
			    (rt->rt6i_dev == NULL ||
			     rt->rt6i_dev->ifindex != rtmsg->rtmsg_ifindex))
				continue;
			if (rtmsg->rtmsg_flags&RTF_GATEWAY &&
			    !ipv6_addr_equal(&rtmsg->rtmsg_gateway, &rt->rt6i_gateway))
				continue;
			if (rtmsg->rtmsg_metric &&
			    rtmsg->rtmsg_metric != rt->rt6i_metric)
				continue;
			dst_hold(&rt->u.dst);
			read_unlock_bh(&table->lock);
			err = ip6_del_rt(rt, nlh, _rtattr, req);
			rt6_put_table(table);
			return err;
		}
	}
	read_unlock_bh(&table->lock);
	rt6_put_table(table);

	return err;
}

/*
 *	Handle redirects
 */
void rt6_redirect(struct in6_addr *dest, struct in6_addr *saddr,
		  struct in6_addr *gateway,
		  struct neighbour *neigh, u8 *lladdr, int on_link)
{
	struct rt6_info *rt, *nrt;

#ifdef CONFIG_IPV6_NDISC_DEBUG
	printk(KERN_DEBUG
		"%s("
		"dest=%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x, "
		"saddr=%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x, "
		"neigh=%p(%s), "
		"lladdr=%p, on_link=%d)\n",
		__FUNCTION__,
		NIP6(*dest), NIP6(*saddr),
		neigh, neigh_state(neigh->nud_state),
		lladdr, on_link);
#endif

	/* Locate old route to this destination. */
	rt = rt6_lookup(dest, saddr, neigh->dev->ifindex, 1);

	if (rt == NULL)
		goto out;

	if (neigh->dev != rt->rt6i_dev)
		goto out;

	/*
	 * Current route is on-link; redirect is always invalid.
	 *
	 * Seems, previous statement is not true. It could
	 * be node, which looks for us as on-link (f.e. proxy ndisc)
	 * But then router serving it might decide, that we should
	 * know truth 8)8) --ANK (980726).
	 */
	if (!(rt->rt6i_flags&RTF_GATEWAY)) {
		if (net_ratelimit())
			printk(KERN_DEBUG "%s(): rt=%p is on-link; ignored.\n", 
				__FUNCTION__, rt);
		goto out;
	}

	/*
	 *	RFC 2461 specifies that redirects should only be
	 *	accepted if they come from the nexthop to the target.
	 *	Due to the way default routers are chosen, this notion
	 *	is a bit fuzzy and one might need to check all default
	 *	routers.
	 */
	if (!ipv6_addr_equal(gateway, &rt->rt6i_gateway)) {
		if (rt->rt6i_flags & RTF_DEFAULT) {
			struct rt6_table *table;
			struct rt6_info *rt1;

			table = rt6_hold_table(rt->rt6i_table);

			read_lock(&table->lock);
			for (rt1 = table->root.leaf; rt1; rt1 = rt1->u.next) {
				if (ipv6_addr_equal(gateway, &rt1->rt6i_gateway)) {
					dst_hold(&rt1->u.dst);
					dst_release(&rt->u.dst);
					read_unlock(&table->lock);
					rt6_put_table(table);
					rt = rt1;
					goto source_ok;
				}
			}
			read_unlock(&table->lock);
			rt6_put_table(table);
		}
		if (net_ratelimit())
			printk(KERN_DEBUG "rt6_redirect: source isn't a valid nexthop "
			       "for redirect target\n");
		goto out;
	}

source_ok:
	/*
	 * Okay, we have finally decided to accept it.
	 */

	neigh_update(neigh, lladdr, NUD_STALE,
		     NEIGH_UPDATE_F_WEAK_OVERRIDE|
		     NEIGH_UPDATE_F_OVERRIDE|
		     (on_link ? 0 : (NEIGH_UPDATE_F_OVERRIDE_ISROUTER|
				     NEIGH_UPDATE_F_ISROUTER))
		     );

	/*
	 * Redirect received -> path was valid.
	 * Look, redirects are sent only in response to data packets,
	 * so that this nexthop apparently is reachable. --ANK
	 */
	dst_confirm(&rt->u.dst);

	/* Duplicate redirect: silently ignore. */
	if (neigh == rt->u.dst.neighbour)
		goto out;

	nrt = ip6_rt_copy(rt);
	if (nrt == NULL)
		goto out;

	nrt->rt6i_flags = RTF_GATEWAY|RTF_UP|RTF_DYNAMIC|RTF_CACHE;
	if (on_link)
		nrt->rt6i_flags &= ~RTF_GATEWAY;

	ipv6_addr_copy(&nrt->rt6i_dst.addr, dest);
	nrt->rt6i_dst.plen = 128;
#ifdef CONFIG_IPV6_SUBTREES
	ipv6_addr_copy(&nrt->rt6i_src.addr, saddr);
	nrt->rt6i_src.plen = 128;
#endif
	nrt->u.dst.flags |= DST_HOST;

	ipv6_addr_copy(&nrt->rt6i_gateway, (struct in6_addr*)neigh->primary_key);
	nrt->rt6i_nexthop = neigh_clone(neigh);
	/* Reset pmtu, it may be better */
	nrt->u.dst.metrics[RTAX_MTU-1] = ipv6_get_mtu(neigh->dev);
	nrt->u.dst.metrics[RTAX_ADVMSS-1] = ipv6_advmss(dst_mtu(&nrt->u.dst));

	if (ip6_ins_rt(nrt, NULL, NULL, NULL))
		goto out;

	if (rt->rt6i_flags&RTF_CACHE) {
		ip6_del_rt(rt, NULL, NULL, NULL);
		return;
	}

out:
        dst_release(&rt->u.dst);
}

/*
 *	Handle ICMP "packet too big" messages
 *	i.e. Path MTU discovery
 */

void rt6_pmtu_discovery(struct in6_addr *daddr, struct in6_addr *saddr,
			struct net_device *dev, u32 pmtu)
{
	struct rt6_info *rt, *nrt;
	int allfrag = 0;

	rt = rt6_lookup(daddr, saddr, dev->ifindex, 0);
	if (rt == NULL)
		return;

	if (pmtu >= dst_mtu(&rt->u.dst))
		goto out;

	if (pmtu < IPV6_MIN_MTU) {
		/*
		 * According to RFC2460, PMTU is set to the IPv6 Minimum Link
		 * MTU (1280) and a fragment header should always be included
		 * after a node receiving Too Big message reporting PMTU is
		 * less than the IPv6 Minimum Link MTU.
		 */
		pmtu = IPV6_MIN_MTU;
		allfrag = 1;
	}

	/* New mtu received -> path was valid.
	   They are sent only in response to data packets,
	   so that this nexthop apparently is reachable. --ANK
	 */
	dst_confirm(&rt->u.dst);

	/* Host route. If it is static, it would be better
	   not to override it, but add new one, so that
	   when cache entry will expire old pmtu
	   would return automatically.
	 */
	if (rt->rt6i_flags & RTF_CACHE) {
		rt->u.dst.metrics[RTAX_MTU-1] = pmtu;
		if (allfrag)
			rt->u.dst.metrics[RTAX_FEATURES-1] |= RTAX_FEATURE_ALLFRAG;
		dst_set_expires(&rt->u.dst, ip6_rt_mtu_expires);
		rt->rt6i_flags |= RTF_MODIFIED|RTF_EXPIRES;
		goto out;
	}

	/* Network route.
	   Two cases are possible:
	   1. It is connected route. Action: COW
	   2. It is gatewayed route or NONEXTHOP route. Action: clone it.
	 */
	if (!rt->rt6i_nexthop && !(rt->rt6i_flags & RTF_NONEXTHOP)) {
		nrt = rt6_cow(rt, daddr, saddr, NULL);
		if (!nrt->u.dst.error) {
			nrt->u.dst.metrics[RTAX_MTU-1] = pmtu;
			if (allfrag)
				nrt->u.dst.metrics[RTAX_FEATURES-1] |= RTAX_FEATURE_ALLFRAG;
			/* According to RFC 1981, detecting PMTU increase shouldn't be
			   happened within 5 mins, the recommended timer is 10 mins.
			   Here this route expiration time is set to ip6_rt_mtu_expires
			   which is 10 mins. After 10 mins the decreased pmtu is expired
			   and detecting PMTU increase will be automatically happened.
			 */
			dst_set_expires(&nrt->u.dst, ip6_rt_mtu_expires);
			nrt->rt6i_flags |= RTF_DYNAMIC|RTF_EXPIRES;
		}
		dst_release(&nrt->u.dst);
	} else {
		nrt = ip6_rt_copy(rt);
		if (nrt == NULL)
			goto out;
		ipv6_addr_copy(&nrt->rt6i_dst.addr, daddr);
		nrt->rt6i_dst.plen = 128;
#ifdef CONFIG_IPV6_SUBTREES
		ipv6_addr_copy(&nrt->rt6i_src.addr, saddr);
		nrt->rt6i_src.plen = 128;
#endif
		nrt->u.dst.flags |= DST_HOST;
		nrt->rt6i_nexthop = neigh_clone(rt->rt6i_nexthop);
		dst_set_expires(&nrt->u.dst, ip6_rt_mtu_expires);
		nrt->rt6i_flags |= RTF_DYNAMIC|RTF_CACHE|RTF_EXPIRES;
		nrt->u.dst.metrics[RTAX_MTU-1] = pmtu;
		if (allfrag)
			nrt->u.dst.metrics[RTAX_FEATURES-1] |= RTAX_FEATURE_ALLFRAG;
		ip6_ins_rt(nrt, NULL, NULL, NULL);
	}

out:
	dst_release(&rt->u.dst);
}

/*
 *	Misc support functions
 */

static struct rt6_info * ip6_rt_copy(struct rt6_info *ort)
{
	struct rt6_info *rt = ip6_dst_alloc();

	if (rt) {
		rt->u.dst.input = ort->u.dst.input;
		rt->u.dst.output = ort->u.dst.output;
		rt->u.dst.obsolete = -1;

		memcpy(rt->u.dst.metrics, ort->u.dst.metrics, RTAX_MAX*sizeof(u32));
		rt->u.dst.dev = ort->u.dst.dev;
		if (rt->u.dst.dev)
			dev_hold(rt->u.dst.dev);
		rt->rt6i_idev = ort->rt6i_idev;
		if (rt->rt6i_idev)
			in6_dev_hold(rt->rt6i_idev);
		rt->u.dst.lastuse = jiffies;
		rt->rt6i_expires = 0;

		ipv6_addr_copy(&rt->rt6i_gateway, &ort->rt6i_gateway);
		rt->rt6i_flags = ort->rt6i_flags & ~RTF_EXPIRES;
		rt->rt6i_metric = 0;

		memcpy(&rt->rt6i_dst, &ort->rt6i_dst, sizeof(struct rt6key));
#ifdef CONFIG_IPV6_SUBTREES
		memcpy(&rt->rt6i_src, &ort->rt6i_src, sizeof(struct rt6key));
#endif
		rt->rt6i_table = rt6_hold_table(ort->rt6i_table);
	}
	return rt;
}

struct rt6_info *rt6_get_dflt_router(struct in6_addr *addr, struct net_device *dev)
{	
	struct rt6_info *rt;
	struct rt6_table *table;

	table = rt6_get_table(rt6_dev_dflt_table_id(dev->ifindex));

	if (unlikely(table == NULL))
		return NULL;

	read_lock_bh(&table->lock);
	for (rt = table->root.leaf; rt; rt=rt->u.next) {
		if (dev == rt->rt6i_dev &&
		    ipv6_addr_equal(&rt->rt6i_gateway, addr))
			break;
	}
	if (rt)
		dst_hold(&rt->u.dst);
	read_unlock_bh(&table->lock);
	rt6_put_table(table);
	return rt;
}

struct rt6_info *rt6_add_dflt_router(struct in6_addr *gwaddr,
				     struct net_device *dev,
				     int pref)
{
	struct in6_rtmsg rtmsg;

	memset(&rtmsg, 0, sizeof(struct in6_rtmsg));
	rtmsg.rtmsg_type = RTMSG_NEWROUTE;
	ipv6_addr_copy(&rtmsg.rtmsg_gateway, gwaddr);
	rtmsg.rtmsg_metric = 1024;
	rtmsg.rtmsg_flags = RTF_GATEWAY | RTF_ADDRCONF | RTF_DEFAULT | RTF_UP | RTF_PREF(pref&3) | RTF_EXPIRES;

	rtmsg.rtmsg_ifindex = dev->ifindex;
	ip6_route_add(&rtmsg, NULL, NULL, NULL, rt6_dev_dflt_table_id(dev->ifindex));
	return rt6_get_dflt_router(gwaddr, dev);
}

static void rt6_table_purge_dflt_routers(unsigned char table_id)
{
	struct rt6_info *rt;
	struct rt6_table *table;

	table = rt6_get_table(table_id);
	if (table == NULL)
		return;
restart:
	read_lock_bh(&table->lock);
	for (rt = table->root.leaf; rt; rt = rt->u.next) {
		if (rt->rt6i_flags & (RTF_DEFAULT | RTF_ADDRCONF)) {
			dst_hold(&rt->u.dst);

			rt6_reset_dflt_pointer(rt);

			read_unlock_bh(&table->lock);

			ip6_del_rt(rt, NULL, NULL, NULL);

			goto restart;
		}
	}
	read_unlock_bh(&table->lock);
	rt6_put_table(table);
}

void rt6_purge_dflt_routers(int ifindex)
{
	if (ifindex)
		rt6_table_purge_dflt_routers(rt6_dev_dflt_table_id(ifindex));
	else {
		int i;
		for (i = RT6_TABLE_MIN; i <= RT6_TABLE_MAX; i++)
			rt6_table_purge_dflt_routers(i);
	}
}

int ipv6_route_ioctl(unsigned int cmd, void __user *arg)
{
	struct in6_rtmsg rtmsg;
	int err;

	switch(cmd) {
	case SIOCADDRT:		/* Add a route */
	case SIOCDELRT:		/* Delete a route */
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		err = copy_from_user(&rtmsg, arg,
				     sizeof(struct in6_rtmsg));
		if (err)
			return -EFAULT;
			
		rtnl_lock();
		switch (cmd) {
		case SIOCADDRT:
			err = ip6_route_add(&rtmsg, NULL, NULL,
					    NULL, RT6_TABLE_MAIN);
			break;
		case SIOCDELRT:
			err = ip6_route_del(&rtmsg, NULL, NULL,
					    NULL, RT6_TABLE_MAIN);
			break;
		default:
			err = -EINVAL;
		}
		rtnl_unlock();

		return err;
	};

	return -EINVAL;
}

/*
 *	Drop the packet on the floor
 */

static int ip6_pkt_discard(struct sk_buff *skb)
{
#ifdef CONFIG_IPV6_STATISTICS
        struct dst_entry *dst = skb->dst;
 	struct inet6_dev *idev = ((struct rt6_info *)dst)->rt6i_idev;
#endif
	if (skb->dst->error == -ENETUNREACH) {
#ifdef CONFIG_IPV6_STATISTICS
	        IP6_INC_STATS(idev, IPSTATS_MIB_OUTNOROUTES);
#else
		IP6_INC_STATS(IPSTATS_MIB_OUTNOROUTES);
#endif
		icmpv6_send(skb, ICMPV6_DEST_UNREACH,
			    ICMPV6_NOROUTE, 0, skb->dev);
	} else {
#ifdef CONFIG_IPV6_STATISTICS
	        IP6_INC_STATS(idev, IPSTATS_MIB_OUTDISCARDS);
#else
		IP6_INC_STATS(IPSTATS_MIB_OUTDISCARDS);
#endif
		if (skb->dst->error == -EACCES)
			icmpv6_send(skb, ICMPV6_DEST_UNREACH,
				    ICMPV6_ADM_PROHIBITED, 0, skb->dev);
	}
	kfree_skb(skb);
	return 0;
}

static int ip6_pkt_discard_out(struct sk_buff *skb)
{
	skb->dev = skb->dst->dev;
	return ip6_pkt_discard(skb);
}

/*
 *	Allocate a dst for local (unicast / anycast) address.
 */

struct rt6_info *addrconf_dst_alloc(struct inet6_dev *idev,
				    const struct in6_addr *addr,
				    int anycast)
{
	struct rt6_info *rt = ip6_dst_alloc();

	if (rt == NULL)
		return ERR_PTR(-ENOMEM);

	dev_hold(&loopback_dev);
	in6_dev_hold(idev);

	rt->u.dst.flags = DST_HOST;
	rt->u.dst.input = ip6_input;
	rt->u.dst.output = ip6_output;
	rt->rt6i_dev = &loopback_dev;
	rt->rt6i_idev = idev;
	rt->u.dst.metrics[RTAX_MTU-1] = ipv6_get_mtu(rt->rt6i_dev);
	rt->u.dst.metrics[RTAX_ADVMSS-1] = ipv6_advmss(dst_mtu(&rt->u.dst));
	rt->u.dst.metrics[RTAX_HOPLIMIT-1] = -1;
	rt->u.dst.obsolete = -1;

	rt->rt6i_flags = RTF_UP | RTF_NONEXTHOP;
	if (!anycast)
		rt->rt6i_flags |= RTF_LOCAL;
	rt->rt6i_nexthop = ndisc_get_neigh(rt->rt6i_dev, &rt->rt6i_gateway);
	if (rt->rt6i_nexthop == NULL) {
		dst_free((struct dst_entry *) rt);
		return ERR_PTR(-ENOMEM);
	}

	ipv6_addr_copy(&rt->rt6i_dst.addr, addr);
	rt->rt6i_dst.plen = 128;

	rt->rt6i_table = rt6_get_table(RT6_TABLE_LOCAL);

	atomic_set(&rt->u.dst.__refcnt, 1);

	return rt;
}

#ifdef CONFIG_IPV6_SUBTREES

static int fib6_ifa_del(struct rt6_info *rt, void *p_ifa)
{
	struct inet6_ifaddr *ifa = p_ifa;

	if (rt->rt6i_flags&RTF_CACHE && rt->rt6i_idev == ifa->idev &&
	    ipv6_addr_equal(&rt->rt6i_src.addr, &ifa->addr)) {
		RT6_TRACE("deleted by saddr_del %p\n", rt);
		return -1;
	}
	return 0;
}

/* Clean up cache routes using deleted source address.*/

void rt6_ifa_del(struct inet6_ifaddr *ifa)
{
	int i;

	for (i = RT6_TABLE_MIN; i <= RT6_TABLE_MAX; i++) {
		struct rt6_table *table = rt6_get_table(i);
		if (table) {
			write_lock(&table->lock);
			fib6_clean_tree(&table->root, fib6_ifa_del, 0, ifa);
			write_unlock(&table->lock);
			rt6_put_table(table);
		}
	}
}

#endif

static int fib6_ifdown(struct rt6_info *rt, void *arg)
{
	if (((void*)rt->rt6i_dev == arg || arg == NULL) &&
	    rt != &ip6_null_entry) {
		RT6_TRACE("deleted by ifdown %p\n", rt);
		return -1;
	}
	return 0;
}

void rt6_ifdown(struct net_device *dev)
{
	int i;
	for (i = RT6_TABLE_MIN; i <= RT6_TABLE_MAX; i++) {
		struct rt6_table *table = rt6_get_table(i);
		if (table) {
			write_lock_bh(&table->lock);
			fib6_clean_tree(&table->root, fib6_ifdown, 0, dev);
			write_unlock_bh(&table->lock);
			rt6_put_table(table);
		}
	}
}

struct rt6_mtu_change_arg
{
	struct net_device *dev;
	unsigned mtu;
};

static int rt6_mtu_change_route(struct rt6_info *rt, void *p_arg)
{
	struct rt6_mtu_change_arg *arg = (struct rt6_mtu_change_arg *) p_arg;
	struct inet6_dev *idev;

	/* In IPv6 pmtu discovery is not optional,
	   so that RTAX_MTU lock cannot disable it.
	   We still use this lock to block changes
	   caused by addrconf/ndisc.
	*/

	idev = __in6_dev_get(arg->dev);
	if (idev == NULL)
		return 0;

	/* For administrative MTU increase, there is no way to discover
	   IPv6 PMTU increase, so PMTU increase should be updated here.
	   Since RFC 1981 doesn't include administrative MTU increase
	   update PMTU increase is a MUST. (i.e. jumbo frame)
	 */
	/*
	   If new MTU is less than route PMTU, this new MTU will be the
	   lowest MTU in the path, update the route PMTU to reflect PMTU
	   decreases; if new MTU is greater than route PMTU, and the
	   old MTU is the lowest MTU in the path, update the route PMTU
	   to reflect the increase. In this case if the other nodes' MTU
	   also have the lowest MTU, TOO BIG MESSAGE will be lead to
	   PMTU discouvery.
	 */
	if (rt->rt6i_dev == arg->dev &&
	    !dst_metric_locked(&rt->u.dst, RTAX_MTU) &&
            (dst_mtu(&rt->u.dst) > arg->mtu ||
             (dst_mtu(&rt->u.dst) < arg->mtu &&
	      dst_mtu(&rt->u.dst) == idev->cnf.mtu6)))
		rt->u.dst.metrics[RTAX_MTU-1] = arg->mtu;
	rt->u.dst.metrics[RTAX_ADVMSS-1] = ipv6_advmss(arg->mtu);
	return 0;
}

void rt6_mtu_change(struct net_device *dev, unsigned mtu)
{
	struct rt6_mtu_change_arg arg;
	int i;

	arg.dev = dev;
	arg.mtu = mtu;

	for (i = RT6_TABLE_MIN; i <= RT6_TABLE_MAX; i++) {
		struct rt6_table *table = rt6_get_table(i);

		if (table != NULL) {
			read_lock_bh(&table->lock);
			fib6_clean_tree(&table->root,
					rt6_mtu_change_route, 0, &arg);
			read_unlock_bh(&table->lock);
			rt6_put_table(table);
		}
	}
}

static int inet6_rtm_to_rtmsg(struct rtmsg *r, struct rtattr **rta,
			      struct in6_rtmsg *rtmsg)
{
	memset(rtmsg, 0, sizeof(*rtmsg));

	rtmsg->rtmsg_dst_len = r->rtm_dst_len;
	rtmsg->rtmsg_src_len = r->rtm_src_len;
	rtmsg->rtmsg_flags = RTF_UP;
	if (r->rtm_type == RTN_UNREACHABLE)
		rtmsg->rtmsg_flags |= RTF_REJECT;
	if (r->rtm_flags & RTM_F_PREFIX)
		rtmsg->rtmsg_flags |= RTF_PREFIX_RT;
	if (r->rtm_flags & RTM_F_DEFAULT)
		rtmsg->rtmsg_flags |= RTF_DEFAULT;
	if (r->rtm_flags & RTM_F_ADDRCONF)
		rtmsg->rtmsg_flags |= RTF_ADDRCONF;
		
	if (rta[RTA_GATEWAY-1]) {
		if (rta[RTA_GATEWAY-1]->rta_len != RTA_LENGTH(16))
			return -EINVAL;
		memcpy(&rtmsg->rtmsg_gateway, RTA_DATA(rta[RTA_GATEWAY-1]), 16);
		rtmsg->rtmsg_flags |= RTF_GATEWAY;
	}
	if (rta[RTA_DST-1]) {
		if (RTA_PAYLOAD(rta[RTA_DST-1]) < ((r->rtm_dst_len+7)>>3))
			return -EINVAL;
		memcpy(&rtmsg->rtmsg_dst, RTA_DATA(rta[RTA_DST-1]), ((r->rtm_dst_len+7)>>3));
	}
	if (rta[RTA_SRC-1]) {
		if (RTA_PAYLOAD(rta[RTA_SRC-1]) < ((r->rtm_src_len+7)>>3))
			return -EINVAL;
		memcpy(&rtmsg->rtmsg_src, RTA_DATA(rta[RTA_SRC-1]), ((r->rtm_src_len+7)>>3));
	}
	if (rta[RTA_OIF-1]) {
		if (rta[RTA_OIF-1]->rta_len != RTA_LENGTH(sizeof(int)))
			return -EINVAL;
		memcpy(&rtmsg->rtmsg_ifindex, RTA_DATA(rta[RTA_OIF-1]), sizeof(int));
	}
	if (rta[RTA_PRIORITY-1]) {
		if (rta[RTA_PRIORITY-1]->rta_len != RTA_LENGTH(4))
			return -EINVAL;
		memcpy(&rtmsg->rtmsg_metric, RTA_DATA(rta[RTA_PRIORITY-1]), 4);
	}
	return 0;
}

int inet6_rtm_delroute(struct sk_buff *skb, struct nlmsghdr* nlh, void *arg)
{
	struct rtmsg *r = NLMSG_DATA(nlh);
	struct in6_rtmsg rtmsg;

	if (inet6_rtm_to_rtmsg(r, arg, &rtmsg))
		return -EINVAL;
	return ip6_route_del(&rtmsg, nlh, arg, &NETLINK_CB(skb), r->rtm_table);
}

int inet6_rtm_newroute(struct sk_buff *skb, struct nlmsghdr* nlh, void *arg)
{
	struct rtmsg *r = NLMSG_DATA(nlh);
	struct in6_rtmsg rtmsg;

	if (inet6_rtm_to_rtmsg(r, arg, &rtmsg))
		return -EINVAL;
	return ip6_route_add(&rtmsg, nlh, arg, &NETLINK_CB(skb), r->rtm_table);
}

struct rt6_rtnl_dump_arg
{
	struct sk_buff *skb;
	struct netlink_callback *cb;
};

static int rt6_fill_node(struct sk_buff *skb, struct rt6_info *rt,
			 struct in6_addr *dst, struct in6_addr *src,
			 int iif, int type, u32 pid, u32 seq,
			 int prefix, unsigned int flags)
{
	struct rtmsg *rtm;
	struct nlmsghdr  *nlh;
	unsigned char	 *b = skb->tail;
	struct rta_cacheinfo ci;

	if (prefix) {	/* user wants prefix routes only */
		if (!(rt->rt6i_flags & RTF_PREFIX_RT)) {
			/* success since this is not a prefix route */
			return 1;
		}
	}

	nlh = NLMSG_NEW(skb, pid, seq, type, sizeof(*rtm), flags);
	rtm = NLMSG_DATA(nlh);
	rtm->rtm_family = AF_INET6;
	rtm->rtm_dst_len = rt->rt6i_dst.plen;
	rtm->rtm_src_len = rt->rt6i_src.plen;
	rtm->rtm_tos = 0;
	if (rt->rt6i_table)
		rtm->rtm_table = rt->rt6i_table->table_id;
	else
		rtm->rtm_table = RT6_TABLE_UNSPEC;
	if (rt->rt6i_flags&RTF_REJECT)
		rtm->rtm_type = RTN_UNREACHABLE;
	else if (rt->rt6i_dev && (rt->rt6i_dev->flags&IFF_LOOPBACK))
		rtm->rtm_type = RTN_LOCAL;
	else
		rtm->rtm_type = RTN_UNICAST;
	rtm->rtm_flags = 0;
	rtm->rtm_scope = RT_SCOPE_UNIVERSE;
	rtm->rtm_protocol = rt->rt6i_protocol;
	if (rt->rt6i_flags&RTF_DYNAMIC)
		rtm->rtm_protocol = RTPROT_REDIRECT;
	else if (rt->rt6i_flags & RTF_ADDRCONF)
		rtm->rtm_protocol = RTPROT_KERNEL;
	else if (rt->rt6i_flags&RTF_DEFAULT)
		rtm->rtm_protocol = RTPROT_RA;

	if (rt->rt6i_flags&RTF_CACHE)
		rtm->rtm_flags |= RTM_F_CLONED;

	if (rt->rt6i_flags&RTF_PREFIX_RT)
		rtm->rtm_flags |= RTM_F_PREFIX;

	if (rt->rt6i_flags&RTF_DEFAULT)
		rtm->rtm_flags |= RTM_F_DEFAULT;

	if (rt->rt6i_flags&RTF_ADDRCONF)
		rtm->rtm_flags |= RTM_F_ADDRCONF;
	if (dst) {
		RTA_PUT(skb, RTA_DST, 16, dst);
	        rtm->rtm_dst_len = 128;
	} else if (rtm->rtm_dst_len)
		RTA_PUT(skb, RTA_DST, 16, &rt->rt6i_dst.addr);
#ifdef CONFIG_IPV6_SUBTREES
	if (src) {
		RTA_PUT(skb, RTA_SRC, 16, src);
	        rtm->rtm_src_len = 128;
	} else if (rtm->rtm_src_len)
		RTA_PUT(skb, RTA_SRC, 16, &rt->rt6i_src.addr);
#endif
	if (iif)
		RTA_PUT(skb, RTA_IIF, 4, &iif);
	else if (dst) {
		if (!src) {
			struct in6_addr saddr_buf;
			if (ipv6_get_saddr(&rt->u.dst, dst, &saddr_buf) > 0)
				RTA_PUT(skb, RTA_PREFSRC, 16, &saddr_buf);
#ifdef CONFIG_IPV6_SUBTREES
		} else {
			RTA_PUT(skb, RTA_PREFSRC, 16, src);
#endif
		}
	}
	if (rtnetlink_put_metrics(skb, rt->u.dst.metrics) < 0)
		goto rtattr_failure;
	if (rt->u.dst.neighbour)
		RTA_PUT(skb, RTA_GATEWAY, 16, &rt->u.dst.neighbour->primary_key);
	if (rt->u.dst.dev)
		RTA_PUT(skb, RTA_OIF, sizeof(int), &rt->rt6i_dev->ifindex);
	RTA_PUT(skb, RTA_PRIORITY, 4, &rt->rt6i_metric);
	ci.rta_lastuse = jiffies_to_clock_t(jiffies - rt->u.dst.lastuse);
	if (rt->rt6i_expires)
		ci.rta_expires = jiffies_to_clock_t(rt->rt6i_expires - jiffies);
	else
		ci.rta_expires = 0;
	ci.rta_used = rt->u.dst.__use;
	ci.rta_clntref = atomic_read(&rt->u.dst.__refcnt);
	ci.rta_error = rt->u.dst.error;
	ci.rta_id = 0;
	ci.rta_ts = 0;
	ci.rta_tsage = 0;
	RTA_PUT(skb, RTA_CACHEINFO, sizeof(ci), &ci);
	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

nlmsg_failure:
rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}

static int rt6_dump_route(struct rt6_info *rt, void *p_arg)
{
	struct rt6_rtnl_dump_arg *arg = (struct rt6_rtnl_dump_arg *) p_arg;
	int prefix;

	if (arg->cb->nlh->nlmsg_len >= NLMSG_LENGTH(sizeof(struct rtmsg))) {
		struct rtmsg *rtm = NLMSG_DATA(arg->cb->nlh);
		prefix = (rtm->rtm_flags & RTM_F_PREFIX) != 0;
	} else
		prefix = 0;

	return rt6_fill_node(arg->skb, rt, NULL, NULL, 0, RTM_NEWROUTE,
		     NETLINK_CB(arg->cb->skb).pid, arg->cb->nlh->nlmsg_seq,
		     prefix, NLM_F_MULTI);
}

static int fib6_dump_node(struct fib6_walker_t *w)
{
	int res;
	struct rt6_info *rt;

	for (rt = w->leaf; rt; rt = rt->u.next) {
		res = rt6_dump_route(rt, w->args);
		if (res < 0) {
			/* Frame is full, suspend walking */
			w->leaf = rt;
			return 1;
		}
		BUG_TRAP(res!=0);
	}
	w->leaf = NULL;
	return 0;
}

static void fib6_dump_end(struct netlink_callback *cb)
{
	struct fib6_walker_t *w = (void*)cb->args[0];

	if (w) {
		cb->args[0] = 0;
		kfree(w);
	}
	if (cb->args[1]) {
		cb->done = (void*)cb->args[1];
		cb->args[1] = 0;
	}
}

static int fib6_dump_done(struct netlink_callback *cb)
{
	fib6_dump_end(cb);
	return cb->done(cb);
}

int inet6_dump_fib(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct rt6_rtnl_dump_arg arg;
	struct fib6_walker_t *w;
	struct rt6_table *table;
	int res = 0;
	int i;

	arg.skb = skb;
	arg.cb = cb;

	w = (void*)cb->args[0];
	if (w == NULL) {
		/* New dump:
		 *
		 * 1. hook callback destructor.
		 */
		cb->args[1] = (long)cb->done;
		cb->done = fib6_dump_done;

		/*
		 * 2. allocate and initialize walker.
		 */
		w = kmalloc(sizeof(*w), GFP_ATOMIC);
		if (w == NULL)
			return -ENOMEM;
		RT6_TRACE("dump<%p", w);
		memset(w, 0, sizeof(*w));
		w->func = fib6_dump_node;
		w->args = &arg;
		cb->args[0] = (long)w;
		i = RT6_TABLE_MIN;
	restart_dump_iterator:
		while (i <= RT6_TABLE_MAX) {
			table = rt6_get_table(i);
			if (table != NULL) {
				read_lock_bh(&table->lock);
				w->root = &table->root;
				res = fib6_walk(w);
				read_unlock_bh(&table->lock);
				rt6_put_table(table);
				if (res)
					break;
			}
			i++;
		}
		cb->args[2] = i;
	} else {
		i = cb->args[2];

		if (i > RT6_TABLE_MAX)
			goto out;

		w->args = &arg;
		table = rt6_get_table(i);
		if (likely(table != NULL)) {
			read_lock_bh(&table->lock);
			res = fib6_walk_continue(w);
			read_unlock_bh(&table->lock);
			rt6_put_table(table);
			if (res) {
				if (res < 0)
					fib6_walker_unlink(w);
				goto out;
			}
		}
		fib6_walker_unlink(w);
		i++;
		goto restart_dump_iterator;
	}

out:
#if RT6_DEBUG >= 3
	if (res <= 0 && skb->len == 0)
		RT6_TRACE("%p>dump end\n", w);
#endif
	res = res < 0 ? res : skb->len;
	/* res < 0 is an error. (really, impossible)
	   res == 0 means that dump is complete, but skb still can contain data.
	   res > 0 dump is not complete, but frame is full.
	 */
	/* Destroy walker, if dump of this table is complete. */
	if (res <= 0)
		fib6_dump_end(cb);
	return res;
}

int inet6_rtm_getroute(struct sk_buff *in_skb, struct nlmsghdr* nlh, void *arg)
{
	struct rtattr **rta = arg;
	int iif = 0;
	int err = -ENOBUFS;
	struct sk_buff *skb;
	struct flowi fl;
	struct rt6_info *rt;

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL)
		goto out;

	/* Reserve room for dummy headers, this skb can pass
	   through good chunk of routing engine.
	 */
	skb->mac.raw = skb->data;
	skb_reserve(skb, MAX_HEADER + sizeof(struct ipv6hdr));

	memset(&fl, 0, sizeof(fl));
	if (rta[RTA_SRC-1])
		ipv6_addr_copy(&fl.fl6_src,
			       (struct in6_addr*)RTA_DATA(rta[RTA_SRC-1]));
	if (rta[RTA_DST-1])
		ipv6_addr_copy(&fl.fl6_dst,
			       (struct in6_addr*)RTA_DATA(rta[RTA_DST-1]));

	if (rta[RTA_IIF-1])
		memcpy(&iif, RTA_DATA(rta[RTA_IIF-1]), sizeof(int));

	if (iif) {
		struct net_device *dev;
		dev = __dev_get_by_index(iif);
		if (!dev) {
			err = -ENODEV;
			goto out_free;
		}
		fl.iif = iif;
	}

	if (rta[RTA_OIF-1])
		memcpy(&fl.oif, RTA_DATA(rta[RTA_OIF-1]), sizeof(int));

	rt = (struct rt6_info*)ip6_route_output(NULL, &fl);

	skb->dst = &rt->u.dst;

	NETLINK_CB(skb).dst_pid = NETLINK_CB(in_skb).pid;
	err = rt6_fill_node(skb, rt,
			    &fl.fl6_dst, &fl.fl6_src,
			    iif,
			    RTM_NEWROUTE, NETLINK_CB(in_skb).pid,
			    nlh->nlmsg_seq, 0, 0);
	if (err < 0) {
		err = -EMSGSIZE;
		goto out_free;
	}

	err = netlink_unicast(rtnl, skb, NETLINK_CB(in_skb).pid, MSG_DONTWAIT);
	if (err > 0)
		err = 0;
out:
	return err;
out_free:
	kfree_skb(skb);
	goto out;	
}

void inet6_rt_notify(int event, struct rt6_info *rt, struct nlmsghdr *nlh,
			struct netlink_skb_parms *req)
{
	struct sk_buff *skb;
	int size = NLMSG_SPACE(sizeof(struct rtmsg)+256);
	u32 pid = current->pid;
	u32 seq = 0;

	if (req)
		pid = req->pid;
	if (nlh)
		seq = nlh->nlmsg_seq;
	
	skb = alloc_skb(size, gfp_any());
	if (!skb) {
		netlink_set_err(rtnl, 0, RTNLGRP_IPV6_ROUTE, ENOBUFS);
		return;
	}
	if (rt6_fill_node(skb, rt, NULL, NULL, 0, event, pid, seq, 0, 0) < 0) {
		kfree_skb(skb);
		netlink_set_err(rtnl, 0, RTNLGRP_IPV6_ROUTE, EINVAL);
		return;
	}
	NETLINK_CB(skb).dst_group = RTNLGRP_IPV6_ROUTE;
	netlink_broadcast(rtnl, skb, 0, RTNLGRP_IPV6_ROUTE, gfp_any());
}

/*
 *	/proc
 */

#ifdef CONFIG_PROC_FS

#define RT6_INFO_LEN (32 + 4 + 32 + 4 + 32 + 40 + 5 + 1)

struct rt6_proc_arg
{
	char *buffer;
	int offset;
	int length;
	int skip;
	int len;
};

static int rt6_info_route(struct rt6_info *rt, void *p_arg)
{
	struct rt6_proc_arg *arg = (struct rt6_proc_arg *) p_arg;

	if (arg->skip < arg->offset / RT6_INFO_LEN) {
		arg->skip++;
		return 0;
	}

	if (arg->len >= arg->length)
		return 0;

	arg->len += sprintf(arg->buffer + arg->len, 
			    "%04x%04x%04x%04x%04x%04x%04x%04x %02x ",
			    NIP6(rt->rt6i_dst.addr),
			    rt->rt6i_dst.plen);

#ifdef CONFIG_IPV6_SUBTREES
	arg->len += sprintf(arg->buffer + arg->len, 
			    "%04x%04x%04x%04x%04x%04x%04x%04x %02x ",
			    NIP6(rt->rt6i_src.addr),
			    rt->rt6i_src.plen);
#else
	arg->len += sprintf(arg->buffer + arg->len,
			    "00000000000000000000000000000000 00 ");
#endif

	if (rt->rt6i_nexthop)
		arg->len += sprintf(arg->buffer + arg->len,
				    "%04x%04x%04x%04x%04x%04x%04x%04x",
				    NIP6(*((struct in6_addr *)rt->rt6i_nexthop->primary_key)));
	else
		arg->len += sprintf(arg->buffer + arg->len,
				    "00000000000000000000000000000000");

	arg->len += sprintf(arg->buffer + arg->len,
			    " %08x %08x %08x %08x %8s\n",
			    rt->rt6i_metric, atomic_read(&rt->u.dst.__refcnt),
			    rt->u.dst.__use, rt->rt6i_flags,
			    rt->rt6i_dev ? rt->rt6i_dev->name : "");
	return 0;
}

static int rt6_proc_info(char *buffer, char **start, off_t offset, int length)
{
	struct rt6_proc_arg arg;
	int i;

	arg.buffer = buffer;
	arg.offset = offset;
	arg.length = length;
	arg.skip = 0;
	arg.len = 0;

	for (i = RT6_TABLE_MIN; i <= RT6_TABLE_MAX; i++) {
		struct rt6_table *table = rt6_get_table(i);
		if (table != NULL) {
			read_lock_bh(&table->lock);
			fib6_clean_tree(&table->root, rt6_info_route, 0, &arg);
			read_unlock_bh(&table->lock);
			rt6_put_table(table);
		}
	}

	*start = buffer;
	if (offset)
		*start += offset % RT6_INFO_LEN;

	arg.len -= offset % RT6_INFO_LEN;

	if (arg.len > length)
		arg.len = length;
	if (arg.len < 0)
		arg.len = 0;

	return arg.len;
}

static int rt6_stats_seq_show(struct seq_file *seq, void *v)
{
	seq_printf(seq, "%04x %04x %04x %04x %04x %04x %04x\n",
		      rt6_stats.fib_nodes, rt6_stats.fib_route_nodes,
		      rt6_stats.fib_rt_alloc, rt6_stats.fib_rt_entries,
		      rt6_stats.fib_rt_cache,
		      atomic_read(&ip6_dst_ops.entries),
		      rt6_stats.fib_discarded_routes);

	return 0;
}

static int rt6_stats_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, rt6_stats_seq_show, NULL);
}

static struct file_operations rt6_stats_seq_fops = {
	.owner	 = THIS_MODULE,
	.open	 = rt6_stats_seq_open,
	.read	 = seq_read,
	.llseek	 = seq_lseek,
	.release = single_release,
};
#endif	/* CONFIG_PROC_FS */

#ifdef CONFIG_SYSCTL

static int flush_delay;

static
int ipv6_sysctl_rtcache_flush(ctl_table *ctl, int write, struct file * filp,
			      void __user *buffer, size_t *lenp, loff_t *ppos)
{
	if (write) {
		proc_dointvec(ctl, write, filp, buffer, lenp, ppos);
		fib6_run_gc(flush_delay <= 0 ? ~0UL : (unsigned long)flush_delay);
		return 0;
	} else
		return -EINVAL;
}

ctl_table ipv6_route_table[] = {
        {
		.ctl_name	=	NET_IPV6_ROUTE_FLUSH,
		.procname	=	"flush",
         	.data		=	&flush_delay,
		.maxlen		=	sizeof(int),
		.mode		=	0200,
         	.proc_handler	=	&ipv6_sysctl_rtcache_flush
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_GC_THRESH,
		.procname	=	"gc_thresh",
         	.data		=	&ip6_dst_ops.gc_thresh,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_MAX_SIZE,
		.procname	=	"max_size",
         	.data		=	&ip6_rt_max_size,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_GC_MIN_INTERVAL,
		.procname	=	"gc_min_interval",
         	.data		=	&ip6_rt_gc_min_interval,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec_jiffies,
		.strategy	=	&sysctl_jiffies,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_GC_TIMEOUT,
		.procname	=	"gc_timeout",
         	.data		=	&ip6_rt_gc_timeout,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec_jiffies,
		.strategy	=	&sysctl_jiffies,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_GC_INTERVAL,
		.procname	=	"gc_interval",
         	.data		=	&ip6_rt_gc_interval,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec_jiffies,
		.strategy	=	&sysctl_jiffies,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_GC_ELASTICITY,
		.procname	=	"gc_elasticity",
         	.data		=	&ip6_rt_gc_elasticity,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec_jiffies,
		.strategy	=	&sysctl_jiffies,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_MTU_EXPIRES,
		.procname	=	"mtu_expires",
         	.data		=	&ip6_rt_mtu_expires,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec_jiffies,
		.strategy	=	&sysctl_jiffies,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_MIN_ADVMSS,
		.procname	=	"min_adv_mss",
         	.data		=	&ip6_rt_min_advmss,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec_jiffies,
		.strategy	=	&sysctl_jiffies,
	},
	{
		.ctl_name	=	NET_IPV6_ROUTE_GC_MIN_INTERVAL_MS,
		.procname	=	"gc_min_interval_ms",
         	.data		=	&ip6_rt_gc_min_interval,
		.maxlen		=	sizeof(int),
		.mode		=	0644,
         	.proc_handler	=	&proc_dointvec_ms_jiffies,
		.strategy	=	&sysctl_ms_jiffies,
	},
	{ .ctl_name = 0 }
};

#endif

void __init ip6_route_init(void)
{
	struct proc_dir_entry *p;

	ip6_dst_ops.kmem_cachep = kmem_cache_create("ip6_dst_cache",
						     sizeof(struct rt6_info),
						     0, SLAB_HWCACHE_ALIGN,
						     NULL, NULL);
	if (!ip6_dst_ops.kmem_cachep)
		panic("cannot create ip6_dst_cache");

#ifdef CONFIG_IPV6_MULTIPLE_TABLES
	rt6_tables_init();
#endif
	fib6_init();
#ifdef 	CONFIG_PROC_FS
	p = proc_net_create("ipv6_route", 0, rt6_proc_info);
	if (p)
		p->owner = THIS_MODULE;

	proc_net_fops_create("rt6_stats", S_IRUGO, &rt6_stats_seq_fops);
#endif
#ifdef CONFIG_XFRM
	xfrm6_init();
#endif
}

void ip6_route_cleanup(void)
{
#ifdef CONFIG_PROC_FS
	proc_net_remove("ipv6_route");
	proc_net_remove("rt6_stats");
#endif
#ifdef CONFIG_XFRM
	xfrm6_fini();
#endif
	rt6_ifdown(NULL);
	fib6_gc_cleanup();
#ifdef CONFIG_IPV6_MULTIPLE_TABLES
	rt6_tables_cleanup();
#endif
	kmem_cache_destroy(ip6_dst_ops.kmem_cachep);
}

