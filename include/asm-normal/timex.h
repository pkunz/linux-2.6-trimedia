#ifndef _ASM_NORMAL_TIMEX_H
#define _ASM_NORMAL_TIMEX_H

#define CLOCK_TICK_RATE	1193180 /* Underlying HZ */

typedef unsigned long cycles_t;

static inline cycles_t get_cycles(void)
{
	return 0;
}

#endif
