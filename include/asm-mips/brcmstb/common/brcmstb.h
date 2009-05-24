/*
 * arch/mips/brcm/setup.c
 *
 * Copyright (C) 2001 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Setup for Broadcom eval boards
 *
 * 10-01-2003   THT    Created
 */


#ifndef _BRCMSTB_H
#define _BRCMSTB_H

#if defined(CONFIG_MIPS_BCM3560)
#include <asm/brcmstb/brcm93560/bcmuart.h>
#include <asm/brcmstb/brcm93560/bcmtimer.h>
#include <asm/brcmstb/brcm93560/bcmebi.h>
#include <asm/brcmstb/brcm93560/int1.h>
#include <asm/brcmstb/brcm93560/board.h>
#include <asm/brcmstb/brcm93560/bchp_irq0.h>
#include <asm/brcmstb/brcm93560/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7038A0)
#include <asm/brcmstb/brcm97038/bcmuart.h>
#include <asm/brcmstb/brcm97038/bcmtimer.h>
#include <asm/brcmstb/brcm97038/bcmebi.h>
#include <asm/brcmstb/brcm97038/int1.h>
#include <asm/brcmstb/brcm97038/board.h>
#include <asm/brcmstb/brcm97038/bchp_irq0.h>
#include <asm/brcmstb/brcm97038/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7038B0)
#include <asm/brcmstb/brcm97038b0/bcmuart.h>
#include <asm/brcmstb/brcm97038b0/bcmtimer.h>
#include <asm/brcmstb/brcm97038b0/bcmebi.h>
#include <asm/brcmstb/brcm97038b0/int1.h>
#include <asm/brcmstb/brcm97038b0/board.h>
#include <asm/brcmstb/brcm97038b0/bchp_irq0.h>
#include <asm/brcmstb/brcm97038b0/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7038C0)
#include <asm/brcmstb/brcm97038c0/bcmuart.h>
#include <asm/brcmstb/brcm97038c0/bcmtimer.h>
#include <asm/brcmstb/brcm97038c0/bcmebi.h>
#include <asm/brcmstb/brcm97038c0/int1.h>
#include <asm/brcmstb/brcm97038c0/board.h>
#include <asm/brcmstb/brcm97038c0/bchp_irq0.h>
#include <asm/brcmstb/brcm97038c0/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7401A0)
#include <asm/brcmstb/brcm97401a0/bcmuart.h>
#include <asm/brcmstb/brcm97401a0/bcmtimer.h>
#include <asm/brcmstb/brcm97401a0/bcmebi.h>
#include <asm/brcmstb/brcm97401a0/int1.h>
#include <asm/brcmstb/brcm97401a0/bchp_pci_cfg.h>
#include <asm/brcmstb/brcm97401a0/board.h>
#include <asm/brcmstb/brcm97401a0/bchp_irq0.h>
#include <asm/brcmstb/brcm97401a0/bcmintrnum.h>



#elif defined(CONFIG_MIPS_BCM7110)
#include <asm/brcmstb/brcm97110/bcm97110.h>
#include <asm/brcmstb/brcm97110/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7111)
#include <asm/brcmstb/brcm97111/bcm97111.h>
#include <asm/brcmstb/brcm97111/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7112)
#include <asm/brcmstb/brcm97112/bcm97112.h>
#include <asm/brcmstb/brcm97112/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7115)
#include <asm/brcmstb/brcm97115/bcm97115.h>
#include <asm/brcmstb/brcm97115/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7312)
#include <asm/brcmstb/brcm97312/bcm97312.h>
#include <asm/brcmstb/brcm97312/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7314)
#include <asm/brcmstb/brcm97314/bcm97314.h>
#include <asm/brcmstb/brcm97314/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7315)
#include <asm/brcmstb/brcm97315/bcm97315.h>
#include <asm/brcmstb/brcm97315/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7317)
#include <asm/brcmstb/brcm97317/bcm97317.h>
#include <asm/brcmstb/brcm97317/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7318)
#include <asm/brcmstb/brcm97318/bcm97318.h>
#include <asm/brcmstb/brcm97318/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7319)
#include <asm/brcmstb/brcm97319/bcm97319.h>
#include <asm/brcmstb/brcm97319/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7320)
#include <asm/brcmstb/brcm97320/bcm97320.h>
#include <asm/brcmstb/brcm97320/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7327)
#include <asm/brcmstb/brcm97327/bcm97327.h>
#include <asm/brcmstb/brcm97327/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7328)
#include <asm/brcmstb/brcm97328/bcm97328.h>
#include <asm/brcmstb/brcm97328/bcmintrnum.h>

#elif defined(CONFIG_MIPS_BCM7329)
#include <asm/brcmstb/brcm97329/bcm97329.h>
#include <asm/brcmstb/brcm97329/bcmintrnum.h>
#else
#error "unknown BCM STB chip!!!"
#endif

extern void (*irq_setup)(void);
extern void uart_puts(const char*);

#endif /*__BRCMSTB_H */

