#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/timex.h>
#include <asm/timer.h>

/*
 * HPET memory read is slower than tsc reads, but is more dependable as it
 * always runs at constant frequency and reduces complexity due to
 * cpufreq. So, we prefer HPET timer to tsc based one. Also, we cannot use
 * timer_pit when HPET is active. So, we default to timer_tsc.
 */

/* list of timers, ordered by preference, NULL terminated */
static struct init_timer_opts* __initdata timers[] = {
#ifdef CONFIG_X86_CYCLONE_TIMER
	&timer_cyclone_init,
#endif
#ifdef CONFIG_HPET_TIMER
	&timer_hpet_init,
#endif
#ifdef CONFIG_X86_PM_TIMER
	&timer_pmtmr_init,
#endif
#ifdef CONFIG_X86_TSC_TIMER
	&timer_tsc_init,
#endif
	&timer_pit_init,
	NULL,
};

int tsc_disable __devinitdata = 0;

static char clock_override[10] __initdata;

static int __init clock_setup(char* str)
{
	if (str)
		strlcpy(clock_override, str, sizeof(clock_override));
	return 1;
}
__setup("clock=", clock_setup);


/* The chosen timesource has been found to be bad.
 * Fall back to a known good timesource (the PIT)
 */
void clock_fallback(void)
{
	cur_timer = &timer_pit;
}

/* iterates through the list of timers, returning the first 
 * one that initializes successfully.
 */
struct timer_opts* __init select_timer(void)
{
	int i = 0;
	
	/* find most preferred working timer */
	while (timers[i]) {
		if (timers[i]->init)
			if (timers[i]->init(clock_override) == 0)
				return timers[i]->opts;
		++i;
	}
		
	panic("select_timer: Cannot find a suitable timer\n");
	return NULL;
}

int read_current_timer(unsigned long *timer_val)
{
	if (cur_timer->read_timer) {
		*timer_val = cur_timer->read_timer();
		return 0;
	}
	return -1;
}

#ifndef CONFIG_X86_TSC_TIMER
unsigned long long sched_clock(void)
{
	return (unsigned long long)jiffies_64 * (1000000000 / HZ);
}
#endif
