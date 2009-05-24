/* 
 *  linux/fs/binfmt_fn.c
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/slab.h>
#include <linux/binfmts.h>
#include <linux/personality.h>
#include <linux/resource.h>
#include <linux/sched.h>
#include <linux/init.h>

#include <asm/byteorder.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/unaligned.h>
#include <asm/cacheflush.h>

/****************************************************************************/

static int load_fn_binary(struct linux_binprm *, struct pt_regs * regs);

struct linux_binfmt fn_format = {
	.module		= THIS_MODULE,
	.load_binary	= load_fn_binary,
};

/****************************************************************************/
/*
 * create_fn_tables() parses the env- and arg-strings in new user
 * memory and creates the pointer tables from them, and puts their
 * addresses on the "stack", returning the new stack pointer value.
 */
#define fn_stack_align(sp) (sp)

static unsigned long create_fn_tables(
	unsigned long pp,
	struct linux_binprm * bprm)
{
	unsigned long *argv,*envp;
	unsigned long * sp;
	char * p = (char*)pp;
	int argc = bprm->argc;
	int envc = bprm->envc;
	char dummy;

	sp = (unsigned long *) ((-(unsigned long)sizeof(char *))&(unsigned long) p);

	sp -= envc+1;
	envp = sp;
	sp -= argc+1;
	argv = sp;

	fn_stack_align(sp);

	--sp; put_user((unsigned long) envp, sp);
	--sp; put_user((unsigned long) argv, sp);

	put_user(argc,--sp);
	current->mm->arg_start = (unsigned long) p;
	while (argc-->0) {
		put_user((unsigned long) p, argv++);
		do {
			get_user(dummy, p); p++;
		} while (dummy);
	}
	put_user((unsigned long) NULL, argv);
	current->mm->arg_end = current->mm->env_start = (unsigned long) p;
	while (envc-->0) {
		put_user((unsigned long)p, envp); envp++;
		do {
			get_user(dummy, p); p++;
		} while (dummy);
	}
	put_user((unsigned long) NULL, envp);
	current->mm->env_end = (unsigned long) p;
	return (unsigned long)sp;
}

/****************************************************************************/
#define USERSPACE_STACK_SIZE   (PAGE_SIZE*2)

static int load_fn_file(struct linux_binprm * bprm,unsigned long *extra_stack)
{
	unsigned long stack_len;
	unsigned long stack_start;
	unsigned long start_code, end_code;
	unsigned long result;
	unsigned long rlim;

	stack_len = USERSPACE_STACK_SIZE; 
	if (extra_stack) {
		stack_len += *extra_stack;
		*extra_stack = stack_len;
	}

	/*
	 * Check initial limits. This avoids letting people circumvent
	 * size limits imposed on them by creating programs with large
	 * arrays in the data or bss.
	 */
	rlim = current->signal->rlim[RLIMIT_DATA].rlim_cur;
	if (rlim >= RLIM_INFINITY)
		rlim = ~0;

	/* Flush all traces of the currently running executable */
	result = flush_old_exec(bprm);
	if (result)
		return result;

	/* OK, This is the point of no return */
	set_personality(PER_LINUX);

	/*
	 * there are a couple of cases here,  the separate code/data
	 * case,  and then the fully copied to RAM case which lumps
	 * it all together.
	 */
	down_write(&current->mm->mmap_sem);
	stack_start= do_mmap(0, 0,stack_len,
			PROT_READ | PROT_EXEC | PROT_WRITE, MAP_PRIVATE, 0);
	up_write(&current->mm->mmap_sem);
	if (!stack_start|| stack_start >= (unsigned long) -4096) {
		if (!stack_start)
			stack_start = (unsigned long) -ENOMEM;
		printk("Unable to allocate RAM for process text/data, errno %d\n",
				(int)-stack_start);
		return(stack_start);
	}

	/* The main program needs a little extra setup in the task structure */
	start_code = (unsigned long)bprm->filename;
	end_code = start_code + PAGE_SIZE*2;

	current->mm->start_code = start_code;
	current->mm->end_code = end_code;
	current->mm->start_data = 0;
	current->mm->end_data = 0;
	/*
	 * set up the brk stuff, uses any slack left in data/bss/stack
	 * allocation.  We put the brk after the bss (between the bss
	 * and stack) like other platforms.
	 */
	current->mm->start_brk = 0;
	current->mm->brk = 0;
	current->mm->context.end_brk = stack_start;
	set_mm_counter(current->mm, rss, 0);

	flush_icache_range(start_code, end_code);

	memset((unsigned char*)stack_start,0,stack_len);

	return 0;
}

extern void userapp_start(void);

/****************************************************************************/
static int load_fn_binary(struct linux_binprm * bprm, struct pt_regs * regs)
{
	unsigned long p = bprm->p;
	unsigned long stack_len;
	unsigned long start_addr;
	unsigned long *sp;
	unsigned long i;
	int res;

	/*
	 * We have to add the size of our arguments to our stack size
	 * otherwise it's too easy for users to create stack overflows
	 * by passing in a huge argument list.  And yes,  we have to be
	 * pedantic and include space for the argv/envp array as it may have
	 * a lot of entries.
	 */
#define TOP_OF_ARGS (PAGE_SIZE * MAX_ARG_PAGES - sizeof(void *))
	stack_len = TOP_OF_ARGS - bprm->p;             /* the strings */
	stack_len += (bprm->argc + 1) * sizeof(char *); /* the argv array */
	stack_len += (bprm->envc + 1) * sizeof(char *); /* the envp array */

	res = load_fn_file(bprm, &stack_len);
	if (res > (unsigned long)-4096)
		return res;
	
	compute_creds(bprm);
 	current->flags &= ~PF_FORKNOEXEC;

	set_binfmt(&fn_format);

	p = ((current->mm->context.end_brk + stack_len + 3) & ~3) - 4;

	/* copy the arg pages onto the stack, this could be more efficient :-) */
	for (i = TOP_OF_ARGS - 1; i >= bprm->p; i--)
		* (char *) --p =
			((char *) page_address(bprm->page[i/PAGE_SIZE]))[i % PAGE_SIZE];

	sp = (unsigned long *) create_fn_tables(p, bprm);
	
	start_addr = (unsigned long)bprm->filename;

	/* Stash our initial stack pointer into the mm structure */
	current->mm->start_stack = (unsigned long )sp;

	regs->r6=start_addr;

	start_thread(regs, (unsigned long)&userapp_start, current->mm->start_stack);

	if (current->ptrace & PT_PTRACED)
		send_sig(SIGTRAP, current, 0);

	return 0;
}

/****************************************************************************/

static int __init init_fn_binfmt(void)
{
	return register_binfmt(&fn_format);
}

static void __exit exit_fn_binfmt(void)
{
	unregister_binfmt(&fn_format);
}

/****************************************************************************/

core_initcall(init_fn_binfmt);
module_exit(exit_fn_binfmt);

/****************************************************************************/
