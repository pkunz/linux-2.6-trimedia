/* 
 * arch/trimedia/kernel/quirks.c : work-arounds for trimedia platform bugs.
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/config.h>
#include <linux/pci.h>
#include <linux/irq.h>

#ifdef CONFIG_SERIAL_8250
/*
 * FIXME: On MBE platform, the interrupt of netmos 9835 pci to serial card
 *    fails to route to trimedia, so we have to disable it.
 *    I don't know why, help me to debug it.
 */
static void __devinit quirk_netmos_disable_irq(struct pci_dev *dev)
{
	printk(KERN_INFO "Netmos 9835 serial/parallel ports card detected, using poll instead.\n");
	dev->irq = 0;
}
DECLARE_PCI_FIXUP_ENABLE_ONE(PCI_VENDOR_ID_NETMOS, PCI_DEVICE_ID_NETMOS_9835, quirk_netmos_disable_irq);
#endif 
