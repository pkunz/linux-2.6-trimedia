/*
 * xfrm6_output.c - Common IPsec encapsulation code for IPv6.
 * Copyright (C) 2002 USAGI/WIDE Project
 * Copyright (c) 2004 Herbert Xu <herbert@gondor.apana.org.au>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/icmpv6.h>
#include <net/dsfield.h>
#include <net/inet_ecn.h>
#include <net/ipv6.h>
#include <net/xfrm.h>
#ifdef CONFIG_IPV6_MIP6
#include <net/mip6.h>
#endif

#ifdef CONFIG_XFRM_ENHANCEMENT
static int xfrm6_place_find(struct sk_buff *skb, u8 **nexthdr, u8 proto)
{
	switch (proto) {
	case IPPROTO_ESP:
	case IPPROTO_AH:
	case IPPROTO_COMP:
	default: /* XXX */
		return ip6_find_1stfragopt(skb, nexthdr);
#ifdef CONFIG_IPV6_MIP6
	case IPPROTO_ROUTING:
		return mip6_rthdr_place_find(skb, nexthdr);
	case IPPROTO_DSTOPTS:
		return mip6_destopt_place_find(skb, nexthdr);
#endif
	}
}
#endif

/* Add encapsulation header.
 *
 * In transport mode, the IP header and mutable extension headers will be moved
 * forward to make space for the encapsulation header.
 *
 * In tunnel mode, the top IP header will be constructed per RFC 2401.
 * The following fields in it shall be filled in by x->type->output:
 *	payload_len
 *
 * On exit, skb->h will be set to the start of the encapsulation header to be
 * filled in by x->type->output and skb->nh will be set to the nextheader field
 * of the extension header directly preceding the encapsulation header, or in
 * its absence, that of the top IP header.  The value of skb->data will always
 * point to the top IP header.
 */
static void xfrm6_encap(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct xfrm_state *x = dst->xfrm;
	struct ipv6hdr *iph, *top_iph;
	int dsfield;

	skb_push(skb, x->props.header_len);
	iph = skb->nh.ipv6h;

#ifdef CONFIG_XFRM_ENHANCEMENT
	if (x->props.mode != XFRM_MODE_TUNNEL) {
#else
	if (!x->props.mode) {
#endif
		u8 *prevhdr;
		int hdr_len;

#ifdef CONFIG_XFRM_ENHANCEMENT
		hdr_len = xfrm6_place_find(skb, &prevhdr, x->id.proto);
#else
		hdr_len = ip6_find_1stfragopt(skb, &prevhdr);
#endif
		skb->nh.raw = prevhdr - x->props.header_len;
		skb->h.raw = skb->data + hdr_len;
		memmove(skb->data, iph, hdr_len);
		return;
	}

	skb->nh.raw = skb->data;
	top_iph = skb->nh.ipv6h;
	skb->nh.raw = &top_iph->nexthdr;
	skb->h.ipv6h = top_iph + 1;

	top_iph->version = 6;
	top_iph->priority = iph->priority;
	top_iph->flow_lbl[0] = iph->flow_lbl[0];
	top_iph->flow_lbl[1] = iph->flow_lbl[1];
	top_iph->flow_lbl[2] = iph->flow_lbl[2];
	dsfield = ipv6_get_dsfield(top_iph);
	dsfield = INET_ECN_encapsulate(dsfield, dsfield);
	if (x->props.flags & XFRM_STATE_NOECN)
		dsfield &= ~INET_ECN_MASK;
	ipv6_change_dsfield(top_iph, 0, dsfield);
	top_iph->nexthdr = IPPROTO_IPV6; 
	top_iph->hop_limit = dst_metric(dst->child, RTAX_HOPLIMIT);
	ipv6_addr_copy(&top_iph->saddr, (struct in6_addr *)&x->props.saddr);
	ipv6_addr_copy(&top_iph->daddr, (struct in6_addr *)&x->id.daddr);
}

static int xfrm6_tunnel_check_size(struct sk_buff *skb)
{
	int mtu, ret = 0;
	struct dst_entry *dst = skb->dst;

	mtu = dst_mtu(dst);
	if (mtu < IPV6_MIN_MTU)
		mtu = IPV6_MIN_MTU;

	if (skb->len > mtu) {
		skb->dev = dst->dev;
		icmpv6_send(skb, ICMPV6_PKT_TOOBIG, 0, mtu, skb->dev);
		ret = -EMSGSIZE;
	}

	return ret;
}

int xfrm6_output(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct xfrm_state *x = dst->xfrm;
	int err;
	
	if (skb->ip_summed == CHECKSUM_HW) {
		err = skb_checksum_help(skb, 0);
		if (err)
			goto error_nolock;
	}

#ifdef CONFIG_XFRM_ENHANCEMENT
	if (x->props.mode == XFRM_MODE_TUNNEL) {
#else
	if (x->props.mode) {
#endif
		err = xfrm6_tunnel_check_size(skb);
		if (err)
			goto error_nolock;
	}

	spin_lock_bh(&x->lock);
	err = xfrm_state_check(x, skb);
	if (err)
		goto error;

	xfrm6_encap(skb);

	err = x->type->output(x, skb);
	if (err)
		goto error;

	x->curlft.bytes += skb->len;
	x->curlft.packets++;
	x->curlft.use_time = (unsigned long) xtime.tv_sec;

	spin_unlock_bh(&x->lock);

	skb->nh.raw = skb->data;
	
	if (!(skb->dst = dst_pop(dst))) {
		err = -EHOSTUNREACH;
		goto error_nolock;
	}
	err = NET_XMIT_BYPASS;

out_exit:
	return err;
error:
	spin_unlock_bh(&x->lock);
error_nolock:
	kfree_skb(skb);
	goto out_exit;
}
