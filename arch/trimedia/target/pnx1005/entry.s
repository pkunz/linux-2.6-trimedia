/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <asm/linkage.h>
#include <asm/asm-offsets.h>
#include <asm/regdef.h>
#include <asm/entry.h>

/*
* CACULATE_SYSCALL_ROUTINE: caculate system routine according syscall number
*/
#define CACULATE_SYSCALL_ROUTINE 				\
	(* cycle 0 *) 						\
	IF r1   nop,                             		\
	IF r1   imm32(SYMBOL_NAME(sys_call_table)) -> REG_temp1,\
	IF r1   imm32(NR_syscalls) -> REG_temp2,                \
	IF r1   imm32(SYMBOL_NAME(sys_ni_syscall)) -> REG_temp3,\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 1 *) 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   ugtr REG_temp2 REG_SYSCALL_NR -> REG_temp2,     \
	IF r1   ld32x REG_temp1 REG_SYSCALL_NR -> REG_temp1,    \
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 2 *) 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 3 *) 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 4 *) 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 5 *) 						\
	IF REG_temp2   iadd r0 REG_temp1 -> REG_temp3,          \
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 6 *) 						\
	IF r1   iadd r0 REG_temp3 -> REG_SYSCALL_ROUTINE, 	\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 

#define SET_SYSCALL_RP 						\
	(* cycle 0 *) 						\
	IF r1   nop,                              		\
	IF r1   nop,                       			\
	IF r1   nop,                        			\
	IF r1   imm32(SYMBOL_NAME(ret_from_syscall)) -> r2, 	\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop;

#define PREPARE_SYSCALL_ENV 					\
	(* cycle 0 *) 						\
	IF r1   nop,                             		\
	IF r1   nop, 						\
	IF r1   isubi(SIZEOF_SYSCALL_ARGS) r4 -> REG_temp,      \
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 1 *) 						\
	IF r1   nop,                    			\
	IF r1   h_st32d(20) REG_SYSCALL_ARG6 REG_temp,          \
	IF r1   h_st32d(16) REG_SYSCALL_ARG5 REG_temp,          \
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 2 *) 						\
	IF r1   nop, 						\
	IF r1   h_st32d(12) REG_SYSCALL_ARG4 REG_temp,          \
	IF r1   h_st32d(8) REG_SYSCALL_ARG3 REG_temp,           \
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 3 *) 						\
	IF r1   nop, 						\
	IF r1   h_st32d(4) REG_SYSCALL_ARG2 REG_temp,           \
	IF r1   h_st32d(0) REG_SYSCALL_ARG1 REG_temp,         	\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
								\
	(* cycle 4 *) 						\
	IF r1   nop,                         			\
	IF r1   iadd r0 REG_SYSCALL_ARG4 -> r8,                 \
	IF r1   isubi(SIZEOF_SYSCALL_ARGS) r4 -> r4,            \
	IF r1   iadd r0 REG_SYSCALL_ARG3 -> r7,                 \
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop; 						\
 								\
	(* cycle 5 *) 						\
	IF r1   iadd r0 REG_SYSCALL_ARG2 -> r6,                 \
	IF r1   nop,                      			\
	IF r1   nop,                      			\
	IF r1   iadd r0 REG_SYSCALL_ARG1 -> r5,                 \
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop, 						\
	IF r1   nop;

#define JUMP_TO_SYSCALL_ROUTINE 			\
	(* cycle 0 *) 					\
	IF r1   ijmpt r1 REG_SYSCALL_ROUTINE,          	\
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
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 2 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 3 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 4 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 5 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop; 					\
 							\
	(* cycle 6 *) 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop, 					\
	IF r1   nop;

ENTRY(system_call)
	COMMON_INTR_ENTRY
	CACULATE_SYSCALL_ROUTINE
	SET_SYSCALL_RP
	PREPARE_SYSCALL_ENV
	JUMP_TO_SYSCALL_ROUTINE

ENTRY(ret_from_syscall)
	/*
	* pop up the 6 system call arguments space
	*/
	(* cycle 0 *)
        IF r1   iaddi(SIZEOF_SYSCALL_ARGS) r4 -> r4,
        IF r1   nop,
        IF r1   nop,
        IF r1   nop,
        IF r1   nop,
        IF r1   nop,
        IF r1   nop,
        IF r1   nop;

	/*
	* So that after end_of_interrupt, common_intr_exit will be called
	*/
	(* cycle 1 *)
        IF r1   nop,     
        IF r1   nop,    
        IF r1   nop,
        IF r1   imm32(SYMBOL_NAME(common_intr_exit)) -> r2, 
        IF r1   nop,     
        IF r1   nop,     
        IF r1   nop,    
	IF r1   nop; 

	/*
	*Record the system call return value to pt_regs
	*call end_of_interrupt(1,r4)
	*/
	(* cycle 2 *)
	IF r1   ijmpi(SYMBOL_NAME(end_of_interrupt)),               
	IF r1   iadd r0 r4 -> r6,                       
	IF r1   h_st32d(24) r5 r4,                      
	IF r1   iadd r0 r1 -> r5,                       
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 3 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 4 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 5 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 6 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 7 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 8 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

ENTRY(ret_from_intr)
	(* cycle 0 *)
        IF r1   nop,     
        IF r1   nop,    
        IF r1   nop,
        IF r1   imm32(SYMBOL_NAME(common_intr_exit)) -> r2, 
        IF r1   nop,     
        IF r1   nop,     
        IF r1   nop,    
	IF r1   nop; 

	(* cycle 1 *)
	IF r1   ijmpi(SYMBOL_NAME(end_of_interrupt)),               
	IF r1   iadd r0 r4 -> r6,                       
	IF r1   iadd r0 r0 -> r5,                       
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 2 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 3 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 4 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 5 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 6 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 7 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

ENTRY(common_intr_exit)
	COMMON_INTR_EXIT

