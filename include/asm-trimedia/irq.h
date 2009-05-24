/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_IRQ_H_
#define _ASM_TRIMEDIA_IRQ_H_

#include <linux/config.h>

/*
 * the system has an on-CPU PIC and another PIC on the FPGA and other PICs on other peripherals,
 * so we do some routing in irq-routing.[ch] to reduce the number of false-positives seen by
 * drivers
 */

/* this number is used when no interrupt has been assigned */
#define NO_IRQ				(-1)

/*
 *FIXME: I don't know what NR_IRQS should be.
 */
#define NR_IRQS				64	

static inline int irq_canonicalize(int irq)
{
	return irq;
}

extern void disable_irq_nosync(unsigned int irq);
extern void disable_irq(unsigned int irq);
extern void enable_irq(unsigned int irq);


#endif /* _ASM_TRIMEDIA_IRQ_H_ */

