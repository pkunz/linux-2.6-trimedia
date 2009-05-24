/* 
 *  linux/fs/binfmt_tcs.c
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#define USE_TCS_MKTIME
#include <linux/module.h>
#include <linux/string.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/binfmts.h>
#include <linux/elf.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/errno.h>
#include <linux/personality.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/ptrace.h>
#include <linux/resource.h>
#include <linux/sched.h>

#include <asm/byteorder.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/unaligned.h>
#include <asm/cacheflush.h>
#include <asm/semaphore.h>

#include <IODrivers.h>
#include <tmlib/tmtypes.h>
#include <tmlib/DynamicLoader.h>

#undef DYNLOAD_DEBUG

#ifndef DYNLOAD_DEBUG
#define DEBUGP(fmt,args...)
#else
#define DEBUGP  printk
#endif 

MODULE_DESCRIPTION("Module to Load TriMedia excutable applications");
MODULE_AUTHOR("camelguo@gmail.com");

struct kmem_cache_t *dynload_module_cache;

static int load_tcs_binary(struct linux_binprm * bprm, struct pt_regs * regs);
static int tcs_core_dump(struct linux_binprm * bprm, struct pt_regs* regs, struct file* file);

#define TCS_EXEC_PAGESIZE   PAGE_SIZE

static struct linux_binfmt tcs_format = {
	.module		= THIS_MODULE,
	.load_binary	= load_tcs_binary,
	.core_dump 	= tcs_core_dump,
	.min_coredump   = TCS_EXEC_PAGESIZE,
};

static UID_Driver tcs_fs_driver;

/*err when call sys_open/close/lseek/read */
static int err_syscall = 0;

static inline int DynLoad_translate_err(DynLoad_Status stat)
{
	int ret=-EFAULT;

	/*
	 * if error report by sys_open/close/lseek/read,
	 * DO NOT translate it.
	 * */
	if(err_syscall!=0) {
		ret = -err_syscall;
		err_syscall = 0;
		return ret;
	}

	switch(stat){
		case DynLoad_OK:	
			ret=0;
			break;
		case DynLoad_FileNotFound:
		case DynLoad_NotPresent:
			ret=-ENOENT;
			break;
		case DynLoad_InsufficientMemory:
		case DynLoad_StackOverflow:
			ret=-ENOMEM;
			break;
		case DynLoad_UnknownObjectVersion:
		case DynLoad_WrongEndian:
		case DynLoad_WrongChecksum:
		case DynLoad_NotUnloadable:
		case DynLoad_UnresolvedSymbol:
		case DynLoad_NotADll:
		case DynLoad_NotAnApp:
		case DynLoad_InconsistentObject:
			ret=-ENOEXEC;
			break;
		case DynLoad_StillReferenced:
			ret=-EFAULT;
			break;
		default:
			printk(KERN_ERR "Unknown DynLoad status %d\n",stat);
			break;
	}

	return ret;
}

/****************************************************************************/

/*
 * Routine writes a core dump image in the current directory.
 * Currently only a stub-function.
 *
 * Note that setuid/setgid files won't make a core-dump if the uid/gid
 * changed due to the set[u|g]id. It's enforced by the "current->mm->dumpable"
 * field, which also makes sure the core-dumps won't be recursive if the
 * dumping of the process results in another error..
 */
static int tcs_core_dump(struct linux_binprm * bprm, struct pt_regs* regs, struct file* file)
{
	int has_dumped = 0;

	/*TODO: write useful information about user process to core dump file.*/
	has_dumped = 1;

	return has_dumped;
}

#define USERSPACE_STACK_SIZE   (PAGE_SIZE*4)

DECLARE_MUTEX(dynload_sem);

static int load_tcs_file(struct linux_binprm * bprm,unsigned long *extra_stack)
{
	unsigned long stack_len;
	unsigned long stack_start;
	unsigned long start_code, end_code;
	unsigned long result;
	int ret=0;
	struct dynload_module* dynmodule;
	DynLoad_Status load_status=DynLoad_OK;

	dynmodule=kmem_cache_alloc(dynload_module_cache,GFP_KERNEL);
	if(dynmodule==NULL){
		DEBUGP("Unable to allocate dynload module from SLAB cache!\n");
		return -ENOMEM;
	}
	atomic_set(&dynmodule->count,0);
	dynmodule->module=NULL;

	down(&dynload_sem);
	load_status = DynLoad_load_application(bprm->interp,&dynmodule->module);
	up(&dynload_sem);
	if(load_status!=DynLoad_OK){
		DEBUGP("failed to load '%s' with status %d\n",
				bprm->filename,load_status);
		ret=DynLoad_translate_err(load_status);
		goto free_dynload;
	}

	/* Flush all traces of the currently running executable */
	ret = flush_old_exec(bprm);
	if (ret != 0) {
		DEBUGP("flush old exec failed, with return %d\n",ret);
		send_sig(SIGKILL, current, 0);
		goto unload_app;
	}
	
	stack_len = USERSPACE_STACK_SIZE; 
	if (extra_stack) {
		stack_len += *extra_stack;
		*extra_stack = stack_len;
	}

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
		DEBUGP("Unable to allocate RAM for process text/data, errno %d\n",
				(int)-stack_start);
		ret = stack_start;
		send_sig(SIGKILL, current, 0);
		goto unload_app;
	}

	/* OK, This is the point of no return */
	set_personality(PER_LINUX);

	get_dynload_module(dynmodule);
	current->thread.dynmodule=dynmodule;

	/* The main program needs a little extra setup in the task structure */
	start_code = (unsigned long)(dynmodule->module->start);
	/*
	 *FIXME: I don't know the executable file format, so ......
	 */
	end_code = start_code + PAGE_SIZE*2;

	current->mm->start_code = start_code;
	current->mm->end_code = end_code;
	/*
	 *FIXME: I don't know the executable file format, so ......
	 */
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

unload_app:
	down(&dynload_sem);
	DynLoad_unload_application(dynmodule->module);
	up(&dynload_sem);
free_dynload:
	kmem_cache_free(dynload_module_cache,dynmodule);
	return ret;
}

/****************************************************************************/
/*
 * create_tcs_tables() parses the env- and arg-strings in new user
 * memory and creates the pointer tables from them, and puts their
 * addresses on the "stack", returning the new stack pointer value.
 */
#define tcs_stack_align(sp) (sp)

static unsigned long create_tcs_tables(
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

	tcs_stack_align(sp);

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
static int load_tcs_binary(struct linux_binprm * bprm, struct pt_regs * regs)
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

	res = load_tcs_file(bprm, &stack_len);
	if (res > (unsigned long)-4096)
		return res;
	
	compute_creds(bprm);
 	current->flags &= ~PF_FORKNOEXEC;

	set_binfmt(&tcs_format);

	p = ((current->mm->context.end_brk + stack_len + 3) & ~3) - 4;

	/* copy the arg pages onto the stack, this could be more efficient :-) */
	for (i = TOP_OF_ARGS - 1; i >= bprm->p; i--)
		* (char *) --p =
			((char *) page_address(bprm->page[i/PAGE_SIZE]))[i % PAGE_SIZE];

	sp = (unsigned long *) create_tcs_tables(p, bprm);
	
	start_addr = current->mm->start_code; 
	/* Stash our initial stack pointer into the mm structure */
	current->mm->start_stack = (unsigned long )sp;

	/*printk("process %d:'%s' load at 0x%08x\n",current->pid,current->comm, start_addr);*/
	start_thread(regs, start_addr, current->mm->start_stack);

	if (current->ptrace & PT_PTRACED)
		send_sig(SIGTRAP, current, 0);

	return 0;
}

extern Int _rts_trace_dynldr;

static unsigned char*
temp_malloc(unsigned long size)
{
	unsigned char* addr=NULL;
	addr=kmalloc(size,GFP_KERNEL);
	WARN_ON(addr==NULL);
	return addr;
}

static void
temp_free(unsigned char* addr)
{
	kfree(addr);
	return;
}

static unsigned char*
perm_malloc(unsigned long size)
{
	unsigned char* addr=NULL;
	addr=kmalloc(PAGE_ALIGN(size),GFP_KERNEL);
	WARN_ON(addr==NULL);
	return addr;
}

static void
perm_free(unsigned char* addr)
{
	kfree(addr);
	return;
}

static void
error_handler(DynLoad_Status status,String name)
{
	return;
}

static Bool
DynLoad_InitFunc(void)
{
	return True;
}

static Bool
DynLoad_TermFunc(void)
{
	return True;
}

static Bool
DynLoad_RecogFunc(String path)
{
	return True;
}

static int
DynLoad_OpenFunc(String path, Int32 oflag, Int32 mode)
{
	int fd=-1;
	fd=sys_open(path,O_RDONLY,mode);
	if(fd<0){
		err_syscall = -fd;
		fd=-1;
		goto out;
	}
	err_syscall = 0;
out:
	return fd;
}

static int
DynLoad_OpenDllFunc(String path)
{
	return -1;
}

static int
DynLoad_CloseFunc(int fd)
{
	int ret=-1;
	ret=sys_close(fd);
	if(ret!=0){
		err_syscall = -ret;
		ret=-1;
		goto out;	
	}
	err_syscall = 0;
out:
	return ret;
}

static int
DynLoad_ReadFunc(int fd,Address buffer,Int32 size)
{
	int ret=-1;
	ret=sys_read(fd,buffer,size);
	if(ret<0){
		err_syscall = -ret;
		ret=-1;
		goto out;
	}
	err_syscall = 0;
out:
	return ret;
}

static int
DynLoad_SeekFunc(int fd,long offset,int ptrname)
{
	int ret=-1;
	/*
	 *FIXME: the argument 'offset' may not be what I want
	 */
	ret=sys_lseek(fd,offset,ptrname);
	if(ret<0){
		err_syscall = -ret;
		ret=-1;
		goto out;
	}
	err_syscall = 0;
out:
	return ret;
}

static int __init init_tcs_binfmt(void)
{
	int ret=0;
	DynLoad_MallocFun tm=NULL;
	DynLoad_MallocFun pm=NULL;
	DynLoad_FreeFun tf=NULL;
	DynLoad_FreeFun pf=NULL;
	DynLoad_ErrorFun errh=NULL;
	
	tcs_fs_driver=IOD_install_driver(
			       (IOD_RecogFunc) DynLoad_ReadFunc,
			       (IOD_InitFunc) DynLoad_InitFunc,
			       (IOD_TermFunc) DynLoad_TermFunc,
			       (IOD_OpenFunc) DynLoad_OpenFunc,
			       (IOD_OpenDllFunc) DynLoad_OpenDllFunc,
			       (IOD_CloseFunc) DynLoad_CloseFunc,
			       (IOD_ReadFunc) DynLoad_ReadFunc,
			       (IOD_WriteFunc) NULL,
			       (IOD_SeekFunc) DynLoad_SeekFunc,
			       (IOD_IsattyFunc) NULL,
			       (IOD_FstatFunc) NULL,
			       (IOD_FcntlFunc) NULL,
			       (IOD_StatFunc) NULL
			);
	if(tcs_fs_driver==NULL){
		DEBUGP("init_tcs_binfmt: can not register IOD driver!\n");
		ret=-ENOMEM;
		goto out;
	}

	tm= temp_malloc;
	pm= perm_malloc;
	tf= temp_free;
	pf= perm_free;
	DynLoad_swap_mm(&pm,&pf,&tm,&tf);

	errh= error_handler;
	DynLoad_swap_stub_error_handler(&errh);	

	_rts_trace_dynldr=False;	
	
	ret=register_binfmt(&tcs_format);
	if(ret!=0){
		DEBUGP("init_tcs_binfmt: can not register tcs_binfmt!\n");
		goto unregister_dynload;
	}

	dynload_module_cache=kmem_cache_create("dynload_module",
			sizeof(struct dynload_module),0,0,NULL,NULL);
	if(dynload_module_cache==NULL){
		DEBUGP("init_tcs_binfmt: can not allocate dynload module cache!\n");
		ret=-ENOMEM;
		goto unreg_binfmt;
	}
	
	return ret;

unreg_binfmt:
	unregister_binfmt(&tcs_format);
unregister_dynload:
	_rts_trace_dynldr=False;
	DynLoad_swap_stub_error_handler(NULL);
	DynLoad_swap_mm(NULL,NULL,NULL,NULL);
	IOD_uninstall_driver(tcs_fs_driver);
	tcs_fs_driver=NULL;
out:
	return ret;
}

static void __exit exit_tcs_binfmt(void)
{
	kmem_cache_destroy(dynload_module_cache);
	unregister_binfmt(&tcs_format);
	_rts_trace_dynldr=False;
	DynLoad_swap_stub_error_handler(NULL);
	DynLoad_swap_mm(NULL,NULL,NULL,NULL);
	IOD_uninstall_driver(tcs_fs_driver);
	tcs_fs_driver=NULL;
}

core_initcall(init_tcs_binfmt);
module_exit(exit_tcs_binfmt);
MODULE_LICENSE("GPL");
