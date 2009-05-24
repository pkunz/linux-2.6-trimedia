/*
 *  linux/arch/arm/kernel/ptrace.c
 *
 *  By Ross Biro 1/23/92
 * edited by Linus Torvalds
 * ARM modifications Copyright (C) 2000 Russell King
 * WindRiver (C) 2005-2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/init.h>
#include <linux/signal.h>

#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/traps.h>

#include "ptrace.h"

#define REG_PC	15
#define REG_PSR	16

#include <asm/cacheflush.h>

typedef unsigned int UINT32;
typedef int INT32;

#define BIT(n) ((UINT32)1U << (n))
#define BITSET(x,n) (((UINT32)(x) & (1U<<(n))) >> (n))
#define BITS(x,m,n) (((UINT32)((x) & (BIT(n) - BIT(m) + BIT(n)))) >> (m))

/* #define DEBUG_IT */

#ifdef DEBUG_IT
#define PRINTK(args...) printk(KERN_ALERT args)
#else
#define PRINTK(args...)
#endif				/* DEBUG_IT */

/*
 * ccTable is used to determine whether an instruction will be executed,
 * according to the flags in the PSR and the condition field of the
 * instruction. The table has an entry for each possible value of the
 * condition field of the instruction. Each bit indicates whether a particular
 * combination of flags will cause the instruction to be executed. Since
 * ther are four flags, this makes 16 possible TRUE/FALSE values.
 */

static UINT32 ccTable[] = {
	0xF0F0, 0x0F0F, 0xCCCC, 0x3333, 0xFF00, 0x00FF, 0xAAAA, 0x5555,
	0x0C0C, 0xF3F3, 0xAA55, 0x55AA, 0x0A05, 0xF5FA, 0xFFFF, 0x0000
};

static u32 *armGetNpc(u32 instr,	/* the current instruction */
		      struct task_struct *child	/* pointer to task registers */
    );
static u16 *thumbGetNpc(u16 instr,	/* the current instruction */
			struct task_struct *child,	/* pointer to task registers */
			int *backToArm);

/*
 * does not yet catch signals sent when the child dies.
 * in exit.c or in signal.c.
 */

#if 0
/*
 * Breakpoint SWI instruction: SWI &9F0001
 */
#define BREAKINST_ARM	0xef9f0001
#define BREAKINST_THUMB	0xdf00	/* fill this in later */
#else
/*
 * New breakpoints - use an undefined instruction.  The ARM architecture
 * reference manual guarantees that the following instruction space
 * will produce an undefined instruction exception on all CPUs:
 *
 *  ARM:   xxxx 0111 1111 xxxx xxxx xxxx 1111 xxxx
 *  Thumb: 1101 1110 xxxx xxxx
 */
#define BREAKINST_ARM	0xe7f001f0
#define BREAKINST_THUMB	0xde01
#endif

/*
 * Get the address of the live pt_regs for the specified task.
 * These are saved onto the top kernel stack when the process
 * is not running.
 *
 * Note: if a user thread is execve'd from kernel space, the
 * kernel stack will not be empty on entry to the kernel, so
 * ptracing these tasks will fail.
 */
static inline struct pt_regs *get_user_regs(struct task_struct *task)
{
	return (struct pt_regs *)
	    ((unsigned long)task->thread_info + THREAD_SIZE -
	     8 - sizeof(struct pt_regs));
}

/*
 * this routine will get a word off of the processes privileged stack.
 * the offset is how far from the base addr as stored in the THREAD.
 * this routine assumes that all the privileged stacks are in our
 * data space.
 */
static inline long get_user_reg(struct task_struct *task, int offset)
{
	return get_user_regs(task)->uregs[offset];
}

/*
 * this routine will put a word on the processes privileged stack.
 * the offset is how far from the base addr as stored in the THREAD.
 * this routine assumes that all the privileged stacks are in our
 * data space.
 */
static inline int put_user_reg(struct task_struct *task, int offset, long data)
{
	struct pt_regs newregs, *regs = get_user_regs(task);
	int ret = -EINVAL;

	newregs = *regs;
	newregs.uregs[offset] = data;

	if (valid_user_regs(&newregs)) {
		regs->uregs[offset] = data;
		ret = 0;
	}

	return ret;
}

static inline int
read_u32(struct task_struct *task, unsigned long addr, u32 * res)
{
	int ret;

	ret = access_process_vm(task, addr, res, sizeof(*res), 0);

	return ret == sizeof(*res) ? 0 : -EIO;
}

static inline int
read_instr(struct task_struct *task, unsigned long addr, u32 * res,
	   int is_thumb)
{
	int ret;

	if ((addr & 1) || is_thumb) {
		u16 val;
		PRINTK("%s: reading a THUMB instrution\n", __FUNCTION__);
		ret = access_process_vm(task, addr & ~1, &val, sizeof(val), 0);
		ret = ret == sizeof(val) ? 0 : -EIO;
		*res = val;
	} else {
		u32 val;
		PRINTK("%s: reading an ARM instrution\n", __FUNCTION__);
		ret = access_process_vm(task, addr & ~3, &val, sizeof(val), 0);
		ret = ret == sizeof(val) ? 0 : -EIO;
		*res = val;
	}
	return ret;
}

static int
swap_insn(struct task_struct *task, unsigned long addr,
	  void *old_insn, void *new_insn, int size)
{
	int ret;

	/* new_isns is set by caller */
	PRINTK("%s new insn 0x%x for addr 0x%lx, size %d\n",
	       __FUNCTION__, *((int *)new_insn), addr, size);

	ret = access_process_vm(task, addr, old_insn, size, 0);

	if (size == 2) {
		short old, new;
		old = *((short *)old_insn);
		new = *((short *)new_insn);
		PRINTK("replace thumb 0x%x by 0x%x\n", old, new);
	}

	if (size == 4) {
		int old, new;
		old = *((int *)old_insn);
		new = *((int *)new_insn);
		PRINTK("replace arm 0x%x by 0x%x\n", old, new);
	}

	if (ret == size)
		ret = access_process_vm(task, addr, new_insn, size, 1);
	else
		PRINTK("%s ERROR\n", __FUNCTION__);
	return ret;
}

static void
add_breakpoint(struct task_struct *task,
	       struct debug_info *dbg,
	       unsigned long addr, int is_thumb /* what kind of BP to set */ )
{
	int nr = dbg->nsaved;

	if (nr < 2) {
		u32 new_insn;
		int res;

		if (is_thumb) {
			/* THIERRY */
			u16 thumb_insn = BREAKINST_THUMB;
			PRINTK("%s:Will set 0x%x at 0x%lx\n", __FUNCTION__,
			       thumb_insn, addr);
			res =
			    swap_insn(task, addr, &dbg->bp[nr].insn,
				      &thumb_insn, 2);

			if (res == 2) {
				dbg->bp[nr].address = addr;
				dbg->nsaved += 1;
				dbg->bp[nr].is_thumb = 1;
			}
		} else {
			new_insn = BREAKINST_ARM;
			res =
			    swap_insn(task, addr, &dbg->bp[nr].insn, &new_insn,
				      4);

			if (res == 4) {
				dbg->bp[nr].address = addr;
				dbg->nsaved += 1;
				dbg->bp[nr].is_thumb = 0;
			}
		}
	} else
		printk(KERN_ERR "ptrace: too many breakpoints\n");
}

/*
 * Clear one breakpoint in the user program.
 */
static void clear_breakpoint(struct task_struct *task, struct debug_entry *bp)
{
	unsigned long addr = bp->address;
	union debug_insn old_insn;
	int ret;

	PRINTK("%s at 0x%lx, %s mode\n",
	       __FUNCTION__, addr, bp->is_thumb ? "thumb" : "arm");

	if (bp->is_thumb) {
		ret = swap_insn(task, addr & ~1, &old_insn.thumb,
				&bp->insn.thumb, 2);

		if (ret != 2 || old_insn.thumb != BREAKINST_THUMB)
			printk(KERN_ERR "%s:%d: corrupted Thumb breakpoint at "
			       "0x%08lx (0x%04x)\n", task->comm, task->pid,
			       addr, old_insn.thumb);
	} else {
		ret = swap_insn(task, addr & ~3, &old_insn.arm,
				&bp->insn.arm, 4);

		if (ret != 4 || old_insn.arm != BREAKINST_ARM)
			printk(KERN_ERR "%s:%d: corrupted ARM breakpoint at "
			       "0x%08lx (0x%08x)\n", task->comm, task->pid,
			       addr, old_insn.arm);
	}
}

void ptrace_set_bpt(struct task_struct *child)
{
	struct pt_regs *regs;
	unsigned long pc;
	u32 insn;
	int res;
	unsigned long next_pc;
	struct debug_info *dbg = &child->thread.debug;
	int is_thumb;
	int toThumb = 0;
	int backToArm = 0;
	int putAthumb;

	regs = get_user_regs(child);
	pc = instruction_pointer(regs);
	is_thumb = thumb_mode(regs);

	res = read_instr(child, pc, &insn, is_thumb);
	if (res) {
		PRINTK("ptrace_set_bpt failed: inst=%x pc=%lx\n", insn, pc);
		return;
	}

	if (is_thumb)
		next_pc = (unsigned long)thumbGetNpc(insn, child, &backToArm);
	else
		/* Compute the next address */
		next_pc = (unsigned long)armGetNpc(insn, child);

	dbg = &child->thread.debug;
	dbg->nsaved = 0;

	PRINTK("%s (%s mode) ptrace_set_bpt inst=%x pc=%lx next_pc=%lx\n",
	       __FUNCTION__, is_thumb ? "thumb" : "arm", insn, pc, next_pc);

	if (next_pc & 0x1) {
		toThumb = 1;
		next_pc &= (next_pc & ~0x1);
		PRINTK("mode switch ! next pc is actually 0x%lx\n", next_pc);
	}

	putAthumb = ((!is_thumb && toThumb) || (is_thumb && !backToArm));

	PRINTK("BP to set is in %s mode\n", putAthumb ? "thumb" : "arm");

	add_breakpoint(child, dbg, next_pc, putAthumb);
}

/*
 * Ensure no single-step breakpoint is pending.  Returns non-zero
 * value if child was being single-stepped.
 */
void ptrace_cancel_bpt(struct task_struct *child)
{
	int i, nsaved = child->thread.debug.nsaved;

	child->thread.debug.nsaved = 0;

	if (nsaved > 2) {
		printk("ptrace_cancel_bpt: bogus nsaved: %d!\n", nsaved);
		nsaved = 2;
	}

	for (i = 0; i < nsaved; i++)
		clear_breakpoint(child, &child->thread.debug.bp[i]);
}

/*
 * Called by kernel/ptrace.c when detaching..
 *
 * Make sure the single step bit is not set.
 */
void ptrace_disable(struct task_struct *child)
{
	child->ptrace &= ~PT_SINGLESTEP;
	ptrace_cancel_bpt(child);
}

/*
 * Handle hitting a breakpoint.
 */
void ptrace_break(struct task_struct *tsk, struct pt_regs *regs)
{
	siginfo_t info;
	PRINTK("Caught a break at %lx\n", regs->ARM_pc);
	ptrace_cancel_bpt(tsk);

	info.si_signo = SIGTRAP;
	info.si_errno = 0;
	info.si_code = TRAP_BRKPT;
	info.si_addr = (void __user *)instruction_pointer(regs);

	force_sig_info(SIGTRAP, &info, tsk);
}

static int break_trap(struct pt_regs *regs, unsigned int instr)
{
	ptrace_break(current, regs);
	return 0;
}

static struct undef_hook arm_break_hook = {
	.instr_mask = 0x0fffffff,
	.instr_val = 0x07f001f0,
	.cpsr_mask = PSR_T_BIT,
	.cpsr_val = 0,
	.fn = break_trap,
};

static struct undef_hook thumb_break_hook = {
	.instr_mask = 0xffff,
	.instr_val = 0xde01,
	.cpsr_mask = PSR_T_BIT,
	.cpsr_val = PSR_T_BIT,
	.fn = break_trap,
};

static int __init ptrace_break_init(void)
{
	register_undef_hook(&arm_break_hook);
	register_undef_hook(&thumb_break_hook);
	return 0;
}

core_initcall(ptrace_break_init);

/*
 * Read the word at offset "off" into the "struct user".  We
 * actually access the pt_regs stored on the kernel stack.
 */
static int ptrace_read_user(struct task_struct *tsk, unsigned long off,
			    unsigned long __user * ret)
{
	unsigned long tmp;

	if (off & 3 || off >= sizeof(struct user))
		return -EIO;

	tmp = 0;
	if (off < sizeof(struct pt_regs))
		tmp = get_user_reg(tsk, off >> 2);

	return put_user(tmp, ret);
}

/*
 * Write the word at offset "off" into "struct user".  We
 * actually access the pt_regs stored on the kernel stack.
 */
static int ptrace_write_user(struct task_struct *tsk, unsigned long off,
			     unsigned long val)
{
	if (off & 3 || off >= sizeof(struct user))
		return -EIO;

	if (off >= sizeof(struct pt_regs))
		return 0;

	return put_user_reg(tsk, off >> 2, val);
}

/*
 * Get all user integer registers.
 */
static int ptrace_getregs(struct task_struct *tsk, void __user * uregs)
{
	struct pt_regs *regs = get_user_regs(tsk);

	return copy_to_user(uregs, regs, sizeof(struct pt_regs)) ? -EFAULT : 0;
}

/*
 * Set all user integer registers.
 */
static int ptrace_setregs(struct task_struct *tsk, void __user * uregs)
{
	struct pt_regs newregs;
	int ret;

	ret = -EFAULT;
	if (copy_from_user(&newregs, uregs, sizeof(struct pt_regs)) == 0) {
		struct pt_regs *regs = get_user_regs(tsk);

		ret = -EINVAL;
		if (valid_user_regs(&newregs)) {
			*regs = newregs;
			ret = 0;
		}
	}

	return ret;
}

/*
 * Get the child FPU state.
 */
static int ptrace_getfpregs(struct task_struct *tsk, void __user * ufp)
{
	return copy_to_user(ufp, &tsk->thread_info->fpstate,
			    sizeof(struct user_fp)) ? -EFAULT : 0;
}

/*
 * Set the child FPU state.
 */
static int ptrace_setfpregs(struct task_struct *tsk, void __user * ufp)
{
	struct thread_info *thread = tsk->thread_info;
	thread->used_cp[1] = thread->used_cp[2] = 1;
	return copy_from_user(&thread->fpstate, ufp,
			      sizeof(struct user_fp)) ? -EFAULT : 0;
}

#ifdef CONFIG_IWMMXT

/*
 * Get the child iWMMXt state.
 */
static int ptrace_getwmmxregs(struct task_struct *tsk, void __user * ufp)
{
	struct thread_info *thread = tsk->thread_info;
	void *ptr = &thread->fpstate;

	if (!test_ti_thread_flag(thread, TIF_USING_IWMMXT))
		return -ENODATA;
	iwmmxt_task_disable(thread);	/* force it to ram */
	/* The iWMMXt state is stored doubleword-aligned.  */
	if (((long)ptr) & 4)
		ptr += 4;
	return copy_to_user(ufp, ptr, 0x98) ? -EFAULT : 0;
}

/*
 * Set the child iWMMXt state.
 */
static int ptrace_setwmmxregs(struct task_struct *tsk, void __user * ufp)
{
	struct thread_info *thread = tsk->thread_info;
	void *ptr = &thread->fpstate;

	if (!test_ti_thread_flag(thread, TIF_USING_IWMMXT))
		return -EACCES;
	iwmmxt_task_release(thread);	/* force a reload */
	/* The iWMMXt state is stored doubleword-aligned.  */
	if (((long)ptr) & 4)
		ptr += 4;
	return copy_from_user(ptr, ufp, 0x98) ? -EFAULT : 0;
}

#endif

static int do_ptrace(int request, struct task_struct *child, long addr,
		     long data)
{
	unsigned long tmp;
	int ret;

	switch (request) {
		/*
		 * read word at location "addr" in the child process.
		 */
	case PTRACE_PEEKTEXT:
	case PTRACE_PEEKDATA:
		ret = access_process_vm(child, addr, &tmp,
					sizeof(unsigned long), 0);
		if (ret == sizeof(unsigned long))
			ret = put_user(tmp, (unsigned long __user *)data);
		else
			ret = -EIO;
		break;

	case PTRACE_PEEKUSR:
		ret =
		    ptrace_read_user(child, addr, (unsigned long __user *)data);
		break;

		/*
		 * write the word at location addr.
		 */
	case PTRACE_POKETEXT:
	case PTRACE_POKEDATA:
		ret = access_process_vm(child, addr, &data,
					sizeof(unsigned long), 1);
		if (ret == sizeof(unsigned long))
			ret = 0;
		else
			ret = -EIO;
		break;

	case PTRACE_POKEUSR:
		ret = ptrace_write_user(child, addr, data);
		break;

		/*
		 * continue/restart and stop at next (return from) syscall
		 */
	case PTRACE_SYSCALL:
	case PTRACE_CONT:
		ret = -EIO;
		if (!valid_signal(data))
			break;
		if (request == PTRACE_SYSCALL)
			set_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
		else
			clear_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
		child->exit_code = data;
		/* make sure single-step breakpoint is gone. */
		child->ptrace &= ~PT_SINGLESTEP;
		ptrace_cancel_bpt(child);
		wake_up_process(child);
		ret = 0;
		break;

		/*
		 * make the child exit.  Best I can do is send it a sigkill.
		 * perhaps it should be put in the status that it wants to
		 * exit.
		 */
	case PTRACE_KILL:
		/* make sure single-step breakpoint is gone. */
		child->ptrace &= ~PT_SINGLESTEP;
		ptrace_cancel_bpt(child);
		if (child->exit_state != EXIT_ZOMBIE) {
			child->exit_code = SIGKILL;
			wake_up_process(child);
		}
		ret = 0;
		break;

		/*
		 * execute single instruction.
		 */
	case PTRACE_SINGLESTEP:
		PRINTK("PTRACE_SINGLESTEP on %d\n", child->pid);
		ret = -EIO;
		if (!valid_signal(data))
			break;
		child->ptrace |= PT_SINGLESTEP;
		clear_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
		child->exit_code = data;
		/* give it a chance to run. */
		wake_up_process(child);
		ret = 0;
		break;

	case PTRACE_DETACH:
		ret = ptrace_detach(child, data);
		break;

	case PTRACE_GETREGS:
		ret = ptrace_getregs(child, (void __user *)data);
		break;

	case PTRACE_SETREGS:
		ret = ptrace_setregs(child, (void __user *)data);
		break;

	case PTRACE_GETFPREGS:
		ret = ptrace_getfpregs(child, (void __user *)data);
		break;

	case PTRACE_SETFPREGS:
		ret = ptrace_setfpregs(child, (void __user *)data);
		break;

#ifdef CONFIG_IWMMXT
	case PTRACE_GETWMMXREGS:
		ret = ptrace_getwmmxregs(child, (void __user *)data);
		break;

	case PTRACE_SETWMMXREGS:
		ret = ptrace_setwmmxregs(child, (void __user *)data);
		break;
#endif

	case PTRACE_GET_THREAD_AREA:
		ret = put_user(child->thread_info->tp_value,
			       (unsigned long __user *)data);
		break;

	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

asmlinkage int sys_ptrace(long request, long pid, long addr, long data)
{
	struct task_struct *child;
	int ret;

	lock_kernel();
	ret = -EPERM;
	if (request == PTRACE_TRACEME) {
		/* are we already being traced? */
		if (current->ptrace & PT_PTRACED)
			goto out;
		ret = security_ptrace(current->parent, current);
		if (ret)
			goto out;
		/* set the ptrace bit in the process flags. */
		current->ptrace |= PT_PTRACED;
		ret = 0;
		goto out;
	}
	ret = -ESRCH;
	read_lock(&tasklist_lock);
	child = find_task_by_pid(pid);
	if (child)
		get_task_struct(child);
	read_unlock(&tasklist_lock);
	if (!child)
		goto out;

	ret = -EPERM;
	if (pid == 1)		/* you may not mess with init */
		goto out_tsk;

	if (request == PTRACE_ATTACH) {
		ret = ptrace_attach(child);
		goto out_tsk;
	}
	ret = ptrace_check_attach(child, request == PTRACE_KILL);
	if (ret == 0)
		ret = do_ptrace(request, child, addr, data);

      out_tsk:
	put_task_struct(child);
      out:
	unlock_kernel();
	return ret;
}

asmlinkage void syscall_trace(int why, struct pt_regs *regs)
{
	unsigned long ip;

	if (!test_thread_flag(TIF_SYSCALL_TRACE))
		return;
	if (!(current->ptrace & PT_PTRACED))
		return;

	/*
	 * Save IP.  IP is used to denote syscall entry/exit:
	 *  IP = 0 -> entry, = 1 -> exit
	 */
	ip = regs->ARM_ip;
	regs->ARM_ip = why;

	/* the 0x80 provides a way for the tracing parent to distinguish
	   between a syscall stop and SIGTRAP delivery */
	ptrace_notify(SIGTRAP | ((current->ptrace & PT_TRACESYSGOOD)
				 ? 0x80 : 0));
	/*
	 * this isn't the same as continuing with a signal, but it will do
	 * for normal use.  strace only continues with a signal if the
	 * stopping signal is not SIGTRAP.  -brl
	 */
	if (current->exit_code) {
		send_sig(current->exit_code, current, 1);
		current->exit_code = 0;
	}
	regs->ARM_ip = ip;
}

/*
 * The following is borrowed from vxWorks
 */

static UINT32 armShiftedRegVal(struct pt_regs *pRegs,	/* pointer to task registers */
			       UINT32 instr,	/* machine instruction */
			       int cFlag	/* value of carry flag */
    )
{
	UINT32 res, shift, rm, rs, shiftType;

	rm = BITS(instr, 0, 3);
	shiftType = BITS(instr, 5, 6);

	if (BITSET(instr, 4)) {
		rs = BITS(instr, 8, 11);
		shift =
		    (rs ==
		     15 ? (UINT32) pRegs->ARM_pc + 8 : pRegs->uregs[rs]) & 0xFF;
	} else
		shift = BITS(instr, 7, 11);

	res = rm == 15 ? (UINT32) pRegs->ARM_pc + (BITSET(instr, 4) ? 12 : 8)
	    : pRegs->uregs[rm];

	switch (shiftType) {
	case 0:		/* LSL */
		res = shift >= 32 ? 0 : res << shift;
		break;

	case 1:		/* LSR */
		res = shift >= 32 ? 0 : res >> shift;
		break;

	case 2:		/* ASR */
		if (shift >= 32)
			shift = 31;
		res = (res & 0x80000000L) ? ~((~res) >> shift) : res >> shift;
		break;

	case 3:		/* ROR */
		shift &= 31;
		if (shift == 0)
			res = (res >> 1) | (cFlag ? 0x80000000L : 0);
		else
			res = (res >> shift) | (res << (32 - shift));
		break;
	}
	return res;

}				/* armShiftedRegVal() */

/* When in thumb mode */

static u16 *thumbGetNpc(u16 instr,	/* the current instruction */
			struct task_struct *child, int *backToArm)
{
	UINT32 pc;		/* current program counter */
	UINT32 nPc;		/* next program counter */
	struct pt_regs *pRegs;	/* registers            */

	u16 instr16 = instr;

	PRINTK("%s:instruction 0x%x\n", __FUNCTION__, instr16);

	/*
	 * Get user registers
	 */

	pRegs = get_user_regs(child);

	pc = (UINT32) pRegs->ARM_pc & ~1;	/* current PC as a UINT32 */
	nPc = pc + 2;		/* Thumb default */

	/*
	 * Now examine the instruction
	 * Following code is derived from the ARM symbolic debugger.
	 */

	switch (BITS(instr, 12, 15)) {
	case 0x0:
	case 0x1:
	case 0x2:
	case 0x3:
	case 0x5:
	case 0x6:
	case 0x7:
	case 0x8:
	case 0x9:
	case 0xA:
	case 0xC:
		/* no effect on PC - next instruction executes */
		PRINTK("no effect on PC\n");
		break;

	case 4:
		if (BITS(instr, 7, 11) == 0x0E) {
			/* BX */
			int rn;

			rn = BITS(instr, 3, 6);
			nPc = rn == 15 ? pc + 4 : pRegs->uregs[rn];
			PRINTK("BX, at 0x%x\n", nPc);
			if (!(nPc & 0x1))
				*backToArm = 1;
			break;
		}

		if (BITSET(instr, 7) && (BITS(instr, 0, 11) & 0xC07) == 0x407) {
			/* do something to pc */
			int rn;
			UINT32 operand;

			rn = BITS(instr, 3, 6);
			operand = rn == 15 ? pc + 4 : pRegs->uregs[rn];
			switch (BITS(instr, 8, 9)) {
			case 0:	/* ADD */
				nPc = pc + 4 + operand;
				break;
			case 1:	/* CMP */
				break;
			case 2:	/* MOV */
				nPc = operand;
				break;
			case 3:	/* BX - already handled */
				PRINTK("BX again ???\n");
				break;
			}
		}
		break;

	case 0xB:
		if (BITS(instr, 8, 11) == 0xD) {
			/* POP {rlist, pc} */
			INT32 offset = 0;
			UINT32 regList, regBit;

			for (regList = BITS(instr, 0, 7); regList != 0;
			     regList &= ~regBit) {
				regBit = regList & (-regList);
				offset += 4;
			}
			nPc = *(UINT32 *) (pRegs->uregs[13] + offset);

			/* don't check for new pc == pc like ARM debugger does */
		}
		break;

	case 0xD:
		{
			/* SWI or conditional branch */
			UINT32 cond;

			cond = (instr >> 8) & 0xF;
			if (cond == 0xF)
				break;	/* SWI */

			/* Conditional branch
			 * Use the same mechanism as armGetNpc() to determine whether
			 * the branch will be taken
			 */
			if (((ccTable[cond] >> (pRegs->ARM_cpsr >> 28)) & 1) ==
			    0)
				break;	/* instruction will not be executed */

			/* branch will be taken */
			nPc = pc + 4 + (((instr & 0x00FF) << 1) |
					(BITSET(instr, 7) ? 0xFFFFFE00 : 0));
		}
		break;

	case 0xE:
		if (BITSET(instr, 11) == 0)
			/* Unconditional branch */
			nPc = pc + 4 + (((instr & 0x07FF) << 1) |
					(BITSET(instr, 10) ? 0xFFFFF000 : 0));
		break;

	case 0xF:
		/* BL */
		PRINTK("BL\n");
		if (BITSET(instr, 11)) {
			/* second half of BL - PC should never be here */

			nPc = pRegs->uregs[14] + ((instr & 0x07FF) << 1);
		} else {
			/* first half of BL */

			UINT32 nextBit;

			if (read_instr(child, pc + 2, &nextBit, 1) != 0) {
				PRINTK("Unable to read second half of BL\n");
				break;
			}
			if ((nextBit & 0xF800) != 0xF800)
				/* Something strange going on */
				break;

			nPc = pc + 4 + (INT32) ((((instr & 0x7FF) << 11) |
						 (nextBit & 0x7FF)) << 10) /
			    (1 << 9);
		}
		break;

	}			/* switch */

	return (u16 *) ((UINT32) nPc);

}				/* thumbGetNpc() */

/* When in ARM mode. the returned next address can be either
 * an ARM or a THUMB zone */

static u32 *armGetNpc(u32 instr,	/* the current instruction */
		      struct task_struct *child)
{
	UINT32 pc;		/* current program counter */
	UINT32 nPc;		/* next program counter */
	struct pt_regs *pRegs;	/* registers            */

	/*
	 * Get user registers
	 */

	pRegs = get_user_regs(child);

	/*
	 * Early versions of this file looked at the PSR to determine whether the
	 * CPU was in ARM state or Thumb state and decode the next instruction
	 * accordingly. This has been removed since there is to be no support for
	 * ARM/Thumb interworking.
	 */

	pc = (UINT32) pRegs->ARM_pc;	/* current PC as a UINT32 */
	nPc = pc + 4;		/* default */

	PRINTK("nPc %x CPSR %lx\n", nPc, pRegs->ARM_cpsr);

	/*
	 * Now examine the instruction
	 * First, check the current condition codes against the condition
	 * field of the instruction since, if this instruction is not going
	 * to be executed, we can return immediately
	 *
	 * The following code is a translation of the code supplied by ARM
	 * for instruction decoding (EAN-26). Note that this version, unlike
	 * the original assembly language version cannot generate unaligned
	 * accesses which might be faulted by some systems.
	 *
	 * Briefly, there are 16 entries in ccTable, one for each possible
	 * value of the condition part of an instruction. Each entry has one
	 * bit for each possible value of the flags in the PSR. The table
	 * entry is extracted using the condition part of the instruction and
	 * the bits are indexed using the value obtained by extracting the
	 * flags from the PSR. If the bit so obtained is 1, the instruction
	 * will be executed.
	 */

	PRINTK("Index %x\n", ((instr >> 28) & 0xF));
	PRINTK("Value %x\n", (ccTable[(instr >> 28) & 0xF]));
	PRINTK("CPSRd %lx\n", (pRegs->ARM_cpsr >> 28) & 0xF);
	PRINTK("Res %x\n",
	       ((ccTable[(instr >> 28) & 0xF] >>
		 ((pRegs->ARM_cpsr >> 28) & 0xF))));

	if (((ccTable[(instr >> 28) & 0xF] >> ((pRegs->ARM_cpsr >> 28) & 0xF)) &
	     1) == 0)
		return (u32 *) nPc;	/* instruction will not be executed */

	/*
	 * This instruction WILL be executed so look at its type
	 * We're looking for anything that affects the PC e.g.
	 *    B
	 *    BL
	 *    any data processing op where PC is the destination
	 *    any LDR with the PC as the destination
	 *    any LDM with the PC in the list of registers to be loaded
	 *
	 * Following code is derived from the ARM symbolic debugger.
	 */

	switch (BITS(instr, 24, 27)) {
	case 1:		/* check for halfword or signed byte load to PC */
		if (BITSET(instr, 4) && BITSET(instr, 7) && BITSET(instr, 20) &&
		    BITS(instr, 5, 6) != 0 && BITS(instr, 12, 15) == 15)
			break;	/* bad instruction */

		/* FALL THROUGH */

	case 0:		/* data processing */
	case 2:
	case 3:
		{
			UINT32 rn, op1, op2, cFlag;

			if (BITS(instr, 12, 15) != 15)
				/* Rd */
				/* operation does not affect PC */
				break;

			if (BITS(instr, 22, 25) == 0 && BITS(instr, 4, 7) == 9)
				/* multiply with PC as destination not allowed */
				break;

			if (BITS(instr, 4, 23) == 0x2FFF1) {
				/* BX */
				rn = BITS(instr, 0, 3);
				nPc = (rn == 15 ? pc + 8 : pRegs->uregs[rn]);
				break;
			}

			if (BITS(instr, 4, 23) == 0x2FFF3) {
				/* BLX */
				rn = BITS(instr, 0, 3);
				nPc = (rn == 15 ? pc + 8 : pRegs->uregs[rn]);
				break;
			}

			cFlag = BITSET(pRegs->ARM_cpsr, 29);
			rn = BITS(instr, 16, 19);
			op1 = rn == 15 ? pc + 8 : pRegs->uregs[rn];

			if (BITSET(instr, 25)) {
				UINT32 immVal, rotate;

				immVal = BITS(instr, 0, 7);
				rotate = 2 * BITS(instr, 8, 11);
				op2 =
				    (immVal >> rotate) | (immVal <<
							  (32 - rotate));
			} else
				op2 = armShiftedRegVal(pRegs, instr, cFlag);

			switch (BITS(instr, 21, 24)) {
			case 0x0:	/* AND */
				nPc = op1 & op2;
				break;
			case 0x1:	/* EOR */
				nPc = op1 ^ op2;
				break;
			case 0x2:	/* SUB */
				nPc = op1 - op2;
				break;
			case 0x3:	/* RSB */
				nPc = op2 - op1;
				break;
			case 0x4:	/* ADD */
				nPc = op1 + op2;
				break;
			case 0x5:	/* ADC */
				nPc = op1 + op2 + cFlag;
				break;
			case 0x6:	/* SBC */
				nPc = op1 - op2 + cFlag;
				break;
			case 0x7:	/* RSC */
				nPc = op2 - op1 + cFlag;
				break;
			case 0x8:	/* TST */
			case 0x9:	/* TEQ */
			case 0xa:	/* CMP */
			case 0xb:	/* CMN */
				break;
			case 0xc:	/* ORR */
				nPc = op1 | op2;
				break;
			case 0xd:	/* MOV */
				nPc = op2;
				break;
			case 0xe:	/* BIC */
				nPc = op1 & ~op2;
				break;
			case 0xf:	/* MVN */
				nPc = ~op2;
				break;
			}
		}
		break;

	case 4:		/* data transfer */
	case 5:
	case 6:
	case 7:
		if (BITSET(instr, 20) && BITS(instr, 12, 15) == 15 &&
		    !BITSET(instr, 22))
			/* load, PC and not a byte load */
		{
			UINT32 rn, cFlag, base;
			INT32 offset;

			rn = BITS(instr, 16, 19);
			base = rn == 15 ? pc + 8 : pRegs->uregs[rn];
			cFlag = BITSET(pRegs->ARM_cpsr, 29);
			offset = BITSET(instr, 25)
			    ? armShiftedRegVal(pRegs, instr, cFlag)
			    : BITS(instr, 0, 11);

			if (!BITSET(instr, 23))	/* down */
				offset = -offset;

			if (BITSET(instr, 24))	/* pre-indexed */
				base += offset;

			/* Can'to do this : nPc = *(INSTR *) base; */
			read_instr(child, base, &nPc, 0);
			PRINTK("nPc1 = %x\n", nPc);

			/*
			 * don't check for nPc == pc like the ARM debugger does but
			 * let the higher level (or user) notice.
			 */
		}
		break;

	case 8:
	case 9:		/* block transfer */
		if (BITSET(instr, 20) && BITSET(instr, 15)) {	/* loading PC */
			UINT32 rn;
			INT32 offset = 0;

			rn = BITS(instr, 16, 19);
			if (BITSET(instr, 23)) {	/* up */
				UINT32 regBit, regList;

				for (regList = BITS(instr, 0, 14); regList != 0;
				     regList &= ~regBit) {
					regBit = regList & (-regList);
					offset += 4;
				}
				if (BITSET(instr, 24))	/* preincrement */
					offset += 4;
			} else /* down */ if (BITSET(instr, 24))	/* predecrement */
				offset = -4;

			/* Can't do this nPc = *(UINT32 *) (pRegs->uregs[rn] + offset); */
			read_instr(child, (pRegs->uregs[rn] + offset), &nPc, 0);
			PRINTK("nPc2 = %x\n", nPc);

			/*
			 * don't check for nPc == pc like the ARM debugger does but
			 * let the higher level (or user) notice.
			 */
		}
		break;

	case 0xA:		/* branch */
	case 0xB:		/* branch & link */
		/*
		 * extract offset, sign extend it and add it to current PC,
		 * adjusting for the pipeline
		 */
		nPc = pc + 8 + ((INT32) (instr << 8) >> 6);
		break;

	case 0xC:
	case 0xD:
	case 0xE:		/* coproc ops */
	case 0xF:		/* SWI */
		break;
	}

	return (u32 *) nPc;

}				/* armGetNpc() */
