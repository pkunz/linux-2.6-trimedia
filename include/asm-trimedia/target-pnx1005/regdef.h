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

#ifndef __ASM_TRIMEDIA_PNX1005_REGDEF_H
#define __ASM_TRIMEDIA_PNX1005_REGDEF_H

#ifdef __ASSEMBLY__

/*
* The register is only used to pass syscall information,
* So, must be caller save registers.
*/
#define REG_SYSCALL_NR	    r33
#define REG_SYSCALL_ROUTINE REG_SYSCALL_NR
#define REG_SYSCALL_ARG1    r34
#define REG_SYSCALL_ARG2    r35
#define REG_SYSCALL_ARG3    r36
#define REG_SYSCALL_ARG4    r37
#define REG_SYSCALL_ARG5    r38
#define REG_SYSCALL_ARG6    r39

/*
*The registers are used to save the status registers when interrupt occurs.
*/
#define REG_INTR_STATUS_PCSW		r127
#define REG_INTR_STATUS_DPC		r126
#define REG_INTR_STATUS_SPC		r125
#define REG_INTR_STATUS_RP		r124
#define REG_INTR_STATUS_FP		r123
#define REG_INTR_STATUS_SP		r122
#define REG_SWTO_PREV_KSP 		r121
#define REG_SWTO_NEXT_KSP 		r120

/*
 * macro local registers, used just as temp registers.
 */
#define REG_temp 			r64
#define REG_temp1 			r65
#define REG_temp2 			r66
#define REG_temp3 			r67
#define REG_temp4  			r68
#define REG_temp5 			r69

#else  /*__ASSEMBLY__*/

#define REG_SYSCALL_NR(regs)     ((regs)->caller_save_regs[0])
#define REG_SYSCALL_ARG1(regs)   ((regs)->caller_save_regs[1])
#define REG_SYSCALL_ARG2(regs)   ((regs)->caller_save_regs[2])
#define REG_SYSCALL_ARG3(regs)   ((regs)->caller_save_regs[3])
#define REG_SYSCALL_ARG4(regs)   ((regs)->caller_save_regs[4])
#define REG_SYSCALL_ARG5(regs)   ((regs)->caller_save_regs[5])
#define REG_SYSCALL_ARG6(regs)   ((regs)->caller_save_regs[6])

#endif  /*__ASSEMBELY__*/

#endif /*__ASM_TRIMEDIA_PNX1005_REGDEF_H*/

