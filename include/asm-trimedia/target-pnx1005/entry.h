/* 
 * include/asm-trimedia/target-pnx1005/entry.h
 * defines the macro used in interrupt entry point and exit point for pnx1005
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef _ASM_TRIMEDIA_PNX1005_ENTRY_H
#define _ASM_TRIMEDIA_PNX1005_ENTRY_H

#include <asm/regdef.h>

#ifdef __ASSEMBLY__

/*
 * INTERRUPT_DELAY: any interrupt or exception handler must begin with 2 empty delays.
 */

#define INTERRUPT_DELAY 				\
	(* inserted 4 empty delay *) 			\
	nop, nop, nop, nop, nop, nop, nop, nop;		\
	nop, nop, nop, nop, nop, nop, nop, nop;		\
	nop, nop, nop, nop, nop, nop, nop, nop;		\
	nop, nop, nop, nop, nop, nop, nop, nop;		

/*
* PRE_SAVE_ALL is used to save the status registers to temp registers so that 
* SWITCH_STACK can be used to switch stack safely.
*/
#define PRE_SAVE_ALL 					\
	(* cycle 0 *) 					\
	IF r1   readpcsw -> REG_INTR_STATUS_PCSW,       \
	IF r1   iadd r0 r4 -> REG_INTR_STATUS_SP,       \
	IF r1   nop,                             	\
	IF r1   nop,             			\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 1 *) 					\
	IF r1   readspc -> REG_INTR_STATUS_SPC,         \
	IF r1   iadd r0 r3 -> REG_INTR_STATUS_FP,       \
	IF r1   iadd r0 r2 -> REG_INTR_STATUS_RP,       \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 2 *) 					\
	IF r1   readdpc -> REG_INTR_STATUS_DPC,        	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 

/*
* SWITCH_STACK is used to switch the stack from userspace stack to kernel space stack
*    extern struct thread_info* __current_thread_info is the pointer to the current thread
*    which is only changed in __switch_to
*/
#define SWITCH_STACK 							\
	(* cycle 0 *) 							\
	IF r1   imm32(SYMBOL_NAME(__current_thread_info)) -> REG_temp1, \
	IF r1   nop, 							\
	IF r1   asli(PAGE_SIZE_ORDER_PLUS_THREAD_SIZE_ORDER) r1 -> REG_temp3,\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 1 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(0) REG_temp1 -> REG_temp2, 			\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 2 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 3 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 4 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 5 *) 							\
	IF r1   iadd REG_temp2 REG_temp3 -> REG_temp1,                 	\
	IF r1   nop, 							\
	IF r1   ugtr REG_temp2 r4 -> REG_temp3,                       	\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 6 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ugtr r4 REG_temp1 -> REG_temp2,   			\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 7 *) 							\
	IF r1   bitor32 REG_temp3 REG_temp2 -> REG_temp4,     		\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 8 *) 							\
	IF r1   nop,                          				\
	IF r1   nop,             					\
	IF r1   nop,                          				\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 9 *) 							\
	IF REG_temp4   iadd r0 REG_temp1 -> r4, 			\
	IF REG_temp4   iadd r0 REG_temp1 -> r3, 			\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 

/*
*SAVE_ALL save all the necessary register to kernel stack
*/
#define SAVE_ALL 					\
	(* cycle 0 *) 					\
	IF r1   imm32(SIZEOF_STRUCT_PT_REGS) -> REG_temp1,\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 1 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   isub r4 REG_temp1 -> REG_temp2,        	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 2 *) 					\
	IF r1   nop,  					\
	IF r1   h_st32d(160) r63 REG_temp2,            	\
	IF r1   h_st32d(156) r62 REG_temp2,             \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 3 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(152) r61 REG_temp2,             \
	IF r1   h_st32d(148) r60 REG_temp2,             \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 4 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(144) r59 REG_temp2,             \
	IF r1   h_st32d(140) r58 REG_temp2,             \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 5 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(136) r57 REG_temp2,            	\
	IF r1   h_st32d(132) r56 REG_temp2,           	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 6 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(128) r55 REG_temp2,             \
	IF r1   h_st32d(124) r54 REG_temp2,             \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 7 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(120) r53 REG_temp2,             \
	IF r1   h_st32d(116) r52 REG_temp2,             \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 8 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(112) r51 REG_temp2,             \
	IF r1   h_st32d(108) r50 REG_temp2,             \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 9 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(104) r49 REG_temp2,            	\
	IF r1   h_st32d(100) r48 REG_temp2,             \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 10 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(96) r47 REG_temp2,              \
	IF r1   h_st32d(92) r46 REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 11 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(88) r45 REG_temp2,             	\
	IF r1   h_st32d(84) r44 REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 12 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(80) r43 REG_temp2,              \
	IF r1   h_st32d(76) r42 REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 13 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(72) r41 REG_temp2,              \
	IF r1   h_st32d(68) r40 REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 14 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(64) r39 REG_temp2,             	\
	IF r1   h_st32d(60) r38 REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 15 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(56) r37 REG_temp2,              \
	IF r1   h_st32d(52) r36 REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 16 *) 					\
	IF r1   nop,                             	\
	IF r1   h_st32d(48) r35 REG_temp2,              \
	IF r1   h_st32d(44) r34 REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 17 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(40) r33 REG_temp2,             	\
	IF r1   h_st32d(36) r8 REG_temp2,               \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 18 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(32) r7 REG_temp2,               \
	IF r1   h_st32d(28) r6 REG_temp2,               \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 19 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(24) r5 REG_temp2,               \
	IF r1   h_st32d(20) REG_INTR_STATUS_PCSW REG_temp2,\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 20 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(16) REG_INTR_STATUS_DPC REG_temp2,\
	IF r1   h_st32d(12) REG_INTR_STATUS_SPC REG_temp2,\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 21 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(8) REG_INTR_STATUS_RP REG_temp2,\
	IF r1   h_st32d(4) REG_INTR_STATUS_FP REG_temp2,\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 22 *) 					\
	IF r1   nop, 					\
	IF r1   h_st32d(0) REG_INTR_STATUS_SP REG_temp2,\
	IF r1   isub r4 REG_temp1 -> r4, 		\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 
 
/* 
* RESTORE_ALL restore all the necessary registers, 
* so that the context can be continued from the interrupt point.
*/
#define RESTORE_ALL 					\
	(* cycle 0 *) 					\
	IF r1   nop,                       		\
	IF r1   nop, 					\
	IF r1   isubi(0x1) r0 -> REG_temp2,             \
	IF r1   ld32d(20) r4 -> REG_temp1,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 1 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(24) r4 -> r5,                     \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 2 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(28) r4 -> r6,                    	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 3 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(32) r4 -> r7,                    	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 4 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(36) r4 -> r8,                    	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 5 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(40) r4 -> r33,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 6 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(44) r4 -> r34,                    \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 7 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(48) r4 -> r35,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 8 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(52) r4 -> r36,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 9 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(56) r4 -> r37,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 10 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(60) r4 -> r38,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 11 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(64) r4 -> r39,                    \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 12 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(68) r4 -> r40,                    \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 13 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(72) r4 -> r41,                    \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 14 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(76) r4 -> r42,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 15 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(80) r4 -> r43,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 16 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(84) r4 -> r44,                    \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 17 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(88) r4 -> r45,                    \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 18 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(92) r4 -> r46,                    \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 19 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(96) r4 -> r47,                   	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 20 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(100) r4 -> r48,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 21 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(104) r4 -> r49,                  	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 22 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(108) r4 -> r50,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 23 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(112) r4 -> r51,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 24 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(116) r4 -> r52,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 25 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(120) r4 -> r53,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 26 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(124) r4 -> r54,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 27 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(128) r4 -> r55,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 28 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(132) r4 -> r56,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 29 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(136) r4 -> r57,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 30 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(140) r4 -> r58,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 31 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(144) r4 -> r59,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 32 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(148) r4 -> r60,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 33 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(152) r4 -> r61,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 34 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(156) r4 -> r62,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 35 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(160) r4 -> r63,                   \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 36 *) 					\
	IF r1   writepcsw REG_temp1 REG_temp2,         	\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 37 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 38 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(16) r4 -> REG_temp1,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 39 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(12) r4 -> REG_temp2,              \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 40 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop,                    		\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 41 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(8) r4 -> r2,     			\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 42 *) 					\
	IF r1   nop,                  			\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   ld32d(4) r4 -> r3,   			\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 43 *) 					\
	IF r1   writedpc REG_temp1, 			\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 44 *) 					\
	IF r1   writespc REG_temp2, 			\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 45 *) 					\
	IF r1   nop,               			\
	IF r1   nop,              			\
	IF r1   nop, 					\
	IF r1   ld32d(0) r4 -> r4, 			\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 46 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 47 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 48 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 49 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 50 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 51 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 52 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 53 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop;

/*
 * JUMP_TO_DPC: used to return from interrupt or switch_to
 */
#define JUMP_TO_DPC 				\
	(* cycle 0 *) 				\
	IF r1   readdpc -> REG_temp,            \
	IF r1   nop,                       	\
	IF r1   nop,                        	\
	IF r1   nop,                		\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 1 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop,                    	\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 2 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 3 *) 				\
	IF r1   ijmpt r1 REG_temp,              \
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 4 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 5 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 6 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 7 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 8 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 				\
 						\
	(* cycle 9 *) 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop, 				\
	IF r1   nop; 

#define DISABLE_IRQS 					\
	(* cycle 0 *) 					\
	IF r1   imm32(0x400) -> REG_temp,               \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 1 *) 					\
	IF r1   writepcsw r0 REG_temp,                  \
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop;

/*
* Common interrupt(trap, device interrupt, exception) entry point
*/
#define COMMON_INTR_ENTRY 	\
	INTERRUPT_DELAY 	\
	PRE_SAVE_ALL 		\
	SWITCH_STACK 		\
	SAVE_ALL

/*
* Common interrupt exit point
*/
#define COMMON_INTR_EXIT      	\
	INTERRUPT_DELAY 	\
	RESTORE_ALL 		\
	JUMP_TO_DPC 

#endif /*__ASSEMBLY__*/

#endif /*_ASM_TRIMEDIA_PNX1500_ENTRY_H*/

