/*
 * TCP extension for IPv6 Connection Tracking
 * Linux INET6 implementation
 *
 * Copyright (C)2003 USAGI/WIDE Project
 *
 * Authors:
 *	Yasuyuki Kozakai	<yasuyuki.kozakai@toshiba.co.jp>
 *
 * Based on: net/ipv4/netfilter/ip_conntrack_proto_tcp.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/netfilter.h>
#include <linux/module.h>
#include <linux/in.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <linux/string.h>

#include <net/tcp.h>

#include <linux/netfilter_ipv6/ip6_conntrack.h>
#include <linux/netfilter_ipv6/ip6_conntrack_protocol.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

/* Protects conntrack->proto.tcp */
static DEFINE_RWLOCK(tcp_lock);

/* FIXME: Examine ipfilter's timeouts and conntrack transitions more
   closely.  They're more complex. --RR */

/* Actually, I believe that neither ipmasq (where this code is stolen
   from) nor ipfilter do it exactly right.  A new conntrack machine taking
   into account packet loss (which creates uncertainty as to exactly
   the conntrack of the connection) is required.  RSN.  --RR */

static const char *tcp_conntrack_names[] = {
	"NONE",
	"ESTABLISHED",
	"SYN_SENT",
	"SYN_RECV",
	"FIN_WAIT",
	"TIME_WAIT",
	"CLOSE",
	"CLOSE_WAIT",
	"LAST_ACK",
	"LISTEN"
};

#define SECS *HZ
#define MINS * 60 SECS
#define HOURS * 60 MINS
#define DAYS * 24 HOURS


static unsigned long tcp_timeouts[]
= { 30 MINS, 	/*	TCP_CONNTRACK_NONE,	*/
    5 DAYS,	/*	TCP_CONNTRACK_ESTABLISHED,	*/
    2 MINS,	/*	TCP_CONNTRACK_SYN_SENT,	*/
    60 SECS,	/*	TCP_CONNTRACK_SYN_RECV,	*/
    2 MINS,	/*	TCP_CONNTRACK_FIN_WAIT,	*/
    2 MINS,	/*	TCP_CONNTRACK_TIME_WAIT,	*/
    10 SECS,	/*	TCP_CONNTRACK_CLOSE,	*/
    60 SECS,	/*	TCP_CONNTRACK_CLOSE_WAIT,	*/
    30 SECS,	/*	TCP_CONNTRACK_LAST_ACK,	*/
    2 MINS,	/*	TCP_CONNTRACK_LISTEN,	*/
};

#define sNO TCP_CONNTRACK_NONE
#define sES TCP_CONNTRACK_ESTABLISHED
#define sSS TCP_CONNTRACK_SYN_SENT
#define sSR TCP_CONNTRACK_SYN_RECV
#define sFW TCP_CONNTRACK_FIN_WAIT
#define sTW TCP_CONNTRACK_TIME_WAIT
#define sCL TCP_CONNTRACK_CLOSE
#define sCW TCP_CONNTRACK_CLOSE_WAIT
#define sLA TCP_CONNTRACK_LAST_ACK
#define sLI TCP_CONNTRACK_LISTEN
#define sIV TCP_CONNTRACK_MAX

static enum tcp_conntrack tcp_conntracks[2][5][TCP_CONNTRACK_MAX] = {
	{
/*	ORIGINAL */
/* 	  sNO, sES, sSS, sSR, sFW, sTW, sCL, sCW, sLA, sLI 	*/
/*syn*/	{sSS, sES, sSS, sSR, sSS, sSS, sSS, sSS, sSS, sLI },
/*fin*/	{sTW, sFW, sSS, sTW, sFW, sTW, sCL, sTW, sLA, sLI },
/*ack*/	{sES, sES, sSS, sES, sFW, sTW, sCL, sCW, sLA, sES },
/*rst*/ {sCL, sCL, sSS, sCL, sCL, sTW, sCL, sCL, sCL, sCL },
/*none*/{sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV }
	},
	{
/*	REPLY */
/* 	  sNO, sES, sSS, sSR, sFW, sTW, sCL, sCW, sLA, sLI 	*/
/*syn*/	{sSR, sES, sSR, sSR, sSR, sSR, sSR, sSR, sSR, sSR },
/*fin*/	{sCL, sCW, sSS, sTW, sTW, sTW, sCL, sCW, sLA, sLI },
/*ack*/	{sCL, sES, sSS, sSR, sFW, sTW, sCL, sCW, sCL, sLI },
/*rst*/ {sCL, sCL, sCL, sCL, sCL, sCL, sCL, sCL, sLA, sLI },
/*none*/{sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV }
	}
};

static int tcp_pkt_to_tuple(const struct sk_buff *skb,
			     unsigned int dataoff,
			     struct ip6_conntrack_tuple *tuple)
{
	struct tcphdr hdr;

	/* Actually only need first 8 bytes. */
	if (skb_copy_bits(skb, dataoff, &hdr, 8) != 0)
		return 0;

	tuple->src.u.tcp.port = hdr.source;
	tuple->dst.u.tcp.port = hdr.dest;

	return 1;
}

static int tcp_invert_tuple(struct ip6_conntrack_tuple *tuple,
			    const struct ip6_conntrack_tuple *orig)
{
	tuple->src.u.tcp.port = orig->dst.u.tcp.port;
	tuple->dst.u.tcp.port = orig->src.u.tcp.port;
	return 1;
}

/* Print out the per-protocol part of the tuple. */
static unsigned int tcp_print_tuple(char *buffer,
				    const struct ip6_conntrack_tuple *tuple)
{
	return sprintf(buffer, "sport=%hu dport=%hu ",
		       ntohs(tuple->src.u.tcp.port),
		       ntohs(tuple->dst.u.tcp.port));
}

/* Print out the private part of the conntrack. */
static unsigned int tcp_print_conntrack(char *buffer,
					const struct ip6_conntrack *conntrack)
{
	enum tcp_conntrack state;

	read_lock_bh(&tcp_lock);
	state = conntrack->proto.tcp.state;
	read_unlock_bh(&tcp_lock);

	return sprintf(buffer, "%s ", tcp_conntrack_names[state]);
}

static unsigned int get_conntrack_index(const struct tcphdr *tcph)
{
	if (tcph->rst) return 3;
	else if (tcph->syn) return 0;
	else if (tcph->fin) return 1;
	else if (tcph->ack) return 2;
	else return 4;
}

/* Returns verdict for packet, or -1 for invalid. */
static int tcp_packet(struct ip6_conntrack *conntrack,
		      const struct sk_buff *skb,
		      unsigned int dataoff,
		      enum ip6_conntrack_info ctinfo)
{
	enum tcp_conntrack newconntrack, oldtcpstate;
	struct tcphdr tcph;

	if (skb_copy_bits(skb, dataoff, &tcph, sizeof(tcph)) != 0)
		return -1;

	write_lock_bh(&tcp_lock);
	oldtcpstate = conntrack->proto.tcp.state;
	newconntrack
		= tcp_conntracks
		[CTINFO2DIR(ctinfo)]
		[get_conntrack_index(&tcph)][oldtcpstate];

	/* Invalid */
	if (newconntrack == TCP_CONNTRACK_MAX) {
		DEBUGP("ip6_conntrack_tcp: Invalid dir=%i index=%u conntrack=%u\n",
		       CTINFO2DIR(ctinfo), get_conntrack_index(&tcph),
		       conntrack->proto.tcp.state);
		write_unlock_bh(&tcp_lock);
		return -1;
	}

	conntrack->proto.tcp.state = newconntrack;

	/* Poor man's window tracking: record SYN/ACK for handshake check */
	if (oldtcpstate == TCP_CONNTRACK_SYN_SENT
	    && CTINFO2DIR(ctinfo) == IP6_CT_DIR_REPLY
	    && tcph.syn && tcph.ack)
		conntrack->proto.tcp.handshake_ack
			= htonl(ntohl(tcph.seq) + 1);

	/* If only reply is a RST, we can consider ourselves not to
	   have an established connection: this is a fairly common
	   problem case, so we can delete the conntrack
	   immediately.  --RR */
	if (!test_bit(IP6S_SEEN_REPLY_BIT, &conntrack->status) && tcph.rst) {
		write_unlock_bh(&tcp_lock);
		if (del_timer(&conntrack->timeout))
			conntrack->timeout.function((unsigned long)conntrack);
	} else {
		/* Set ASSURED if we see see valid ack in ESTABLISHED after SYN_RECV */
		if (oldtcpstate == TCP_CONNTRACK_SYN_RECV
		    && CTINFO2DIR(ctinfo) == IP6_CT_DIR_ORIGINAL
		    && tcph.ack && !tcph.syn
		    && tcph.ack_seq == conntrack->proto.tcp.handshake_ack)
			set_bit(IP6S_ASSURED_BIT, &conntrack->status);

		write_unlock_bh(&tcp_lock);
		ip6_ct_refresh(conntrack, tcp_timeouts[newconntrack]);
	}

	return NF_ACCEPT;
}

/* Called when a new connection for this protocol found. */
static int tcp_new(struct ip6_conntrack *conntrack, const struct sk_buff *skb,
		   unsigned int dataoff)
{
	enum tcp_conntrack newconntrack;
	struct tcphdr tcph;

	if (skb_copy_bits(skb, dataoff, &tcph, sizeof(tcph)) != 0)
		return -1;

	/* Don't need lock here: this conntrack not in circulation yet */
	newconntrack
		= tcp_conntracks[0][get_conntrack_index(&tcph)]
		[TCP_CONNTRACK_NONE];

	/* Invalid: delete conntrack */
	if (newconntrack == TCP_CONNTRACK_MAX) {
		DEBUGP("ip6_conntrack_tcp: invalid new deleting.\n");
		return 0;
	}

	conntrack->proto.tcp.state = newconntrack;
	return 1;
}

static int tcp_exp_matches_pkt(struct ip6_conntrack_expect *exp,
			       const struct sk_buff *skb,
			       unsigned int dataoff)
{
	struct tcphdr tcph;
	unsigned int datalen;

	if (skb_copy_bits(skb, dataoff, &tcph, sizeof(tcph)) != 0)
		return 0;
	datalen = skb->len - dataoff;

	return between(exp->seq, ntohl(tcph.seq), ntohl(tcph.seq) + datalen);
}

struct ip6_conntrack_protocol ip6_conntrack_protocol_tcp
= { { NULL, NULL }, IPPROTO_TCP, "tcp",
    tcp_pkt_to_tuple, tcp_invert_tuple, tcp_print_tuple, tcp_print_conntrack,
    tcp_packet, tcp_new, NULL, tcp_exp_matches_pkt, NULL };
