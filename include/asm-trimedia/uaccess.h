/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_UACCESS_H__
#define _ASM_TRIMEDIA_UACCESS_H__

/*
 * User space memory access functions
 */

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/string.h>

#include <asm/segment.h>
#include <asm/bug.h>
#ifdef CONFIG_PCI
#include <asm/pci.h>
#endif 

#define VERIFY_READ	0
#define VERIFY_WRITE	1

extern int __Rt_mbase;
extern unsigned long memory_start;
extern unsigned long memory_end;

static inline int in_memory_hole(const void* addr, unsigned long size)
{
	unsigned long hole_end = (unsigned long)&__Rt_mbase;

	return ( (((unsigned long)addr) <= hole_end) || 
			((((unsigned long)addr) + size) <= hole_end) );
}

static inline int in_kernel_text(const void* addr, unsigned long size)
{
	unsigned long hole_end = (unsigned long)&__Rt_mbase;

	return (((unsigned long)addr) > hole_end) && 
		(((unsigned long)addr) <= memory_start);
}

static inline int in_regular_mem(const void* addr, unsigned long size)
{
	return (((unsigned long)addr) > memory_start) 
		&& ((((unsigned long)addr)+size) < memory_end) 
		&& ((((unsigned long)addr)+size) > ((unsigned long)addr));
}

#ifdef CONFIG_PCI
static inline int in_pci_mem(const void* addr, unsigned long size)
{
	return is_pci_mem(addr) && 
		is_pci_mem((void*)(((unsigned long)addr)+size));
}

static inline int in_pci_io(const void* addr, unsigned long size)
{
	return is_pci_ioport((unsigned long)addr) && 
		is_pci_ioport((unsigned long)addr+size);
}
#else
static inline int in_pci_mem(const void* addr, unsigned long size)
{
	return 0;
}

static inline int in_pci_io(const void* addr, unsigned long size)
{
	return 0;
}
#endif 

static inline int in_mmio(const void* addr, unsigned long size)
{
	/*FIXME: in MMIO, always return 0 for userspace*/
 	return 0;
}

static inline int access_ok(int type, const void *addr, unsigned long size)
{
	return (in_kernel_text(addr, size))
		|| in_regular_mem(addr, size) 
		|| in_pci_mem(addr, size) 
		|| in_pci_io(addr, size)
		|| in_mmio(addr, size);
}

/*
 * The exception table consists of pairs of addresses: the first is the
 * address of an instruction that is allowed to fault, and the second is
 * the address at which the program should continue.  No registers are
 * modified, so it is entirely up to the continuation code to figure out
 * what to do.
 *
 * All the routines below use bits of fixup code that are out of line
 * with the main instruction path.  This means when everything is well,
 * we don't even have to jump over them.  Further, they do not intrude
 * on our cache or tlb entries.
 */

struct exception_table_entry
{
	unsigned long insn, fixup;
};

/* Returns 0 if exception not found and fixup otherwise.  */
extern unsigned long search_exception_table (unsigned long);

/*
 * These are the main single-value transfer routines.  They automatically
 * use the right size if we just have the right pointer type.
 */

#define __get_user(var, ptr)						      \
  ({									      \
	  int __gu_err = 0;						      \
	  typeof(*(ptr)) __gu_val = 0;					      \
	  switch (sizeof (*(ptr))) {					      \
	  case 1:							      \
	  case 2:							      \
	  case 4:							      \
		  __gu_val = *(ptr);					      \
		  break;						      \
	  case 8:							      \
		  memcpy(&__gu_val, ptr, sizeof(__gu_val));		      \
		  break;						      \
	  default:							      \
		  __gu_val = 0;						      \
		  __gu_err = __get_user_bad ();				      \
		  break;						      \
	  }								      \
	  (var) = __gu_val;						      \
	  __gu_err;							      \
  })
#ifndef CONFIG_FORCE_CHECK
#define __get_user_bad()	(-EFAULT)
#else
#define __get_user_bad()	({ BUG(); (-EFAULT); })
#endif 

#define __put_user(var, ptr)						      \
  ({									      \
	  int __pu_err = 0;						      \
	  switch (sizeof (*(ptr))) {					      \
	  case 1:							      \
	  case 2:							      \
	  case 4:							      \
		  *(ptr) = (var);					      \
		  break;						      \
	  case 8: { 							      \
		  typeof(*(ptr)) __pu_val = var;			      \
		  memcpy(ptr, &__pu_val, sizeof(__pu_val));		      \
   		  } 							      \
		  break;						      \
	  default:							      \
		  __pu_err = __put_user_bad ();				      \
		  break;						      \
	  }								      \
	  __pu_err;							      \
  })

#ifndef CONFIG_FORCE_CHECK
#define __put_user_bad()	(-EFAULT)
#else
#define __put_user_bad()	({ BUG(); (-EFAULT); })
#endif 

/*
 * FIXME: Should check the access permission.
 * */
#define put_user(x, ptr)       	__put_user(x, ptr) 
#define get_user(x, ptr)       	__get_user(x, ptr) 

#define __copy_from_user(to, from, n)	(memcpy (to, from, n), 0)
#define __copy_to_user(to, from, n)	(memcpy(to, from, n), 0)

#define __copy_to_user_inatomic __copy_to_user
#define __copy_from_user_inatomic __copy_from_user

static inline int copy_from_user(void* to, void __user * from, unsigned long n)
{
	if(unlikely(n==0)) return 0;

	if(access_ok(VERIFY_READ,from,n)){
		return __copy_from_user(to,from,n);
	}
	return -EFAULT;
}

static inline int copy_to_user(void __user * to, void* from, unsigned long n)
{
	if(unlikely(n==0)) return 0;

	if(access_ok(VERIFY_WRITE,to,n)){
		return __copy_to_user(to,from,n);
	}
	return -EFAULT;
}

#define copy_to_user_ret(to,from,n,retval) \
  ({ if (copy_to_user (to,from,n)) return retval; })

#define copy_from_user_ret(to,from,n,retval) \
  ({ if (copy_from_user (to,from,n)) return retval; })

/*
 * Copy a null terminated string from userspace.
 */

static inline long
strncpy_from_user (char *dst, const char *src, long count)
{
	if(access_ok(VERIFY_READ, src, count)) {
		char *tmp;
		strncpy (dst, src, count);
		for (tmp = dst; *tmp && count > 0; tmp++, count--)
			;
		return tmp - dst;
	}
	return -EFAULT;
}

/*
 * Return the size of a string (including the ending 0)
 *
 * Return 0 on exception, a value greater than N if too long
 */
static inline long strnlen_user (const char *src, long n)
{
	return strlen (src) + 1;
}

#define strlen_user(str)	strnlen_user (str, 32767)

/*
 * Zero Userspace
 */

static inline unsigned long
clear_user (void *to, unsigned long n)
{
	memset (to, 0, n);
	return 0;
}

#endif /* _ASM_TRIMEDIA_UACCESS_H__ */

