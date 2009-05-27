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
#include <asm/regdef.h>
#include <asm/asm-offsets.h>

ENTRY(__syscall)
	(* cycle 0 *)
	IF r1   uimm(__MMIO_base_init+1050656) -> REG_temp1, 
	IF r1   asli(0xa) r1 -> REG_temp2,                   
	IF r1   asli(0x1f) r1 -> REG_temp3,                  
	IF r1   ld32d(24) r4 -> REG_SYSCALL_ARG6,                   
	IF r1   ld32d(20) r4 -> REG_SYSCALL_ARG5;                   

	(* cycle 1 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   ld32d(16) r4 -> REG_SYSCALL_ARG4,                  
	IF r1   nop;

	(* cycle 2 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   writepcsw r0 REG_temp2,                     
	IF r1   nop,
	IF r1   nop;

	(* cycle 3 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   h_st32d(0) REG_temp3 REG_temp1,                   
	IF r1   nop;

	(* cycle 4 *)
	IF r1   uimm(SYMBOL_NAME(__syscall)) -> REG_temp4,              
	IF r1   nop,
	IF r1   writepcsw REG_temp2 REG_temp2,                   
	IF r1   nop,
	IF r1   nop;

	(* cycle 5 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   writespc REG_temp4,                        
	IF r1   nop,
	IF r1   nop;

	nop,nop,nop,nop,nop;

	(* cycle 6 *)
	IF r1   nop,
	IF r1   ijmpt r1 r2,                         
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 7 *)
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
	IF r1   nop;

	(* cycle 9 *)
	IF r1   iadd r0 r5 -> REG_SYSCALL_NR,                  
	IF r1   iadd r0 r8 -> REG_SYSCALL_ARG3,                  
	IF r1   iadd r0 r7 -> REG_SYSCALL_ARG2,                  
	IF r1   iadd r0 r6 -> REG_SYSCALL_ARG1,                  
	IF r1   nop;
