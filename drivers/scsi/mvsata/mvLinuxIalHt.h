/*******************************************************************************
*
*   Marvell Serial ATA Linux Driver
*   Copyright 2004 
*   Marvell International Ltd.
*
* This software program (the "Program") is distributed by Marvell International
* ltd. under the terms of the GNU General Public License Version 2, June 1991 
* (the "License").  You may use, redistribute and/or modify this Program in 
* accordance with the terms and conditions of the License, a copy of which is
* available along with the Program in the license.txt file or by writing to the 
* Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
* MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
*
* THE PROGRAM IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
* EXPRESSLY DISCLAIMED.  The License provides additional details about this 
* warranty disclaimer.
*
* For more information about the Program or the License as it applies to the
* Program, please contact Marvell International Ltd. via its affiliate, Marvell
* Semiconductor, Inc., 700 First Avenue, Sunnyvale, CA 94010
*
********************************************************************************
* file_name - mvLinuxIalHt.h
*
* DESCRIPTION: header file for the layer that emulates SCSI adapter on the
*           SATA adapter
*
*
* DEPENDENCIES:
*   None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
******************************************************************************/
#ifndef __INCmvLinuxIalHth
#define __INCmvLinuxIalHth

#include <linux/version.h>
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/types.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_tcq.h>
#else
#include <linux/blk.h>
#include "scsi.h"
#include "hosts.h"
#endif

#include "mvOs.h"
#include "mvSata.h"
#include "mvStorageDev.h"
#include "mvScsiAtaLayer.h"
#include "mvLinuxIalLib.h"
#include "mvIALCommon.h"

#include <linux/blkdev.h>
#include <linux/spinlock.h>
/* Common forward declarations for all Linux-versions: */

/* Interfaces to the midlevel Linux SCSI driver */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
extern int mv_ial_ht_detect (struct scsi_host_template *);
#endif
extern int mv_ial_ht_release (struct Scsi_Host *);
extern int mv_ial_ht_queuecommand (struct scsi_cmnd *, void (*done) (struct scsi_cmnd *));
extern int mv_ial_ht_bus_reset (struct scsi_cmnd *);
extern int mv_ial_ht_abort(struct scsi_cmnd *SCpnt);

#define HOSTDATA(host) ((IAL_HOST_T *)&host->hostdata)
#define MV_IAL_ADAPTER(host) (HOSTDATA(host)->pAdapter)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#define mvSata                                                          \
{                                                                           \
    module:     THIS_MODULE,\
    proc_name:          "mvSata",                   /* proc_name */     \
    proc_info:          mv_ial_ht_proc_info,    /*proc info fn */   \
    slave_configure:    mv_ial_ht_slave_configure,\
    name:               "Marvell SCSI to SATA adapter", /*name*/            \
    release:            mv_ial_ht_release,              /*release fn*/      \
    queuecommand:       mv_ial_ht_queuecommand,         /*queuecommand fn*/ \
    bios_param:         NULL    /*mv_ial_ht_biosparam*/,/*bios fn*/     \
    eh_device_reset_handler: NULL/*mv_ial_ht_dev_reset*/,                   \
    eh_bus_reset_handler: mv_ial_ht_bus_reset,                              \
    eh_abort_handler:   mv_ial_ht_abort,                                    \
    can_queue:          MV_SATA_SW_QUEUE_SIZE,           /* unlimited */     \
    this_id:            MV_SATA_PM_MAX_PORTS,                          /*set by detect*/   \
    sg_tablesize:       64,                             /*sg_tablesize*/    \
    max_sectors:        256,                                                \
    cmd_per_lun:        MV_SATA_SW_QUEUE_SIZE,           /*cmd_per_lun*/     \
    unchecked_isa_dma:  0,                              /*32-Bit Busmaster*/\
    emulated:           1,                      /* not real scsi adapter */ \
    use_clustering:     ENABLE_CLUSTERING               /*use_clustering*/  \
}
#else
#define mvSata                                                          \
{                                                                           \
    proc_name:          "mvSata",                   /* proc_name */     \
    proc_info:          mv_ial_ht_proc_info24,  /*proc info fn */   \
    select_queue_depths: NULL,              \
    name:               "Marvell SCSI to SATA adapter", /*name*/            \
    detect:             mv_ial_ht_detect,               /*detect fn*/       \
    release:            mv_ial_ht_release,              /*release fn*/      \
    command:            NULL,                           /*command fn*/      \
    queuecommand:       mv_ial_ht_queuecommand,         /*queuecommand fn*/ \
    bios_param:         NULL    /*mv_ial_ht_biosparam*/,/*bios fn*/     \
    eh_device_reset_handler: NULL/*mv_ial_ht_dev_reset*/,                   \
    eh_bus_reset_handler: mv_ial_ht_bus_reset,                              \
    eh_abort_handler:   mv_ial_ht_abort,                                    \
    can_queue:          MV_SATA_SW_QUEUE_SIZE,                         /* unlimited */     \
    this_id:            MV_SATA_PM_MAX_PORTS,                              /*set by detect*/   \
    sg_tablesize:       64,                             /*sg_tablesize*/    \
    max_sectors:        256,                                                \
    cmd_per_lun:        MV_SATA_SW_QUEUE_SIZE,           /*cmd_per_lun*/     \
    unchecked_isa_dma:  0,                              /*32-Bit Busmaster*/\
    emulated:           1,                      /* not real scsi adapter */ \
    use_new_eh_code:    1,                                                  \
    highmem_io:         1,                           /*highmem_io enabled*/\
    use_clustering:     ENABLE_CLUSTERING               /*use_clustering*/  \
}
#endif


#define MV_IAL_HT_SACOALT_DEFAULT   4
#define MV_IAL_HT_SAITMTH_DEFAULT   (150 * 50)

/****************************************/
/*          GENERAL Definitions         */
/****************************************/

#define REQUESTS_ARRAY_SIZE         (9 * MV_EDMA_REQUEST_QUEUE_SIZE) /* 9 K bytes */
#define RESPONSES_ARRAY_SIZE        (12 * MV_EDMA_RESPONSE_QUEUE_SIZE) /* 3 K bytes */


struct IALHost;

/*struct prdPool;*/
typedef struct IALAdapter
{
    MV_SATA_ADAPTER     mvSataAdapter;
    MV_U8               activeHosts;
    int                 maxHosts;
    struct IALHost      *host[MV_SATA_CHANNELS_NUM];
    struct pci_dev      *pcidev;
    u8                  rev_id; /* adapter revision id */
    u8                  *requestsArrayBaseAddr;
    u8                  *requestsArrayBaseAlignedAddr;
    dma_addr_t          requestsArrayBaseDmaAddr;
    dma_addr_t          requestsArrayBaseDmaAlignedAddr;
    u8                  *responsesArrayBaseAddr;
    u8                  *responsesArrayBaseAlignedAddr;
    dma_addr_t          responsesArrayBaseDmaAddr;
    dma_addr_t          responsesArrayBaseDmaAlignedAddr;
    u32                 procNumOfInterrupts;
    MV_IAL_COMMON_ADAPTER_EXTENSION ialCommonExt;
    MV_BOOLEAN          stopAsyncTimer;
    struct timer_list   asyncStartTimer;
    MV_SAL_ADAPTER_EXTENSION  *ataScsiAdapterExt;
    spinlock_t          adapter_lock;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    struct semaphore    rescan_mutex;
    atomic_t            stopped;
#endif
} IAL_ADAPTER_T;

typedef struct IALHost
{
    struct Scsi_Host* scsihost;
    MV_U8 channelIndex;
    IAL_ADAPTER_T* pAdapter;
    u8  queueDepth;
    void  *prdPool[MV_SATA_SW_QUEUE_SIZE];
    void  *prdPoolAligned[MV_SATA_SW_QUEUE_SIZE];
    MV_U32  freePRDsNum;
    struct scsi_cmnd *scsi_cmnd_done_head, *scsi_cmnd_done_tail;
    MV_BOOLEAN  hostBlocked;
} IAL_HOST_T;

/******************************************************************************
* We use the Scsi_Pointer structure that's included with each command
* SCSI_Cmnd as a scratchpad for our SRB. This allows us to accept
* an unlimited number of commands.
*
* SCp will always point to mv_comp_info structure
*******************************************************************************/

/* UDMA command completion info */
struct mv_comp_info
{
    struct scsi_cmnd           *SCpnt;
    MV_SATA_EDMA_PRD_ENTRY  *cpu_PRDpnt;
    dma_addr_t      dma_PRDpnt;
    dma_addr_t      single_buff_busaddr;
    unsigned int        allocated_entries;
    unsigned int        seq_number;
    MV_SATA_SCSI_CMD_BLOCK  *pSALBlock;
    struct scsi_cmnd           *next_done;
};


/* Once pci64_ DMA mapping interface is in, kill this. */
/*#define pci64_alloc_consistent(d,s,p) pci_alloc_consistent((d),(s),(p))*/
/*#define pci64_free_consistent(d,s,c,a) pci_free_consistent((d),(s),(c),(a))*/

#define pci64_map_single(d,c,s,dir) pci_map_single((d),(c),(s),(dir))
#define pci64_map_sg(d,s,n,dir) pci_map_sg((d),(s),(n),(dir))
#define pci64_unmap_single(d,a,s,dir) pci_unmap_single((d),(a),(s),(dir))
#define pci64_unmap_sg(d,s,n,dir) pci_unmap_sg((d),(s),(n),(dir))

#if (BITS_PER_LONG > 32) || defined(CONFIG_HIGHMEM64G)
#define pci64_dma_hi32(a) ((u32) (0xffffffff & (((u64)(a))>>32)))
#define pci64_dma_lo32(a) ((u32) (0xffffffff & (((u64)(a)))))
#else
#define pci64_dma_hi32(a) 0
#define pci64_dma_lo32(a) (a)
#endif  /* BITS_PER_LONG */
#define sg_dma64_address(s) sg_dma_address(s)
#define sg_dma64_len(s) sg_dma_len(s)


#endif /* __INCmvLinuxIalHth */
