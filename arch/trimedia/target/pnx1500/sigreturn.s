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

ENTRY(sigreturn)
	(* cycle 0 *)
	IF r1   nop,            
	IF r1   ijmpi(SYMBOL_NAME(__syscall)),          
	IF r1   nop,            
	IF r1   nop,           
	IF r1   nop;            

	(* cycle 1 *)
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,
	IF r1   nop;

	(* cycle 2 *)
	IF r1   iaddi(__NR_sigreturn) r0 -> r5,        
	IF r1   nop,
	IF r1   nop,
	IF r1   nop,          
	IF r1   nop;          

	(* cycle 3 *)
	IF r1   nop,        
	IF r1   nop,
	IF r1   nop,            
	IF r1   nop,          
	IF r1   nop;           

ENTRY(rt_sigreturn)
	(* cycle 0 *)
	IF r1   nop,          
	IF r1   ijmpi(SYMBOL_NAME(__syscall)),        
	IF r1   uimm(__NR_rt_sigreturn) -> r5,          
	IF r1   nop,        
	IF r1   nop;         

	(* cycle 1 *)
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
	IF r1   nop;

	(* cycle 3 *)
	IF r1   nop,          
	IF r1   nop,      
	IF r1   nop,     
	IF r1   nop,                   
	IF r1   nop;                  

