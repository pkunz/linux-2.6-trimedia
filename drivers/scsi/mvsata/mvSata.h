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
* mvSata.h - Header File for mvSata.c.
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       mvOs.h
*       ATA/ATAPI-6 standard
*
*******************************************************************************/
#ifndef __INCmvSatah
#define __INCmvSatah
#ifdef __cplusplus
extern "C" /*{*/
#endif /* __cplusplus */

/* Includes */
#include "mvOs.h"
#include "mvRegs.h"

/* Definitions */
#define MV_CORE_DRIVER_LOG_ID                   0



/* MV88SX50XX specific defines */
#define MV_SATA_VENDOR_ID                       0x11AB
#define MV_SATA_DEVICE_ID_5080                  0x5080
#define MV_SATA_DEVICE_ID_5081                  0x5081
#define MV_SATA_DEVICE_ID_5040                  0x5040
#define MV_SATA_DEVICE_ID_5041                  0x5041
#define MV_SATA_DEVICE_ID_6041                  0x6041
#define MV_SATA_DEVICE_ID_6081                  0x6081
#define MV_SATA_CHANNELS_NUM                    8
#define MV_SATA_UNITS_NUM                       2

#define MV_SATA_PM_MAX_PORTS                    15
#define MV_SATA_PM_CONTROL_PORT                 15

#define MV_EDMA_QUEUE_LENGTH                    32  /* Up to 32 outstanding  */
/* commands per SATA channel*/
#ifndef MV_SATA_OVERRIDE_SW_QUEUE_SIZE
    #define MV_SATA_SW_QUEUE_SIZE                   (MV_EDMA_QUEUE_LENGTH -1)
#else
    #define MV_SATA_SW_QUEUE_SIZE                   MV_SATA_REQUESTED_SW_QUEUE_SIZE
#endif


#define MV_EDMA_QUEUE_MASK                      0x1F
#define MV_EDMA_REQUEST_ENTRY_SIZE              32
#define MV_EDMA_RESPONSE_ENTRY_SIZE             8
#define MV_EDMA_REQUEST_QUEUE_SIZE              1024 /* 32*32 = 1KBytes */
#define MV_EDMA_RESPONSE_QUEUE_SIZE             256  /* 32*8 = 256 Bytes */

#define MV_EDMA_PRD_ENTRY_SIZE                  16      /* 16Bytes*/
#define MV_EDMA_PRD_NO_SNOOP_FLAG               MV_BIT0
#define MV_EDMA_PRD_EOT_FLAG                    MV_BIT15

#define MV_ATA_IDENTIFY_DEV_DATA_LENGTH         256 /* number of words(2 byte)*/
#define MV_ATA_MODEL_NUMBER_LEN                 40
#define ATA_SECTOR_SIZE                         512
#define ATA_SECTOR_SIZE_IN_WORDS                256

#define MV_SATA_COMM_INIT_DELAY                 1000 /*1000 us*/
#define MV_SATA_COMM_INIT_WAIT_DELAY            20000 /*20 ms*/

/*Channel to Channel*/
#ifdef MV_SATA_C2C_COMM
    #define MV_SATA_REGISTER_HOST_2_DEVICE_FIS         0x00000034
    #define MV_SATA_DMA_ACTIVATE_FIS                   0x00000039
    #define MV_C2C_MESSAGE_SIZE                        10

#endif

#ifdef MV_SATA_IO_GRANULARITY
    #define MV_IOG_QUEUE_SIZE           0x40
    #define MV_IOG_INVALID_COMMAND_ID   MV_IOG_QUEUE_SIZE
#endif

/* Typedefs    */
typedef enum mvUdmaType
{
    MV_UDMA_TYPE_READ, MV_UDMA_TYPE_WRITE
} MV_UDMA_TYPE;

typedef enum mvFlushType
{
    MV_FLUSH_TYPE_CALLBACK, MV_FLUSH_TYPE_NONE
} MV_FLUSH_TYPE;

typedef enum mvCompletionType
{
    MV_COMPLETION_TYPE_NORMAL, MV_COMPLETION_TYPE_ERROR,
    MV_COMPLETION_TYPE_ABORT
} MV_COMPLETION_TYPE;

typedef enum mvEventType
{
    MV_EVENT_TYPE_ADAPTER_ERROR, MV_EVENT_TYPE_SATA_CABLE,
    MV_EVENT_TYPE_SATA_ERROR
} MV_EVENT_TYPE;

typedef enum mvSataCableEvent
{
    MV_SATA_CABLE_EVENT_DISCONNECT = 0,
    MV_SATA_CABLE_EVENT_CONNECT,
    MV_SATA_CABLE_EVENT_PM_HOT_PLUG
} MV_SATA_CABLE_EVENT;

typedef enum mvSataErrorEvent
{
    MV_SATA_RECOVERABLE_COMMUNICATION_ERROR = 0,
    MV_SATA_UNRECOVERABLE_COMMUNICATION_ERROR,
    MV_SATA_DEVICE_ERROR
} MV_SATA_ERROR_EVENT;

#ifdef MV_SATA_C2C_COMM
typedef enum mvC2CEventType
{
    MV_C2C_REGISTER_DEVICE_TO_HOST_FIS_DONE,
    MV_C2C_REGISTER_DEVICE_TO_HOST_FIS_ERROR,
    MV_C2C_BM_DMA_DONE,
    MV_C2C_BM_DMA_ERROR,
} MV_C2C_EVENT_TYPE;
#endif

typedef enum mvEdmaMode
{
    MV_EDMA_MODE_QUEUED,
    MV_EDMA_MODE_NOT_QUEUED,
    MV_EDMA_MODE_NATIVE_QUEUING
} MV_EDMA_MODE;

typedef enum mvQueueCommandResult
{
    MV_QUEUE_COMMAND_RESULT_OK = 0,
    MV_QUEUE_COMMAND_RESULT_QUEUED_MODE_DISABLED,
    MV_QUEUE_COMMAND_RESULT_FULL,
    MV_QUEUE_COMMAND_RESULT_BAD_LBA_ADDRESS,
    MV_QUEUE_COMMAND_RESULT_BAD_PARAMS
} MV_QUEUE_COMMAND_RESULT;

typedef enum mvNonUdmaProtocol
{
    MV_NON_UDMA_PROTOCOL_NON_DATA,
    MV_NON_UDMA_PROTOCOL_PIO_DATA_IN,
    MV_NON_UDMA_PROTOCOL_PIO_DATA_OUT
} MV_NON_UDMA_PROTOCOL;

typedef enum mvSataGeneration
{
    MV_SATA_GEN_I, MV_SATA_GEN_II
} MV_SATA_GEN;

typedef enum mvSataInterfaceSpeed
{
    MV_SATA_IF_SPEED_1_5_GBPS, MV_SATA_IF_SPEED_3_GBPS,
    MV_SATA_IF_SPEED_NO_LIMIT, MV_SATA_IF_SPEED_INVALID
} MV_SATA_IF_SPEED;

#ifdef MV_SATA_C2C_COMM
typedef enum mvSataC2CMode
{
    MV_SATA_C2C_MODE_INITIATOR, MV_SATA_C2C_MODE_TARGET
} MV_SATA_C2C_MODE;
#endif

typedef enum mvSataDeviceType
{
    MV_SATA_DEVICE_TYPE_UNKNOWN = 0,
    MV_SATA_DEVICE_TYPE_ATA_DISK,
    MV_SATA_DEVICE_TYPE_ATAPI_DISK,
    MV_SATA_DEVICE_TYPE_PM
} MV_SATA_DEVICE_TYPE;

struct mvDmaRequestQueueEntry;
struct mvDmaResponseQueueEntry;
struct mvDmaCommandEntry;

struct mvSataAdapter;
struct mvStorageDevRegisters;
struct mvSataChannel;

typedef MV_BOOLEAN (* mvSataCommandCompletionCallBack_t)(struct mvSataAdapter *,
                                                         MV_U8,
                                                         MV_COMPLETION_TYPE,
                                                         MV_VOID_PTR, MV_U16,
                                                         MV_U32,
                                                         struct mvStorageDevRegisters *);
#ifdef MV_SATA_C2C_COMM
typedef  MV_BOOLEAN (*C2CCallBack_t)(struct mvSataAdapter *,
                                     struct mvSataChannel *,
                                     MV_C2C_EVENT_TYPE event,
                                     MV_U32 msgSize,
                                     MV_U8* msg);

#endif

typedef enum mvQueuedCommandType
{
    MV_QUEUED_COMMAND_TYPE_UDMA,
    MV_QUEUED_COMMAND_TYPE_NONE_UDMA
} MV_QUEUED_COMMAND_TYPE;

#ifdef MV_SATA_IO_GRANULARITY
typedef enum mvIogQueuedCommandType
{
    MV_IOG_COMMAND_TYPE_FIRST,
    MV_IOG_COMMAND_TYPE_NEXT
}   MV_IOG_COMMAND_TYPE;
#endif

typedef enum mvSataInterruptScheme
{
    MV_SATA_INTERRUPT_HANDLING_IN_ISR,
    MV_SATA_INTERRUPT_HANDLING_IN_TASK,
    MV_SATA_INTERRUPTS_DISABLED
}MV_SATA_INTERRUPT_SCHEME;

typedef struct mvUdmaCommandParams
{
    MV_UDMA_TYPE readWrite;
    MV_BOOLEAN   isEXT;
    MV_BOOLEAN   FUA;
    MV_U32       lowLBAAddress;
    MV_U16       highLBAAddress;
    MV_U16       numOfSectors;
    MV_U32       prdLowAddr;
    MV_U32       prdHighAddr;
    mvSataCommandCompletionCallBack_t callBack;
    MV_VOID_PTR  commandId;
#ifdef MV_SATA_IO_GRANULARITY
    MV_BOOLEAN   ioGranularityEnabled;
    MV_IOG_COMMAND_TYPE  iogCommandType;
    union
    {
        MV_U8 transId;
        MV_U8 transCount;
    }   ioGranularityCommandParam;
    MV_U8 iogCurrentTransId;
#endif
} MV_UDMA_COMMAND_PARAMS;

typedef struct mvNoneUdmaCommandParams
{
    MV_NON_UDMA_PROTOCOL protocolType;
    MV_BOOLEAN  isEXT;
    MV_U16_PTR  bufPtr;
    MV_U32      count;
    MV_U16      features;
    MV_U16      sectorCount;
    MV_U16      lbaLow;
    MV_U16      lbaMid;
    MV_U16      lbaHigh;
    MV_U8       device;
    MV_U8       command;
    mvSataCommandCompletionCallBack_t callBack;
    MV_VOID_PTR  commandId;
} MV_NONE_UDMA_COMMAND_PARAMS;

typedef struct mvQueueCommandInfo
{
    MV_QUEUED_COMMAND_TYPE  type;
    MV_U8   PMPort;
    union
    {
        MV_UDMA_COMMAND_PARAMS      udmaCommand;
        MV_NONE_UDMA_COMMAND_PARAMS NoneUdmaCommand;
    } commandParams;
} MV_QUEUE_COMMAND_INFO;

/* The following structure is for the Core Driver internal usage */
typedef struct mvQueuedCommandEntry
{
    MV_BOOLEAN   isFreeEntry;
    MV_U8        commandTag;
    MV_BOOLEAN   isCommandInEdma;
    struct mvQueuedCommandEntry *next;
    struct mvQueuedCommandEntry *prev;
    MV_QUEUE_COMMAND_INFO   commandInfo;
} MV_QUEUED_COMMAND_ENTRY;


typedef enum mvNCQErrorHandlingState
{
    MV_NCQ_ERROR_HANDLING_STATE_IDLE,
    MV_NCQ_ERROR_HANDLING_STATE_WAIT_FOR_COMPLETIONS,
    MV_NCQ_ERROR_HANDLING_STATE_WAIT_FOR_BUSY,
    MV_NCQ_ERROR_HANDLING_STATE_REQUEST_SENSE
}MV_NCQ_ERROR_HANDLING_STATE;

typedef struct
{
    MV_NCQ_ERROR_HANDLING_STATE state;
    MV_U16                      PortsWithErrors;/*which ports reported errors*/
    MV_U8                       CurrPort;
    MV_QUEUED_COMMAND_ENTRY     *pReadLogExtEntry;
    MV_U16_PTR                  ReadLogExtBuffer;
}MV_NCQ_ERROR_HANDLING_INFO;
/* The following structures are part of the Core Driver API */
typedef struct mvSataChannel
{
    /* Fields set by Intermediate Application Layer */
    MV_U8                       channelNumber;
    struct mvDmaRequestQueueEntry  *requestQueue;
    struct mvDmaResponseQueueEntry *responseQueue;
    MV_U32                      requestQueuePciHiAddress;
    MV_U32                      requestQueuePciLowAddress;
    MV_U32                      responseQueuePciHiAddress;
    MV_U32                      responseQueuePciLowAddress;
    /* DRQ Data Block size in sectors, Core Driver sets a default value of 1 */
    /* sector in mvSataConfigureChannel*/
    MV_U8                       DRQDataBlockSize;
    /* Fields set by CORE driver */
    struct mvSataAdapter        *mvSataAdapter;
    MV_OS_SEMAPHORE             semaphore;
    MV_U32                      eDmaRegsOffset;
    MV_BOOLEAN                  EdmaActive;
    MV_EDMA_MODE                queuedDMA;
    MV_U8                       outstandingCommands;
    MV_U8                       portQueuedCommands[MV_SATA_PM_MAX_PORTS + 1];
    struct mvQueuedCommandEntry commandsQueue[MV_SATA_SW_QUEUE_SIZE];
    struct mvQueuedCommandEntry *commandsQueueHead;
    struct mvQueuedCommandEntry *commandsQueueTail;
    MV_BOOLEAN                  queueCommandsEnabled;
    MV_U8                       noneUdmaOutstandingCommands;
    MV_U8                       EdmaQueuedCommands;
    MV_U32                      freeIDsStack[MV_SATA_SW_QUEUE_SIZE];
    MV_U32                      freeIDsNum;
    MV_U32                      reqInPtr;
    MV_U32                      rspOutPtr;
    /* Port Multiplier fiels*/
    MV_BOOLEAN                  PMSupported;
    MV_SATA_DEVICE_TYPE         deviceType;

#ifdef MV_SATA_C2C_COMM

    /* Channel 2 Channel*/
    MV_BOOLEAN                  C2CmodeEnabled;
    MV_SATA_C2C_MODE            C2CMode;
    C2CCallBack_t               C2CCallback;
#endif
    MV_U8                       recoveredErrorsCounter;
    /* NCQ error handling*/
    MV_NCQ_ERROR_HANDLING_INFO  NCQErrHandlingInfo;
} MV_SATA_CHANNEL;



typedef struct mvSataAdapter
{
    /* Fields set by Intermediate Application Layer */
    MV_U32            adapterId;
    MV_VOID_PTR       IALData;
    MV_U8             pciConfigRevisionId;
    MV_U16            pciConfigDeviceId;
    MV_BUS_ADDR_T     adapterIoBaseAddress;
    MV_U32            intCoalThre[MV_SATA_UNITS_NUM];
    MV_U32            intTimeThre[MV_SATA_UNITS_NUM];
    MV_BOOLEAN        (*mvSataEventNotify)(struct mvSataAdapter *,
                                           MV_EVENT_TYPE,
                                           MV_U32, MV_U32);
    MV_SATA_CHANNEL   *sataChannel[MV_SATA_CHANNELS_NUM];
    MV_U32            pciCommand;
    MV_U32            pciSerrMask;
    MV_U32            pciInterruptMask;


    /* Fields set by CORE driver */
    MV_SATA_GEN       sataAdapterGeneration;
    MV_BOOLEAN        staggaredSpinup[MV_SATA_CHANNELS_NUM]; /* For 60x1 only */
    MV_BOOLEAN        limitInterfaceSpeed[MV_SATA_CHANNELS_NUM]; /* For 60x1 only */
    MV_SATA_IF_SPEED  ifSpeed[MV_SATA_CHANNELS_NUM];  /* For 60x1 only */
    MV_U8             numberOfChannels;/* 4 channels for 504x, 8 for 508x*/
    MV_U8             numberOfUnits;/* 1 for 504x, 2 for 508x*/
    MV_OS_SEMAPHORE   semaphore;
    MV_U32            mainMask;
    MV_BOOLEAN        interruptsAreMasked;
    MV_SATA_INTERRUPT_SCHEME interruptsScheme;
    MV_OS_SEMAPHORE   interruptsMaskSem;
    MV_BOOLEAN        chipIs50XXB0;
    MV_BOOLEAN        chipIs50XXB2;
    MV_BOOLEAN        chipIs60X1B2;
    MV_BOOLEAN        chipIs60X1C0;
    MV_U8             signalAmps[MV_SATA_CHANNELS_NUM];
    MV_U8             pre[MV_SATA_CHANNELS_NUM];
#ifdef MV_SATA_IO_GRANULARITY
    MV_BOOLEAN        iogEnabled;
    MV_U8             iogFreeIdsStack[MV_IOG_QUEUE_SIZE];
    MV_U8             iogFreeIdsNum;
    MV_OS_SEMAPHORE   iogSemaphore;
#endif
} MV_SATA_ADAPTER;

/* this structure used by the IAL defines the PRD entries used by the EDMA HW */
typedef struct mvSataEdmaPRDEntry
{
    volatile MV_U32 lowBaseAddr;
    volatile MV_U16 byteCount;
    volatile MV_U16 flags;
    volatile MV_U32 highBaseAddr;
    volatile MV_U32 reserved;
}MV_SATA_EDMA_PRD_ENTRY;

/* API Functions */

/* CORE driver Adapter Management */
MV_BOOLEAN mvSataInitAdapter(MV_SATA_ADAPTER *pAdapter);

MV_BOOLEAN mvSataShutdownAdapter(MV_SATA_ADAPTER *pAdapter);

MV_U32  mvSataReadReg(MV_SATA_ADAPTER *pAdapter, MV_U32 regOffset);

MV_VOID mvSataWriteReg(MV_SATA_ADAPTER *pAdapter, MV_U32 regOffset,
                       MV_U32 regValue);

/* CORE driver SATA Channel Management */
MV_BOOLEAN mvSataConfigureChannel(MV_SATA_ADAPTER *pAdapter,
                                  MV_U8 channelIndex);

MV_BOOLEAN mvSataRemoveChannel(MV_SATA_ADAPTER *pAdapter, MV_U8 channelIndex);

MV_BOOLEAN mvSataIsStorageDeviceConnected(MV_SATA_ADAPTER *pAdapter,
                                          MV_U8 channelIndex);

MV_BOOLEAN mvSataChannelHardReset(MV_SATA_ADAPTER *pAdapter,
                                  MV_U8 channelIndex);


MV_BOOLEAN mvSataConfigEdmaMode(MV_SATA_ADAPTER *pAdapter, MV_U8 channelIndex,
                                MV_EDMA_MODE eDmaMode, MV_U8 maxQueueDepth);

MV_BOOLEAN mvSataEnableChannelDma(MV_SATA_ADAPTER *pAdapter,
                                  MV_U8 channelIndex);

MV_BOOLEAN mvSataDisableChannelDma(MV_SATA_ADAPTER *pAdapter,
                                   MV_U8 channelIndex);

MV_BOOLEAN mvSataFlushDmaQueue(MV_SATA_ADAPTER *pAdapter, MV_U8 channelIndex,
                               MV_FLUSH_TYPE flushType);

MV_U8 mvSataNumOfDmaCommands(MV_SATA_ADAPTER *pAdapter, MV_U8 channelIndex);

MV_U8 mvSataGetNumOfPortQueuedCommands(MV_SATA_ADAPTER *pAdapter,
                                  MV_U8 channelIndex,
                                  MV_U8 PMPort,
                                  MV_U8 *pCommandsPerChannel);

MV_BOOLEAN mvSataSetIntCoalParams (MV_SATA_ADAPTER *pAdapter, MV_U8 sataUnit,
                                   MV_U32 intCoalThre, MV_U32 intTimeThre);

MV_BOOLEAN mvSataSetChannelPhyParams(MV_SATA_ADAPTER *pAdapter,
                                     MV_U8 channelIndex,
                                     MV_U8 signalAmps, MV_U8 pre);

MV_BOOLEAN mvSataChannelPhyShutdown(MV_SATA_ADAPTER *pAdapter,
                                    MV_U8 channelIndex);

MV_BOOLEAN mvSataChannelPhyPowerOn(MV_SATA_ADAPTER *pAdapter,
                                   MV_U8 channelIndex);

MV_BOOLEAN mvSataChannelFarLoopbackDiagnostic(MV_SATA_ADAPTER *pAdapter,
                                              MV_U8 channelIndex);
/* Queue ATA command */
MV_QUEUE_COMMAND_RESULT mvSataQueueCommand(MV_SATA_ADAPTER *pAdapter,
                                           MV_U8 channelIndex,
                                           MV_QUEUE_COMMAND_INFO *pCommandParams);


/* Interrupt Service Routine */
MV_BOOLEAN mvSataInterruptServiceRoutine(MV_SATA_ADAPTER *pAdapter);

MV_BOOLEAN mvSataMaskAdapterInterrupt(MV_SATA_ADAPTER *pAdapter);

MV_BOOLEAN mvSataUnmaskAdapterInterrupt(MV_SATA_ADAPTER *pAdapter);

MV_BOOLEAN mvSataCheckPendingInterrupt(MV_SATA_ADAPTER *pAdapter);

MV_BOOLEAN mvSataSetInterruptsScheme(MV_SATA_ADAPTER *pAdapter,
                                     MV_SATA_INTERRUPT_SCHEME interruptScheme);

/*
 * Staggered spin-ip support and SATA interface speed control
 * (relevant for 60x1 adapters)
 */
MV_BOOLEAN mvSataEnableStaggeredSpinUpAll (MV_SATA_ADAPTER *pAdapter);

MV_BOOLEAN mvSataEnableStaggeredSpinUp (MV_SATA_ADAPTER *pAdapter,
                                        MV_U8 channelIndex);

MV_BOOLEAN mvSataDisableStaggeredSpinUpAll (MV_SATA_ADAPTER *pAdapter);

MV_BOOLEAN mvSataDisableStaggeredSpinUp (MV_SATA_ADAPTER *pAdapter,
                                         MV_U8 channelIndex);


MV_BOOLEAN mvSataSetInterfaceSpeed (MV_SATA_ADAPTER *pAdapter,
                                    MV_U8 channelIndex,
                                    MV_SATA_IF_SPEED ifSpeed);

MV_SATA_IF_SPEED mvSataGetInterfaceSpeed (MV_SATA_ADAPTER *pAdapter,
                                          MV_U8 channelIndex);


/* Command Completion and Event Notification (user implemented) */
MV_BOOLEAN mvSataEventNotify(MV_SATA_ADAPTER *, MV_EVENT_TYPE ,
                             MV_U32, MV_U32);
#ifdef MV_SATA_C2C_COMM

/* Channel 2 Channel communication */
MV_BOOLEAN mvSataC2CInit (MV_SATA_ADAPTER *pAdapter, MV_U8 channelIndex,
                          MV_SATA_C2C_MODE mvSataC2CMode,
                          C2CCallBack_t mvSataC2CCallBack);

MV_BOOLEAN mvSataC2CStop (MV_SATA_ADAPTER *pAdapter, MV_U8 channelIndex);


MV_BOOLEAN  mvSataC2CSendRegisterDeviceToHostFIS(
                                                MV_SATA_ADAPTER *pAdapter,
                                                MV_U8 channelIndex,
                                                MV_U8 pmPort,
                                                MV_BOOLEAN bInterrupt,
                                                MV_U8 message[MV_C2C_MESSAGE_SIZE]);

MV_BOOLEAN  mvSataC2CActivateBmDma(MV_SATA_ADAPTER *pAdapter,
                                   MV_U8 channelIndex,
                                   MV_U8 pmPort,
                                   MV_U32 prdTableHi,
                                   MV_U32 prdTableLow,
                                   MV_UDMA_TYPE dmaType);

MV_BOOLEAN mvSataC2CResetBmDma(MV_SATA_ADAPTER *pAdapter,
                               MV_U8 channelIndex);

#endif

#ifdef MV_SATA_IO_GRANULARITY

MV_BOOLEAN mvSataEnableIoGranularity(MV_SATA_ADAPTER *pAdapter,
                                     MV_BOOLEAN enable);

#endif

#ifdef __cplusplus

/*}*/
#endif /* __cplusplus */

#endif /* __INCmvSatah */
