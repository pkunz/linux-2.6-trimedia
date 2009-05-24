/* 
 * arch/trimedia/kernel/process.c: architecture-dependent parts of process handling..
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/user.h>
#include <linux/interrupt.h>
#include <linux/reboot.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/setup.h>
#include <asm/pgtable.h>
#include <asm/regdef.h>

struct syscall_args
{
	unsigned long args[6];
};

asmlinkage void ret_from_fork(void);

static void default_idle(void) { }

void (*idle)(void) = default_idle;

/*
 * The idle thread. There's no useful work to be
 * done, so just try to conserve power and have a
 * low exit latency (ie sit in a loop waiting for
 * somebody to say that they'd like to reschedule)
 */
extern void 
cpu_idle(void)
{
	/* endless idle loop with no priority at all */
	while (1) {
		idle();
		preempt_enable_no_resched();
		schedule();
		preempt_disable();
	}
}

extern void 
machine_restart(char * __unused)
{
	for(;;);
}

extern void 
machine_halt(void)
{
	for (;;);
}

extern void 
machine_power_off(void)
{
	for (;;);
}

extern void 
show_regs(struct pt_regs * regs)
{
	printk("regs->sp is %x\n",regs->sp);
	printk("regs->fp is %x\n",regs->fp);
	printk("regs->rp is %x\n",regs->rp);
	printk("regs->spc is %x\n",regs->spc);
	printk("regs->dpc is %x\n",regs->dpc);
	printk("regs->pcsw is %x\n",regs->pcsw);
}

static void kernel_thread_helper(void *dummy, int (*fn)(void *), void *arg)
{
	fn(arg);
	do_exit(-1); /*should never reach here*/
}

/*
 * Create a kernel thread
 */
extern int 
kernel_thread(int (*fn)(void *), void * arg, unsigned long flags)
{
	long pid;

	struct {
		struct switch_stack sw;
		struct syscall_args args;
		struct pt_regs regs;
	}fatherstack;
	
	memset(&fatherstack,0,sizeof(fatherstack));

	fatherstack.sw.pcsw=READPCSW();

	fatherstack.regs.pcsw=READPCSW();
	fatherstack.regs.dpc=&kernel_thread_helper;
	fatherstack.regs.r6=fn;
	fatherstack.regs.r7=arg;

	pid=do_fork(flags|CLONE_VM|CLONE_UNTRACED,0,&fatherstack.regs,0,NULL,NULL);
	if(pid<0){
		panic("do_fork failed in kernel thread, errno=%d\n",pid);
	}
	
	return pid;
}

extern void
start_thread(struct pt_regs * regs, unsigned long pc, unsigned long sp)
{
	set_fs(USER_DS);
	regs->dpc=pc;
	regs->sp=sp;
	regs->fp=sp;
	
	return;
}

#ifdef CONFIG_DYNLOAD
static void 
__put_dynload_module(struct dynload_module* dynmodule)
{
	extern struct semaphore dynload_sem;

	if(atomic_dec_and_test(&dynmodule->count)){
		down(&dynload_sem);
		DynLoad_unload_application(dynmodule->module);
		up(&dynload_sem);
		kmem_cache_free(dynload_module_cache,dynmodule);
	}
}

static void
exit_dynload(struct task_struct* tsk)
{
	struct dynload_module* dynmodule=tsk->thread.dynmodule;
	if(dynmodule!=NULL){
		task_lock(tsk);
		tsk->thread.dynmodule=NULL;
		task_unlock(tsk);
		__put_dynload_module(dynmodule);
	}
}
#endif 

extern void 
flush_thread(void)
{
#ifdef CONFIG_DYNLOAD
	exit_dynload(current);
#endif 
}

extern void
exit_thread(void)
{
#ifdef CONFIG_DYNLOAD
	exit_dynload(current);
#endif 
}

extern void
release_thread(struct task_struct *dead_task)
{
	return;
}

asmlinkage int 
trimedia_fork(struct pt_regs *regs)
{
	/* fork almost works, enough to trick you into looking elsewhere :-( */
	return(-EINVAL);
}

/*
 * Used to store user stack context of process calling vfork.
 * TODO: maybe too large, but it should be enough for any cases (-g -O0 and so on)
 * */
struct ustack_trace
{
	/*6 unsigned long*/
	/*r0 - r8*/
	/*r33 - r64*/
	/*16 unsigned long*/
	unsigned long data[61];
};

asmlinkage int 
trimedia_vfork(struct pt_regs *regs)
{
	int ret=0;
	struct ustack_trace ustack;

	/*store user stack*/
	memset(&ustack,0,sizeof(ustack));
	__copy_from_user(ustack.data,(void*)regs->sp,sizeof(ustack));

	ret=do_fork(CLONE_VFORK|CLONE_VM|SIGCHLD,regs->sp,regs, 0, NULL, NULL);

	/*
	 * child process may change the context of father's user stack,
	 * So, restore the initial context here.
	 * */
	__copy_to_user((void*)regs->sp,ustack.data,sizeof(ustack));

	return ret;
}

asmlinkage int 
trimedia_clone(struct pt_regs *regs)
{
	unsigned long clone_flags;
	unsigned long newsp;

	clone_flags=REG_SYSCALL_ARG1(regs);
#ifndef CONFIG_MMU
	/*for non-mmu system, the CLONE_VM should always be set in any ways.*/
	if(!(clone_flags & CLONE_VM)){
		printk(KERN_DEBUG "call clone() should always set CLONE_VM for non-mmu system.\n");
	}
	clone_flags |= CLONE_VM;	
#endif 

	newsp=REG_SYSCALL_ARG2(regs);
	if(!newsp){
		newsp=regs->sp;
	}

	return do_fork(clone_flags,newsp,regs,0,NULL,NULL);
}

extern int 
copy_thread(int nr, unsigned long clone_flags,
		unsigned long usp, unsigned long topstk,
		struct task_struct * p, struct pt_regs * regs)
{
	struct thread_info* childti=p->thread_info;
	struct pt_regs* childregs=(unsigned long)childti+THREAD_SIZE-sizeof(struct pt_regs);
	struct switch_stack* childsw=
		(unsigned long)childregs-sizeof(struct syscall_args)-sizeof(struct switch_stack);
	struct switch_stack* fathersw=
		(unsigned long)regs-sizeof(struct syscall_args)-sizeof(struct switch_stack);

	*childregs=*regs;
	childregs->r5=0;
	
	if(usp==0){
		/*kernel thread*/
		childregs->sp=(unsigned long)childti+THREAD_SIZE;
		childregs->fp=(unsigned long)childti+THREAD_SIZE;
	}else{
		/*userspace thread*/
		childregs->sp=usp;
		childregs->fp=usp;
	}
	*childsw=*fathersw;
	childsw->rp=&ret_from_fork;
	childsw->fp=(unsigned long)childsw+sizeof(struct switch_stack);
	childsw->sp=(unsigned long)childsw+sizeof(struct switch_stack);

	childti->ksp=(unsigned long)childsw;
	
#ifdef CONFIG_DYNLOAD
	p->thread.dynmodule=current->thread.dynmodule;
	get_dynload_module(p->thread.dynmodule);
#endif 

	return 0;
}

/*
 * fill in the user structure for a core dump..
 */
extern void 
dump_thread(struct pt_regs * regs, struct user * dump)
{
}

/*
 *	Generic dumping code. Used for panic and debug.
 */
extern void 
dump(struct pt_regs *fp)
{
}

#ifdef CONFIG_DYNLOAD
/*
 * sys_execve() executes a new program.
 */
asmlinkage int 
sys_execve(char *name, char **argv, char **envp,unsigned long arg4,unsigned long arg5,
		unsigned long arg6,struct pt_regs regs)
{
	int error;
	char *filename;

	filename=getname(name);
	error=PTR_ERR(filename);
	if(IS_ERR(filename))
		goto out;

	error=do_execve(filename,argv,envp,&regs);
	if (error==0){
		task_lock(current);
		current->ptrace &= ~PT_DTRACE;
		task_unlock(current);
	}
	putname(filename);
out:
	return error;
}

#else
#define sys_fn_execve sys_execve

asmlinkage int
sys_fn_execve(unsigned long fn,char **argv,char **envp,unsigned long arg4,unsigned long arg5,
               unsigned long arg6,struct pt_regs regs)
{
       int error;

       error=do_fn_execve(fn,argv,envp,&regs);
       if(error==0){
               task_lock(current);
               current->ptrace &= ~PT_DTRACE;
               task_unlock(current);
       }

       return error;
}

#endif 

/*FIXME: must implement it now*/
extern unsigned long 
get_wchan(struct task_struct *p)
{
	return 0;
}

