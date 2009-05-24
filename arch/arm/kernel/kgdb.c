/*
 * arch/arm/kernel/kgdb.c
 *
 * ARM KGDB support
 *
 * Copyright (c) 2002-2004 MontaVista Software, Inc
 *
 * Authors:  George Davis <davis_g@mvista.com>
 *           Deepak Saxena <dsaxena@plexity.net>
 */
#include <linux/config.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/elf.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kgdb.h>

#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/ptrace.h>
#include <asm/traps.h>

/* <WRS_ADDED> */
#include <asm/cacheflush.h>

/* Right now, we only support ARM mode */
typedef unsigned long INSTR;	/* 32 bit word-aligned instructions */
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
#endif /* DEBUG_IT */

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

static INSTR *stepped_address = 0;
static UINT32 stepped_opcode = 0;
static UINT32 stepped_cpsr_it_mask = 0;
static INSTR *armGetNpc(INSTR instr,	/* the current instruction */
			struct pt_regs *pRegs	/* pointer to task registers */
    );

/* </WRS_ADDED> */

/* Make a local copy of the registers passed into the handler (bletch) */
void regs_to_gdb_regs(unsigned long *gdb_regs, struct pt_regs *kernel_regs)
{
	int regno;

	/* Initialize all to zero (??) */
	for (regno = 0; regno < GDB_MAX_REGS; regno++)
		gdb_regs[regno] = 0;

	gdb_regs[_R0] = kernel_regs->ARM_r0;
	gdb_regs[_R1] = kernel_regs->ARM_r1;
	gdb_regs[_R2] = kernel_regs->ARM_r2;
	gdb_regs[_R3] = kernel_regs->ARM_r3;
	gdb_regs[_R4] = kernel_regs->ARM_r4;
	gdb_regs[_R5] = kernel_regs->ARM_r5;
	gdb_regs[_R6] = kernel_regs->ARM_r6;
	gdb_regs[_R7] = kernel_regs->ARM_r7;
	gdb_regs[_R8] = kernel_regs->ARM_r8;
	gdb_regs[_R9] = kernel_regs->ARM_r9;
	gdb_regs[_R10] = kernel_regs->ARM_r10;
	gdb_regs[_FP] = kernel_regs->ARM_fp;
	gdb_regs[_IP] = kernel_regs->ARM_ip;
	gdb_regs[_SP] = kernel_regs->ARM_sp;
	gdb_regs[_LR] = kernel_regs->ARM_lr;
	gdb_regs[_PC] = kernel_regs->ARM_pc;
	gdb_regs[_CPSR] = kernel_regs->ARM_cpsr;
}

/* Copy local gdb registers back to kgdb regs, for later copy to kernel */
void gdb_regs_to_regs(unsigned long *gdb_regs, struct pt_regs *kernel_regs)
{
	kernel_regs->ARM_r0 = gdb_regs[_R0];
	kernel_regs->ARM_r1 = gdb_regs[_R1];
	kernel_regs->ARM_r2 = gdb_regs[_R2];
	kernel_regs->ARM_r3 = gdb_regs[_R3];
	kernel_regs->ARM_r4 = gdb_regs[_R4];
	kernel_regs->ARM_r5 = gdb_regs[_R5];
	kernel_regs->ARM_r6 = gdb_regs[_R6];
	kernel_regs->ARM_r7 = gdb_regs[_R7];
	kernel_regs->ARM_r8 = gdb_regs[_R8];
	kernel_regs->ARM_r9 = gdb_regs[_R9];
	kernel_regs->ARM_r10 = gdb_regs[_R10];
	kernel_regs->ARM_fp = gdb_regs[_FP];
	kernel_regs->ARM_ip = gdb_regs[_IP];
	kernel_regs->ARM_sp = gdb_regs[_SP];
	kernel_regs->ARM_lr = gdb_regs[_LR];
	kernel_regs->ARM_pc = gdb_regs[_PC];
	kernel_regs->ARM_cpsr = gdb_regs[GDB_MAX_REGS - 1];
}

static inline struct pt_regs *kgdb_get_user_regs(struct task_struct *task)
{
	return (struct pt_regs *)
	    ((unsigned long)task->thread_info + THREAD_SIZE -
	     8 - sizeof(struct pt_regs));
}

void sleeping_thread_to_gdb_regs(unsigned long *gdb_regs,
				 struct task_struct *task)
{
	int regno;
	struct pt_regs *thread_regs;

	/* Just making sure... */
	if (task == NULL)
		return;

	/* Initialize to zero */
	for (regno = 0; regno < GDB_MAX_REGS; regno++)
		gdb_regs[regno] = 0;

	/* Otherwise, we have only some registers from switch_to() */
	thread_regs = kgdb_get_user_regs(task);
	gdb_regs[_R0] = thread_regs->ARM_r0;	/* Not really valid? */
	gdb_regs[_R1] = thread_regs->ARM_r1;	/* "               " */
	gdb_regs[_R2] = thread_regs->ARM_r2;	/* "               " */
	gdb_regs[_R3] = thread_regs->ARM_r3;	/* "               " */
	gdb_regs[_R4] = thread_regs->ARM_r4;
	gdb_regs[_R5] = thread_regs->ARM_r5;
	gdb_regs[_R6] = thread_regs->ARM_r6;
	gdb_regs[_R7] = thread_regs->ARM_r7;
	gdb_regs[_R8] = thread_regs->ARM_r8;
	gdb_regs[_R9] = thread_regs->ARM_r9;
	gdb_regs[_R10] = thread_regs->ARM_r10;
	gdb_regs[_FP] = thread_regs->ARM_fp;
	gdb_regs[_IP] = thread_regs->ARM_ip;
	gdb_regs[_SP] = thread_regs->ARM_sp;
	gdb_regs[_LR] = thread_regs->ARM_lr;
	gdb_regs[_PC] = thread_regs->ARM_pc;
	gdb_regs[_CPSR] = thread_regs->ARM_cpsr;
}

static int compiled_break;
static struct undef_hook kgdb_brkpt_hook;

int kgdb_arch_handle_exception(int exception_vector, int signo,
			       int err_code, char *remcom_in_buffer,
			       char *remcom_out_buffer,
			       struct pt_regs *linux_regs)
{
	long addr;
	char *ptr;
	int error;

	switch (remcom_in_buffer[0]) {
	case 'D':
	case 'k':
	case 'c':
		kgdb_contthread = NULL;

		/*
		 * Try to read optional parameter, pc unchanged if no parm.
		 * If this was a compiled breakpoint, we need to move
		 * to the next instruction or we will just breakpoint
		 * over and over again.
		 */
		ptr = &remcom_in_buffer[1];
		if (kgdb_hex2long(&ptr, &addr)) {
			linux_regs->ARM_pc = addr;
		} else if (compiled_break == 1) {
			linux_regs->ARM_pc += 4;
		}

		compiled_break = 0;

		return 0;
	case 's':
		{
			INSTR *next_addr;
			UINT32 currentInst;

			/* 
			 * Do a software step. We assume that the host debuuger has
			 * already REMOVED the breakpoint, so if we read the memory, we
			 * have the REAL instruction.
			 */
			PRINTK("KGDB: s command\n");

			/* Try to read optional parameter, PC unchanged if none */
			ptr = &remcom_in_buffer[1];
			if (kgdb_hex2long(&ptr, &addr))
				linux_regs->ARM_pc = addr;

			atomic_set(&cpu_doing_single_step, -1);

			/* Use memprobe in case read fails...*/
			kgdb_may_fault = 1;
			if ((kgdb_fault_setjmp(kgdb_fault_jmp_regs)) != 0) {
				kgdb_may_fault = 0;
			return -EINVAL;
			}
			
			/* Read the current instruction at the PC */
			currentInst = *(UINT32 *) linux_regs->ARM_pc;

			PRINTK("KGDB current pc %x %x\n", \
			       linux_regs->ARM_pc, currentInst);

			/* Compute the next address */
			next_addr = armGetNpc(currentInst, linux_regs);
			stepped_address = next_addr;

			PRINTK("KGDB next pc %x\n", next_addr);
			
			/* Saves original instruction */
			if ((error = kgdb_get_mem((char *)next_addr,
						  (unsigned char *) &stepped_opcode,
						  BREAK_INSTR_SIZE)) < 0)
			    	{
				PRINTK("Unable to access opcode at next pc 0x%x\n", \
					(int)next_addr);
				return error;
				break;
				}
				
			/* Sets the temporary breakpoint */
			if ((error = kgdb_set_mem((char *)next_addr, 
					          (char*)&kgdb_brkpt_hook.instr_val,
				                  BREAK_INSTR_SIZE)) < 0)
			    	{
				PRINTK("Unable to write tmp BP at next pc 0x%x\n", \
					(int)next_addr);
				return error;
				break;
				}

			kgdb_may_fault = 0;
			
			/*
			 * Store the value of F & I bit
			 * in order to restore them later.
			 *
			 * Mask the interrupts
			 */
			stepped_cpsr_it_mask =
			    linux_regs->ARM_cpsr & (PSR_F_BIT | PSR_I_BIT);
			linux_regs->ARM_cpsr |= PSR_F_BIT | PSR_I_BIT;

			/* Flush and return */
			flush_icache_range((long)next_addr,
					   (long)next_addr + 4);
			if (kgdb_contthread)
				atomic_set(&cpu_doing_single_step,
					   smp_processor_id());
			return 0;
			break;
		}
	}

	return -1;
}

static int kgdb_brk_fn(struct pt_regs *regs, unsigned int instr)
{
	int error;

	/* If we have been single-stepping, put back the old instruction.
	 * We use stepped_address in case we have stopped more than one
	 * instruction away. */
	if (stepped_opcode != 0) {
		PRINTK("Step done at %x putting back %x\n", \
		       stepped_address, stepped_opcode);
		/* restores original instruction */
		
		if ((error = kgdb_set_mem((char *)stepped_address,(
				          unsigned char *) &stepped_opcode,
				          BREAK_INSTR_SIZE)) < 0)
			{
			PRINTK("Unable to restore original instruction\n");
			return 1;
			}	
		
		flush_icache_range((long)stepped_address,
				   (long)stepped_address + 4);
		/* Restore original interrupts in cpsr regs */
		regs->ARM_cpsr &= ~(PSR_F_BIT | PSR_I_BIT);	/* Clean the I & F bits */
		regs->ARM_cpsr |= stepped_cpsr_it_mask;	/* Add the original values */
	}
	stepped_opcode = 0;
	kgdb_handle_exception(1, SIGTRAP, 0, regs);

	return 0;
}

static int kgdb_compiled_brk_fn(struct pt_regs *regs, unsigned int instr)
{
	compiled_break = 1;
	kgdb_handle_exception(1, SIGTRAP, 0, regs);

	return 0;
}

static struct undef_hook kgdb_brkpt_hook = {
	.instr_mask = 0xffffffff,
	.instr_val = KGDB_BREAKINST,
	.fn = kgdb_brk_fn
};

static struct undef_hook kgdb_compiled_brkpt_hook = {
	.instr_mask = 0xffffffff,
	.instr_val = KGDB_COMPILED_BREAK,
	.fn = kgdb_compiled_brk_fn
};

/*
 * Register our undef instruction hooks with ARM undef core.
 * We regsiter a hook specifically looking for the KGB break inst
 * and we handle the normal undef case within the do_undefinstr
 * handler.
 */
int kgdb_arch_init(void)
{
	register_undef_hook(&kgdb_brkpt_hook);
	register_undef_hook(&kgdb_compiled_brkpt_hook);

	return 0;
}

struct kgdb_arch arch_kgdb_ops = {
#ifndef __ARMEB__
	.gdb_bpt_instr = {0xfe, 0xde, 0xff, 0xe7}
#else
	.gdb_bpt_instr = {0xe7, 0xff, 0xde, 0xfe}
#endif
};

/* Copyright (c) 1996-2001 Wind River Systems, Inc. */ 
/* From dbgArmLib.c */
/* <WRS_ADDED> */
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

static INSTR *armGetNpc(INSTR instr,	/* the current instruction */
			struct pt_regs *pRegs	/* pointer to task registers */
    )
{
	UINT32 pc;		/* current program counter */
	UINT32 nPc;		/* next program counter */

	/*
	 * Early versions of this file looked at the PSR to determine whether the
	 * CPU was in ARM state or Thumb state and decode the next instruction
	 * accordingly. This has been removed since there is to be no support for
	 * ARM/Thumb interworking.
	 */

	pc = (UINT32) pRegs->ARM_pc;	/* current PC as a UINT32 */
	nPc = pc + 4;		/* default */

	PRINTK("nPc %x CPSR %x\n", nPc, pRegs->ARM_cpsr);

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
	PRINTK("CPSRd %x\n", (pRegs->ARM_cpsr >> 28) & 0xF);
	PRINTK(KERN_ALERT "Res %x\n",	\
	       ((ccTable[(instr >> 28) & 0xF] >> \
		 ((pRegs->ARM_cpsr >> 28) & 0xF))));

	if (((ccTable[(instr >> 28) & 0xF] >> ((pRegs->ARM_cpsr >> 28) & 0xF)) &
	     1) == 0)
		return (INSTR *) nPc;	/* instruction will not be executed */

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
				nPc =
				    (rn == 15 ? pc + 8 : pRegs->uregs[rn]) & ~1;
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

			nPc = *(INSTR *) base;

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

			nPc = *(UINT32 *) (pRegs->uregs[rn] + offset);

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

	return (INSTR *) nPc;

}				/* armGetNpc() */
/* </WRS_ADDED> */
