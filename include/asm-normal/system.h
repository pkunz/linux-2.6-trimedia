#ifndef __NORMAL_SYSTEM_H__
#define __NORMAL_SYSTEM_H__

#include <linux/linkage.h>
#include <asm/ptrace.h>


#define prepare_to_switch()	do { } while (0)

/*
 * switch_to(n) should switch tasks to task ptr, first checking that
 * ptr isn't the current task, in which case it does nothing.
 */
struct thread_struct;
extern void *switch_thread (struct thread_struct *last,
			    struct thread_struct *next);

#define switch_to(prev,next,last)					      \
  do {									      \
        if (prev != next) {						      \
 		(last) = switch_thread (&prev->thread, &next->thread);	      \
	}								      \
  } while (0)


/* Enable/disable interrupts.  */
#define local_irq_enable()	
#define local_irq_disable()	

#define local_save_flags(flags) 
#define local_restore_flags(flags) 

/* For spinlocks etc */
#define	local_irq_save(flags) 
#define local_irq_restore(flags) 

extern int irqs_disabled (void);

/*
 * Force strict CPU ordering.
 * Not really required on v850...
 */
#define nop()		
#define mb()		
#define rmb()		
#define wmb()	
#define read_barrier_depends()
#define set_rmb(var, value)
#define set_mb(var, value)
#define set_wmb(var, value)	

#define smp_mb()	
#define smp_rmb()
#define smp_wmb()
#define smp_read_barrier_depends()

#define xchg(ptr, x)  		((__typeof__(*(ptr)))__xchg((unsigned long)(x),(ptr),sizeof(*(ptr))))

extern unsigned long __xchg (unsigned long with,
				    void *ptr, int size);

#define arch_align_stack(x) (x)

#endif /* __NORMAL_SYSTEM_H__ */

