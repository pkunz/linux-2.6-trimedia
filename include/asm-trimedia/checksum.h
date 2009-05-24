/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_CHECKSUM_H
#define _ASM_TRIMEDIA_CHECKSUM_H

#include <ops/custom_ops.h>

/*
 * computes the checksum of a memory block at buff, length len,
 * and adds in "sum" (32-bit)
 *
 * returns a 32-bit number suitable for feeding into itself
 * or csum_tcpudp_magic
 *
 * this function must be called with even lengths, except
 * for the last fragment, which may be odd
 *
 * it's best to have buff aligned on a 32-bit boundary
 */
extern unsigned int csum_partial(const unsigned char * buff, int len, unsigned int sum);

/*
 * the same as csum_partial, but copies from src while it
 * checksums
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */

extern unsigned int csum_partial_copy(const unsigned char *src, char *dst, int len, int sum);

#define csum_partial_copy_nocheck(src, dst, len, sum)	\
	csum_partial_copy((src), (dst), (len), (sum))


/*
 * the same as csum_partial_copy, but copies from user space.
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */

extern unsigned int csum_partial_copy_from_user(const char *src, char *dst,
						int len, int sum, int *csum_err);

extern unsigned short ip_fast_csum(unsigned char *iph, unsigned int ihl);


/*
 *	Fold a partial checksum
 */
static inline unsigned short
csum_fold (unsigned int sum)
{
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);
	return ~sum;
}

/*
 * computes the checksum of the TCP/UDP pseudo-header
 * returns a 16-bit checksum, already complemented
 */

static inline unsigned int csum_tcpudp_nofold(
		unsigned long saddr, unsigned long daddr, unsigned short len,
	       	unsigned short proto, unsigned int sum)
{
#if defined(__LITTLE_ENDIAN)
	unsigned long len_proto = (ntohs(len)<<16) + (proto<<8);
#else
	unsigned long len_proto = (proto<<16) + len;
#endif 

	sum = sum + daddr + CARRY(sum,daddr);
	sum = sum + saddr + CARRY(sum,saddr);
	sum = sum + len_proto + CARRY(sum,len_proto);

	return sum;
}

/*
 * computes the checksum of the TCP/UDP pseudo-header
 * returns a 16-bit checksum, already complemented
 */
static inline unsigned short int csum_tcpudp_magic(unsigned long saddr,
						   unsigned long daddr,
						   unsigned short len,
						   unsigned short proto,
						   unsigned int sum)
{
	return csum_fold(csum_tcpudp_nofold(saddr,daddr,len,proto,sum));
}

/*
 * this routine is used for miscellaneous IP-like checksums, mainly
 * in icmp.c
 */

extern unsigned short ip_compute_csum(const unsigned char * buff, int len);

#endif /* _ASM_TRIMEDIA_CHECKSUM_H */

