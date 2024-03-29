/*
 * (C) Copyright David Brownell 2000-2002
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/config.h>

#ifdef CONFIG_USB_DEBUG
	#define DEBUG
#else
	#undef DEBUG
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/usb.h>
#include "hcd.h"


/* PCI-based HCs are common, but plenty of non-PCI HCs are used too */


/*-------------------------------------------------------------------------*/

/* configure so an HC device and id are always provided */
/* always called with process context; sleeping is OK */

/**
 * usb_hcd_pci_probe - initialize PCI-based HCDs
 * @dev: USB Host Controller being probed
 * @id: pci hotplug id connecting controller to HCD framework
 * Context: !in_interrupt()
 *
 * Allocates basic PCI resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 *
 * Store this function in the HCD's struct pci_driver as probe().
 */
int usb_hcd_pci_probe (struct pci_dev *dev, const struct pci_device_id *id)
{
	struct hc_driver	*driver;
	struct usb_hcd		*hcd;
	int			retval;

	if (usb_disabled())
		return -ENODEV;

	if (!id || !(driver = (struct hc_driver *) id->driver_data))
		return -EINVAL;

	if (pci_enable_device (dev) < 0)
		return -ENODEV;
	dev->current_state = PCI_D0;
	dev->dev.power.power_state = PMSG_ON;
	
        if (!dev->irq) {
        	dev_err (&dev->dev,
			"Found HC with no IRQ.  Check BIOS/PCI %s setup!\n",
			pci_name(dev));
   	        retval = -ENODEV;
		goto err1;
        }

	hcd = usb_create_hcd (driver, &dev->dev, pci_name(dev));
	if (!hcd) {
		retval = -ENOMEM;
		goto err1;
	}

	if (driver->flags & HCD_MEMORY) {	// EHCI, OHCI
		hcd->rsrc_start = (u64) pci_resource_start (dev, 0);
		hcd->rsrc_len = (u64) pci_resource_len (dev, 0);
		if (!request_mem_region ((u32) hcd->rsrc_start, (u32) hcd->rsrc_len,
				driver->description)) {
			dev_dbg (&dev->dev, "controller already in use\n");
printk("request_mem_region [%08x,%08x] failed, continuing\n", (u32) hcd->rsrc_start, (u32) hcd->rsrc_len);
			retval = -EBUSY;
			goto err2;
		}
		hcd->regs = ioremap_nocache ((u32) hcd->rsrc_start, (u32) hcd->rsrc_len);
		if (hcd->regs == NULL) {
			dev_dbg (&dev->dev, "error mapping memory\n");
printk("error mapping memory\n");
			retval = -EFAULT;
			goto err3;
		}
printk("usb_hcd_pci_probe: ioremap_nocache(start=%08x, len=%08x\n", 
	(u32) hcd->rsrc_start, (u32) hcd->rsrc_len);

	} else { 				// UHCI
		int	region;

printk("usb_hcd_pci_probe: Should not get here, UHCI\n");
		for (region = 0; region < PCI_ROM_RESOURCE; region++) {
			if (!(pci_resource_flags (dev, region) &
					IORESOURCE_IO))
				continue;

			hcd->rsrc_start = pci_resource_start (dev, region);
			hcd->rsrc_len = pci_resource_len (dev, region);
			if (request_region ((u32) hcd->rsrc_start, (u32) hcd->rsrc_len,
					driver->description))
				break;
		}
		if (region == PCI_ROM_RESOURCE) {
			dev_dbg (&dev->dev, "no i/o regions available\n");
printk("no i/o regions available\n");
			retval = -EBUSY;
			goto err1;
		}
	}

	pci_set_master (dev);

	retval = usb_add_hcd (hcd, dev->irq, SA_SHIRQ);
	if (retval != 0) {
printk("usb_add_hcd failed\n");
		goto err4;
	}
	return retval;

 err4:
	if (driver->flags & HCD_MEMORY) {
		iounmap (hcd->regs);
 err3:
		release_mem_region ((u32) hcd->rsrc_start, (u32) hcd->rsrc_len);
	} else
		release_region (hcd->rsrc_start, hcd->rsrc_len);
 err2:
	usb_put_hcd (hcd);
 err1:
	pci_disable_device (dev);
	dev_err (&dev->dev, "init %s fail, %d\n", pci_name(dev), retval);
	return retval;
} 
EXPORT_SYMBOL (usb_hcd_pci_probe);


/* may be called without controller electrically present */
/* may be called with controller, bus, and devices active */

/**
 * usb_hcd_pci_remove - shutdown processing for PCI-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_hcd_pci_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 * Store this function in the HCD's struct pci_driver as remove().
 */
void usb_hcd_pci_remove (struct pci_dev *dev)
{
	struct usb_hcd		*hcd;

	hcd = pci_get_drvdata(dev);
	if (!hcd)
		return;

	usb_remove_hcd (hcd);
	if (hcd->driver->flags & HCD_MEMORY) {
		iounmap (hcd->regs);
		release_mem_region (hcd->rsrc_start, hcd->rsrc_len);
	} else {
		release_region (hcd->rsrc_start, hcd->rsrc_len);
	}
	usb_put_hcd (hcd);
	pci_disable_device(dev);
}
EXPORT_SYMBOL (usb_hcd_pci_remove);


#ifdef	CONFIG_PM

/**
 * usb_hcd_pci_suspend - power management suspend of a PCI-based HCD
 * @dev: USB Host Controller being suspended
 * @message: semantics in flux
 *
 * Store this function in the HCD's struct pci_driver as suspend().
 */
int usb_hcd_pci_suspend (struct pci_dev *dev, pm_message_t message)
{
	struct usb_hcd		*hcd;
	int			retval = 0;
	int			has_pci_pm;

	hcd = pci_get_drvdata(dev);

	/* FIXME until the generic PM interfaces change a lot more, this
	 * can't use PCI D1 and D2 states.  For example, the confusion
	 * between messages and states will need to vanish, and messages
	 * will need to provide a target system state again.
	 *
	 * It'll be important to learn characteristics of the target state,
	 * especially on embedded hardware where the HCD will often be in
	 * charge of an external VBUS power supply and one or more clocks.
	 * Some target system states will leave them active; others won't.
	 * (With PCI, that's often handled by platform BIOS code.)
	 */

	/* even when the PCI layer rejects some of the PCI calls
	 * below, HCs can try global suspend and reduce DMA traffic.
	 * PM-sensitive HCDs may already have done this.
	 */
	has_pci_pm = pci_find_capability(dev, PCI_CAP_ID_PM);

	switch (hcd->state) {

	/* entry if root hub wasn't yet suspended ... from sysfs,
	 * without autosuspend, or if USB_SUSPEND isn't configured.
	 */
	case HC_STATE_RUNNING:
		hcd->state = HC_STATE_QUIESCING;
		retval = hcd->driver->suspend (hcd, message);
		if (retval) {
			dev_dbg (hcd->self.controller, 
					"suspend fail, retval %d\n",
					retval);
			break;
		}
		hcd->state = HC_STATE_SUSPENDED;
		/* FALLTHROUGH */

	/* entry with CONFIG_USB_SUSPEND, or hcds that autosuspend: the
	 * controller and/or root hub will already have been suspended,
	 * but it won't be ready for a PCI resume call.
	 *
	 * FIXME only CONFIG_USB_SUSPEND guarantees hub_suspend() will
	 * have been called, otherwise root hub timers still run ...
	 */
	case HC_STATE_SUSPENDED:
		/* no DMA or IRQs except when HC is active */
		if (dev->current_state == PCI_D0) {
			pci_save_state (dev);
			pci_disable_device (dev);
		}

		if (!has_pci_pm) {
			dev_dbg (hcd->self.controller, "--> PCI D0/legacy\n");
			break;
		}

		/* NOTE:  dev->current_state becomes nonzero only here, and
		 * only for devices that support PCI PM.  Also, exiting
		 * PCI_D3 (but not PCI_D1 or PCI_D2) is allowed to reset
		 * some device state (e.g. as part of clock reinit).
		 */
		retval = pci_set_power_state (dev, PCI_D3hot);
		if (retval == 0) {
			dev_dbg (hcd->self.controller, "--> PCI D3\n");
			retval = pci_enable_wake (dev, PCI_D3hot, hcd->remote_wakeup);
			if (retval)
				break;
			retval = pci_enable_wake (dev, PCI_D3cold, hcd->remote_wakeup);
		} else if (retval < 0) {
			dev_dbg (&dev->dev, "PCI D3 suspend fail, %d\n",
					retval);
			(void) usb_hcd_pci_resume (dev);
			break;
		}
		break;
	default:
		dev_dbg (hcd->self.controller, "hcd state %d; not suspended\n",
			hcd->state);
		WARN_ON(1);
		retval = -EINVAL;
		break;
	}

	/* update power_state **ONLY** to make sysfs happier */
	if (retval == 0)
		dev->dev.power.power_state = message;
	return retval;
}
EXPORT_SYMBOL (usb_hcd_pci_suspend);

/**
 * usb_hcd_pci_resume - power management resume of a PCI-based HCD
 * @dev: USB Host Controller being resumed
 *
 * Store this function in the HCD's struct pci_driver as resume().
 */
int usb_hcd_pci_resume (struct pci_dev *dev)
{
	struct usb_hcd		*hcd;
	int			retval;

	hcd = pci_get_drvdata(dev);
	if (hcd->state != HC_STATE_SUSPENDED) {
		dev_dbg (hcd->self.controller, 
				"can't resume, not suspended!\n");
		return 0;
	}

	/* NOTE:  chip docs cover clean "real suspend" cases (what Linux
	 * calls "standby", "suspend to RAM", and so on).  There are also
	 * dirty cases when swsusp fakes a suspend in "shutdown" mode.
	 */
	if (dev->current_state != PCI_D0) {
#ifdef	DEBUG
		int	pci_pm;
		u16	pmcr;

		pci_pm = pci_find_capability(dev, PCI_CAP_ID_PM);
		pci_read_config_word(dev, pci_pm + PCI_PM_CTRL, &pmcr);
		pmcr &= PCI_PM_CTRL_STATE_MASK;
		if (pmcr) {
			/* Clean case:  power to USB and to HC registers was
			 * maintained; remote wakeup is easy.
			 */
			dev_dbg(hcd->self.controller, "resume from PCI D%d\n",
					pmcr);
		} else {
			/* Clean:  HC lost Vcc power, D0 uninitialized
			 *   + Vaux may have preserved port and transceiver
			 *     state ... for remote wakeup from D3cold
			 *   + or not; HCD must reinit + re-enumerate
			 *
			 * Dirty: D0 semi-initialized cases with swsusp
			 *   + after BIOS init
			 *   + after Linux init (HCD statically linked)
			 */
			dev_dbg(hcd->self.controller,
				"PCI D0, from previous PCI D%d\n",
				dev->current_state);
		}
#endif
		retval = pci_enable_wake (dev, dev->current_state, 0);
		if (retval) {
			dev_err(hcd->self.controller,
				"can't enable_wake to %d, %d!\n",
				dev->current_state, retval);
			return retval;
		}
		retval = pci_enable_wake (dev, PCI_D3cold, 0);
		if (retval) {
			dev_err(hcd->self.controller,
				"can't enable_wake to %d, %d!\n",
				PCI_D3cold, retval);
			return retval;
		}
	} else {
		/* Same basic cases: clean (powered/not), dirty */
		dev_dbg(hcd->self.controller, "PCI legacy resume\n");
	}

	/* NOTE:  the PCI API itself is asymmetric here.  We don't need to
	 * pci_set_power_state(PCI_D0) since that's part of re-enabling;
	 * but that won't re-enable bus mastering.  Yet pci_disable_device()
	 * explicitly disables bus mastering...
	 */
	retval = pci_enable_device (dev);
	if (retval < 0) {
		dev_err (hcd->self.controller,
			"can't re-enable after resume, %d!\n", retval);
		return retval;
	}
	pci_set_master (dev);
	pci_restore_state (dev);

	dev->dev.power.power_state = PMSG_ON;

	hcd->state = HC_STATE_RESUMING;
	hcd->saw_irq = 0;

	retval = hcd->driver->resume (hcd);
	if (!HC_IS_RUNNING (hcd->state)) {
		dev_dbg (hcd->self.controller, 
				"resume fail, retval %d\n", retval);
		usb_hc_died (hcd);
	}

	retval = pci_enable_device(dev);
	return retval;
}
EXPORT_SYMBOL (usb_hcd_pci_resume);

#endif	/* CONFIG_PM */


