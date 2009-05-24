/* 
 * driver/net/trimedia/lan100too.c 
 * LAN100 NIC driver for linux kernel.
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <tmhwEth.h>
#include <tmbslPhy.h>

#include "lan100.h"

#define LAN100_DEBUG
#define LAN100_ERR_EXIT

#ifndef LAN100_DEBUG 
#define ASSERT(expr) 		do {} while (0)
#define DEBUGP(format, args...)
#else
#define ASSERT(expr) 								\
        if(unlikely(!(expr))) {				        		\
        	printk(KERN_ERR "Assertion failed! %s,%s,%s,line=%d\n",		\
        		#expr,__FILE__,__FUNCTION__,__LINE__);		        \
		panic("fatal error"); 						\
        }
#define DEBUGP printk
#endif

#ifndef LAN100_ERR_EXIT
#define LAN100_ERR(format, args...)
#else
#define LAN100_ERR(format, args...)    						\
	do { 									\
		panic(" %s,%s: " format , __FILE__ , __FUNCTION__ , ##args);    \
       	} while(0)
#endif 

#define LAN100_DEF_MSG_ENABLE   		( NETIF_MSG_DRV 	\
						| NETIF_MSG_PROBE 	\
						| NETIF_MSG_LINK 	\
						| NETIF_MSG_IFUP 	\
						| NETIF_MSG_IFDOWN 	\
						| NETIF_MSG_RX_ERR 	\
						| NETIF_MSG_TX_ERR 	\
						| 0 )

#define LAN100_RX_DES_NUM       		16	
#define LAN100_TX_DES_NUM   			8

#define LAN100_WATCHDOG_PERIOD   		(2*HZ)
#define LAN100_NAPI_WEIGHT 	 		(16)

struct lan100_tx_descriptor
{
	u32 frame_addr;
	u32 ctl_info;
	u32 time_stamp;
	u32 pad;
};

struct lan100_rx_descriptor
{
	u32 frame_addr;
	u32 ctl_info;
};

struct lan100_status_descriptor
{
	u32 status;
	u32 time_stamp;
};

struct lan100_nic 
{
	spinlock_t lock;
	unsigned int cur_tx;   /* Produce index for transmit ring*/
	unsigned int dirty_tx; /* The ring entries to be free()ed */

	u32 msg_enable;
	int id;

	/*tx_des same as des_mem, which is used when lan100_close*/
	struct lan100_tx_descriptor* tx_des;  
	struct lan100_status_descriptor* tx_stat_des;

	struct lan100_rx_descriptor* rx_des;
	struct lan100_status_descriptor* rx_stat_des;

	/*tx_buf[0] same as buf_mem, which is used when lan100_close*/
	unsigned char* tx_buf[LAN100_TX_DES_NUM]; 
	unsigned char* rx_buf[LAN100_RX_DES_NUM];

	dma_addr_t des_dma;
	dma_addr_t buf_dma;

	ptmbslPhyConfig_t phy_interface; 

	struct net_device* netdev;
	struct net_device_stats stats; 
};

#define GPIO_CTL0 		0x104000
#define GPIO_CTL1 		0x104004
#define GPIO_CTL2 		0x104008
#define GPIO_CTL3 		0x10400C

/*
 * Set the GPIO PIN about LAN100, 
 * refer Table 1: GPIO Pin List of PNX15XX PNX952x databook. 
 */
static void 
lan100_set_gpio_pin(void)
{
	unsigned long regVal;
	unsigned long i;

	/*The Mode Control bit pairs which control GPIO pins 47-32*/
	regVal=MMIO(GPIO_CTL2);
	/*GPIO pin list from 35 to 47*/
	regVal &= 0x0000003f;
	/*place Pin in primary function mode*/
	regVal = (regVal | 0x55555540);
	MMIO(GPIO_CTL2)=regVal;

	/*The Mode Control bit pairs which control GPIO pins 60-48*/
	regVal=MMIO(GPIO_CTL3);
	/*GPIO pin from 48 to 50*/
	regVal &= 0xffffffc0;
	/*place Pin in primary function mode*/
	regVal = (regVal | 0x00000015);
	MMIO(GPIO_CTL3)=regVal;

	for(i=0; i<1000; i++)
	{
		;
	}
}

static void lan100_rx_intr(struct lan100_nic* nic)
{
	tmErrorCode_t ethStats=TM_OK;
	unsigned long produceIndex=0;
	unsigned long consumeIndex=0;
	int margin=0;
	int entry=0;

	ethStats = tmhwEthRxGetConsumeIndex(nic->id,&consumeIndex);
	ethStats |= tmhwEthRxGetProduceIndex(nic->id,&produceIndex);
	ASSERT(ethStats==TM_OK);

	while(produceIndex!=consumeIndex){
		int pkt_status=0;
		int pkt_len=0;
		int pkt_err=0;
		int discard=0;

		pkt_status = nic->rx_stat_des[consumeIndex].status;
		pkt_err = pkt_status & ETH_RX_DS_ALL_ERROR_MASK;

		/*HW BUG: range error of receive descriptor status sometime is invalid*/
		if((pkt_err) && (pkt_err!= ETH_RX_DS_ONLY_RANGE_ERROR_MASK)){
			discard=1;
		}else{
			pkt_len= (pkt_status & ETH_RX_DS_ENTRY_LVL_MASK) + 1;
			if(pkt_len > LAN100_PKT_BUF_SZ){
				/*no error occured, but huge frame, discard it*/
				if(netif_msg_rx_err(nic)){
					printk(KERN_DEBUG "%s: Huge frame received with %d bytes at slot %d\n",
							nic->netdev->name,pkt_len,consumeIndex);
				}

				nic->stats.rx_length_errors++;
				discard=1;
			}
		}

		if(!discard){
			/*valid packet!!*/
			struct sk_buff* skb=NULL;

			skb = dev_alloc_skb(pkt_len+2);
			if(likely(skb!=NULL)){
				if(netif_msg_rx_status(nic)){
					printk(KERN_DEBUG "%s: Receive %d bytes at slot %d\n",
							nic->netdev->name,pkt_len,consumeIndex);
				}

				skb->dev = nic->netdev;
				skb_reserve (skb, 2);	/* 16 byte align the IP fields. */
				eth_copy_and_sum (skb, nic->rx_des[consumeIndex].frame_addr, pkt_len, 0);
				skb_put (skb, pkt_len);
				skb->protocol = eth_type_trans (skb, nic->netdev);
				nic->netdev->last_rx = jiffies;
				nic->stats.rx_bytes += pkt_len;
				nic->stats.rx_packets++;

				netif_receive_skb (skb);
			}else{
				if (net_ratelimit()) 
					printk (KERN_WARNING
							"%s: Memory squeeze, dropping packet.\n",
							nic->netdev->name);
				if(netif_msg_rx_err(nic)){
					printk(KERN_DEBUG "%s: no memory to alloc skb at slot %d.\n",
							nic->netdev->name,consumeIndex);
				}
				nic->stats.rx_dropped++;

				LAN100_ERR("%s: no memory discard packet\n",nic->netdev->name);
			}
		}else{
			/*invalid packet, forget it*/
			if(netif_msg_rx_err(nic)){
				printk(KERN_DEBUG "%s: packet receive error at slot %d, with status 0x%08x\n",
						nic->netdev->name,consumeIndex,pkt_err);	
			}

			LAN100_ERR("%s: error packets received.\n",nic->netdev->name);

			nic->stats.rx_errors++;
			if(pkt_err & ETH_RX_DS_LENGTH_ERROR) nic->stats.rx_length_errors++;
			if(pkt_err & ETH_RX_DS_CRC_ERROR)  nic->stats.rx_crc_errors++;	
			if(pkt_err & ETH_RX_DS_RANGE_ERROR) nic->stats.rx_length_errors++;
			if(pkt_err & ETH_RX_DS_NO_DESCRIPTOR) nic->stats.rx_over_errors++;
			if(pkt_err & ETH_RX_DS_OVERRUN) nic->stats.rx_fifo_errors++;
		}

		/*reuse rx descriptors*/
		nic->rx_stat_des[consumeIndex].status = 0;

		consumeIndex++;
		if(consumeIndex==LAN100_RX_DES_NUM) consumeIndex=0;
	}
	
	ethStats=tmhwEthRxSetConsumeIndex(nic->id,consumeIndex); 
	ASSERT(ethStats==TM_OK);

	return;
}

static void lan100_tx_intr(struct lan100_nic* nic)
{
	unsigned long dirty_tx=0;
	unsigned long tx_left=0;

	dirty_tx=nic->dirty_tx;
	tx_left=nic->cur_tx-dirty_tx;

	while(tx_left>0){
		int entry = dirty_tx % LAN100_TX_DES_NUM;
		int pkt_status = 0;
		int pkt_len = 0;

		pkt_status = nic->tx_stat_des[entry].status;
		pkt_len = (nic->tx_des[entry].ctl_info & LAN100_ETH_TX_DC_PACKET_SIZE_MASK) + 1;

		/*check the status of every packets and update the stats*/
		if(unlikely(pkt_status & LAN100_TX_DS_ERROR)){
			/*transmit packet error*/
			if(netif_msg_tx_err(nic)){
				printk(KERN_DEBUG "%s: tx err at slot %d, with status 0x%08x\n",
						nic->netdev->name,entry,pkt_status);
			}
			
			LAN100_ERR("%s: tx err at slot %d\n",nic->netdev->name,entry);

			/*update stats*/
			nic->stats.tx_errors++;
			/*FIXME: research net_device_status carefully*/
			if(pkt_status & LAN100_TX_DS_NO_DESCRIPTOR)
				nic->stats.tx_fifo_errors++;
			if(pkt_status & LAN100_TX_DS_LATE_COLLISION)
				nic->stats.tx_window_errors++;
		}else{
			/*transmit packet success*/
			if(netif_msg_tx_done(nic)){
				printk(KERN_DEBUG "%s: tx done at slot %d\n",
						nic->netdev->name,entry);
			}
			nic->stats.tx_packets++;
			nic->stats.tx_bytes += pkt_len; 
		}


		/*reuse the #entry rx status/descriptor*/
		nic->tx_des[entry].ctl_info = 0;
		nic->tx_stat_des[entry].status = 0;
		
		dirty_tx++;
		tx_left--;	
	}

	if(nic->dirty_tx!=dirty_tx){
		nic->dirty_tx = dirty_tx;
		mb();
		netif_wake_queue(nic->netdev);
	}	
}

static irqreturn_t lan100_intr(int irq,void* dev_id,struct pt_regs* regs)
{
	tmErrorCode_t ret=TM_OK;
	struct net_device* netdev=(struct net_device*)dev_id;
	u32 IntStatus;
	struct lan100_nic* nic=netdev_priv(netdev);
	int handled=0;

	spin_lock(&nic->lock);

	ret=tmhwEthIntGetStatus(nic->id,&IntStatus);
	ASSERT(ret==TM_OK);

	/*shared irq? */
	if(unlikely(IntStatus==0))
		goto out;

	if(netif_msg_intr(nic)){
		printk(KERN_DEBUG "%s: interrupted with status 0x%08x\n",
				nic->netdev->name,IntStatus);
	}

	handled = 1;

	ret=tmhwEthIntClear(nic->id,IntStatus);
	ASSERT(ret==TM_OK);

	/*close possible race's with dev_close*/
	if(unlikely(!netif_running(netdev))){
		goto out;
	}

	if(IntStatus & (TMHW_ETH_INTR_RX_DONE | TMHW_ETH_INTR_TX_ERROR)){
		/*packet received, do packet handling*/
		lan100_rx_intr(nic);
	}
	
	if(IntStatus & (TMHW_ETH_INTR_TX_DONE | TMHW_ETH_INTR_TX_ERROR)){
		lan100_tx_intr(nic);
	}

	/*Other command/error handling here */
	ASSERT(!(IntStatus & TMHW_ETH_INTR_RX_OVERRUN));

out:
	spin_unlock(&nic->lock);
	return IRQ_RETVAL(handled);
}

static int lan100_xmit_frame(struct sk_buff* skb,struct net_device* netdev)
{
	struct lan100_nic* nic=netdev_priv(netdev);
	unsigned long flags;
	int entry=0;
	int need_tx_done_intr=0;

	/* Calculate the tx descriptor entry */
	entry = nic->cur_tx % LAN100_TX_DES_NUM;

	if(likely(skb->len < LAN100_PKT_BUF_SZ)){
		skb_copy_and_csum_dev(skb,nic->tx_buf[entry]);
		dev_kfree_skb(skb);	
	}else{
		dev_kfree_skb(skb);
		nic->stats.tx_dropped++;	
		return 0;
	}

	if( nic->cur_tx % 4 == 0){
		/*Not every packet sent needs a Tx Done interrupt.*/
		need_tx_done_intr=1;
	}

	spin_lock_irqsave(&nic->lock,flags);

	{
		/*send transmit request to lan100 hardware chip*/
		u32 control_info=0;
		tmErrorCode_t ethStatus=TM_OK;

		control_info = skb->len-1;
		control_info &= LAN100_ETH_TX_DC_PACKET_SIZE_MASK;

		control_info |= LAN100_ETH_TX_DC_APPEND_HW_CRC | LAN100_ETH_TX_DC_HUGE_FRAME| \
				LAN100_ETH_TX_DC_PAD_SHORT_PACKET | LAN100_ETH_TX_DC_OVERRIDE;		

		if(need_tx_done_intr){
			control_info |= LAN100_ETH_TX_DC_GEN_INT;
		}
		
		control_info |= LAN100_ETH_TX_DC_LAST_PACKET;

		nic->tx_des[entry].ctl_info=control_info;
		
		ethStatus=tmhwEthTxSetProduceIndex(nic->id,(entry+1)%LAN100_TX_DES_NUM,tmhwEthNonRealTime);
		ASSERT(ethStatus==TM_OK);
	}
	
	nic->cur_tx++;
	wmb();

	if((int)(nic->cur_tx - nic->dirty_tx) >= LAN100_TX_DES_NUM){
		netif_stop_queue(netdev);
	}
	
	netdev->trans_start=jiffies;

	spin_unlock_irqrestore(&nic->lock,flags);

	if (netif_msg_tx_queued(nic))
		printk (KERN_DEBUG "%s: Queued Tx packet size %u to slot %d.\n",
			netdev->name, skb->len, entry);

	return 0;
}

static struct net_device_stats* lan100_get_stats(struct net_device* netdev)
{
	struct lan100_nic* nic = netdev_priv(netdev);
	DEBUGP("lan100_get_stats was called!\n");
	return &nic->stats;
}

/* Set or clear the multicast filter for this adaptor.
   This routine is not state sensitive and need not be SMP locked. */
static void lan100_set_rx_mode(struct net_device* netdev)
{
	struct lan100_nic* nic= netdev_priv(netdev);
	
	if(netdev->flags & IFF_PROMISC) {
	
	} else if (netdev->flags & IFF_ALLMULTI) {
	
	} else {
	
	}
}

static void lan100_set_multicast_list(struct net_device* netdev)
{
	struct lan100_nic* nic=netdev_priv(netdev);
	unsigned long flags=0;

	DEBUGP("lan100_set_multicast_list was called!\n");

	spin_lock_irqsave(&nic->lock,flags);
	lan100_set_rx_mode(netdev);
	spin_unlock_irqrestore(&nic->lock,flags);
}

static int
lan100_do_ioctl(struct net_device* netdev, struct ifreq* ifr, int cmd)
{
	int ret=-EFAULT;

	DEBUGP("lan100_do_ioctl was called!\n")	;

	return ret;
}

static void 
lan100_tx_timeout(struct net_device* netdev)
{
	DEBUGP("lan100_tx_timeout was called\n");
	return;
}

static int lan100_poll(struct net_device* netdev, int* budget)
{
	int ret=-EFAULT;

	DEBUGP("lan100_poll was called!\n");

	return ret;
}


static inline void
lan100_disable_intr(struct lan100_nic* nic)
{
	tmErrorCode_t ethStatus=TM_OK;
	
	// Power up the device
	// clear and enable interrupts
	ethStatus = tmhwEthSetPowerState(nic->id, tmPowerOff);
	ethStatus |= tmhwEthIntDisable(nic->id,LAN100_ETH_INTR_ALL);
	ethStatus |= tmhwEthIntClear(nic->id, LAN100_ETH_INTR_ALL);
	ethStatus |= tmhwEthRxWolClear(nic->id, LAN100_ETH_INTR_ALL);
	ASSERT(ethStatus==TM_OK);
}

static inline void 
lan100_enable_intr(struct lan100_nic* nic)
{
	tmErrorCode_t ethStatus=TM_OK;
	
	// Power up the device
	// clear and enable interrupts
	ethStatus = tmhwEthSetPowerState(nic->id, tmPowerOn);
	ethStatus |= tmhwEthIntDisable(nic->id,LAN100_ETH_INTR_ALL);
	ethStatus |= tmhwEthIntClear(nic->id, LAN100_ETH_INTR_ALL);
	ethStatus |= tmhwEthRxWolClear(nic->id, LAN100_ETH_INTR_ALL);
	ethStatus |=tmhwEthIntEnable(nic->id,LAN100_ETH_INT_MASK);
	ASSERT(ethStatus==TM_OK);

	return;
}

static inline void 
lan100_softreset(struct lan100_nic* nic)
{
	tmErrorCode_t ethStats=TM_OK;

	ethStats = tmhwEthSoftReset(nic->id,TMHW_ETH_FULL_RESET);
	ethStats |= nic->phy_interface->softResetFunc(nic->id);
	ASSERT(ethStats==TM_OK);
}

static int
lan100_close(struct net_device* netdev)
{
	struct lan100_nic* nic=netdev_priv(netdev);
	int buf_size=0;
	int des_size=0;

	if(netif_msg_ifdown(nic)){
		printk(KERN_DEBUG "%s: lan100 nic #%d close\n",nic->netdev->name,nic->id);
	}

	netif_stop_queue(netdev);
	lan100_disable_intr(nic);
	lan100_softreset(nic);

	buf_size= LAN100_PKT_BUF_SZ*LAN100_TX_DES_NUM +
		LAN100_PKT_BUF_SZ*LAN100_RX_DES_NUM;
	dma_free_coherent(NULL,buf_size,nic->tx_buf[0],nic->buf_dma);

	des_size = sizeof(struct lan100_tx_descriptor)*LAN100_TX_DES_NUM +
		sizeof(struct lan100_status_descriptor)*LAN100_TX_DES_NUM +
		sizeof(struct lan100_rx_descriptor)*LAN100_RX_DES_NUM +
		sizeof(struct lan100_status_descriptor)*LAN100_RX_DES_NUM;
	dma_free_coherent(NULL,des_size,nic->rx_des,nic->des_dma);
	
	synchronize_irq(netdev->irq);
	free_irq(netdev->irq,netdev);

	return 0;
}

static int 
lan100_open(struct net_device* netdev)
{
	tmErrorCode_t ethStatus=TM_OK;
	int changeConfig=0;
	tmhwEthTxDescriptor_t       ethTxDescriptor;
	tmhwEthRxDescriptor_t       ethRxDescriptor;
	tmbslPhyEnableDisable_t linkStatus=tmbslPhyDisable;
	dma_addr_t rx_bufs_dma=0;
	unsigned char* p=NULL;
	int i=0;
	int buf_size=0;
	unsigned char* buf_mem=NULL;
	dma_addr_t buf_dma=0;
	int des_size=0;
	void* des_mem=NULL;
	dma_addr_t des_dma=0;
	int ret=0;
	struct lan100_nic* nic=netdev_priv(netdev);

	/*
	 * Tell network stack this carrier is closed
	 * unless open the carrier, no packets will pass to it.
	 * */
	netif_carrier_off(netdev);
	
	ret=request_irq(netdev->irq,lan100_intr,SA_SHIRQ,netdev->name,netdev);
	if(ret!=0){
		printk(KERN_ERR "%s: failed to request irq\n",netdev->name);
		return ret;
	}

	/*
	 * Allocate descriptors: TX/TXStats, RX/RXStats and record it into lan100_nic
	 * */
	des_size = sizeof(struct lan100_tx_descriptor)*LAN100_TX_DES_NUM +
		sizeof(struct lan100_status_descriptor)*LAN100_TX_DES_NUM +
		sizeof(struct lan100_rx_descriptor)*LAN100_RX_DES_NUM +
		sizeof(struct lan100_status_descriptor)*LAN100_RX_DES_NUM;
	des_mem=dma_alloc_coherent(NULL,des_size,&des_dma);
	if(des_mem==NULL){
		printk(KERN_ERR, "%s: failed to request memory for NIC descriptors\n",netdev->name);
		ret=-ENOMEM;
		goto irq_free;
	}
	memset(des_mem,0,des_size);

	/*record it into lan100_nic*/
	nic->tx_des=(struct lan100_tx_descriptor*)des_mem;
	nic->tx_stat_des=(struct lan100_status_descriptor*)(nic->tx_des+LAN100_TX_DES_NUM);
	nic->rx_des=(struct lan100_rx_descriptor*)nic->tx_stat_des+LAN100_TX_DES_NUM; 
	nic->rx_stat_des=(struct lan100_status_descriptor*)(nic->rx_des+LAN100_RX_DES_NUM);
	nic->des_dma=des_dma;

	/*
	 * Allocate tx/rx buffers and record them into tx/rx descriptors.
	 * */
	/*LAN100_PKT_BUF_SZ has been aligned accord the lan100 datasheet.*/
	buf_size= LAN100_PKT_BUF_SZ*LAN100_TX_DES_NUM +
		LAN100_PKT_BUF_SZ*LAN100_RX_DES_NUM;
	buf_mem = dma_alloc_coherent(NULL,buf_size,&buf_dma); 
	if(buf_mem==NULL){
		printk(KERN_ERR, "%s: failed to request memory for NIC buffers.\n",netdev->name);
		ret=-ENOMEM;
		goto des_mem_free;
	}
	memset(buf_mem,0,buf_size);

	/*record the nic->tx_buf*/
	p = buf_mem;
	for(i=0;i<LAN100_TX_DES_NUM;i++){
		nic->tx_buf[i]= p+i*LAN100_PKT_BUF_SZ;
	}

	/*record the nic->rx_buf*/
	p=buf_mem+LAN100_PKT_BUF_SZ*LAN100_TX_DES_NUM;
	for(i=0;i<LAN100_RX_DES_NUM;i++){
		nic->rx_buf[i]= p+i*LAN100_PKT_BUF_SZ;
	}

	/*fill them into tx/rx descriptors*/
	for(i=0;i<LAN100_TX_DES_NUM;i++){
		nic->tx_des[i].frame_addr=buf_dma+i*LAN100_PKT_BUF_SZ;
	}
	rx_bufs_dma=buf_dma+LAN100_TX_DES_NUM*LAN100_PKT_BUF_SZ;
	for(i=0;i<LAN100_RX_DES_NUM;i++){
		nic->rx_des[i].frame_addr=rx_bufs_dma+i*LAN100_PKT_BUF_SZ;
		nic->rx_des[i].ctl_info= (LAN100_PKT_BUF_SZ-1) | ETH_RX_DC_GEN_INT;
	}

	/*
	 * Init the TX indexes and Rx indexes.
	 * */
	nic->cur_tx=0;
	nic->dirty_tx=0;

#if 0
	/*
	 * lan100 nic softreset (mac and phy)
	 * FIXME: should do softreset anyway and 
	 *   set default rx config and tx config below 
	 * */
	lan100_softreset(nic);
#endif 

	/*
	 * through mii, check link status and autonegticate if need.
	 * FIXME: should use MII of linux
	 * */
#if 0
	/*
	 * HW BUG: always fail to get link status through phy.
	 * */
	nic->phy_interface->getLinkStatusFunc(nic->id,&linkStatus);
#endif 
	if(1){
		tmbslPhyAutoNegotiationMask_t autoMask;
		tmbslPhyBasicModeStatus_t phyModeStatus;
		tmhwEthConfig_t ethConfig;
		u32 ethMode;
		u32 ethSpeed;

		/*carrer on and do autonegticate.*/
		netif_carrier_on(netdev);

		autoMask.masknextPageDesired = LAN100_ETH_CAPABILITY_DECLARED;
		autoMask.maskRemoteFault = LAN100_ETH_CAPABILITY_MASKED;
		autoMask.mask100BaseT4 = LAN100_ETH_CAPABILITY_MASKED;
		autoMask.mask100BaseTxFullDuplexSupport = LAN100_ETH_CAPABILITY_DECLARED;
		autoMask.mask100BaseTxSupport = LAN100_ETH_CAPABILITY_DECLARED;
		autoMask.mask10BaseTFullDuplexSupport = LAN100_ETH_CAPABILITY_DECLARED;
		autoMask.mask10BaseTSupport = LAN100_ETH_CAPABILITY_DECLARED;
		autoMask.protocolSel = tmbslPhyIEEE8023;

		ethStatus = nic->phy_interface->autoNegotiateFunc(nic->id, &autoMask);
		ethStatus |= nic->phy_interface->getBasicModeStatusFunc(nic->id, &phyModeStatus);
		ethStatus |= tmhwEthGetConfig(nic->id, &ethConfig);
		ASSERT(ethStatus==TM_OK);

		ethMode = ethConfig.ethMacConfig & TMHW_ETH_MAC_FULL_DUPLEX;
		ethSpeed = ethConfig.phySupport & TMHW_ETH_PHYSUPPORT_100MBPS_MODE;

		if ((phyModeStatus.duplexMode == tmbslPhyHalfDuplex) && (ethMode))
		{
			changeConfig = 1;
			ethConfig.back2BackIntrePacketGap = LAN100_ETH_B2B_INTERPKTGAP_HD;
			ethConfig.ethMacConfig &= ~(TMHW_ETH_MAC_FULL_DUPLEX);
		}
		else if ( (phyModeStatus.duplexMode == tmbslPhyFullDuplex) && (!(ethMode)) )
		{
			changeConfig = 1;
			ethConfig.back2BackIntrePacketGap = LAN100_ETH_B2B_INTERPKTGAP_FD;
			ethConfig.ethMacConfig |= TMHW_ETH_MAC_FULL_DUPLEX;
		}

		if ((phyModeStatus.speed == tmbslPhySpeed10Mbps) && (ethSpeed))
		{
			changeConfig = 1;
			ethConfig.phySupport &= ~(TMHW_ETH_PHYSUPPORT_100MBPS_MODE);
			ethConfig.phySupport |= TMHW_ETH_PHYSUPPORT_10BASE_T_ENDEC_MODE;
		}
		else if ( (phyModeStatus.speed == tmbslPhySpeed100Mbps) && (!(ethSpeed)) )
		{
			changeConfig = 1;
			ethConfig.phySupport |= TMHW_ETH_PHYSUPPORT_100MBPS_MODE;
			ethConfig.phySupport &= ~(TMHW_ETH_PHYSUPPORT_10BASE_T_ENDEC_MODE);
		}

		if (changeConfig)
		{
			// Change MAC configuration if required 
			// depending on PHY configuration after autonegotiation
			ethMode = tmhwEthConfig(nic->id, &ethConfig);
			ASSERT(ethMode==TM_OK);
		}
	}

	/*
	 * lan100 HW nic initialization: 
	 * write descriptors, configure rx mode, configure tx mode and enable interrupts
	 * After that, some packets may come in.
	 * */

	des_dma= nic->des_dma;
	ethTxDescriptor.txDescriptor = (UInt32) des_dma;
	ethTxDescriptor.txDescriptorNumber = LAN100_TX_DES_NUM - 1;
	des_dma= des_dma+sizeof(struct lan100_tx_descriptor)*LAN100_TX_DES_NUM;
	ethTxDescriptor.txStatus = (UInt32) des_dma;

	ethStatus = tmhwEthTxConfig(nic->id, &ethTxDescriptor, tmhwEthNonRealTime);
	ethStatus |= tmhwEthTxSetProduceIndex(nic->id, 0, tmhwEthNonRealTime);
	ASSERT(ethStatus==TM_OK);

	des_dma= des_dma+sizeof(struct lan100_status_descriptor)*LAN100_TX_DES_NUM;
	ethRxDescriptor.rxDescriptor = (UInt32)des_dma;
	ethRxDescriptor.rxDescriptorNumber = LAN100_RX_DES_NUM - 1;
	des_dma= des_dma+sizeof(struct lan100_rx_descriptor)*LAN100_RX_DES_NUM;
	ethRxDescriptor.rxStatus = (UInt32)des_dma;
	ethRxDescriptor.rxFlowControl = tmhwEthEnable;  // Enable Rx Flow Control

	ethStatus |= tmhwEthRxConfig(nic->id, &ethRxDescriptor);
	ethStatus |= tmhwEthRxSetConsumeIndex(nic->id, 0);
	ASSERT(ethStatus==TM_OK);

	/*TODO: configure tx/rx default mode*/

	lan100_enable_intr(nic);

	if(netif_msg_ifup(nic)){
		printk(KERN_DEBUG "%s: lan100 NIC #%d opened.\n",nic->netdev->name,nic->id);
	}

	/*
	 * start a kernel thread or timer to check link status periodly.
	 * */

	/*
	 * Tell network stack I am ready to work now.
	 * */
	netif_start_queue(netdev);

	return 0;

buf_mem_free:
	if(buf_mem!=NULL){
		dma_free_coherent(NULL,buf_size,buf_mem,buf_dma);
	}
des_mem_free:
	if(des_mem!=NULL){
		dma_free_coherent(NULL,des_size,des_mem,des_dma);
	}
irq_free:
	free_irq(netdev->irq,netdev);
	return ret;
}

static int lan100_nic_index=-1;

/*
 * TODO: implement mtl device and bus managment.
 * */
struct mtl_device
{
	int irq;
	void* sysdata;
};


static inline void mtl_set_drvdata(struct mtl_device* mdev,void* data)
{
	mdev->sysdata=data;
}

static inline void* mtl_get_drvdata(struct mtl_device* mdev)
{
	return mdev->sysdata;
}

static inline void* mtl_device_irq(struct mtl_device* mdev)
{
	return mdev->irq;
}

/***********************************************************/

static int lan100_init_one(struct mtl_device* mdev)
{
	ptmbslPhyConfig_t EthPhyInterface=NULL;
	int i=0;
	struct {
		unsigned char byte[6];
	} mac_addr;
	tmhwEthConfig_t ethConfig;
	int setDefaultMacAddr=0;
	int ret=0;
	struct lan100_nic* nic=NULL;
	struct net_device* dev=NULL;
	tmErrorCode_t ethStatus=TM_OK;

	lan100_nic_index++;

	/*
	 * FIXME: should be called at board-specific initliazation.
	 */
	lan100_set_gpio_pin();

	/*
	 * Ensure the HW CHIP found is LAN100 NIC chip, if so, do hardware level initialization.
	 * Note: after tmhwEthInit: the interrupt of LAN100 NIC is disabled.
	 * */
	ethStatus = tmhwEthInit(lan100_nic_index);
	if (ethStatus != TM_OK)
	{
		printk(KERN_EMERG "The HW chip found is not LAN100 NIC chip!\n");
		return -EFAULT;
	}

	/*
	 * Make sure the lan100 is attached with a PHY and init it.
	 * */
	tmbslPhyGetInterface(lan100_nic_index,&EthPhyInterface);
	if(EthPhyInterface==NULL){
		printk(KERN_EMERG "Can not get PHY attached to LAN100 NIC #%d!\n",
				lan100_nic_index);
		ret=-EFAULT;
		goto tmhweth_deinit;
	}

	if (EthPhyInterface->lanClkSrc == 0x01)
	{
		MMIO(0x4702c)=0x83B425ED; // 25MHz
		MMIO(0x47118)=0x1b;       // Use dds7 and enable
	}
	MMIO(0x4711c)=0x3;
	MMIO(0x47120)=0x3;

	ethStatus = EthPhyInterface->initFunc(lan100_nic_index);
	if (ethStatus != TM_OK)
	{
		printk(KERN_EMERG "failed to init the PHY attached to LAN100 NIC #%d!\n",
				lan100_nic_index);
		ret=-EFAULT;
		goto tmhweth_deinit;
	}

	/*
	 * Allocate the net_device instance and init the private and public member for it.
	 * */
	dev=alloc_etherdev(sizeof(struct lan100_nic));
	if(dev==NULL){
		printk(KERN_ERR "fail to alloc memory for net device instance of LAN100 NIC #%d\n",
				lan100_nic_index);
		ret=-ENOMEM;
		goto phy_deinit;
	}
	SET_MODULE_OWNER(dev);

	/*init the net device instance private member: lan100_nic*/
	nic=netdev_priv(dev);
	spin_lock_init(&nic->lock);
	nic->msg_enable = LAN100_DEF_MSG_ENABLE;
	nic->phy_interface=EthPhyInterface;
	nic->netdev=dev;
	nic->id=lan100_nic_index;
	memset(&nic->stats,0,sizeof(nic->stats));

	/*init the net device instance public members*/
	dev->open = lan100_open;
	dev->hard_start_xmit = lan100_xmit_frame;
	dev->poll = lan100_poll;
	dev->weight = LAN100_NAPI_WEIGHT;
	dev->stop = lan100_close;
	dev->get_stats = lan100_get_stats;
	dev->set_multicast_list = lan100_set_multicast_list;
	dev->do_ioctl = lan100_do_ioctl;
#if 0
	dev->ethtool_ops = &lan100_ethtool_ops;
#endif 
	dev->tx_timeout = lan100_tx_timeout;
	dev->watchdog_timeo = LAN100_WATCHDOG_PERIOD;
#ifdef CONFIG_NET_POLL_CONTROLLER
	dev->poll_controller = lan100_poll_controller;
#endif
#if 0
	dev->set_mac_address = lan100_set_mac_address;
	dev->change_mtu = lan100_change_mtu;
#endif 

	/*
	 * throught the use of skb_copy_and_csum_dev, which can deal with NETIF_F_SG
	 * */
	dev->features |= NETIF_F_SG | NETIF_F_HW_CSUM | NETIF_F_HIGHDMA;
	dev->irq=mtl_device_irq(mdev);

	/*
	 * Read the MAC address and assign it to dev->dev_addr
	 * */
	// Read and Set mac address if possible.
	ethStatus = tmhwEthGetConfig(lan100_nic_index, &ethConfig);
	ASSERT(ethStatus==TM_OK);

	mac_addr.byte[0] = ethConfig.stationAddress.sa1;
	mac_addr.byte[1] = ethConfig.stationAddress.sa2;
	mac_addr.byte[2] = ethConfig.stationAddress.sa3;
	mac_addr.byte[3] = ethConfig.stationAddress.sa4;
	mac_addr.byte[4] = ethConfig.stationAddress.sa5;
	mac_addr.byte[5] = ethConfig.stationAddress.sa6;

	setDefaultMacAddr = 1;
	for (i=0; i<6; i++)
	{
		if (mac_addr.byte[i] != 0) setDefaultMacAddr = 0;
	}

	if (setDefaultMacAddr)
	{
		mac_addr.byte[0] = 0x00;
		mac_addr.byte[1] = 0xB0;
		mac_addr.byte[2] = 0xD0;
		mac_addr.byte[3] = 0x69;
		mac_addr.byte[4] = 0x2D;
		mac_addr.byte[5] = 0xF3;
		ethConfig.stationAddress.sa1 = mac_addr.byte[0];
		ethConfig.stationAddress.sa2 = mac_addr.byte[1];
		ethConfig.stationAddress.sa3 = mac_addr.byte[2];
		ethConfig.stationAddress.sa4 = mac_addr.byte[3];
		ethConfig.stationAddress.sa5 = mac_addr.byte[4];
		ethConfig.stationAddress.sa6 = mac_addr.byte[5];
		ethStatus = tmhwEthConfig(lan100_nic_index, &ethConfig);	
		ASSERT(ethStatus==TM_OK);
	}

	for(i=0;i<6;i++){
		dev->dev_addr[i]=mac_addr.byte[i];
		dev->perm_addr[i]=mac_addr.byte[i];
	}

	/*
	 * Register the lan100 net_device instance.
	 * */
	ret=register_netdev(dev);
	if(ret!=0){
		printk(KERN_ERR "Can not register lan100 network device %s (%p)\n",dev->name,dev);
		goto netdev_free;
	}

	mtl_set_drvdata(mdev,dev);

	DEBUGP("LAN100 NIC chip mac address is %x:%x:%x:%x:%x:%x\n", 
			mac_addr.byte[0],
			mac_addr.byte[1],
			mac_addr.byte[2],
			mac_addr.byte[3],
			mac_addr.byte[4],
			mac_addr.byte[5]
			);

	if(netif_msg_probe(nic)){
		printk(KERN_DEBUG "probe lan100 nic #%d\n", lan100_nic_index);
	}

	return 0;

netdev_free:
	free_netdev(dev);
phy_deinit:
	if(EthPhyInterface!=NULL){
		EthPhyInterface->deinitFunc(lan100_nic_index);
	}
tmhweth_deinit:
	tmhwEthDeinit(lan100_nic_index);
	return ret;
}

static void lan100_remove_one(struct mtl_device* mdev)
{
	struct net_device* dev=mtl_get_drvdata(mdev);	
	struct lan100_nic* nic=netdev_priv(dev);

	if(netif_msg_probe(nic)){
		printk(KERN_DEBUG "Remove lan100 nic #%d\n", nic->id);
	}

	mtl_set_drvdata(mdev,NULL);
	unregister_netdev(dev);
	nic->phy_interface->deinitFunc(nic->id);
	tmhwEthDeinit(nic->id);
	free_netdev(dev);

	return;
}

/*
 * FIXME: Without mtl bus management, I have to define a static lan100 mtl device.
 * */
static struct mtl_device lan100_mtldev = {
	.irq = intINT_15,
};

static int __init lan100_init_module(void)
{
	int ret=0;
	ret=lan100_init_one(&lan100_mtldev);
	return ret;
}

static void __exit lan100_exit_module(void)
{
	lan100_remove_one(&lan100_mtldev);
	return;
}

module_init(lan100_init_module);
module_exit(lan100_exit_module);
