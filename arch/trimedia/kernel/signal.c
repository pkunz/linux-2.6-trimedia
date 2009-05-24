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

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/compiler.h>
#include <linux/ptrace.h>
#include <linux/personality.h>

#include <asm/cacheflush.h>
#include <asm/ucontext.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/ptrace.h>
#include <asm/signal.h>
#include <asm/sigcontext.h>
#include <asm/thread_info.h>
#include <asm/target/regdef.h>

#define _BLOCKABLE (~(sigmask(SIGKILL)|sigmask(SIGSTOP)))

asmlinkage int
sys_rt_sigsuspend(sigset_t __user *unewset, size_t sigsetsize, unsigned long arg3,unsigned long arg4,
		unsigned long arg5,unsigned long arg6,unsigned long history)
{
	struct pt_regs* regs=(struct pt_regs*)&history;
	sigset_t saveset, newset;

	/* XXX: Don't preclude handling different sized sigset_t's. */
	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (copy_from_user(&newset, unewset, sizeof(newset)))
		return -EFAULT;
	sigdelsetmask(&newset, ~_BLOCKABLE);

	spin_lock_irq(&current->sighand->siglock);
	saveset = current->blocked;
	current->blocked = newset;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
	regs->r5 = -EINTR;

	while (1) {
		current->state = TASK_INTERRUPTIBLE;
		schedule();
		if (do_signal(&saveset, regs, 0))
			return regs->r5;
	}
}

asmlinkage int
sys_sigsuspend(old_sigset_t mask,unsigned long arg2,unsigned long arg3,unsigned long arg4,
		unsigned long arg5,unsigned long arg6,unsigned long history)
{
	struct pt_regs* regs=(struct pt_regs*)&history;
	sigset_t saveset;

	mask &=_BLOCKABLE;
	spin_lock_irq(&current->sighand->siglock);
	saveset = current->blocked;
	siginitset(&current->blocked, mask);
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
	regs->r5= -EINTR;

	while (1) {
		current->state = TASK_INTERRUPTIBLE;
		schedule();
		if (do_signal(&saveset, regs, 0))
			return regs->r5;
	}

}

asmlinkage int 
sys_sigaction(int sig, const struct old_sigaction __user *act,
	      struct old_sigaction __user *oact)
{
	struct k_sigaction new_ka, old_ka;
	int ret;

	if (act) {
		old_sigset_t mask;
		if (!access_ok(VERIFY_READ, act, sizeof(*act)) ||
		    __get_user(new_ka.sa.sa_handler, &act->sa_handler) ||
		    __get_user(new_ka.sa.sa_restorer, &act->sa_restorer))
			return -EFAULT;
		__get_user(new_ka.sa.sa_flags, &act->sa_flags);
		__get_user(mask, &act->sa_mask);
		siginitset(&new_ka.sa.sa_mask, mask);
	}

	ret = do_sigaction(sig, act ? &new_ka : NULL, oact ? &old_ka : NULL);

	if (!ret && oact) {
		if (!access_ok(VERIFY_WRITE, oact, sizeof(*oact)) ||
		    __put_user(old_ka.sa.sa_handler, &oact->sa_handler) ||
		    __put_user(old_ka.sa.sa_restorer, &oact->sa_restorer))
			return -EFAULT;
		__put_user(old_ka.sa.sa_flags, &oact->sa_flags);
		__put_user(old_ka.sa.sa_mask.sig[0], &oact->sa_mask);
	}

	return ret;
}

asmlinkage int
sys_sigaltstack(const stack_t *uss, stack_t* uoss,unsigned long arg3,unsigned long arg4,
		unsigned long arg5,unsigned long arg6,struct pt_regs regs)
{
	return do_sigaltstack(uss,uoss,regs.sp);
}

struct sigframe{
	struct sigcontext sc;
	unsigned long extramask[_NSIG_WORDS-1];
};

struct rt_sigframe{
	struct siginfo info;
	struct ucontext uc;
};

static int
restore_sigcontext(struct pt_regs* regs,struct sigcontext __user *sc)
{
	int err=0;

	err=__copy_from_user(regs,&sc->regs,sizeof(struct pt_regs));
	
	return err;
}

asmlinkage int
sys_rt_sigreturn(unsigned long arg1,unsigned long arg2,unsigned long arg3,unsigned long arg4,
		unsigned long arg5,unsigned long arg6,unsigned long history)
{
	struct rt_sigframe __user* frame;
	sigset_t set;
	struct pt_regs* regs=(struct pt_regs*)&history;

	current_thread_info()->restart_block.fn= do_no_restart_syscall;

	if(regs->sp&4){
		goto badframe;
	}
	
	frame=(struct rt_sigframe __user*)regs->sp;

	if(!access_ok(VERIFY_READ,frame,sizeof(*frame))){
		goto badframe;
	}
	if(__copy_from_user(&set,&frame->uc.uc_sigmask,sizeof(set))){
		goto badframe;
	}

	sigdelsetmask(&set,~_BLOCKABLE);
	spin_lock_irq(&current->sighand->siglock);
	current->blocked=set;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);

	if(restore_sigcontext(regs,&frame->uc.uc_mcontext)){
		goto badframe;
	}

	if(do_sigaltstack(&frame->uc.uc_stack,NULL,regs->sp) == EFAULT){
		goto badframe;
	}

	/*
	 *FIXME:  I don't know how to trace TriMedia executable.
	 */
#if 0
	if(current->ptrace & PT_SINGLESTEP){
		ptrace_cancel_bpt(current);
		send_sig(SIGTRAP,current,1);
	}
#endif 
	
	return regs->r5;
	
badframe:
	force_sig(SIGSEGV,current);
	return 0;
}

asmlinkage int
sys_sigreturn(unsigned long arg1,unsigned long arg2,unsigned long arg3,unsigned long arg4,
		unsigned long arg5,unsigned long arg6,unsigned long history)
{
	struct sigframe __user *frame;
	sigset_t set;
	struct pt_regs* regs=(struct pt_regs*)&history;

	current_thread_info()->restart_block.fn=do_no_restart_syscall;

	if(regs->sp&4){
		goto badframe;
	}

	frame=(struct sigframe __user*)regs->sp;

	if(!access_ok(VERIFY_READ,frame,sizeof(*frame))){
		goto badframe;
	}

	if (__get_user(set.sig[0], &frame->sc.oldmask)
	    || (_NSIG_WORDS > 1
	        && __copy_from_user(&set.sig[1], &frame->extramask,
				    sizeof(frame->extramask))))
		goto badframe;

	sigdelsetmask(&set,~_BLOCKABLE);
	spin_lock_irq(&current->sighand->siglock);
	current->blocked=set;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);	

	if(restore_sigcontext(regs,&frame->sc)){
		goto badframe;
	}

	/*
	 *FIXME: I don't know how to trace TriMedia executable.
	 */
#if 0
	/* Send SIGTRAP if we're single-stepping */
	if (current->ptrace & PT_SINGLESTEP) {
		ptrace_cancel_bpt(current);
		send_sig(SIGTRAP, current, 1);
	}
#endif 
	
	return regs->r5;
	
badframe:
	force_sig(SIGSEGV,current);
	return 0;
}

extern void rt_sigreturn(void);

static int
setup_rt_return(struct pt_regs* regs,struct rt_sigframe __user* frame,struct k_sigaction* ka,int usig)
{
	unsigned long handler=(unsigned long)ka->sa.sa_handler;
	unsigned long retcode;

	if(ka->sa.sa_flags & SA_RESTORER){
		retcode=(unsigned long)ka->sa.sa_restorer;
	}else{
		retcode=(unsigned long)rt_sigreturn;
	}

	regs->r5=usig;
	regs->r6=(unsigned long)&frame->info;
	regs->r7=(unsigned long)&frame->uc;
	regs->sp=(unsigned long)frame;
	regs->fp=(unsigned long)frame;
	regs->dpc=handler;
	regs->rp=retcode;

	return 0;
}

static int
setup_rt_frame(int usig, struct k_sigaction* ka,siginfo_t* info,
		sigset_t* set,struct pt_regs* regs)
{
	struct rt_sigframe __user *frame=get_sigframe(ka,regs,sizeof(*frame));
	stack_t stack;
	int err=0;

	if(!frame){
		return 1;
	}
	
	err |= copy_siginfo_to_user(&frame->info,info);
	err |= __put_user(0,&frame->uc.uc_flags);
	err |= __put_user(NULL,&frame->uc.uc_link);

	memset(&stack,0,sizeof(stack));
	stack.ss_sp=(void __user*)current->sas_ss_sp;
	stack.ss_flags=sas_ss_flags(regs->sp);
	stack.ss_size=current->sas_ss_size;
	err |= __copy_to_user(&frame->uc.uc_stack,&stack,sizeof(stack));

	err |= setup_sigcontext(&frame->uc.uc_mcontext,regs,set->sig[0]);
	err |= __copy_to_user(&frame->uc.uc_sigmask,set,sizeof(*set));

	if(err==0){
		err=setup_rt_return(regs,frame,ka,usig);
	}

	return err;
}

extern void sigreturn(void);

static int
setup_return(struct pt_regs* regs,void __user* frame,struct k_sigaction* ka,int usig)
{
	unsigned long handler=(unsigned long)ka->sa.sa_handler;
	unsigned long retcode;

	if(ka->sa.sa_flags & SA_RESTORER){
		retcode=(unsigned long)ka->sa.sa_restorer;
	}else{
		retcode=(unsigned long)sigreturn;
	}

	regs->r5=usig;
	regs->sp=(unsigned long)frame;
	regs->fp=(unsigned long)frame;
	regs->dpc=handler;
	regs->rp=retcode;

	return 0;
}

static int
setup_sigcontext(struct sigcontext __user* sc,struct pt_regs* regs,unsigned long mask)
{
	int err=0;

	err |= __copy_to_user(&sc->regs,regs,sizeof(struct pt_regs));
	err |= __put_user(mask,&sc->oldmask);

	return err;
}

static inline void __user*
get_sigframe(struct k_sigaction* ka,struct pt_regs* regs,int framesize)
{
	unsigned long sp=regs->sp;
	void __user* frame;

	/*
	 *This is The X/Open sanctioned signal stack switching.
	 */
	if((ka->sa.sa_flags & SA_ONSTACK) && !sas_ss_flags(sp)){
		sp=current->sas_ss_sp + current->sas_ss_size;
	}

	frame=(void __user*)((sp-framesize)&~4);

	if(!access_ok(VERIFY_WRITE,frame,framesize)){
		frame=NULL;
	}

	return frame;
}

static int
setup_frame(int usig, struct k_sigaction* ka,sigset_t* set,struct pt_regs* regs)
{
	struct sigframe __user *frame=get_sigframe(ka,regs,sizeof(struct sigframe));
	int err=0;

	if(!frame){
		return 1;
	}

	err |= setup_sigcontext(&frame->sc,regs,set->sig[0]);

	if(_NSIG_WORDS >1) {
		err |= __copy_to_user(frame->extramask,&set->sig[1],
				sizeof(frame->extramask));
	}

	if(err == 0){
		err=setup_return(regs,frame,ka,usig);
	}
	
	return err;
}

static void inline
restart_syscall(struct pt_regs* regs)
{
	regs->dpc = regs->spc;
	regs->r5 = REG_SYSCALL_NR(regs);
}

static void
handle_signal(unsigned long sig, struct k_sigaction *ka,
	      siginfo_t *info, sigset_t *oldset,
	      struct pt_regs * regs, int syscall)
{
	struct thread_info *thread = current_thread_info();
	struct task_struct *tsk = current;
	int usig = sig;
	int ret;

	/*
	 * If we were from a system call, check for system call restarting...
	 */
	if (syscall) {
		switch (regs->r5) {
		case -ERESTART_RESTARTBLOCK:
		case -ERESTARTNOHAND:
			regs->r5 = -EINTR;
			break;
		case -ERESTARTSYS:
			if (!(ka->sa.sa_flags & SA_RESTART)) {
				regs->r5 = -EINTR;
				break;
			}
			/* fallthrough */
		case -ERESTARTNOINTR:
			restart_syscall(regs);
		}
	}

	/*
	 * translate the signal
	 */
	if (usig < 32 && thread->exec_domain && thread->exec_domain->signal_invmap)
		usig = thread->exec_domain->signal_invmap[usig];

	/*
	 * Set up the stack frame
	 */
	if (ka->sa.sa_flags & SA_SIGINFO)
		ret = setup_rt_frame(usig, ka, info, oldset, regs);
	else
		ret = setup_frame(usig, ka, oldset, regs);

	if (ret != 0) {
		force_sigsegv(sig, tsk);
		return;
	}

	/*
	 * Block the signal if we were successful.
	 */
	spin_lock_irq(&tsk->sighand->siglock);
	sigorsets(&tsk->blocked, &tsk->blocked,
		  &ka->sa.sa_mask);
	if (!(ka->sa.sa_flags & SA_NODEFER))
		sigaddset(&tsk->blocked, sig);
	recalc_sigpending();
	spin_unlock_irq(&tsk->sighand->siglock);
}

static int do_signal(sigset_t *oldset, struct pt_regs *regs, int syscall)
{
	struct k_sigaction ka;
	siginfo_t info;
	int signr;

	/*
	 * We want the common case to go fast, which
	 * is why we may in certain cases get here from
	 * kernel mode. Just return without doing anything
	 * if so.
	 */
	if (!user_mode(regs))
		return 0;

	if (try_to_freeze())
		goto no_signal;

	/*
	 *FIXME: I don't know how to trace TriMedia executable.
	 */
#if 0
	if (current->ptrace & PT_SINGLESTEP)
		ptrace_cancel_bpt(current);
#endif 
	
	signr = get_signal_to_deliver(&info, &ka, regs, NULL);
	if (signr > 0) {
		handle_signal(signr, &ka, &info, oldset, regs, syscall);
		/*
		 *FIXME: I don't know how to trace TriMedia executable.
		 */
#if 0
		if (current->ptrace & PT_SINGLESTEP)
			ptrace_set_bpt(current);
#endif 
		return 1;
	}

 no_signal:
	/*
	 * No signal to deliver to the process - restart the syscall.
	 */
	if (syscall) {
		if (regs->r5 == -ERESTART_RESTARTBLOCK) {
			regs->r5=__NR_restart_syscall;	
			regs->dpc=regs->spc;
		}		
		if (regs->r5 == -ERESTARTNOHAND ||
		    regs->r5 == -ERESTARTSYS ||
		    regs->r5 == -ERESTARTNOINTR) {
			restart_syscall(regs);
		}
	}

	/*
	 *FIXME: I don't know how to trace TriMedia executable.
	 */
#if 0
	if (current->ptrace & PT_SINGLESTEP)
		ptrace_set_bpt(current);
#endif 
	return 0;
}
	
extern void 
do_notify_resume(int syscall,unsigned int thread_flags,struct pt_regs* regs)
{
	if(thread_flags & _TIF_SIGPENDING){
		do_signal(&current->blocked,regs,syscall);
	}
}
