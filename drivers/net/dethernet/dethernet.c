/*
 *
 * $Id: 06-fsl_8548cds-PQ3_PCI-agent-2.patch,v 1.1 2006/07/17 20:08:03 rmagee Exp $
 *
 * PCI Agent Ethernet Driver for Freescale PowerPC85xx Processor
 *
 * Last Modify: Sept 23, 2004
 * Author: Jason Jin & Lunsheng Wang
 *
 * For more information, please consult the following manuals:
 *
 * MPC8540 PowerQUICC III(TM) Integrated Host Processor Reference Manual.
 * MPC8560 PowerQUICC III(TM) Integrated Host Processor Reference Manual.
 * You can visit web site: http://www.freescale.com to get them.
 *
 * Copyright (C) 2004, 2005 Freescale, Inc.
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *    
 * Modified History:
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h> 
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/errno.h>  
#include <linux/types.h>  
#include <linux/interrupt.h> 
#include <linux/in.h>
#include <linux/netdevice.h>   
#include <linux/etherdevice.h> 
#include <linux/ip.h>          
#include <linux/tcp.h>        
#include <linux/skbuff.h>
#include <linux/in6.h>
#include <asm-ppc/checksum.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <asm-ppc/uaccess.h>
#include <asm-ppc/io.h>
#include <asm-ppc/irq.h>
#include <asm-ppc/page.h>
#include <linux/ioport.h>
#include <asm-ppc/immap_85xx.h>
#include <asm-ppc/mpc85xx.h>

MODULE_AUTHOR("Jason Jin, Lunsheng Wang");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("MPC85xx PQIII Processor PCI Agent Ethernet Driver");

static int eth = 0; 
module_param(eth, int, 0);

static int timeout = 5;
module_param(timeout, int, 0);

#undef DEBUG
#ifdef DEBUG 
  #define DPRINTK(fmt, args...)   printk(KERN_DEBUG "%s: " fmt, __FUNCTION__ , ##args)
#else
  #define DPRINTK(fmt, args...)
#endif

#define	PCI_SPACE_ADDRESS	0x80000000
#define	PCI_BASE_OFFSET 	0x08000

#define   CTL_STATUS_SIZE	24
#define	RX_SPACE_SIZE		2*1024-12
#define	TX_SPACE_SIZE		2*1024-12
#define	AGENT_DRIVER_MEM_SIZE	CTL_STATUS_SIZE+RX_SPACE_SIZE+TX_SPACE_SIZE

/* These are the flags in the message register */
#define HOST_SEND	0x0001
#define HOST_GET	0x0002
#define AGENT_SEND	0x0001
#define AGENT_GET	0x0002

#define	IRQ_NUM		109

#define WRITE_REG(n, v) 	*((volatile u32 *)(n)) = (volatile u32)v
#define READ_REG(n, v) 		(volatile u32)v = *((volatile u32 *)(n))

struct dether_share_mem {
	u32	hstatus;
	u32	astatus;
	
	u32 	rx_flags;
	u32 	rx_packetlen;
	u8	rxbuf[2*1024 - 12];

	u32	tx_flags;
	u32	tx_packetlen;
	u8	txbuf[2*1024 - 12];
};


struct dether_priv {
	struct dether_share_mem *share_mem;
	
	struct sk_buff *cur_tx_skb;
   	u32 ccsrbar; 
	spinlock_t lock;
	struct net_device_stats stats;
};

void dether_tx_timeout (struct net_device *dev);
static irqreturn_t dether_interrupt(int irq,void *dev_id,struct pt_regs *regs);

void inline pci_agent_cache_flush(void *addr)
{
	asm volatile("dcbf %0, %1"::"r"(0),"r"((u32)addr));
}


/* Set up inbound window winno for PCI device */
static int setup_agent_inbound_window(
		u32 pciaddr,
		u32 localaddr,
		u32 size,
		u32 winno,
		u32 ccsrbar)
{
        volatile struct ccsr_pci *pci;
        u32 value;
        int order = 0;
                                                                                                 
        if (winno<1 || winno>3) {
                printk(KERN_ERR "Window %d dose not exist\n", winno);
                return -1;
        }
        pci = (struct ccsr_pci *)(ccsrbar+PCI_BASE_OFFSET);
                                                                                                 
        value = size;
        while ((value = value>>1) > 1) {
                order++;
        }
        
        switch (winno) {
        case 1:
                pci->pitar1 = (localaddr>>12) & 0x000fffff;
                pci->piwbar1 = (pciaddr>>12) & 0x000fffff;
                pci->piwar1 = 0x80f55000 | order;
                break;
        case 2:
                pci->pitar2 = (localaddr>>12) & 0x000fffff;
                pci->piwbar2 = (pciaddr>>12) & 0x000fffff;
                pci->piwar2 = 0x80f55000 | order;
                break;
        case 3:
                pci->pitar3 = (localaddr>>12) & 0x000fffff;
                pci->piwbar3 = (pciaddr>>12) & 0x000fffff;
                pci->piwar3 = 0x80f55000 | order;
                break;
        default:
                break;
        }
	
	return 0;
}


/*
 * Enable a message interrupt in 8540,
 * if needed direct it to the IRQ_OUT,
 * then enable it.
 */
void ena_message_interrupt(int i, int direct_out,u32 ccsrbar)
{
	u32 reg_addr;
	u32 value;
	
	if (i>3) return;

	reg_addr = ccsrbar + 0x51600 + i*0x20;
	READ_REG(reg_addr, value);
	value = value & 0x7fffffff;
	WRITE_REG(reg_addr, value);
	 
	if(direct_out)	/* if need, direct it to the IRQ_OUT */
		WRITE_REG(ccsrbar + 0x51610 + i*0x10, 0x80000001);
	
	READ_REG(ccsrbar + 0x41500, value);   /* the message enable register */
	WRITE_REG(ccsrbar + 0x41500, value |(0x01 << i));/* Enable the interrupt */
	
	return;
}


/*
 * Write a message to the message register to trig the interrupt
 * The host read it to clear the interrupt.
 * The host can also clear the interrupt by reading the status register.
 */
void trig_message_interrupt(int i, u32 message,u32 ccsrbar)
{
	u32 reg_addr;

	if (i>3)
		 return;

	reg_addr= ccsrbar + 0x41400 + i*0x10;
	WRITE_REG(reg_addr, message);
	return;
}	


void clear_message_interrupt(int i,u32 ccsrbar)
{
	u32 reg_addr;
	u32 value;

	if (i>3) return;

	reg_addr = ccsrbar + 0x41400 + i*0x10;
	READ_REG(reg_addr, value);  /* clear the interrupt by read the message register */
	return;
}


void stop_message_interrupt(int i,u32 ccsrbar)
{
	u32 reg_addr;
	u32 value;

	if (i>3) return;

	reg_addr = ccsrbar + 0x41500;
	READ_REG(reg_addr, value);
	WRITE_REG(reg_addr, value&(~(0x01 <<i)));	/* disable message interrupt i */
	READ_REG(ccsrbar + 0x51600 + i*0x20, value);
	WRITE_REG(ccsrbar + 0x51600 + i*0x20, value | 0x80000000);  /* set the mask bit */

	return;
}


/*
 * Open and close
 */
int dether_open(struct net_device *dev)
{
	struct dether_priv *priv = netdev_priv(dev);
	int retval;
	int winno;
	int order;
 	u32 phys_addr;

	retval = request_irq (dev->irq, dether_interrupt, SA_SHIRQ, dev->name, dev);
	if (retval) {
		printk ("EXIT, returning %d\n", retval);
		return retval;
	 }
	memset(priv, 0, sizeof(struct dether_priv));

	order = get_order(sizeof(struct dether_share_mem));
	priv->share_mem = (struct dether_share_mem *)__get_free_pages(GFP_KERNEL, order);
	if (priv->share_mem == NULL)
	{
		free_irq(dev->irq, dev);
		return -ENOMEM;
	}
	memset(priv->share_mem, 0, sizeof(struct dether_share_mem));
	
	phys_addr = virt_to_phys((void *)priv->share_mem);

	priv->ccsrbar = (u32)ioremap(get_ccsrbar(), MPC85xx_CCSRBAR_SIZE);
	
	if (check_mem_region(PCI_SPACE_ADDRESS,AGENT_DRIVER_MEM_SIZE)) {
		DPRINTK("dethernet:memory already in use!\n");
		free_pages((unsigned long)priv->share_mem, order);
		free_irq(dev->irq, dev);
		return -EBUSY;
	}	
	request_mem_region(PCI_SPACE_ADDRESS, AGENT_DRIVER_MEM_SIZE, "dethernet");

	/* Setup inbound window 1 in agent */
	winno = 1;
	setup_agent_inbound_window(PCI_SPACE_ADDRESS, phys_addr,
			AGENT_DRIVER_MEM_SIZE, winno, priv->ccsrbar);

	/* enable message interrupt 0 and direct it to the irq_out */
	ena_message_interrupt(0, 1, priv->ccsrbar);
	/* enable message interrupt 1,let the host interrupt me */
	ena_message_interrupt(1, 0, priv->ccsrbar);

	/* 
	 * Assign the hardware address of the agent: use "\0FSLD0",
	 * The hardware address of the host use "\0FSLD1";
	 */
	memcpy(dev->dev_addr, "\0FSLD0", ETH_ALEN);

	netif_start_queue(dev);
	return 0;
}


int dether_release(struct net_device *dev)
{
	struct dether_priv *priv = netdev_priv(dev);
	struct dether_share_mem *share_mem = priv->share_mem;
	int order;
	
	order = get_order(sizeof(struct dether_share_mem));

	netif_stop_queue(dev); /* can't transmit any more */
	stop_message_interrupt(0, priv->ccsrbar);
	stop_message_interrupt(1, priv->ccsrbar);
	release_mem_region(PCI_SPACE_ADDRESS, AGENT_DRIVER_MEM_SIZE);
	free_pages((unsigned long)share_mem, order);
	/* release ports, irq and such */
	synchronize_irq (dev->irq);
	free_irq(dev->irq, dev);

	return 0;
}


/*
 * Configuration changes (passed on by ifconfig)
 */
int dether_config(struct net_device *dev, struct ifmap *map)
{
	if (dev->flags & IFF_UP) /* can't act on a running interface */
		return -EBUSY;
	/* Don't allow changing the I/O address */
	if (map->base_addr != dev->base_addr) {
		printk(KERN_WARNING "dethernet: Can't change I/O address\n");
	return -EOPNOTSUPP;
	}
	/* Allow changing the IRQ */
	if (map->irq != dev->irq) {
		dev->irq = map->irq;
	}
	/* ignore other fields */
	return 0;
}


/*
 * Receive a packet: retrieve, encapsulate and pass over to upper levels
 */
void dether_rx(struct net_device *dev, int len, unsigned char *buf)
{
	struct dether_priv *priv = netdev_priv(dev);
	struct sk_buff *skb;

	skb = dev_alloc_skb(len+2);
	if (!skb) {
		printk("dether rx: low on mem - packet dropped\n");
		priv->stats.rx_dropped++;
		spin_unlock(&priv->lock);
		return;
	}
      
	skb_reserve(skb, 2);   
	memcpy(skb_put(skb, len), buf, len);

	/* Write metadata, and then pass to the receive level */
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
	priv->stats.rx_packets++;
	priv->stats.rx_bytes += len;

	netif_rx(skb);
	return;
}
    

/*
 * The interrupt entry point
 */
static irqreturn_t dether_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct dether_priv *priv;
	struct dether_share_mem *shmem;
	u32  statusword;

	struct net_device *dev = (struct net_device *)dev_id;
	if (!dev )  return IRQ_NONE;

	priv = netdev_priv(dev);

	/* Lock the device */
	spin_lock(&priv->lock);
	
	shmem = (struct dether_share_mem *) priv->share_mem;

	READ_REG((u32)(priv->ccsrbar) + 0x41410, statusword);
	if (statusword & HOST_SEND) {
		dether_rx(dev, shmem->rx_packetlen, shmem->rxbuf);
		shmem->hstatus = 0;
	}
	else{
	         spin_unlock(&priv->lock);
	         return IRQ_HANDLED;
	}
	
	/* Unlock the device and we are done */
	spin_unlock(&priv->lock);
	return IRQ_HANDLED;
}


/*
 * Transmit a packet (called by the kernel)
 */
int dether_tx(struct sk_buff *skb, struct net_device *dev)
{
	int len;
	int j = 0;
	char *data, shortpkt[ETH_ZLEN];
	struct dether_priv *priv = netdev_priv(dev);
  	struct dether_share_mem *shmem = priv->share_mem;

	data = skb->data;
	len = skb->len; 
	if (len < ETH_ZLEN) {
		memset(shortpkt, 0, ETH_ZLEN);
		memcpy(shortpkt, skb->data, skb->len);
		len = ETH_ZLEN;
		data = shortpkt;
        }
	if (len > 2040) {
		 printk("packet too long (%i octets)\n",len);
                 priv->stats.tx_dropped++;
                 dev_kfree_skb(skb);
                 return 0;					 
	}

	spin_lock(priv->lock);
	
	dev->trans_start = jiffies; /* save the timestamp */
	priv->cur_tx_skb = skb;
	while (shmem->astatus) {
		udelay(2);j++;
		if(j > 1000) break;
	}
	if (j > 1000)
	{
		netif_stop_queue(dev);
                priv->stats.tx_dropped++;
                spin_unlock(&priv->lock);
                return 0;
	}

      shmem->tx_packetlen = len;
      memcpy(shmem->txbuf, data, len);
      shmem->astatus = 1;
     
      dev_kfree_skb(priv->cur_tx_skb);
      priv->stats.tx_packets++;
      priv->stats.tx_bytes += len;
      
      trig_message_interrupt(0, AGENT_SEND, priv->ccsrbar);

      spin_unlock(&priv->lock);
      return 0; 
}

/*
 * Deal with a transmit timeout.
 */
void dether_tx_timeout (struct net_device *dev)
{
	struct dether_priv *priv = netdev_priv(dev);
   	struct dether_share_mem *shmem = priv->share_mem;
 
	dev_kfree_skb(priv->cur_tx_skb);
	shmem->astatus = 0;
	priv->stats.tx_errors++;
	netif_wake_queue(dev);
	return;
}


/*
 * Ioctl commands 
 */
int dether_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	printk("ioctl\n");
	return 0;
}


struct net_device_stats *dether_stats(struct net_device *dev)
{
	struct dether_priv *priv = netdev_priv(dev);
	return &priv->stats;
}

int dether_rebuild_header(struct sk_buff *skb)
{
	struct ethhdr *eth = (struct ethhdr *) skb->data;
	struct net_device *dev = skb->dev;
    
	memcpy(eth->h_source, dev->dev_addr, dev->addr_len);
	memcpy(eth->h_dest, dev->dev_addr, dev->addr_len);
	eth->h_dest[ETH_ALEN-1]   ^= 0x01;   /* dest is us xor 1 */

	return 0;
}


int dether_header(struct sk_buff *skb, struct net_device *dev,
                unsigned short type, void *daddr, void *saddr,
                unsigned int len)
{
	struct ethhdr *eth = (struct ethhdr *)skb_push(skb,ETH_HLEN);

	eth->h_proto = htons(type);
	memcpy(eth->h_source, dev->dev_addr, dev->addr_len);
	memcpy(eth->h_dest,   dev->dev_addr, dev->addr_len);
	eth->h_dest[ETH_ALEN-1]   ^= 0x01;   
        return (dev->hard_header_len);
}


int dether_change_mtu(struct net_device *dev, int new_mtu)
{
	struct dether_priv *priv = netdev_priv(dev);
	spinlock_t *lock = &priv->lock;
	unsigned long flags;

	/* check ranges */
	if ((new_mtu < 68) || (new_mtu > 1500))
		return -EINVAL;
	spin_lock_irqsave(lock, flags);
	dev->mtu = new_mtu;
	spin_unlock_irqrestore(lock, flags);
	return 0; /* success */
}


/*
 * init function.
 */
void dether_init(struct net_device *dev)
{
	struct dether_priv *priv;

	ether_setup(dev); /* assign some of the fields */

	dev->open		= dether_open;
	dev->stop		= dether_release;
	dev->set_config		= dether_config;
	dev->hard_start_xmit	= dether_tx;
	dev->do_ioctl		= dether_ioctl;
	dev->get_stats		= dether_stats;
	dev->change_mtu		= dether_change_mtu;  
	dev->rebuild_header	= dether_rebuild_header;
	dev->hard_header	= dether_header;
	dev->tx_timeout		= dether_tx_timeout;
	dev->watchdog_timeo	= timeout;
	/* keep the default flags, just add NOARP */
	dev->flags		|= IFF_NOARP;
	dev->features		|= NETIF_F_NO_CSUM;
	dev->hard_header_cache = NULL;      /* Disable caching */
	dev->irq = IRQ_NUM;
	SET_MODULE_OWNER(dev);

	/*
	 * Zero the private struct data.
	 */
	priv = netdev_priv(dev);
	memset(priv, 0, sizeof(struct dether_priv));

	spin_lock_init(&priv->lock);
	return;
}


/*
 * The device
 */
struct net_device *dether_dev;


int dether_init_module(void)
{
	int result;
	int dether_eth;
	char interface_name[IFNAMSIZ];
	
	dether_eth = eth; /* copy the cfg datum in the non-static place */
	if (!dether_eth) 
		strcpy(interface_name, "deth%d");
	else  
		strcpy(interface_name, "eth%d");

	/* Allocate the devices */
	dether_dev = alloc_netdev(sizeof(struct dether_priv), interface_name,
			dether_init);
	if (dether_dev == NULL)
		return -ENOMEM;
	
	if ( (result = register_netdev(dether_dev)) ) {
		printk("dether: error %i registering device \"%s\"\n",
			result, dether_dev->name);
		free_netdev(dether_dev);
		return -ENODEV;
	}

	return 0;
}


void dether_cleanup(void)
{
	unregister_netdev(dether_dev);
	free_netdev(dether_dev);

        return;
}
module_init(dether_init_module);
module_exit(dether_cleanup);
