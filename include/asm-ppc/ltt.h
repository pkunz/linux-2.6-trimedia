/*
 * linux/include/asm-ppc/ltt.h
 *
 * Copyright (C)	2002, Karim Yaghmour
 *		 	2005, Mathieu Desnoyers
 *
 * PowerPC definitions for tracing system
 */

#ifndef _ASM_PPC_LTT_H
#define _ASM_PPC_LTT_H

#include <linux/config.h>
#include <linux/jiffies.h>

/* Current arch type */
#define LTT_ARCH_TYPE LTT_ARCH_TYPE_PPC

/* PowerPC variants */
#define LTT_ARCH_VARIANT_PPC_4xx 1	/* 4xx systems (IBM embedded series) */
#define LTT_ARCH_VARIANT_PPC_6xx 2	/* 6xx/7xx/74xx/8260/POWER3 systems
					   (desktop flavor) */
#define LTT_ARCH_VARIANT_PPC_8xx 3	/* 8xx system (Motoral embedded series)
					 */
#define LTT_ARCH_VARIANT_PPC_ISERIES 4	/* 8xx system (iSeries) */
#define LTT_ARCH_VARIANT_PPC_E500         5   /* E500 (Freescale 85x0) */
#define LTT_ARCH_VARIANT_PPC_POWER4       6   /* POWER4 (IBM 970) */
#define LTT_ARCH_VARIANT_PPC_PSERIES      7   /* PPC64 only */
#define LTT_ARCH_VARIANT_PPC_PMAC         8   /* apple powermac */
#define LTT_ARCH_VARIANT_PPC_PMAC64       9   /* apple powermac64 */

/* Current variant type */
#if defined(CONFIG_4xx)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_4xx
#elif defined(CONFIG_6xx)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_6xx
#elif defined(CONFIG_8xx)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_8xx
#elif defined(CONFIG_PPC_ISERIES)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_ISERIES
#elif defined(CONFIG_E500)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_E500
#elif defined(CONFIG_POWER4)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_POWER4
#elif defined(CONFIG_PSERIES)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_PSERIES
#elif defined(CONFIG_PPC_PMAC)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_PMAC
#elif defined(CONFIG_PPC_PMAC64)
#define LTT_ARCH_VARIANT LTT_ARCH_VARIANT_PPC_PMAC64
#else
#error "a PPC arch variant must be defined!!"
#endif

#define LTTNG_LOGICAL_SHIFT 13

extern atomic_t lttng_logical_clock;

/*
 * Do not compile these unused functions.  get_pvr() does not exist
 * for 2.6.14, anyway.
 */

#if 0
/* The shift overflow doesn't matter */
static inline u32 _ltt_get_timestamp32(void)
{
	unsigned long seq;
	unsigned long try = 5;
	u32 ret;

	do {
		seq = read_seqbegin(&xtime_lock);
		ret = (jiffies << LTTNG_LOGICAL_SHIFT)
			| (atomic_add_return(1, &lttng_logical_clock));
	} while(read_seqretry(&xtime_lock, seq) && (--try) > 0);

	if(try == 0) return 0;
	else return ret;
}

static inline _ltt_get_tb32(u32 *p)
{
	unsigned lo;
	asm volatile("mftb %0"
		 : "=r" (lo));
	p[0] = lo;
}

static inline u32 ltt_get_timestamp32(void)
{
	u32 ret;
	if ((get_pvr() >> 16) == 1) ret = _ltt_get_timestamp32();
	else _ltt_get_tb32((u32*)&ret);
	return ret;
}
#endif

/* The shift overflow doesn't matter */
static inline u64 _ltt_get_timestamp64(void)
{
	unsigned long seq;
	unsigned long try = 5;
	u64 ret;

	do {
		seq = read_seqbegin(&xtime_lock);
		ret = (jiffies_64 << LTTNG_LOGICAL_SHIFT)
			| (atomic_add_return(1, &lttng_logical_clock));
	} while(read_seqretry(&xtime_lock, seq) && (--try) > 0);

	if(try == 0) return 0;
	else return ret;
}

#ifdef SMP
#define pvr_ver (PVR_VER(current_cpu_data.pvr))
#else
#define pvr_ver (PVR_VER(mfspr(SPRN_PVR)))
#endif

/* from arch/ppc/xmon/xmon.c */
static inline void _ltt_get_tb64(unsigned *p)
{
	unsigned hi, lo, hiagain;

	do {
		asm volatile("mftbu %0; mftb %1; mftbu %2"
			 : "=r" (hi), "=r" (lo), "=r" (hiagain));
	} while (hi != hiagain);
	p[0] = hi;
	p[1] = lo;
}

static inline u64 ltt_get_timestamp64(void)
{
	u64 ret;
  if (pvr_ver == 1) ret = _ltt_get_timestamp64();
	else _ltt_get_tb64((unsigned*)&ret);
	return ret;
}

/* this has to be called with the write seqlock held */
static inline void ltt_reset_timestamp(void)
{
	if (pvr_ver == 1) atomic_set(&lttng_logical_clock, 0);
}

/* based on the comment in arch/ppc/kernel/time.c :
 * The decrementer counts down by 128 every 128ns on a 601.
 *
 * That means 1 tick every ns, so 1000000000 ticks per second. */

static inline unsigned int ltt_frequency(void)
{
	extern unsigned tb_ticks_per_jiffy;
	if (pvr_ver == 1)
            return HZ << LTTNG_LOGICAL_SHIFT;
	else
            return (tb_ticks_per_jiffy * HZ);
}

static inline u32 ltt_freq_scale(void)
{
	return 1;
}

#endif //_ASM_PPC_LTT_H
