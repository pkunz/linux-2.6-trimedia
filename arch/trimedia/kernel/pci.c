/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#define USE_TMHWPCI_H
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/pci.h>

#include <tmhwPci.h>
#include <tmMemUncached.h>
#include <tmbslCore.h>

#define PCIBIOS_DEBUG 

static inline unsigned long 
PCI_ADDR_ASSEMBLE(struct pci_bus* bus,unsigned long devfn,int regs)
{
	return ((bus->number)<< 16) | ((devfn) << 8) | (regs);
}

/*
 * TODO: Convert tmhwPci return value to pcibios return value of linux
 */
static inline int
pci_config_return(tmErrorCode_t ret)
{
	int err=PCIBIOS_SUCCESSFUL;

	if(ret!=TM_OK){
		err=PCIBIOS_SET_FAILED;
	}	

	return err;
}

static int
tm_pci_read(struct pci_bus* bus,unsigned int devfn, int offs, int size, u32* rval)
{
	tmErrorCode_t ret;
	int err=0;

#ifdef PCIBIOS_DEBUG
	if(size!=1 && size!=2 && size!=4){
		printk(KERN_ERR "Invalid register number!\n");
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}
#endif 

	ret=tmhwPciReadConfig(0,PCI_ADDR_ASSEMBLE(bus,devfn,0),offs,size,rval);
	
	return pci_config_return(ret);	
}

static int 
tm_pci_write(struct pci_bus* bus,unsigned int devfn, int offs, int size, u32 val)
{
	tmErrorCode_t ret;

#ifdef PCIBIOS_DEBUG
	if(size!=1 && size!=2 && size!=4){
		printk(KERN_ERR "Invalid register number!\n");
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}
#endif 

	ret=tmhwPciWriteConfig(0,PCI_ADDR_ASSEMBLE(bus,devfn,0),offs,size,val);

	return pci_config_return(ret);
}

static struct pci_ops tm_pci_config_ops = {
	.read = tm_pci_read,
	.write = tm_pci_write,
};

/* PCI Initialization.  */

static struct pci_bus *tm_pci_bus=NULL;

/* Resource allocation.  */
static void __devinit pcibios_assign_resources (void)
{
	struct pci_dev *dev = NULL;
	struct resource *r;

	for_each_pci_dev(dev) {
		unsigned char irq=0;
		unsigned di_num;
		unsigned class = dev->class >> 8;

		if (class && class != PCI_CLASS_BRIDGE_HOST) {
			unsigned r_num;
			for(r_num = 0; r_num < 6; r_num++) {
				r = &dev->resource[r_num];
				if (!r->start && r->end)
					pci_assign_resource (dev, r_num);
			}
		}

		/*FIXME: because the incorrect hardware bug of MBE,
		 * I have to put it here. should move to board-speicific part*/
		switch(PCI_SLOT(dev->devfn)){
			case 0x11:
				irq = 2;
				break;
			case 0x12:
				irq = 1;
				break;
			default:
				BUG();
				break;
		}
#ifdef PCIBIOS_DEBUG
		printk("pci device '%s' is assigned irq %d\n",pci_name(dev),irq);
#endif 
		pci_write_config_byte(dev,PCI_INTERRUPT_LINE,irq);
		dev->irq = irq;
	}
}

unsigned long xio_region_start = 0x0;
unsigned long xio_region_end = 0x0;

/* Do initial PCI setup.  */
static int __devinit pcibios_init (void)
{
	/*do pcixio initliazation*/
	tmbslCoreMemRegionInfo_t * memRegionInfo=NULL;
	tmErrorCode_t ret = TM_OK;

	ret = tmbslCoreGetMemRegion(tmbslCoreMemSysXioMem, 0, &memRegionInfo);
	if(ret!=TM_OK){
		printk(KERN_ERR "Can not get PCI-XIO memory region information.\n");
		return -EFAULT;
	}

	xio_region_start = (unsigned long)memRegionInfo->pMemRegionVirtAddr;
	xio_region_end   = xio_region_start + memRegionInfo->memRegionSize; 

	tm_pci_bus = pci_scan_bus(0,&tm_pci_config_ops,0);
	if(tm_pci_bus==NULL){
		printk(KERN_ERR "Can not find PCI/HOST Controller.\n");
		return -ENOENT;	
	}
	/* 
	 * tmhwPci have assigned resource and interrupt line for any pci devices.
	 */
	pcibios_assign_resources();
	return 0;
}

subsys_initcall (pcibios_init);

extern char __devinit *pcibios_setup (char *option)
{
	/* Don't handle any options. */
	return option;
}

/*
 * FIXME: the following are stubs, implement it in furture
 */

extern int 
pcibios_scan_all_fns(struct pci_bus* bus,int devfn)
{
	return 0;
}

extern int
pcibios_assign_all_busses(void)
{
	return 0;
}

extern void 
pcibios_set_master(struct pci_dev* dev)
{
	/*FIXME: Maybe need to implement in furtuer!!!*/
	return;
}

extern struct resource* 
pcibios_select_root(struct pci_dev* pdev, struct resource* res)
{
	struct resource *root = NULL;

	if (res->flags & IORESOURCE_IO)
		root = &ioport_resource;
	if (res->flags & IORESOURCE_MEM)
		root = &iomem_resource;

	return root;
}

extern void
pcibios_align_resource(void* data,struct resource* res,unsigned long size,unsigned long align)
{
	/*FIXME: maybe need to implement in furture.*/
	return;
}

extern void 
pcibios_fixup_bus(struct pci_bus* bus)
{
	/*FIXME: must implement it!!!*/
	return;
}

extern int 
pcibios_enable_device (struct pci_dev *dev, int mask)
{
	u16 cmd, old_cmd;
	unsigned long class= (dev->class) >> 8;
	int idx;
	struct resource *r;

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	old_cmd = cmd;
	for (idx = 0; idx < 6; idx++) {
		r = &dev->resource[idx];
		if (!r->start && r->end) {
			printk(KERN_ERR "PCI: Device %s not available because "
			       "of resource collisions\n", pci_name(dev));
			return -EINVAL;
		}
		if (r->flags & IORESOURCE_IO)
			cmd |= PCI_COMMAND_IO;
		if (r->flags & IORESOURCE_MEM)
			cmd |= PCI_COMMAND_MEMORY;
	}

	/* if pci to pci bridge device always enable IO/MEM resource*/
	if(class == PCI_CLASS_BRIDGE_PCI){
		cmd |= PCI_COMMAND_IO;
		cmd |= PCI_COMMAND_MEMORY;
	}

	if (cmd != old_cmd) {
		printk("PCI: Enabling device %s (%04x -> %04x)\n",
		       pci_name(dev), old_cmd, cmd);
		pci_write_config_word(dev, PCI_COMMAND, cmd);
	}
	return 0;
}

/**
 * pcibios_resource_to_bus - convert resource to PCI bus address
 * @dev: device which owns this resource
 * @region: converted bus-centric region (start,end)
 * @res: resource to convert
 *
 * Convert a resource to a PCI device bus address or bus window.
 */
extern void
pcibios_resource_to_bus(struct pci_dev *dev, struct pci_bus_region *region,
			 struct resource *res)
{
	region->start = res->start;
	region->end = res->end;
}

extern void
pcibios_bus_to_resource(struct pci_dev *dev, struct resource *res,
			struct pci_bus_region *region)
{
	res->start = region->start;
	res->end = region->end;
}

extern void 
pcibios_add_platform_entries(struct pci_dev* dev)
{
	return;
}

extern void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long maxlen)
{
	unsigned long start = pci_resource_start(dev, bar);
	unsigned long len = pci_resource_len(dev, bar);
	unsigned long flags = pci_resource_flags(dev, bar);

	if (!len || !start)
		return NULL;
	if (maxlen && len > maxlen)
		len = maxlen;
	if (flags & IORESOURCE_IO)
		return ioport_map(start, len);
	if (flags & IORESOURCE_MEM) {
		if (flags & IORESOURCE_CACHEABLE)
			return ioremap(start, len);
		return ioremap_nocache(start, len);
	}
	/* What? */
	return NULL;
}

extern void pci_iounmap(struct pci_dev *dev, void __iomem * addr)
{
	IO_COND(addr, /* nothing */, iounmap(addr));
}
