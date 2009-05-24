#include <linux/config.h>
#include <linux/init.h>

#ifdef CONFIG_NETFILTER

#include <linux/kernel.h>
#include <linux/ipv6.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <net/dst.h>
#include <net/ipv6.h>
#include <net/ip6_route.h>

int ip6_route_me_harder(struct sk_buff *skb)
{
	struct ipv6hdr *iph = skb->nh.ipv6h;
	struct dst_entry *dst;
#ifndef __TCS__
	struct flowi fl = {
		.oif = skb->sk ? skb->sk->sk_bound_dev_if : 0,
		.iif = ((struct inet6_skb_parm *)skb->cb)->iif,
		.nl_u =
		{ .ip6_u =
		  { .daddr = iph->daddr,
		    .saddr = iph->saddr,
		    .flowlabel = (* (__u32 *) iph)&IPV6_FLOWINFO_MASK, } },
		.proto = iph->nexthdr,
	};
#else
	struct flowi fl;
	memset(&fl,0,sizeof(fl));
	fl.oif=skb->sk ? skb->sk->sk_bound_dev_if : 0;
	fl.iif=((struct inet6_skb_parm *)skb->cb)->iif;
	fl.nl_u.ip6_u.daddr=iph->daddr;
	fl.nl_u.ip6_u.saddr=iph->saddr;
	fl.nl_u.ip6_u.flowlabel=(* (__u32 *) iph)&IPV6_FLOWINFO_MASK;
	fl.nl_u.proto=iph->nexthdr;
#endif 

	dst = ip6_route_output(skb->sk, &fl);

	if (dst->error) {
#ifdef CONFIG_IPV6_STATISTICS
		struct inet6_dev *idev = ((struct rt6_info *)dst)->rt6i_idev;
		IP6_INC_STATS(idev, IPSTATS_MIB_OUTNOROUTES);
#else
		IP6_INC_STATS(IPSTATS_MIB_OUTNOROUTES);
#endif
		LIMIT_NETDEBUG(KERN_DEBUG "ip6_route_me_harder: No more route.\n");
		dst_release(dst);
		return -EINVAL;
	}

	/* Drop old route. */
	dst_release(skb->dst);

	skb->dst = dst;
	return 0;
}
EXPORT_SYMBOL(ip6_route_me_harder);
 
/* This does not belong here, but locally generated errors need it if connection
   tracking in use: without this, connection may not be in hash table, and hence
   manufactured ICMPv6 or RST packets will not be associated with it. */
void (*ip6_ct_attach)(struct sk_buff *, struct sk_buff *);
EXPORT_SYMBOL(ip6_ct_attach);

void nf_ct6_attach(struct sk_buff *new, struct sk_buff *skb)
{
	void (*attach)(struct sk_buff *, struct sk_buff *);

	if (skb->nfct && (attach = ip6_ct_attach) != NULL) {
		mb(); /* Just to be sure: must be read before executing this */
		attach(new, skb);
	}
}
EXPORT_SYMBOL(nf_ct6_attach);

/*
 * Extra routing may needed on local out, as the QUEUE target never
 * returns control to the table.
 */

struct ip6_rt_info {
	struct in6_addr daddr;
	struct in6_addr saddr;
};

static void save(const struct sk_buff *skb, struct nf_info *info)
{
	struct ip6_rt_info *rt_info = nf_info_reroute(info);

	if (info->hook == NF_IP6_LOCAL_OUT) {
		struct ipv6hdr *iph = skb->nh.ipv6h;

		rt_info->daddr = iph->daddr;
		rt_info->saddr = iph->saddr;
	}
}

static int reroute(struct sk_buff **pskb, const struct nf_info *info)
{
	struct ip6_rt_info *rt_info = nf_info_reroute(info);

	if (info->hook == NF_IP6_LOCAL_OUT) {
		struct ipv6hdr *iph = (*pskb)->nh.ipv6h;
		if (!ipv6_addr_equal(&iph->daddr, &rt_info->daddr) ||
		    !ipv6_addr_equal(&iph->saddr, &rt_info->saddr))
			return ip6_route_me_harder(*pskb);
	}
	return 0;
}

static struct nf_queue_rerouter ip6_reroute = {
	.rer_size	= sizeof(struct ip6_rt_info),
	.save 		= &save,
	.reroute	= &reroute,
};

int __init ipv6_netfilter_init(void)
{
	return nf_register_queue_rerouter(PF_INET6, &ip6_reroute);
}

void ipv6_netfilter_fini(void)
{
	nf_unregister_queue_rerouter(PF_INET6);
}

#else /* CONFIG_NETFILTER */
int __init ipv6_netfilter_init(void)
{
	return 0;
}

void ipv6_netfilter_fini(void)
{
}
#endif /* CONFIG_NETFILTER */
