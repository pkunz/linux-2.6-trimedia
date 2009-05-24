/* 
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_BITOPS_H
#define _ASM_TRIMEDIA_BITOPS_H

#include <linux/config.h>

#ifdef __KERNEL__

#include <asm/system.h>

static inline int test_and_set_bit(int nr,unsigned long* addr)
{
	int mask, retval;
	unsigned long flags;

	addr += nr >> 5;
	mask = 1 << (nr & 0x1f);

	local_irq_save(flags);
	retval = (mask & *addr) != 0;
	*addr |= mask;
	local_irq_restore(flags);
	
	return retval;
}

static inline int 
test_and_clear_bit(int nr,unsigned long* addr)
{
	int mask, retval;
	unsigned long flags;

	addr += nr >> 5;
	mask = 1 << (nr & 0x1f);

	local_irq_save(flags);
	retval = (mask & *addr) != 0;
	*addr &= ~mask;
	local_irq_restore(flags);

	return retval;
}

static inline int 
test_and_change_bit(int nr,unsigned long* addr)
{
	int mask,retval;
	unsigned long flags;

	addr+= nr>>5;
	mask= 1<<(nr&0x1f);

	local_irq_save(flags);
	retval= (mask& *addr)!=0;
	*addr^=mask;
	local_irq_restore(flags);

	return retval;
}

#define set_bit(nr,addr)    test_and_set_bit(nr,addr)
#define clear_bit(nr,addr)  test_and_clear_bit(nr,addr)
#define change_bit(nr,addr) test_and_change_bit(nr,addr)

static inline int 
__test_and_set_bit(int nr,unsigned long* addr)
{
	int mask, retval;

	addr += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *addr) != 0;
	*addr |= mask;
	return retval;
}

static inline int 
__test_and_clear_bit(int nr,unsigned long* addr)
{
	int mask, retval;

	addr += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *addr) != 0;
	*addr &= ~mask;

	return retval;
}

static inline int 
__test_and_change_bit(int nr,unsigned long* addr)
{
	int mask,retval;

	addr+= nr>>5;
	mask= 1<<(nr&0x1f);
	retval= (mask& *addr)!=0;
	*addr^=mask;

	return retval;
}

#define __set_bit(nr,addr)  	__test_and_set_bit(nr,addr)
#define __clear_bit(nr,addr) 	__test_and_clear_bit(nr,addr)
#define __change_bit(nr,addr)   __test_and_change_bit(nr,addr)

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline int 
test_bit(int nr, const volatile unsigned long* addr)
{
	int mask;

	addr += nr >> 5;
	mask = 1 << (nr & 0x1f);

	return ((mask & *addr) != 0);
}

/*
 *FIXME: I do not know how to implement it.
 */
#define smp_mb__after_clear_bit()    	
#define smp_mb__before_clear_bit()  	

/**
 * ffz - find first zero in word.
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
static inline unsigned long 
ffz(unsigned long word)
{
	int k;

	word = ~word;
	k = 31;
	if (word & 0x0000ffff) { k -= 16; word <<= 16; }
	if (word & 0x00ff0000) { k -= 8;  word <<= 8;  }
	if (word & 0x0f000000) { k -= 4;  word <<= 4;  }
	if (word & 0x30000000) { k -= 2;  word <<= 2;  }
	if (word & 0x40000000) { k -= 1; }
        return k;
}

/**
 * find_first_zero_bit - find the first zero bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit-number of the first zero bit, not the number of the byte
 * containing a bit.
 */

#define find_first_zero_bit(addr, size) \
	find_next_zero_bit((addr), (size), 0)

/**
 * find_next_zero_bit - find the first zero bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The maximum size to search
 */
static inline int 
find_next_zero_bit (const void * addr, int size, int offset)
{
	unsigned long *p = ((unsigned long *) addr) + (offset >> 5);
	unsigned long result = offset & ~31UL;
	unsigned long tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset &= 31UL;
	if (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (32-offset);
		if (size < 32)
			goto found_first;
		if (~tmp)
			goto found_middle;
		size -= 32;
		result += 32;
	}
	while (size & ~31UL) {
		if (~(tmp = *(p++)))
			goto found_middle;
		result += 32;
		size -= 32;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp |= ~0UL >> size;
found_middle:
	return result + ffz(tmp);
}

/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
static inline unsigned long 
__ffs(unsigned long word)
{
	int k;

	k = 31;
	if (word & 0x0000ffff) { k -= 16; word <<= 16; }
	if (word & 0x00ff0000) { k -= 8;  word <<= 8;  }
	if (word & 0x0f000000) { k -= 4;  word <<= 4;  }
	if (word & 0x30000000) { k -= 2;  word <<= 2;  }
	if (word & 0x40000000) { k -= 1; }
        return k;
}

/*
 * fls: find last bit set.
 */
#define fls(x) generic_fls(x)

#endif 

#ifdef __KERNEL__

/*
 * Every architecture must define this function. It's the fastest
 * way of searching a 140-bit bitmap where the first 100 bits are
 * unlikely to be set. It's guaranteed that at least one of the 140
 * bits is cleared.
 */
static inline int sched_find_first_bit(unsigned long *b)
{
	if (unlikely(b[0]))
		return __ffs(b[0]);
	if (unlikely(b[1]))
		return __ffs(b[1]) + 32;
	if (unlikely(b[2]))
		return __ffs(b[2]) + 64;
	if (b[3])
		return __ffs(b[3]) + 96;
	return __ffs(b[4]) + 128;
}

/**
 * find_next_bit - find the first set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The maximum size to search
 */
static inline unsigned long 
find_next_bit(const unsigned long *addr,
	unsigned long size, unsigned long offset)
{
	unsigned int *p = ((unsigned int *) addr) + (offset >> 5);
	unsigned int result = offset & ~31UL;
	unsigned int tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset &= 31UL;
	if (offset) {
		tmp = *p++;
		tmp &= ~0UL << offset;
		if (size < 32)
			goto found_first;
		if (tmp)
			goto found_middle;
		size -= 32;
		result += 32;
	}
	while (size >= 32) {
		if ((tmp = *p++) != 0)
			goto found_middle;
		result += 32;
		size -= 32;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp &= ~0UL >> (32 - size);
	if (tmp == 0UL)        /* Are any bits set? */
		return result + size; /* Nope. */
found_middle:
	return result + __ffs(tmp);
}

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit-number of the first set bit, not the number of the byte
 * containing a bit.
 */
#define find_first_bit(addr, size) \
	find_next_bit((addr), (size), 0)

/**
 * ffs - find first bit set
 * @x: the word to search
 *
 * This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */
#define ffs(x)	generic_ffs(x)

/**
 * hweightN - returns the hamming weight of a N-bit word
 * @x: the word to weigh
 *
 * The Hamming Weight of a number is the total number of bits set in it.
 */

#define hweight32(x)	generic_hweight32(x)
#define hweight16(x)	generic_hweight16(x)
#define hweight8(x)	generic_hweight8(x)

#endif /* __KERNEL__ */

#ifdef __KERNEL__

static inline int ext2_set_bit(int nr, volatile void * addr)
{
	int		mask, retval;
	unsigned long	flags;
	volatile unsigned char	*ADDR = (unsigned char *) addr;

	ADDR += nr >> 3;
	mask = 1 << (nr & 0x07);
	local_irq_save(flags);
	retval = (mask & *ADDR) != 0;
	*ADDR |= mask;
	local_irq_restore(flags);
	return retval;
}

static inline int ext2_clear_bit(int nr, volatile void * addr)
{
	int		mask, retval;
	unsigned long	flags;
	volatile unsigned char	*ADDR = (unsigned char *) addr;

	ADDR += nr >> 3;
	mask = 1 << (nr & 0x07);
	local_irq_save(flags);
	retval = (mask & *ADDR) != 0;
	*ADDR &= ~mask;
	local_irq_restore(flags);
	return retval;
}

static inline int ext2_test_bit(int nr, const volatile void * addr)
{
	int			mask;
	const volatile unsigned char	*ADDR = (const unsigned char *) addr;

	ADDR += nr >> 3;
	mask = 1 << (nr & 0x07);
	return ((mask & *ADDR) != 0);
}

#define ext2_find_next_zero_bit(addr, size, off)      	\
	find_next_zero_bit(addr, size, off)

#define ext2_find_first_zero_bit(addr, size) 		\
	ext2_find_next_zero_bit((addr), (size), 0)

#define ext2_set_bit_atomic(lock, nr, addr)		\
	({						\
		int ret;				\
		spin_lock(lock);			\
		ret = ext2_set_bit((nr), (addr));	\
		spin_unlock(lock);			\
		ret;					\
	})

#define ext2_clear_bit_atomic(lock, nr, addr)		\
	({						\
		int ret;				\
		spin_lock(lock);			\
		ret = ext2_clear_bit((nr), (addr));	\
		spin_unlock(lock);			\
		ret;					\
	})

/*
 * FIXME: Implement the following macroes carefully.
 * */
#if 0
/* Bitmap functions for the minix filesystem.  */
#define minix_test_and_set_bit(nr,addr)		__test_and_set_bit(nr,addr)
#define minix_set_bit(nr,addr)			__set_bit(nr,addr)
#define minix_test_and_clear_bit(nr,addr)	__test_and_clear_bit(nr,addr)
#define minix_test_bit(nr,addr) test_bit(nr,addr)
#define minix_find_first_zero_bit(addr,size)	find_first_zero_bit(addr,size)
#endif 

#endif /* __KERNEL__ */

#endif /* _ASM_TRIMEDIA_BITOPS_H */

