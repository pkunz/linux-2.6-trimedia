/*
 *	Linux INET6 implementation 
 *
 *	Authors:
 *	Pedro Roque		<roque@di.fc.ul.pt>	
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#ifndef _LINUX_IPV6_ROUTE_H
#define _LINUX_IPV6_ROUTE_H

#define RTF_DEFAULT	0x00010000	/* default - learned via ND	*/
#define RTF_ALLONLINK	0x00020000	/* (deprecated and will be removed)
					   fallback, no routers on link */
#define RTF_ADDRCONF	0x00040000	/* addrconf route - RA		*/
#define RTF_PREFIX_RT	0x00080000	/* A prefix only route - RA	*/

#define RTF_PREF_HIGH	0x08000000
#define RTF_PREF_LOW	0x18000000
#define RTF_PREF_INVAL	0x10000000
#define RTF_PREF_MASK	0x18000000
#define RTF_PREF(pref)	(((pref)&3)<<27)

#define RTF_NONEXTHOP	0x00200000	/* route with no nexthop	*/
#define RTF_EXPIRES	0x00400000

#define RTF_CACHE	0x01000000	/* cache entry			*/
#define RTF_FLOW	0x02000000	/* flow significant route	*/
#define RTF_POLICY	0x04000000	/* policy route			*/

#define RTF_LOCAL	0x80000000

#ifdef __KERNEL__
#define IPV6_UNSHIFT_PREF(flag)		(((flag)&RTF_PREF_MASK)>>27)
#define IPV6_SIGNEDPREF(pref)		((((pref)+2)&3)-2)
#endif

struct in6_rtmsg {
	struct in6_addr		rtmsg_dst;
	struct in6_addr		rtmsg_src;
	struct in6_addr		rtmsg_gateway;
	__u32			rtmsg_type;
	__u16			rtmsg_dst_len;
	__u16			rtmsg_src_len;
	__u32			rtmsg_metric;
	unsigned long		rtmsg_info;
        __u32			rtmsg_flags;
	int			rtmsg_ifindex;
};

#define RTMSG_NEWDEVICE		0x11
#define RTMSG_DELDEVICE		0x12
#define RTMSG_NEWROUTE		0x21
#define RTMSG_DELROUTE		0x22

#endif
