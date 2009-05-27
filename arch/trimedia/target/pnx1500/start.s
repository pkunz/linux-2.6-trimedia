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

ENTRY(goto_tmlinux_kernel)
        (* cycle 0 *)
        IF r1   iadd r0 r5 -> r3,
        IF r1   ijmpi(SYMBOL_NAME(start_kernel)),
        IF r1   iadd r0 r5 -> r4,
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
