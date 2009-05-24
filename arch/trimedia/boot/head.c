/* 
 * arch/trimedia/boot/head.c  
 *
 * Copyright (C) 2009 Gulessoft, Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/sched.h>
#include <linux/jiffies.h>

#include <asm/setup.h>
#include <asm/thread_info.h>
#include <asm/page.h>

#include <tmInterrupts.h>
#include <ops/custom_ops.h>
#include <tmlib/dprintf.h>
#include <string.h>

unsigned long _ramstart;
unsigned long _ramend;

extern char __initramfs_start[];
extern unsigned long __initramfs_length;
extern char command_line[];
extern void start_kernel(void);
extern union thread_union* __init_thread_union_ptr;
extern struct thread_info* __current_thread_info;
extern unsigned long long jiffies_64;

#undef TMLINUX_DEBUG  
/*
 *FIXME: to change tmlinux_main when possible
 */
int
main(void)
{
	/*
	 * Disable irq when in start_kernel.
	 */
	writepcsw(0,tmPCSW_IEN);
	
	/*
	 * Copy linux command linux from bootloader to linux kernel.
	 */
	strncpy(command_line,CONFIG_CMDLINE,COMMAND_LINE_SIZE);
	command_line[COMMAND_LINE_SIZE-1]='\0';
	
	/*
	 * Allocate memory for kernel and setup memory information.
	 */
	_ramstart=(unsigned long)malloc(CONFIG_MEMSIZE*1024*1024);
	if(_ramstart==0){
		printf("In kernel start code: can not allocate so much memmory %dM Bytes!\n",CONFIG_MEMSIZE);
		return -1;
	}
	_ramend=_ramstart+(CONFIG_MEMSIZE*1024*1024);
#ifdef  TMLINUX_DEBUG 
	printf("CONFIG_MEMSIZE is 0x%x,_ramstart is 0x%x, _ramend is 0x%x\n",CONFIG_MEMSIZE,_ramstart,_ramend);
#endif 

#ifdef CONFIG_TRIMEDIA_DP
	/*
	 * Init the trimedia DP mechanism if necessary.
	 */
	{
#define DP_MEM_SIZE 		((CONFIG_TRIMEDIA_DPSIZE)*1024)
		char* addr=malloc(DP_MEM_SIZE);
		if(addr==NULL){
			printf("In kernel start code: can not allocate memory for DP!\n");
			return -1;
		}
		memset(addr,0,DP_MEM_SIZE);
		DP_START(DP_MEM_SIZE,addr);
	}
#endif 

	jiffies_64 = INITIAL_JIFFIES;

	/*
	 * Setup init task and init thread_info 
	 */
	__init_thread_union_ptr=(union thread_union*)
		((((unsigned long)__init_thread_union_ptr)+THREAD_SIZE-1)
		 & (~(THREAD_SIZE-1)));

#define __INIT_THREAD_INFO 	(__init_thread_union_ptr->thread_info)
#define __INIT_STACK 		((unsigned long)__init_thread_union_ptr->stack)
	__current_thread_info=&(__INIT_THREAD_INFO); /*Init __current_thread_info*/
	__INIT_THREAD_INFO.task=&init_task;
	__INIT_THREAD_INFO.exec_domain=&default_exec_domain;
	__INIT_THREAD_INFO.flags=0;
	__INIT_THREAD_INFO.cpu=0;
	/*
	 *FIXME: I do not know how to init __INIT_THREAD_INFO.preempt_count.
	 */
	__INIT_THREAD_INFO.restart_block.fn=do_no_restart_syscall;
	__INIT_THREAD_INFO.ksp=(unsigned long)__INIT_STACK+THREAD_SIZE;
	init_task.thread_info=&__init_thread_union_ptr->thread_info;

	goto_tmlinux_kernel(__INIT_STACK+THREAD_SIZE);

#undef  __INIT_STACK
#undef  __INIT_THREAD_INFO
	
	return -1;
}
