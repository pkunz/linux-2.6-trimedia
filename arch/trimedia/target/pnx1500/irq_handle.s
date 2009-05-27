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
#include <asm/entry.h>
#include <asm/asm-offsets.h>

#define BUILD_IRQ_HANDLE(irq)						\
									\
ENTRY(IRQ##irq##_handler)  						\
	COMMON_INTR_ENTRY 						\
	DISABLE_IRQS 							\
									\
	(* cycle 0 *)  							\
        IF r1   uimm(SYMBOL_NAME(ret_from_intr)) -> r2,   		\
        IF r1   nop,  							\
        IF r1   nop,       						\
        IF r1   nop,      						\
	IF r1   nop;   							\
	  								\
	(* cycle 1 *)  							\
	IF r1   iaddi(irq) r0 -> r5,                   			\
	IF r1   ijmpi(SYMBOL_NAME(do_IRQ)),       			\
	IF r1   iadd r0 r4 -> r6,                        		\
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
  									\
	(* cycle 4 *)  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop,  							\
	IF r1   nop;

BUILD_IRQ_HANDLE(0)
BUILD_IRQ_HANDLE(1)
BUILD_IRQ_HANDLE(2)
BUILD_IRQ_HANDLE(3)
BUILD_IRQ_HANDLE(4)
BUILD_IRQ_HANDLE(5)
BUILD_IRQ_HANDLE(6)
BUILD_IRQ_HANDLE(7)
BUILD_IRQ_HANDLE(8)
BUILD_IRQ_HANDLE(9)
BUILD_IRQ_HANDLE(10)
BUILD_IRQ_HANDLE(11)
BUILD_IRQ_HANDLE(12)
BUILD_IRQ_HANDLE(13)
BUILD_IRQ_HANDLE(14)
BUILD_IRQ_HANDLE(15)
BUILD_IRQ_HANDLE(16)
BUILD_IRQ_HANDLE(17)
BUILD_IRQ_HANDLE(18)
BUILD_IRQ_HANDLE(19)
BUILD_IRQ_HANDLE(20)
BUILD_IRQ_HANDLE(21)
BUILD_IRQ_HANDLE(22)
BUILD_IRQ_HANDLE(23)
BUILD_IRQ_HANDLE(24)
BUILD_IRQ_HANDLE(25)
BUILD_IRQ_HANDLE(26)
BUILD_IRQ_HANDLE(27)
BUILD_IRQ_HANDLE(28)
BUILD_IRQ_HANDLE(29)
BUILD_IRQ_HANDLE(30)
BUILD_IRQ_HANDLE(31)
BUILD_IRQ_HANDLE(32)
BUILD_IRQ_HANDLE(33)
BUILD_IRQ_HANDLE(34)
BUILD_IRQ_HANDLE(35)
BUILD_IRQ_HANDLE(36)
BUILD_IRQ_HANDLE(37)
BUILD_IRQ_HANDLE(38)
BUILD_IRQ_HANDLE(39)
BUILD_IRQ_HANDLE(40)
BUILD_IRQ_HANDLE(41)
BUILD_IRQ_HANDLE(42)
BUILD_IRQ_HANDLE(43)
BUILD_IRQ_HANDLE(44)
BUILD_IRQ_HANDLE(45)
BUILD_IRQ_HANDLE(46)
BUILD_IRQ_HANDLE(47)
BUILD_IRQ_HANDLE(48)
BUILD_IRQ_HANDLE(49)
BUILD_IRQ_HANDLE(50)
BUILD_IRQ_HANDLE(51)
BUILD_IRQ_HANDLE(52)
BUILD_IRQ_HANDLE(53)
BUILD_IRQ_HANDLE(54)
BUILD_IRQ_HANDLE(55)
BUILD_IRQ_HANDLE(56)
BUILD_IRQ_HANDLE(57)
BUILD_IRQ_HANDLE(58)
BUILD_IRQ_HANDLE(59)
BUILD_IRQ_HANDLE(60)
BUILD_IRQ_HANDLE(61)
BUILD_IRQ_HANDLE(62)
BUILD_IRQ_HANDLE(63)
