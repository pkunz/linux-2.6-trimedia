/*
 *
 * Copyright (c) 2002-2005 Broadcom Corporation 
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
 *
*/
#ifndef _BCMINTENET_H_
#define _BCMINTENET_H_

#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include "linux/kernel.h"	/* For barrier() */

#if defined( CONFIG_BCMINTEMAC_7038 ) || defined( CONFIG_BCMINTEMAC_7038_MODULE )
#include <asm/brcmstb/common/bcmtypes.h>
#include <asm/brcmstb/common/brcmstb.h>
#else
#include <6345_map.h>
#endif

#include <linux/spinlock.h>
#ifdef USE_BH
// Obsolete in 2.6, will address this if we ever allow background completion.
//#include <linux/tqueue.h>
#endif

/*---------------------------------------------------------------------*/
/* specify number of BDs and buffers to use                            */
/*---------------------------------------------------------------------*/
#define NR_TX_BDS               32
#define NR_RX_BDS               96
#define ENET_MAX_MTU_SIZE       1536 //1618    /* Body(1500) + EH_SIZE(14) + FCS(4) */
#define DMA_MAX_BURST_LENGTH    0x40      /* in 32 bit words = 256 bytes  THT per David F, to allow 256B burst, was 16; */
#define MAX_RX_BUFS             (NR_RX_BDS * 4)



#if defined( CONFIG_MIPS_BCM7038 ) || defined( CONFIG_MIPS_BCM7401 )
#define ENET_MAC_ADR_BASE		0xb0080000
#define EMAC_RX_DESC_BASE   	0xb0082000	/* MAC DMA Rx Descriptor word */
#define EMAC_TX_DESC_BASE   	(EMAC_RX_DESC_BASE+(8*NR_RX_BDS)) //0xb0082200	/* MAC DMA Tx Descriptor word */
#define EMAC_DMA_BASE   			0xb0082400
#define DMA_ADR_BASE			EMAC_DMA_BASE

#elif defined CONFIG_MIPS_BCM7318
#include <asm/brcmstb/brcm97318/bcm7318.h>

#define ENET_MAC_ADR_BASE 		ENET_ADR_BASE				/* 0xfffd4000 */
#define EMAC_RX_DESC_BASE   	(ENET_ADR_BASE+0x2000)		/* 0xfffd6000  MAC DMA Rx Descriptor word */
#define EMAC_TX_DESC_BASE   	(EMAC_RX_DESC_BASE+(8*NR_RX_BDS))	/* MAC DMA Tx Descriptor word */
#define EMAC_DMA_BASE   			(EMAC_RX_DESC_BASE+0x400)	/* 0xfffd6400 */
#define DMA_ADR_BASE			EMAC_DMA_BASE

#else
  #error "Unsupported platform for IntEMAC\n"
#endif



#define CACHE_TO_NONCACHE(x)	KSEG1ADDR(x)

#define ERROR(x)        printk x
#ifndef ASSERT
#define ASSERT(x)       if (x); else ERROR(("assert: "__FILE__" line %d\n", __LINE__)); 
#endif

//#define DUMP_TRACE
//#define DUMP_DATA
#define IUDMA_INIT_WORKAROUND // for 7038 A0 since IUDMA endine does not get reset properly. Should not need it for 7038B0.

#if defined(DUMP_TRACE)
#define TRACE(x)        printk x
#else
#define TRACE(x)
#endif

#pragma pack(1)
typedef struct Enet_Tx_CB {
    struct sk_buff      *skb;
    uint32              nrBds;          /* # of bds req'd (incl 1 for this header) */
    volatile DmaDesc    *lastBdAddr;
    struct Enet_Tx_CB   *next;          /* ptr to next header in free list */
} Enet_Tx_CB;

typedef struct Enet_Rx_Buff {
    unsigned char       dAddr[ETH_ALEN];/* destination hardware address */
    unsigned char       sAddr[ETH_ALEN];/* source hardware address */
    uint16              type;           /* Ethernet type/length */
    char                buffer[1500];   /* data */
    uint32              crc;            /* normal checksum (FCS1) */
    uint16              pad1;           /* rsvd padding */
    uint32              pad2[4];        /* Must be mult of EMAC_DMA_MAX_BURST_LENGTH */
} Enet_Rx_Buff;					 
#pragma pack()

typedef struct PM_Addr {
    uint16              valid;          /* 1 indicates the corresponding address is valid */
    unsigned char       dAddr[ETH_ALEN];/* perfect match register's destination address */
    unsigned int        ref;            /* reference count */
} PM_Addr;					 
#define MAX_PMADDR      8               /* # of perfect match address */
/*
 * device context
 */ 
typedef struct BcmEnet_devctrl {
    struct net_device *dev;             /* ptr to net_device */
    spinlock_t      lock;               /* Serializing lock */
    struct timer_list timer;            /* used by Tx reclaim */
    atomic_t        devInUsed;          /* device in used */

    struct net_device_stats stats;      /* statistics used by the kernel */

    struct sk_buff  *skb_pool[MAX_RX_BUFS]; /* rx buffer pool */
    int             nextskb;            /* next free skb in skb_pool */ 
    atomic_t        rxDmaRefill;        /* rxDmaRefill == 1 needs refill rxDma */
	
    // THT: Argument to test_and_set_bit must be declared as such
    volatile unsigned long rxbuf_assign_busy;  /* assign_rx_buffers busy */

    /* transmit variables */
    volatile DmaRegs *dmaRegs;
    volatile DmaChannel *txDma;         /* location of transmit DMA register set */

    Enet_Tx_CB      *txCbPtrHead;       /* points to EnetTxCB struct head */
    Enet_Tx_CB      *txCbPtrTail;       /* points to EnetTxCB struct tail */

    Enet_Tx_CB      *txCbQHead;         /* points to EnetTxCB struct queue head */
    Enet_Tx_CB      *txCbQTail;         /* points to EnetTxCB struct queue tail */
    Enet_Tx_CB      *txCbs;             /* memory locaation of tx control block pool */

    volatile DmaDesc *txBds;            /* Memory location of tx Dma BD ring */
    volatile DmaDesc *txLastBdPtr;      /* ptr to last allocated Tx BD */
    volatile DmaDesc *txFirstBdPtr;     /* ptr to first allocated Tx BD */
    volatile DmaDesc *txNextBdPtr;      /* ptr to next Tx BD to transmit with */

    int             nrTxBds;            /* number of transmit bds */
    int             txFreeBds;          /* # of free transmit bds */

    /* receive variables */
    volatile DmaChannel *rxDma;         /* location of receive DMA register set */
    volatile DmaDesc *rxBds;            /* Memory location of rx bd ring */
    volatile DmaDesc *rxBdAssignPtr;    /* ptr to next rx bd to become full */
    volatile DmaDesc *rxBdReadPtr;      /* ptr to next rx bd to be processed */
    volatile DmaDesc *rxLastBdPtr;      /* ptr to last allocated rx bd */
    volatile DmaDesc *rxFirstBdPtr;     /* ptr to first allocated rx bd */

    int             nrRxBds;            /* number of receive bds */
    int             rxBufLen;           /* size of rx buffers for DMA */

    uint16          chipId;             /* chip's id */
    uint16          chipRev;            /* step */
    int             rxIrq;              /* rx dma irq */
    int             phyAddr;            /* 1 - external MII, 0 - internal MII (default after reset) */
    PM_Addr         pmAddr[MAX_PMADDR]; /* perfect match address */
#ifdef USE_BH
    struct tq_struct task;              /* task queue */
#endif
    int             linkState;
} BcmEnet_devctrl;

// BD macros
#define IncTxBDptr(x, s) if (x == ((BcmEnet_devctrl *)s)->txLastbdPtr) \
                             x = ((BcmEnet_devctrl *)s)->txFirstbdPtr; \
                      else x++

#define IncRxBDptr(x, s) if (x == ((BcmEnet_devctrl *)s)->rxLastBdPtr) \
                             x = ((BcmEnet_devctrl *)s)->rxFirstBdPtr; \
                      else x++

#define BcmHalInterruptDisable(irq) disable_irq(irq)
#define BcmHalInterruptEnable(irq)  enable_irq(irq)


//#ifdef CONFIG_CPU_LITTLE_ENDIAN
/* Little endian */
//#define EMAC_SWAP16(x) ( (((x)&0xff00)>>8) |(((x)&0x00ff)<<8) )
//#define EMAC_SWAP32(x) ( (((x)&0xff000000)>>24) |(((x)&0x00ff0000)>>8) | (((x)&0x0000ff00)<<8)  |(((x)&0x000000ff)<<24) )

//#else
/* Big Endian */
#define EMAC_SWAP16(x) (x)
#define EMAC_SWAP32(x) (x)

//#endif


#if 0
#define EMAC_RD_REG(r)  	((sizeof *(r) == sizeof (uint32)) \
	? (uint32)(EMAC_SWAP32(*((uint32*)(r)))) \
	: (uint16)(EMAC_SWAP16(*((uint16*)(r)))))

#define EMAC_WR_REG(r, v) do { ((sizeof *(r) == sizeof (uint32)) \
	? (*((uint32*) (r)) = (uint32) EMAC_SWAP32(v)) \
	: (*((uint16*) (r)) = (uint16) EMAC_SWAP16(v)) ); \
	barrier(); } while(0)
#endif


#endif /* _BCMINTENET_H_ */
