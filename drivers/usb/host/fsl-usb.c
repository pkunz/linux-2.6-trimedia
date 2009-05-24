/* 
 * Copyright (c) 2005 freescale semiconductor
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <linux/config.h>
#ifdef CONFIG_USB_DEBUG
        #define DEBUG
#else
        #undef DEBUG
#endif
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/usb.h>


#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/byteorder.h>
#include <asm/uaccess.h>

#include <asm/unaligned.h>

#include "../core/hcd.h"
#include "fsl-usb.h"

#define DRIVER_VERSION "$Revision: 1.11 $"
#define DRIVER_AUTHOR "Hunter Wu"
#define DRIVER_DESC "USB 2.0 Freescale EHCI Driver"
#define DRIVER_INFO DRIVER_VERSION " " DRIVER_DESC

MODULE_DESCRIPTION("MPC8349 USB Host Controller Driver");

static void mpc8349_usb_reset(void);

#include "ehci-hcd.c"

void mpc8349_board_init(void)
{
#ifdef CONFIG_MPC834x_SYS 
	volatile unsigned char *bcsr5_p;
	void __iomem *bcsr;

	pr_debug("fsl-usb2: BCSR init @ 0x%x\n", BCSR_PHYS_ADDR);

	/* if SYS board is plug into PIB board, force to use the PHY on SYS board */
	bcsr = ioremap(BCSR_PHYS_ADDR, BCSR_SIZE);
	if (!bcsr) {
		printk(KERN_ERR "BCSR ioremap for USB failed");
		return;
	}
	bcsr5_p = (volatile unsigned char *)(bcsr + 0x00000005); 
	if ( (*bcsr5_p & BCSR5_INT_USB) == 0 ) {
		printk(KERN_INFO "fsl-usb2: PIB present, but selecting SYS USB\n");
		*bcsr5_p = (*bcsr5_p | BCSR5_INT_USB);
	} else
		printk(KERN_INFO "fsl_usb: SYS USB already selected\n");
	iounmap(bcsr);
#endif
}

void mpc8349_usb_clk_cfg(void)
{
	unsigned long sccr;
	volatile unsigned long *p;

	pr_debug("fsl-usb: clk init @ 0x%x\n", CFG_IMMR_BASE);
	
	p = (volatile unsigned long *)(CFG_IMMR_BASE + SCCR_OFFS); /* SCCR */
	sccr = *p;
		
#if defined(CONFIG_MPH_USB_SUPPORT)
	sccr &= ~SCCR_USB_MPHCM_11;
	sccr |= SCCR_USB_MPHCM_11;  /* USB CLK 1:3 CSB CLK */
	*p = sccr;
#elif defined(CONFIG_DR_USB_SUPPORT)
	sccr &= ~SCCR_USB_DRCM_11;
	sccr |= SCCR_USB_DRCM_11;  /* USB CLK 1:3 CSB CLK */
	*p = sccr;
#endif

}

void mpc8349_usb_pin_cfg(void)
{
	unsigned long sicrl;
	volatile unsigned long *p;
	
	p = (volatile unsigned long *)(CFG_IMMR_BASE + SICRL_OFFS); /* SCCR */
	sicrl = *p;
	
#if defined(CONFIG_MPH_USB_SUPPORT)
#ifdef CONFIG_MPH0_USB_ENABLE 
	sicrl &= ~SICRL_USB0;
	*p = sicrl;
#endif

#ifdef CONFIG_MPH1_USB_ENABLE
	sicrl &= ~SICRL_USB1;
	*p = sicrl;
#endif
#elif defined(CONFIG_DR_USB_SUPPORT)
	sicrl &= ~SICRL_USB0;
	sicrl |= SICRL_USB1 ;
	*p = sicrl;
#if defined(CONFIG_DR_UTMI)
	sicrl &= ~SICRL_USB0;
	sicrl |= SICRL_USB0;
	*p = sicrl;
#endif

#endif
}

static void mpc8349_usb_reset(void)
{
	u32      portsc;
    
#if defined(CONFIG_MPH_USB_SUPPORT)    
	t_USB_MPH_MAP *p_MphMemMap;
	/* Enable PHY interface in the control reg. */
	p_MphMemMap = (t_USB_MPH_MAP *)MPC83xx_USB_MPH_BASE;
	p_MphMemMap->control = 0x00000004;
	p_MphMemMap->snoop1 = 0x0000001b;
#ifdef CONFIG_MPH0_USB_ENABLE
	portsc = readl(&p_MphMemMap->port_status[0]);
	portsc &= ~PORT_TS;
#if defined(CONFIG_MPH0_ULPI)       
	portsc |= PORT_TS_ULPI; 
#elif defined (CONFIG_MPH0_SERIAL)
	portsc |= PORT_TS_SERIAL;
#endif	
	writel(portsc,&p_MphMemMap->port_status[0]);
#endif
	
#ifdef CONFIG_MPH1_USB_ENABLE
	portsc = readl(&p_MphMemMap->port_status[1]);
	portsc &= ~PORT_TS;
#if defined(CONFIG_MPH1_ULPI)       
	portsc |= PORT_TS_ULPI; 
#elif defined (CONFIG_MPH1_SERIAL)
	portsc |= PORT_TS_SERIAL;
#endif	
	writel(portsc,&p_MphMemMap->port_status[1]);
#endif
        
	p_MphMemMap->pri_ctrl = 0x0000000c;
	p_MphMemMap->age_cnt_thresh = 0x00000040;
	p_MphMemMap->si_ctrl= 0x00000001;

#elif defined(CONFIG_DR_USB_SUPPORT)
	t_USB_DR_MAP *p_DrMemMap;
	p_DrMemMap = (t_USB_DR_MAP *)MPC83xx_USB_DR_BASE;
	p_DrMemMap->control = 0x00000004;
	p_DrMemMap->snoop1 = 0x0000001b;
	portsc = readl(&p_DrMemMap->port_status[0]);
	portsc &= ~PORT_TS;
#if defined(CONFIG_DR_ULPI)     
	portsc |= PORT_TS_ULPI;
#elif defined(CONFIG_DR_SERIAL)
	portsc |= PORT_TS_SERIAL;
#elif defined(CONFIG_DR_UTMI)
	portsc |= PORT_TS_ULPI;
#endif
           
	writel(portsc,&p_DrMemMap->port_status[0]);
	writel(0x00000003,&p_DrMemMap->usbmode);
	p_DrMemMap->pri_ctrl = 0x0000000c;
	p_DrMemMap->age_cnt_thresh = 0x00000040;
	p_DrMemMap->si_ctrl= 0x00000001;
#endif
	pr_debug("fsl-usb: reset complete.\n");
}


static int __init
fsl_usb20_probe(struct device *dev)
{
	struct usb_hcd		*hcd;
	struct ehci_hcd         *ehci;
	int 			retval;
	int 			irq;
#if defined (CONFIG_MPH_USB_SUPPORT)
	t_USB_MPH_MAP 		*p_MphMemMap;
	char			*mode="MPH";
#elif defined (CONFIG_DR_USB_SUPPORT)
    	t_USB_DR_MAP 		*p_DrMemMap;
	char			*mode="DR";
#endif	
	mpc8349_board_init();
	mpc8349_usb_clk_cfg();
	mpc8349_usb_pin_cfg();
	
	hcd = usb_create_hcd(&ehci_driver, dev, "fsl_usb2.0");
	if (!hcd) {
		printk(KERN_ERR "fsl-usb: create_hcd failed\n");
		retval = 0;
		goto err1;
	}

	ehci = hcd_to_ehci(hcd);
	dev_set_drvdata(dev, ehci);
	
#if defined(CONFIG_MPH_USB_SUPPORT)
	p_MphMemMap = (t_USB_MPH_MAP *)MPC83xx_USB_MPH_BASE;
        
        hcd->regs = (void *)(&p_MphMemMap->hc_capbase);
        irq = MPC83xx_USB_MPH_IVEC;
#elif defined (CONFIG_DR_USB_SUPPORT)
	/* Enable PHY interface in the control reg. */
        p_DrMemMap = (t_USB_DR_MAP *)MPC83xx_USB_DR_BASE;
    	hcd->regs = (void *)(&p_DrMemMap->hc_capbase);/* Set the interrupt that is called for this USB. */
        irq = MPC83xx_USB_DR_IVEC;
#endif

	printk(KERN_INFO "fsl-usb: %s @ 0x%p, irq %d\n", mode, hcd->regs, irq);

	hcd->rsrc_start = (int)hcd->regs & ~0UL;
	hcd->rsrc_len = 0x1000;
	hcd->self.controller = dev;
	hcd->self.bus_name = dev->bus_id;
	hcd->product_desc ="fsl usb20";

	retval = usb_add_hcd(hcd, irq, SA_INTERRUPT);
	if (retval < 0) {
		printk(KERN_ERR "fsl-usb: usb_add_hcd failed.\n");
		goto err2;
	}

	retval=ehci_hc_reset(hcd);
	if (retval < 0) {
		printk(KERN_ERR "fsl-usb: hc_reset failed\n");
		goto err2;
	}

	retval = ehci_start(hcd);
	if (retval < 0) {
		printk(KERN_ERR "fsl-usb: ehci_start failed\n");
		goto err2;
	}

	return 0;

err2:
	usb_put_hcd(hcd);
err1:
	printk(KERN_ERR "fsl-usb: init error, %d\n", retval);
	return retval;	
}

static int __init_or_module
fsl_usb20_remove(struct device *dev)
{
	struct ehci_hcd		*ehci = dev_get_drvdata(dev);
	struct usb_hcd		*hcd = ehci_to_hcd(ehci);
	

	if (HC_IS_RUNNING(hcd->state))
		hcd->state = HC_STATE_QUIESCING;

	usb_disconnect(&hcd->self.root_hub);
	
	hcd->driver->stop (hcd);
	
	free_irq(hcd->irq, hcd);

	usb_put_hcd(hcd);
	return 0;
}
	
#define	fsl_usb20_suspend	NULL
#define	fsl_usb20_resume	NULL

static struct device_driver fsl_usb20_driver = {
	.name =		(char *) hcd_name,
	.bus =		&platform_bus_type,

	.probe =	fsl_usb20_probe,
	.remove =	fsl_usb20_remove,

	.suspend =	fsl_usb20_suspend,
	.resume =	fsl_usb20_resume,
};

/*-------------------------------------------------------------------------*/
 
static int __init mpc8349_usb_hc_init(void) 
{
	int ret;

	if (usb_disabled())
		return -ENODEV;

	printk(KERN_INFO "driver %s, %s\n", hcd_name, DRIVER_VERSION);
	ret = driver_register(&fsl_usb20_driver);
	if (ret) {
		printk(KERN_ERR "fsl-usb: driver register failed (%d)\n", ret);
	}
	return ret;
}


static void __exit mpc8349_usb_hc_deinit(void) 
{	
	driver_unregister(&fsl_usb20_driver);
}
MODULE_DESCRIPTION (DRIVER_INFO);
MODULE_AUTHOR (DRIVER_AUTHOR);
MODULE_LICENSE ("GPL");
module_init(mpc8349_usb_hc_init);
module_exit(mpc8349_usb_hc_deinit);
