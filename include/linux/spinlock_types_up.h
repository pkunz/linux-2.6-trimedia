#ifndef __LINUX_SPINLOCK_TYPES_UP_H
#define __LINUX_SPINLOCK_TYPES_UP_H

#ifndef __LINUX_SPINLOCK_TYPES_H
# error "please don't include this file directly"
#endif

/*
 * include/linux/spinlock_types_up.h - spinlock type definitions for UP
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

#ifdef CONFIG_DEBUG_SPINLOCK

typedef struct {
	volatile unsigned int slock;
} raw_spinlock_t;

#define __RAW_SPIN_LOCK_UNLOCKED { 1 }

#else

#ifdef  __GNUC__
/*
 * All gcc 2.95 versions and early versions of 2.96 have a nasty bug
 * with empty initializers.
 */
#if (__GNUC__ > 2)

typedef struct { } raw_spinlock_t;
#define __RAW_SPIN_LOCK_UNLOCKED { }

typedef struct { } raw_rwlock_t;
#define __RAW_RW_LOCK_UNLOCKED { }

#else

typedef struct { int gcc_is_buggy; } raw_spinlock_t;
#define __RAW_SPIN_LOCK_UNLOCKED (raw_spinlock_t) { 0 }

typedef struct {int gcc_is_buggy; } raw_rwlock_t;
#define __RAW_RW_LOCK_UNLOCKED  (raw_rwlock_t) { 0 }

#endif
#endif /*__GNUC__*/

#ifdef __TCS__
typedef struct { int tmcc_is_buggy; } raw_spinlock_t;
#define __RAW_SPIN_LOCK_UNLOCKED 	(raw_spinlock_t){0}

typedef struct { int tmcc_is_buggy; } raw_rwlock_t;
#define __RAW_RW_LOCK_UNLOCKED 		(raw_spinlock_t){0}
#endif 

#endif

#endif /* __LINUX_SPINLOCK_TYPES_UP_H */
