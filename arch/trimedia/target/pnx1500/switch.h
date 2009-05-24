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

#include <asm/linkage.h>
#include <asm/regdef.h>
#include <asm/asm-offsets.h>

/*
* GET_PREV_KSP: used in switch_to, get the pointer of struct switch_stack which need to be
* 	saved in current process kernel stack, which is also ksp need to be saved in thread
* 	information.
*       REG_SWTO_PREV_KSP = r4 - sizeof(struct switch_stack)
*/
#define GET_PREV_KSP   							\
	(* cycle 0 *)   						\
	IF r1   uimm(SIZEOF_STRUCT_SWITCH_STACK) -> REG_temp,        	\
	IF r1   nop,            					\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop;   							\
   									\
	(* cycle 1 *)   						\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop;   							\
   									\
	(* cycle 2 *)   						\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop;   							\
   									\
	(* cycle 3 *)   						\
	IF r1   isub r4 REG_temp -> REG_SWTO_PREV_KSP,    		\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop;   

/*
* SAVE_PREV_SWITCH_STACK: used in switch_to, save the prev process's callee saved register 
*  	into struct switch_stack
*/
#define SAVE_PREV_SWITCH_STACK 						\
	(* cycle 0 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   readdpc -> REG_temp1,                          		\
	IF r1   h_st32d(124) r31 REG_SWTO_PREV_KSP,                     \
	IF r1   h_st32d(128) r32 REG_SWTO_PREV_KSP;                     \
 									\
	(* cycle 1 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(116) r29 REG_SWTO_PREV_KSP;                     \
 									\
	(* cycle 2 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(108) r27 REG_SWTO_PREV_KSP,                     \
	IF r1   h_st32d(120) r30 REG_SWTO_PREV_KSP;                     \
 									\
	(* cycle 3 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(100) r25 REG_SWTO_PREV_KSP,                     \
	IF r1   h_st32d(112) r28 REG_SWTO_PREV_KSP;                     \
 									\
	(* cycle 4 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(92) r23 REG_SWTO_PREV_KSP,                      \
	IF r1   h_st32d(104) r26 REG_SWTO_PREV_KSP;                     \
 									\
	(* cycle 5 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(84) r21 REG_SWTO_PREV_KSP,                      \
	IF r1   h_st32d(96) r24 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 6 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(76) r19 REG_SWTO_PREV_KSP,                      \
	IF r1   h_st32d(88) r22 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 7 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(68) r17 REG_SWTO_PREV_KSP,                      \
	IF r1   h_st32d(80) r20 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 8 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(60) r15 REG_SWTO_PREV_KSP,                     	\
	IF r1   h_st32d(72) r18 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 9 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(52) r13 REG_SWTO_PREV_KSP,                      \
	IF r1   h_st32d(64) r16 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 10 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(44) r11 REG_SWTO_PREV_KSP,                      \
	IF r1   h_st32d(56) r14 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 11 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(36) r9 REG_SWTO_PREV_KSP,                       \
	IF r1   h_st32d(48) r12 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 12 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   readpcsw -> REG_temp3,                         		\
	IF r1   h_st32d(28) r7 REG_SWTO_PREV_KSP,                       \
	IF r1   h_st32d(40) r10 REG_SWTO_PREV_KSP;                      \
 									\
	(* cycle 13 *) 							\
	IF r1   nop, 							\
	IF r1   nop,                            			\
	IF r1   nop, 							\
	IF r1   h_st32d(20) REG_temp3 REG_SWTO_PREV_KSP,                \
	IF r1   h_st32d(32) r8 REG_SWTO_PREV_KSP;                       \
 									\
	(* cycle 14 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   readspc -> REG_temp2,                          		\
	IF r1   h_st32d(12) REG_temp1 REG_SWTO_PREV_KSP,                \
	IF r1   h_st32d(24) r6 REG_SWTO_PREV_KSP;                       \
 									\
	(* cycle 15 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(4) r3 REG_SWTO_PREV_KSP,                        \
	IF r1   h_st32d(16) REG_temp2 REG_SWTO_PREV_KSP;                \
 									\
	(* cycle 16 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   h_st32d(0) r2 REG_SWTO_PREV_KSP,                        \
	IF r1   h_st32d(8) r4 REG_SWTO_PREV_KSP;                      

/*
* SAVE_PREV_KSP: save the current ksp to current thread information structure.
*/
#define SAVE_PREV_KSP  							\
	(* cycle 0 *)  							\
	IF r1   nop,  							\
	IF r1   nop,   							\
	IF r1   nop,  							\
	IF r1   h_st32d(OFFSET_KSP_THREAD_INFO) REG_SWTO_PREV_KSP r5,   \
	IF r1   nop;  							\
  									\
	(* cycle 1 *)  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop;  							\
  									\
	(* cycle 2 *)  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop;  							\
  									\
	(* cycle 3 *)  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop;

/*
*LOAD_NEXT_KSP: load the next task's ksp to REG_SWTO_NEXT_KSP from the next thread
* 	information structure.
*/
#define LOAD_NEXT_KSP  							\
	(* cycle 0 *)  							\
	IF r1   nop,  							\
	IF r1   nop,   							\
	IF r1   nop,  							\
	IF r1   ld32d(OFFSET_KSP_THREAD_INFO) r6 -> REG_SWTO_NEXT_KSP,  \
	IF r1   nop;  							\
  									\
	(* cycle 1 *)  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop;  							\
  									\
	(* cycle 2 *)  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop;  							\
  									\
	(* cycle 3 *)  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop;  							\

/*
*RESTORE_NEXT_SWITCH_STACK: restore the next task's switch stack information to cpu.
*/
#define RESTORE_NEXT_SWITCH_STACK  					\
	(* cycle 0 *) 							\
	IF r1   isubi(0x1) r0 -> REG_temp1,                    		\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(20) REG_SWTO_NEXT_KSP -> REG_temp2,     		\
	IF r1   ld32d(12) REG_SWTO_NEXT_KSP -> REG_temp3;      		\
 									\
	(* cycle 1 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(16) REG_SWTO_NEXT_KSP -> REG_temp4,               \
	IF r1   ld32d(0) REG_SWTO_NEXT_KSP -> REG_temp5;                \
 									\
	(* cycle 2 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(4) REG_SWTO_NEXT_KSP -> r3,                   	\
	IF r1   ld32d(8) REG_SWTO_NEXT_KSP -> r4;                   	\
 									\
	(* cycle 3 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   writepcsw REG_temp2 REG_temp1,                      	\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 4 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(32) REG_SWTO_NEXT_KSP -> r8;                  	\
 									\
	(* cycle 5 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(40) REG_SWTO_NEXT_KSP -> r10,                	\
	IF r1   ld32d(28) REG_SWTO_NEXT_KSP -> r7;                 	\
 									\
	(* cycle 6 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(48) REG_SWTO_NEXT_KSP -> r12,                	\
	IF r1   ld32d(36) REG_SWTO_NEXT_KSP -> r9;                 	\
 									\
	(* cycle 7 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(56) REG_SWTO_NEXT_KSP -> r14,                	\
	IF r1   ld32d(44) REG_SWTO_NEXT_KSP -> r11;                	\
 									\
	(* cycle 8 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(64) REG_SWTO_NEXT_KSP -> r16,                	\
	IF r1   ld32d(52) REG_SWTO_NEXT_KSP -> r13;                	\
 									\
	(* cycle 9 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(72) REG_SWTO_NEXT_KSP -> r18,                	\
	IF r1   ld32d(60) REG_SWTO_NEXT_KSP -> r15;                	\
 									\
	(* cycle 10 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(80) REG_SWTO_NEXT_KSP -> r20,               	\
	IF r1   ld32d(68) REG_SWTO_NEXT_KSP -> r17;               	\
 									\
	(* cycle 11 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(88) REG_SWTO_NEXT_KSP -> r22,               	\
	IF r1   ld32d(76) REG_SWTO_NEXT_KSP -> r19;               	\
 									\
	(* cycle 12 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(96) REG_SWTO_NEXT_KSP -> r24,               	\
	IF r1   ld32d(84) REG_SWTO_NEXT_KSP -> r21;               	\
 									\
	(* cycle 13 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(104) REG_SWTO_NEXT_KSP -> r26,              	\
	IF r1   ld32d(92) REG_SWTO_NEXT_KSP -> r23;               	\
 									\
	(* cycle 14 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   writespc REG_temp4,                          		\
	IF r1   ld32d(112) REG_SWTO_NEXT_KSP -> r28,        		\
	IF r1   ld32d(100) REG_SWTO_NEXT_KSP -> r25;        		\
 									\
	(* cycle 15 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   writedpc REG_temp3,                        		\
	IF r1   ld32d(120) REG_SWTO_NEXT_KSP -> r30,       		\
	IF r1   ld32d(108) REG_SWTO_NEXT_KSP -> r27;       		\
 									\
	(* cycle 16 *) 							\
	IF r1   nop, 							\
	IF r1   nop,                            			\
	IF r1   nop, 							\
	IF r1   ld32d(128) REG_SWTO_NEXT_KSP -> r32,          		\
	IF r1   ld32d(116) REG_SWTO_NEXT_KSP -> r29;          		\
 									\
	(* cycle 17 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   ld32d(24) REG_SWTO_NEXT_KSP -> r6,            		\
	IF r1   ld32d(124) REG_SWTO_NEXT_KSP -> r31;          		\
 									\
	(* cycle 18 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 19 *) 							\
	IF r1   iadd r0 REG_temp5 -> r2,                     		\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 

/*
* UPDATE_CURRENT_THREAD_INFO: update the struct thread_info* __current_thread_info,
*  	which is the current thread information pointer.
* FIXME: why UPDATE_CURRENT_THREAD_INFO does not work!
*/
#define UPDATE_CURRENT_THREAD_INFO   					\
	(* cycle 0 *)   						\
	IF r1   uimm(SYMBOL_NAME(__current_thread_info)) -> REG_temp,   \
	IF r1   nop,                              			\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop;   							\
   									\
	(* cycle 1 *)   						\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop;   							\
   									\
	(* cycle 2 *)   						\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop;   							\
   									\
	(* cycle 3 *)   						\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   nop,   							\
	IF r1   h_st32d(0) r6 REG_temp,                			\
	IF r1   nop;   							

#define JUMP_TO_R2 							\
	(* cycle 0 *) 							\
	IF r1   nop, 							\
	IF r1   ijmpt r1 r2,    					\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 							\
 									\
	(* cycle 1 *) 							\
	IF r1   nop, 							\
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
	IF r1   nop; 							\
 									\
	(* cycle 3 *) 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop, 							\
	IF r1   nop; 
