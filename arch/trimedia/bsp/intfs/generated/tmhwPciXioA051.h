#include <tmFlags.h>
#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)

#if        !defined(_TMHW_RGD_SEEN)
#define             _TMHW_RGD_SEEN
typedef union
{
    struct {
     UBits32 : 8 ;
     UBits32 red : 8 ;
     UBits32 green : 8 ;
     UBits32 blue : 8 ;
    } bits;
    unsigned int u32;
} BGR32 ;
typedef union
{
    struct {
     UBits32 alpha : 8 ;
     UBits32 red : 8 ;
     UBits32 green : 8 ;
     UBits32 blue : 8 ;
    } bits;
    unsigned int u32;
} BGRA32 ;
typedef union
{
    struct {
     unsigned char alpha ;
     unsigned char red ;
     unsigned char green ;
     unsigned char blue ;
    } bits;
    unsigned int u32;
} BGRA ;
typedef union
{
    struct {
     unsigned char alpha ;
     unsigned char green ;
     unsigned char blue ;
     unsigned char addr ;
    } bits;
    unsigned int u32;
} ABGR ;
typedef union
{
    struct {
     UBits32 : 26 ;
     IBits32 s6 : 6 ;
    } bits;
    unsigned int u32;
} S6 ;
typedef union
{
    struct {
     UBits32 : 18 ;
     IBits32 s14 : 14 ;
    } bits;
    unsigned int u32;
} S14 ;
typedef union
{
    struct {
     UBits32 : 17 ;
     IBits32 s15 : 15 ;
    } bits;
    unsigned int u32;
} S15 ;
typedef union
{
    struct {
     UBits32 : 15 ;
     IBits32 s17 : 17 ;
    } bits;
    unsigned int u32;
} S17 ;
typedef union
{
    struct {
     UBits32 : 12 ;
     IBits32 s20 : 20 ;
    } bits;
    unsigned int u32;
} S20 ;
typedef union
{
    struct {
     UBits32 : 11 ;
     IBits32 s21 : 21 ;
    } bits;
    unsigned int u32;
} S21 ;
typedef union
{
    struct {
     UBits32 : 7 ;
     IBits32 s25 : 25 ;
    } bits;
    unsigned int u32;
} S25 ;
typedef union
{
    struct {
     UBits32 : 6 ;
     IBits32 s26 : 26 ;
    } bits;
    unsigned int u32;
} S26 ;
typedef union
{
    struct {
     UBits32 : 4 ;
     IBits32 s28 : 28 ;
    } bits;
    unsigned int u32;
} S28 ;
typedef union
{
    struct {
     UBits32 : 3 ;
     IBits32 s29 : 29 ;
    } bits;
    unsigned int u32;
} S29 ;
typedef union
{
    struct {
     UBits32 : 26 ;
     UBits32 u6 : 6 ;
    } bits;
    unsigned int u32;
} U6 ;
typedef union
{
    struct {
     unsigned char u8 ;
    } bits;
    unsigned int u32;
} U8 ;
typedef union
{
    struct {
     UBits32 : 22 ;
     UBits32 u10 : 10 ;
    } bits;
    unsigned int u32;
} U10 ;
typedef union
{
    struct {
     UBits32 : 20 ;
     UBits32 u12 : 12 ;
    } bits;
    unsigned int u32;
} U12 ;
typedef union
{
    struct {
     UBits32 : 18 ;
     UBits32 u14 : 14 ;
    } bits;
    unsigned int u32;
} U14 ;
typedef union
{
    struct {
     UBits32 : 16 ;
     UBits32 u16 : 16 ;
    } bits;
    unsigned int u32;
} U16 ;
typedef union
{
    struct {
     UBits32 : 12 ;
     UBits32 u20 : 20 ;
    } bits;
    unsigned int u32;
} U20 ;
typedef union
{
    struct {
     UBits32 : 11 ;
     UBits32 u21 : 21 ;
    } bits;
    unsigned int u32;
} U21 ;
typedef union
{
    struct {
     UBits32 : 9 ;
     UBits32 u23 : 23 ;
    } bits;
    unsigned int u32;
} U23 ;
typedef union
{
    struct {
     UBits32 : 8 ;
     UBits32 u24 : 24 ;
    } bits;
    unsigned int u32;
} U24 ;
typedef union
{
    struct {
     UBits32 : 7 ;
     UBits32 u25 : 25 ;
    } bits;
    unsigned int u32;
} U25 ;
typedef union
{
    struct {
     UBits32 : 6 ;
     UBits32 u26 : 26 ;
    } bits;
    unsigned int u32;
} U26 ;
typedef union
{
    struct {
     UBits32 : 4 ;
     UBits32 u28 : 28 ;
    } bits;
    unsigned int u32;
} U28 ;
typedef union
{
    struct {                      // (11,11), (12,12) and (13,11)
     UBits32 y : 16 ;
     UBits32 x : 16 ;
    } bits;
    unsigned int u32;
} XY16 ;
typedef union
{
    struct {                      // signed
     IBits32 y : 13 ;
     IBits32 : 3 ;
     IBits32 x : 13 ;
    } bits;
    unsigned int u32;
} XYS13 ;
typedef union
{
    struct {                // block ID: read only at 0xffc in each block
     UBits32 moduleId : 16 ;
     UBits32 majorRevision : 4 ;
     UBits32 minorRevision : 4 ;
     UBits32 apertureSize : 8 ;
    } bits;
    unsigned int u32;
} tmBlockId_t ;
#endif  // !defined(_TMHW_RGD_SEEN)
#if        !defined(_TMHWPCIXIOA051_RGD_SEEN)
#define             _TMHWPCIXIOA051_RGD_SEEN
#define _RGD_Module_Id_Of_PciXio           a051
#define _RGD_Hex_Module_Id_Of_PciXio       0xa051
#define _RGD_Module_Name_Of_a051           PciXio
typedef struct _tmhwPciXioRegs
{
    unsigned char filler00000[0x00010-0x00000];
    union _pcixioSetup
    {
        struct
        {
            unsigned int : 1;
            unsigned int disReqGnt : 1;
            unsigned int disReqGntB : 1;
            unsigned int disReqGntA : 1;
            unsigned int d2PowerStateSupport : 1;
            unsigned int d1PowerStateSupport : 1;
            unsigned int : 1;
            unsigned int enTargetAbort : 1;
            unsigned int enPciToMmi : 1;
            unsigned int enXio : 1;
            unsigned int base18Prefetchable : 1;
            unsigned int base18Size : 3;
            unsigned int enBase18 : 1;
            unsigned int base14Prefetchable : 1;
            unsigned int : 1;
            unsigned int base14Size : 3;
#define PCI_BAR_2MB                               0
            unsigned int enBase14 : 1;
            unsigned int base10Prefetchable : 1;
            unsigned int base10Size : 3;
#define PCI_BAR_16MB                              3
#define PCI_BAR_32MB                              4
#define PCI_BAR_64MB                              5
#define PCI_BAR_128MB                             6
            unsigned int : 5;
            unsigned int enConfigMgmt : 1;
            unsigned int enPciArb : 1;
        } bits;
        unsigned int u32;
    } setup;
    union _pcixioCtrl
    {
        struct
        {
            unsigned int : 15;
            unsigned int disSwapDtlToPci : 1;
            unsigned int disSwapPciToRegs : 1;
            unsigned int disSwapPciToDtl : 1;
            unsigned int enRegsWritePosting : 1;
            unsigned int enXioWritePosting : 1;
            unsigned int enPci2WritePosting : 1;
            unsigned int enPci1WritePosting : 1;
            unsigned int enSErrMonitored : 1;
            unsigned int : 2;
            unsigned int enBase10Readahead : 1;
            unsigned int enBase14Readahead : 1;
            unsigned int enBase18Readahead : 1;
            unsigned int disableSubwordToBase10 : 1;
            unsigned int disableSubwordToBase14 : 1;
            unsigned int disableSubwordToBase18 : 1;
            unsigned int enRetryTimer : 1;
        } bits;
        unsigned int u32;
    } ctrl;
    union _pcixioBase1Lo
    {
        struct
        {
            unsigned int base1Lo : 11;
            unsigned int : 21;
        } bits;
        unsigned int u32;
    } base1Lo;
    union _pcixioBase1Hi
    {
        struct
        {
            unsigned int base1Hi : 11;
            unsigned int : 21;
        } bits;
        unsigned int u32;
    } base1Hi;
    union _pcixioBase2Lo
    {
        struct
        {
            unsigned int base2Lo : 11;
            unsigned int : 21;
        } bits;
        unsigned int u32;
    } base2Lo;
    union _pcixioBase2Hi
    {
        struct
        {
            unsigned int base2Hi : 11;
            unsigned int : 21;
        } bits;
        unsigned int u32;
    } base2Hi;
    union _pcixioReadLifetime
    {
        struct
        {
            unsigned int : 16;
            unsigned int clock : 16;
        } bits;
        unsigned int u32;
    } readLifetime;
    UInt32 gpMasterAddr;
    UInt32 gpMasterWriteData;
    UInt32 gpMasterReadData;
    union _pcixioGpMasterCtrl
    {
        struct
        {
            unsigned int : 21;
            unsigned int done : 1;
            unsigned int initiate : 1;
            unsigned int read : 1;
            unsigned int cmd : 4;
            unsigned int byteEnables : 4;
        } bits;
        unsigned int u32;
    } gpMasterCtrl;
    union _pcixioUnlock
    {
        struct
        {
            unsigned int : 16;
            unsigned int subsystemIds : 8;
            unsigned int setup : 8;
        } bits;
        unsigned int u32;
    } unlock;
    union _pcixioConfigDevAndVendorId
    {
        struct
        {
            unsigned int deviceId : 16;
#define PCI_DI_VIPER                         0x8550
            unsigned int vendorId : 16;
#define PCI_VI_PHILIPS                       0x1131
        } bits;
        unsigned int u32;
    } configDevAndVendorId;
    union _pcixioConfigCmdAndStatus
    {
        struct
        {
            unsigned int status : 16;
            unsigned int cmd : 16;
        } bits;
        unsigned int u32;
    } configCmdAndStatus;
    union _pcixioConfigClassAndRev
    {
        struct
        {
            unsigned int classCode : 24;
            unsigned int revisionId : 8;
        } bits;
        unsigned int u32;
    } configClassAndRev;
    union _pcixioConfigLatencyAndCache
    {
        struct
        {
            unsigned int bist : 8;
            unsigned int headerType : 8;
            unsigned int latencyTimer : 8;
            unsigned int cacheLineSize : 8;
        } bits;
        unsigned int u32;
    } configLatencyAndCache;
    union _pcixioConfigBase10
    {
        struct
        {
            unsigned int baseAddr : 28;
            unsigned int prefetchable : 1;
            unsigned int type : 3;
        } bits;
        unsigned int u32;
    } configBase10;
    union _pcixioConfigBase14
    {
        struct
        {
            unsigned int baseAddr : 28;
            unsigned int prefetchable : 1;
            unsigned int type : 2;
            unsigned int io : 1;
        } bits;
        unsigned int u32;
    } configBase14;
    union _pcixioConfigBase18
    {
        struct
        {
            unsigned int baseAddr : 28;
            unsigned int prefetchable : 1;
            unsigned int type : 3;
        } bits;
        unsigned int u32;
    } configBase18;
    unsigned char filler0005c[0x0006c-0x0005c];
    union _pcixioConfigSubsystemIds
    {
        struct
        {
            unsigned int subSystemId : 16;
            unsigned int subSystemVendorId : 16;
        } bits;
        unsigned int u32;
    } configSubsystemIds;
    unsigned char filler00070[0x00074-0x00070];
    union _pcixioConfigCapsPtr
    {
        struct
        {
            unsigned int : 24;
            unsigned int ptr : 8;
        } bits;
        unsigned int u32;
    } configCapsPtr;
    unsigned char filler00078[0x0007c-0x00078];
    union _pcixioConfigMisc
    {
        struct
        {
            unsigned int maxLatency : 8;
            unsigned int minGrant : 8;
            unsigned int intPin : 8;
            unsigned int intLine : 8;
        } bits;
        unsigned int u32;
    } configMisc;
    union _pcixioConfigPowerMgmtCaps
    {
        struct
        {
            unsigned int : 5;
            unsigned int d2PowerStateSupport : 1;
            unsigned int d1PowerStateSupport : 1;
            unsigned int : 6;
            unsigned int version : 3;
            unsigned int nextPtr : 8;
            unsigned int capId : 8;
        } bits;
        unsigned int u32;
    } configPowerMgmtCaps;
    union _pcixioConfigPowerMgmtCtrl
    {
        struct
        {
            unsigned int : 30;
            unsigned int powerState : 2;
        } bits;
        unsigned int u32;
    } configPowerMgmtCtrl;
    union _pcixioConfigPciIo
    {
        struct
        {
            unsigned int base2IoAddr3 : 8;
            unsigned int base2IoAddr2 : 8;
            unsigned int : 13;
            unsigned int base2IoAddrMode : 2;
#define PCI_BASE2_IO_UNMOD                        0
#define PCI_BASE2_IO_USE_32                       1
#define PCI_BASE2_IO_USE_3                        2
            unsigned int base2IsIo : 1;
        } bits;
        unsigned int u32;
    } configPciIo;
    union _pcixioSlaveDtlTuning
    {
        struct
        {
            unsigned int : 11;
            unsigned int memReadFetch : 5;
            unsigned int : 4;
            unsigned int threshold : 9;
            unsigned int memReadMultFetch : 3;
#define PCI_SLAVE_MRDMUL_8                        0
#define PCI_SLAVE_MRDMUL_16                       1
#define PCI_SLAVE_MRDMUL_32                       2
#define PCI_SLAVE_MRDMUL_64                       3
#define PCI_SLAVE_MRDMUL_128                      4
#define PCI_SLAVE_MRDMUL_256                      5
#define PCI_SLAVE_MRDMUL_512                      6
#define PCI_SLAVE_MRDMUL_1024                     7
        } bits;
        unsigned int u32;
    } slaveDtlTuning;
    union _pcixioDmaDtlTuning
    {
        struct
        {
            unsigned int : 16;
            unsigned int threshold : 8;
            unsigned int : 5;
            unsigned int fetch : 3;
        } bits;
        unsigned int u32;
    } dmaDtlTuning;
    unsigned char filler00094[0x00800-0x00094];
    UInt32 xioDmaPciAddr;
    UInt32 xioDmaInternalAddr;
    union _pcixioXioDmaSize
    {
        struct
        {
            unsigned int : 16;
            unsigned int sizeInWords : 16;
        } bits;
        unsigned int u32;
    } xioDmaSize;
    union _pcixioXioDmaCtrl
    {
        struct
        {
            unsigned int : 21;
            unsigned int noBurst : 1;
            unsigned int xioTarget : 1;
            unsigned int fixAddr : 1;
            unsigned int maxBurstSize : 3;
#define DMA_DATA_PHASE_8                          0
#define DMA_DATA_PHASE_16                         1
#define DMA_DATA_PHASE_32                         2
#define DMA_DATA_PHASE_64                         3
#define DMA_DATA_PHASE_128                        4
#define DMA_DATA_PHASE_256                        5
#define DMA_DATA_PHASE_512                        6
#define DMA_DATA_PHASE_NO_SPLIT                   7
            unsigned int initiateDma : 1;
            unsigned int cmd : 4;
        } bits;
        unsigned int u32;
    } xioDmaCtrl;
    union _pcixioXioCtrl
    {
        struct
        {
            unsigned : 30;
            unsigned int xioAck : 1;
            unsigned int : 1;
        } bits;
        unsigned int u32;
    } xioCtrl;
    union _pcixioXioSelect
    {
        struct
        {
            unsigned int : 7;
            unsigned int misc : 1;
            unsigned int is16Bit : 1;
            unsigned int useAck : 1;
            unsigned int clockSignalHigh : 4;
            unsigned int clockSignalLow : 4;
            unsigned int waitStates : 5;
            unsigned int offset : 4;
            unsigned int type : 2;
#define XIO_TYPE_68360                            0
#define XIO_TYPE_NOR_FLASH                        1
#define XIO_TYPE_NAND_FLASH                       2
#define XIO_TYPE_IDE                              3
            unsigned int size : 2;
#define XIO_SIZE_8MB                              0
#define XIO_SIZE_16MB                             1
#define XIO_SIZE_32MB                             2
#define XIO_SIZE_64MB                             3
            unsigned int enable : 1;
        } bits;
        unsigned int u32;
    } xioSelect[3];
    UInt32 xioGpAddr;
    UInt32 xioGpWriteData;
    UInt32 xioGpReadData;
    union _pcixioXioGpCtrl
    {
        struct
        {
            unsigned int : 22;
            unsigned int cyclePending : 1;
            unsigned int cycleDone : 1;
            unsigned int clearDone : 1;
            unsigned int cycleStart : 1;
            unsigned int : 1;
            unsigned int cycleRead : 1;
            unsigned int byteEnables : 4;
        } bits;
        unsigned int u32;
    } xioGpCtrl;
    union _pcixioNandFlashControls
    {
        struct
        {
            unsigned int : 10;
            unsigned int addressPhasesMsb : 1;
            unsigned int includeData : 1;
            unsigned int commandCnt : 2;
            unsigned int addressPhases : 2;
            unsigned int commandB : 8;
            unsigned int commandA : 8;
        } bits;
        unsigned int u32;
    } nandFlashControls;
    union _pcixioXioSelect xioSelectMore[2];
    unsigned char filler0083c[0x00fb0-0x0083c];
    union _pcixioXioGpIntStatus
    {
        struct
        {
            unsigned : 17;
            unsigned int xioAckDone : 1;
            unsigned int done : 1;
            unsigned int : 3;
            unsigned int badCmd : 1;
            unsigned int : 6;
            unsigned int receivedMasterAbort : 1;
            unsigned int : 2;
        } bits;
        unsigned int u32;
    } xioGpIntStatus;
    union _pcixioXioGpIntStatus xioGpIntEnable;
    union _pcixioXioGpIntStatus xioGpIntClear;
    union _pcixioXioGpIntStatus xioGpIntSet;
    union _pcixioGpMasterIntStatus
    {
        struct
        {
            unsigned : 21;
            unsigned int done : 1;
            unsigned int badCmd : 1;
            unsigned int : 3;
            unsigned int masterParityErr : 1;
            unsigned int detectedParityErr : 1;
            unsigned int : 1;
            unsigned int receivedMasterAbort : 1;
            unsigned int receivedTargetAbort : 1;
            unsigned int : 1;
        } bits;
        unsigned int u32;
    } gpMasterIntStatus;
    union _pcixioGpMasterIntStatus gpMasterIntEnable;
    union _pcixioGpMasterIntStatus gpMasterIntClear;
    union _pcixioGpMasterIntStatus gpMasterIntSet;
    union _pcixioDmaIntStatus
    {
        struct
        {
            unsigned : 17;
            unsigned int xioAckDone : 1;
            unsigned int : 1;
            unsigned int done : 1;
            unsigned int : 2;
            unsigned int badCmd : 1;
            unsigned int : 3;
            unsigned int masterParityErr : 1;
            unsigned int detectedParityErr : 1;
            unsigned int : 1;
            unsigned int receivedMasterAbort : 1;
            unsigned int receivedTargetAbort : 1;
            unsigned int : 1;
        } bits;
        unsigned int u32;
    } dmaIntStatus;
    union _pcixioDmaIntStatus dmaIntEnable;
    union _pcixioDmaIntStatus dmaIntClear;
    union _pcixioDmaIntStatus dmaIntSet;
    union _pcixioPciIntStatus
    {
        struct
        {
            unsigned int : 5;
            unsigned int pciMasterWrErr : 1;
            unsigned int pciMasterRdErr : 1;
            unsigned int xioSlaveWrErr : 1;
            unsigned int xioSlaveRdErr : 1;
            unsigned int mmioSlaveWrErr : 1;
            unsigned int mmioSlaveRdErr : 1;
            unsigned int powerStateChanged : 1;
            unsigned int : 1;
            unsigned int pci2SlaveWrErr : 1;
            unsigned int pci2SlaveRdErr : 1;
            unsigned int pci1SlaveWrErr : 1;
            unsigned int pci1SlaveRdErr : 1;
            unsigned int xioAckDone : 1;
            unsigned int : 2;
            unsigned int sErrObserved : 1;
            unsigned int : 1;
            unsigned int pciErrFlag : 1;
            unsigned int base10Subword : 1;
            unsigned int base14Subword : 1;
            unsigned int base18Subword : 1;
            unsigned int masterParityErr : 1;
            unsigned int detectedParityErr : 1;
            unsigned int systemError : 1;
            unsigned int receivedMasterAbort : 1;
            unsigned int receivedTargetAbort : 1;
            unsigned int signaledTargetAbort : 1;
        } bits;
        unsigned int u32;
    } pciIntStatus;
    union _pcixioPciIntStatus pciIntEnable;
    union _pcixioPciIntStatus pciIntClear;
    union _pcixioPciIntStatus pciIntSet;
    unsigned char filler00ff0[0x00ffc-0x00ff0];
    tmBlockId_t blockId;
#define PCIXIO_MODULE_ID                 _RGD_Hex_Module_Id_Of_PciXio
#define PCIXIO_MAJOR_REV                          0
#define PCIXIO_MINOR_REV                          0
} tmhwPciXioRegs_NONVOLATILE_t;
typedef volatile tmhwPciXioRegs_NONVOLATILE_t *ptmhwPciXioRegs_t;

#if        defined(RGD_CHECK_OFFSETS)
#if        RGD_CHECK_OFFSETS

#define _RGD_offsetof(p, f) ((unsigned int) (&(((p))->f)))
#define _RGD_Check_Offset(s, f, o)                              \
    {                                                           \
        extern void _##s##_##f##_offset_not_##o(                \
            tp##s,                                              \
            unsigned int                                        \
        );                                                      \
        (void) (                                                \
            (_RGD_offsetof(pRegs, f) == 0x##o)                  \
            ? 0                                                 \
            : (                                                 \
                _##s##_##f##_offset_not_##o(                    \
                    pRegs,                                      \
                    _RGD_offsetof(pRegs, f)                     \
                ),                                              \
                0                                               \
            )                                                   \
        );                                                      \
    }                                                           \

#pragma optimize("agpsw", on)

#if        defined(__cplusplus)
extern "C" {
void _RGD_CPP_Check_tmhwPciXioRegs_Offsets(ptmhwPciXioRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwPciXioRegs_Offsets(ptmhwPciXioRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwPciXioRegs, setup,                         00000010);
    _RGD_Check_Offset(tmhwPciXioRegs, ctrl,                          00000014);
    _RGD_Check_Offset(tmhwPciXioRegs, base1Lo,                       00000018);
    _RGD_Check_Offset(tmhwPciXioRegs, base1Hi,                       0000001c);
    _RGD_Check_Offset(tmhwPciXioRegs, base2Lo,                       00000020);
    _RGD_Check_Offset(tmhwPciXioRegs, base2Hi,                       00000024);
    _RGD_Check_Offset(tmhwPciXioRegs, readLifetime,                  00000028);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterAddr,                  0000002c);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterWriteData,             00000030);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterReadData,              00000034);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterCtrl,                  00000038);
    _RGD_Check_Offset(tmhwPciXioRegs, unlock,                        0000003c);
    _RGD_Check_Offset(tmhwPciXioRegs, configDevAndVendorId,          00000040);
    _RGD_Check_Offset(tmhwPciXioRegs, configCmdAndStatus,            00000044);
    _RGD_Check_Offset(tmhwPciXioRegs, configClassAndRev,             00000048);
    _RGD_Check_Offset(tmhwPciXioRegs, configLatencyAndCache,         0000004c);
    _RGD_Check_Offset(tmhwPciXioRegs, configBase10,                  00000050);
    _RGD_Check_Offset(tmhwPciXioRegs, configBase14,                  00000054);
    _RGD_Check_Offset(tmhwPciXioRegs, configBase18,                  00000058);
    _RGD_Check_Offset(tmhwPciXioRegs, configSubsystemIds,            0000006c);
    _RGD_Check_Offset(tmhwPciXioRegs, configCapsPtr,                 00000074);
    _RGD_Check_Offset(tmhwPciXioRegs, configMisc,                    0000007c);
    _RGD_Check_Offset(tmhwPciXioRegs, configPowerMgmtCaps,           00000080);
    _RGD_Check_Offset(tmhwPciXioRegs, configPowerMgmtCtrl,           00000084);
    _RGD_Check_Offset(tmhwPciXioRegs, configPciIo,                   00000088);
    _RGD_Check_Offset(tmhwPciXioRegs, slaveDtlTuning,                0000008c);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaDtlTuning,                  00000090);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaPciAddr,                 00000800);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaInternalAddr,            00000804);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaSize,                    00000808);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaCtrl,                    0000080c);
    _RGD_Check_Offset(tmhwPciXioRegs, xioCtrl,                       00000810);
    _RGD_Check_Offset(tmhwPciXioRegs, xioSelect,                     00000814);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpAddr,                     00000820);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpWriteData,                00000824);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpReadData,                 00000828);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpCtrl,                     0000082c);
    _RGD_Check_Offset(tmhwPciXioRegs, nandFlashControls,             00000830);
    _RGD_Check_Offset(tmhwPciXioRegs, xioSelectMore,                 00000834);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntStatus,                00000fb0);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntEnable,                00000fb4);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntClear,                 00000fb8);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntSet,                   00000fbc);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntStatus,             00000fc0);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntEnable,             00000fc4);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntClear,              00000fc8);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntSet,                00000fcc);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntStatus,                  00000fd0);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntEnable,                  00000fd4);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntClear,                   00000fd8);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntSet,                     00000fdc);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntStatus,                  00000fe0);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntEnable,                  00000fe4);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntClear,                   00000fe8);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntSet,                     00000fec);
    _RGD_Check_Offset(tmhwPciXioRegs, blockId,                       00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWPCIXIOA051_RGD_SEEN)

#else   // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)

#if        !defined(_TMHW_RGD_SEEN)
#define             _TMHW_RGD_SEEN
typedef union
{
    struct {
     UBits32 blue : 8 ;
     UBits32 green : 8 ;
     UBits32 red : 8 ;
     UBits32 : 8 ;
    } bits;
    unsigned int u32;
} BGR32 ;
typedef union
{
    struct {
     UBits32 blue : 8 ;
     UBits32 green : 8 ;
     UBits32 red : 8 ;
     UBits32 alpha : 8 ;
    } bits;
    unsigned int u32;
} BGRA32 ;
typedef union
{
    struct {
     unsigned char blue ;
     unsigned char green ;
     unsigned char red ;
     unsigned char alpha ;
    } bits;
    unsigned int u32;
} BGRA ;
typedef union
{
    struct {
     unsigned char addr ;
     unsigned char blue ;
     unsigned char green ;
     unsigned char alpha ;
    } bits;
    unsigned int u32;
} ABGR ;
typedef union
{
    struct {
     IBits32 s6 : 6 ;
     UBits32 : 26 ;
    } bits;
    unsigned int u32;
} S6 ;
typedef union
{
    struct {
     IBits32 s14 : 14 ;
     UBits32 : 18 ;
    } bits;
    unsigned int u32;
} S14 ;
typedef union
{
    struct {
     IBits32 s15 : 15 ;
     UBits32 : 17 ;
    } bits;
    unsigned int u32;
} S15 ;
typedef union
{
    struct {
     IBits32 s17 : 17 ;
     UBits32 : 15 ;
    } bits;
    unsigned int u32;
} S17 ;
typedef union
{
    struct {
     IBits32 s20 : 20 ;
     UBits32 : 12 ;
    } bits;
    unsigned int u32;
} S20 ;
typedef union
{
    struct {
     IBits32 s21 : 21 ;
     UBits32 : 11 ;
    } bits;
    unsigned int u32;
} S21 ;
typedef union
{
    struct {
     IBits32 s25 : 25 ;
     UBits32 : 7 ;
    } bits;
    unsigned int u32;
} S25 ;
typedef union
{
    struct {
     IBits32 s26 : 26 ;
     UBits32 : 6 ;
    } bits;
    unsigned int u32;
} S26 ;
typedef union
{
    struct {
     IBits32 s28 : 28 ;
     UBits32 : 4 ;
    } bits;
    unsigned int u32;
} S28 ;
typedef union
{
    struct {
     IBits32 s29 : 29 ;
     UBits32 : 3 ;
    } bits;
    unsigned int u32;
} S29 ;
typedef union
{
    struct {
     UBits32 u6 : 6 ;
     UBits32 : 26 ;
    } bits;
    unsigned int u32;
} U6 ;
typedef union
{
    struct {
     unsigned char u8 ;
    } bits;
    unsigned int u32;
} U8 ;
typedef union
{
    struct {
     UBits32 u10 : 10 ;
     UBits32 : 22 ;
    } bits;
    unsigned int u32;
} U10 ;
typedef union
{
    struct {
     UBits32 u12 : 12 ;
     UBits32 : 20 ;
    } bits;
    unsigned int u32;
} U12 ;
typedef union
{
    struct {
     UBits32 u14 : 14 ;
     UBits32 : 18 ;
    } bits;
    unsigned int u32;
} U14 ;
typedef union
{
    struct {
     UBits32 u16 : 16 ;
     UBits32 : 16 ;
    } bits;
    unsigned int u32;
} U16 ;
typedef union
{
    struct {
     UBits32 u20 : 20 ;
     UBits32 : 12 ;
    } bits;
    unsigned int u32;
} U20 ;
typedef union
{
    struct {
     UBits32 u21 : 21 ;
     UBits32 : 11 ;
    } bits;
    unsigned int u32;
} U21 ;
typedef union
{
    struct {
     UBits32 u23 : 23 ;
     UBits32 : 9 ;
    } bits;
    unsigned int u32;
} U23 ;
typedef union
{
    struct {
     UBits32 u24 : 24 ;
     UBits32 : 8 ;
    } bits;
    unsigned int u32;
} U24 ;
typedef union
{
    struct {
     UBits32 u25 : 25 ;
     UBits32 : 7 ;
    } bits;
    unsigned int u32;
} U25 ;
typedef union
{
    struct {
     UBits32 u26 : 26 ;
     UBits32 : 6 ;
    } bits;
    unsigned int u32;
} U26 ;
typedef union
{
    struct {
     UBits32 u28 : 28 ;
     UBits32 : 4 ;
    } bits;
    unsigned int u32;
} U28 ;
typedef union
{
    struct {                      // (11,11), (12,12) and (13,11)
     UBits32 x : 16 ;
     UBits32 y : 16 ;
    } bits;
    unsigned int u32;
} XY16 ;
typedef union
{
    struct {                      // signed
     IBits32 x : 13 ;
     IBits32 : 3 ;
     IBits32 y : 13 ;
    } bits;
    unsigned int u32;
} XYS13 ;
typedef union
{
    struct {                // block ID: read only at 0xffc in each block
     UBits32 apertureSize : 8 ;
     UBits32 minorRevision : 4 ;
     UBits32 majorRevision : 4 ;
     UBits32 moduleId : 16 ;
    } bits;
    unsigned int u32;
} tmBlockId_t ;
#endif  // !defined(_TMHW_RGD_SEEN)
#if        !defined(_TMHWPCIXIOA051_RGD_SEEN)
#define             _TMHWPCIXIOA051_RGD_SEEN
#define _RGD_Module_Id_Of_PciXio           a051
#define _RGD_Hex_Module_Id_Of_PciXio       0xa051
#define _RGD_Module_Name_Of_a051           PciXio
typedef struct _tmhwPciXioRegs
{
    unsigned char filler00000[0x00010-0x00000];
    union _pcixioSetup
    {
        struct
        {
            unsigned int enPciArb : 1;
            unsigned int enConfigMgmt : 1;
            unsigned int : 5;
            unsigned int base10Size : 3;
#define PCI_BAR_16MB                              3
#define PCI_BAR_32MB                              4
#define PCI_BAR_64MB                              5
#define PCI_BAR_128MB                             6
            unsigned int base10Prefetchable : 1;
            unsigned int enBase14 : 1;
            unsigned int base14Size : 3;
#define PCI_BAR_2MB                               0
            unsigned int : 1;
            unsigned int base14Prefetchable : 1;
            unsigned int enBase18 : 1;
            unsigned int base18Size : 3;
            unsigned int base18Prefetchable : 1;
            unsigned int enXio : 1;
            unsigned int enPciToMmi : 1;
            unsigned int enTargetAbort : 1;
            unsigned int : 1;
            unsigned int d1PowerStateSupport : 1;
            unsigned int d2PowerStateSupport : 1;
            unsigned int disReqGntA : 1;
            unsigned int disReqGntB : 1;
            unsigned int disReqGnt : 1;
        } bits;
        unsigned int u32;
    } setup;
    union _pcixioCtrl
    {
        struct
        {
            unsigned int enRetryTimer : 1;
            unsigned int disableSubwordToBase18 : 1;
            unsigned int disableSubwordToBase14 : 1;
            unsigned int disableSubwordToBase10 : 1;
            unsigned int enBase18Readahead : 1;
            unsigned int enBase14Readahead : 1;
            unsigned int enBase10Readahead : 1;
            unsigned int : 2;
            unsigned int enSErrMonitored : 1;
            unsigned int enPci1WritePosting : 1;
            unsigned int enPci2WritePosting : 1;
            unsigned int enXioWritePosting : 1;
            unsigned int enRegsWritePosting : 1;
            unsigned int disSwapPciToDtl : 1;
            unsigned int disSwapPciToRegs : 1;
            unsigned int disSwapDtlToPci : 1;
        } bits;
        unsigned int u32;
    } ctrl;
    union _pcixioBase1Lo
    {
        struct
        {
            unsigned int : 21;
            unsigned int base1Lo : 11;
        } bits;
        unsigned int u32;
    } base1Lo;
    union _pcixioBase1Hi
    {
        struct
        {
            unsigned int : 21;
            unsigned int base1Hi : 11;
        } bits;
        unsigned int u32;
    } base1Hi;
    union _pcixioBase2Lo
    {
        struct
        {
            unsigned int : 21;
            unsigned int base2Lo : 11;
        } bits;
        unsigned int u32;
    } base2Lo;
    union _pcixioBase2Hi
    {
        struct
        {
            unsigned int : 21;
            unsigned int base2Hi : 11;
        } bits;
        unsigned int u32;
    } base2Hi;
    union _pcixioReadLifetime
    {
        struct
        {
            unsigned int clock : 16;
        } bits;
        unsigned int u32;
    } readLifetime;
    UInt32 gpMasterAddr;
    UInt32 gpMasterWriteData;
    UInt32 gpMasterReadData;
    union _pcixioGpMasterCtrl
    {
        struct
        {
            unsigned int byteEnables : 4;
            unsigned int cmd : 4;
            unsigned int read : 1;
            unsigned int initiate : 1;
            unsigned int done : 1;
        } bits;
        unsigned int u32;
    } gpMasterCtrl;
    union _pcixioUnlock
    {
        struct
        {
            unsigned int setup : 8;
            unsigned int subsystemIds : 8;
        } bits;
        unsigned int u32;
    } unlock;
    union _pcixioConfigDevAndVendorId
    {
        struct
        {
            unsigned int vendorId : 16;
#define PCI_VI_PHILIPS                       0x1131
            unsigned int deviceId : 16;
#define PCI_DI_VIPER                         0x8550
        } bits;
        unsigned int u32;
    } configDevAndVendorId;
    union _pcixioConfigCmdAndStatus
    {
        struct
        {
            unsigned int cmd : 16;
            unsigned int status : 16;
        } bits;
        unsigned int u32;
    } configCmdAndStatus;
    union _pcixioConfigClassAndRev
    {
        struct
        {
            unsigned int revisionId : 8;
            unsigned int classCode : 24;
        } bits;
        unsigned int u32;
    } configClassAndRev;
    union _pcixioConfigLatencyAndCache
    {
        struct
        {
            unsigned int cacheLineSize : 8;
            unsigned int latencyTimer : 8;
            unsigned int headerType : 8;
            unsigned int bist : 8;
        } bits;
        unsigned int u32;
    } configLatencyAndCache;
    union _pcixioConfigBase10
    {
        struct
        {
            unsigned int type : 3;
            unsigned int prefetchable : 1;
            unsigned int baseAddr : 28;
        } bits;
        unsigned int u32;
    } configBase10;
    union _pcixioConfigBase14
    {
        struct
        {
            unsigned int io : 1;
            unsigned int type : 2;
            unsigned int prefetchable : 1;
            unsigned int baseAddr : 28;
        } bits;
        unsigned int u32;
    } configBase14;
    union _pcixioConfigBase18
    {
        struct
        {
            unsigned int type : 3;
            unsigned int prefetchable : 1;
            unsigned int baseAddr : 28;
        } bits;
        unsigned int u32;
    } configBase18;
    unsigned char filler0005c[0x0006c-0x0005c];
    union _pcixioConfigSubsystemIds
    {
        struct
        {
            unsigned int subSystemVendorId : 16;
            unsigned int subSystemId : 16;
        } bits;
        unsigned int u32;
    } configSubsystemIds;
    unsigned char filler00070[0x00074-0x00070];
    union _pcixioConfigCapsPtr
    {
        struct
        {
            unsigned int ptr : 8;
        } bits;
        unsigned int u32;
    } configCapsPtr;
    unsigned char filler00078[0x0007c-0x00078];
    union _pcixioConfigMisc
    {
        struct
        {
            unsigned int intLine : 8;
            unsigned int intPin : 8;
            unsigned int minGrant : 8;
            unsigned int maxLatency : 8;
        } bits;
        unsigned int u32;
    } configMisc;
    union _pcixioConfigPowerMgmtCaps
    {
        struct
        {
            unsigned int capId : 8;
            unsigned int nextPtr : 8;
            unsigned int version : 3;
            unsigned int : 6;
            unsigned int d1PowerStateSupport : 1;
            unsigned int d2PowerStateSupport : 1;
        } bits;
        unsigned int u32;
    } configPowerMgmtCaps;
    union _pcixioConfigPowerMgmtCtrl
    {
        struct
        {
            unsigned int powerState : 2;
        } bits;
        unsigned int u32;
    } configPowerMgmtCtrl;
    union _pcixioConfigPciIo
    {
        struct
        {
            unsigned int base2IsIo : 1;
            unsigned int base2IoAddrMode : 2;
#define PCI_BASE2_IO_UNMOD                        0
#define PCI_BASE2_IO_USE_32                       1
#define PCI_BASE2_IO_USE_3                        2
            unsigned int : 13;
            unsigned int base2IoAddr2 : 8;
            unsigned int base2IoAddr3 : 8;
        } bits;
        unsigned int u32;
    } configPciIo;
    union _pcixioSlaveDtlTuning
    {
        struct
        {
            unsigned int memReadMultFetch : 3;
#define PCI_SLAVE_MRDMUL_8                        0
#define PCI_SLAVE_MRDMUL_16                       1
#define PCI_SLAVE_MRDMUL_32                       2
#define PCI_SLAVE_MRDMUL_64                       3
#define PCI_SLAVE_MRDMUL_128                      4
#define PCI_SLAVE_MRDMUL_256                      5
#define PCI_SLAVE_MRDMUL_512                      6
#define PCI_SLAVE_MRDMUL_1024                     7
            unsigned int threshold : 9;
            unsigned int : 4;
            unsigned int memReadFetch : 5;
        } bits;
        unsigned int u32;
    } slaveDtlTuning;
    union _pcixioDmaDtlTuning
    {
        struct
        {
            unsigned int fetch : 3;
            unsigned int : 5;
            unsigned int threshold : 8;
        } bits;
        unsigned int u32;
    } dmaDtlTuning;
    unsigned char filler00094[0x00800-0x00094];
    UInt32 xioDmaPciAddr;
    UInt32 xioDmaInternalAddr;
    union _pcixioXioDmaSize
    {
        struct
        {
            unsigned int sizeInWords : 16;
        } bits;
        unsigned int u32;
    } xioDmaSize;
    union _pcixioXioDmaCtrl
    {
        struct
        {
            unsigned int cmd : 4;
            unsigned int initiateDma : 1;
            unsigned int maxBurstSize : 3;
#define DMA_DATA_PHASE_8                          0
#define DMA_DATA_PHASE_16                         1
#define DMA_DATA_PHASE_32                         2
#define DMA_DATA_PHASE_64                         3
#define DMA_DATA_PHASE_128                        4
#define DMA_DATA_PHASE_256                        5
#define DMA_DATA_PHASE_512                        6
#define DMA_DATA_PHASE_NO_SPLIT                   7
            unsigned int fixAddr : 1;
            unsigned int xioTarget : 1;
            unsigned int noBurst : 1;
        } bits;
        unsigned int u32;
    } xioDmaCtrl;
    union _pcixioXioCtrl
    {
        struct
        {
            unsigned int : 1;
            unsigned int xioAck : 1;
        } bits;
        unsigned int u32;
    } xioCtrl;
    union _pcixioXioSelect
    {
        struct
        {
            unsigned int enable : 1;
            unsigned int size : 2;
#define XIO_SIZE_8MB                              0
#define XIO_SIZE_16MB                             1
#define XIO_SIZE_32MB                             2
#define XIO_SIZE_64MB                             3
            unsigned int type : 2;
#define XIO_TYPE_68360                            0
#define XIO_TYPE_NOR_FLASH                        1
#define XIO_TYPE_NAND_FLASH                       2
#define XIO_TYPE_IDE                              3
            unsigned int offset : 4;
            unsigned int waitStates : 5;
            unsigned int clockSignalLow : 4;
            unsigned int clockSignalHigh : 4;
            unsigned int useAck : 1;
            unsigned int is16Bit : 1;
            unsigned int misc : 1;
        } bits;
        unsigned int u32;
    } xioSelect[3];
    UInt32 xioGpAddr;
    UInt32 xioGpWriteData;
    UInt32 xioGpReadData;
    union _pcixioXioGpCtrl
    {
        struct
        {
            unsigned int byteEnables : 4;
            unsigned int cycleRead : 1;
            unsigned int : 1;
            unsigned int cycleStart : 1;
            unsigned int clearDone : 1;
            unsigned int cycleDone : 1;
            unsigned int cyclePending : 1;
        } bits;
        unsigned int u32;
    } xioGpCtrl;
    union _pcixioNandFlashControls
    {
        struct
        {
            unsigned int commandA : 8;
            unsigned int commandB : 8;
            unsigned int addressPhases : 2;
            unsigned int commandCnt : 2;
            unsigned int includeData : 1;
            unsigned int addressPhasesMsb : 1;
        } bits;
        unsigned int u32;
    } nandFlashControls;
    union _pcixioXioSelect xioSelectMore[2];
    unsigned char filler0083c[0x00fb0-0x0083c];
    union _pcixioXioGpIntStatus
    {
        struct
        {
            unsigned int : 2;
            unsigned int receivedMasterAbort : 1;
            unsigned int : 6;
            unsigned int badCmd : 1;
            unsigned int : 3;
            unsigned int done : 1;
            unsigned int xioAckDone : 1;
        } bits;
        unsigned int u32;
    } xioGpIntStatus;
    union _pcixioXioGpIntStatus xioGpIntEnable;
    union _pcixioXioGpIntStatus xioGpIntClear;
    union _pcixioXioGpIntStatus xioGpIntSet;
    union _pcixioGpMasterIntStatus
    {
        struct
        {
            unsigned int : 1;
            unsigned int receivedTargetAbort : 1;
            unsigned int receivedMasterAbort : 1;
            unsigned int : 1;
            unsigned int detectedParityErr : 1;
            unsigned int masterParityErr : 1;
            unsigned int : 3;
            unsigned int badCmd : 1;
            unsigned int done : 1;
        } bits;
        unsigned int u32;
    } gpMasterIntStatus;
    union _pcixioGpMasterIntStatus gpMasterIntEnable;
    union _pcixioGpMasterIntStatus gpMasterIntClear;
    union _pcixioGpMasterIntStatus gpMasterIntSet;
    union _pcixioDmaIntStatus
    {
        struct
        {
            unsigned int : 1;
            unsigned int receivedTargetAbort : 1;
            unsigned int receivedMasterAbort : 1;
            unsigned int : 1;
            unsigned int detectedParityErr : 1;
            unsigned int masterParityErr : 1;
            unsigned int : 3;
            unsigned int badCmd : 1;
            unsigned int : 2;
            unsigned int done : 1;
            unsigned int : 1;
            unsigned int xioAckDone : 1;
        } bits;
        unsigned int u32;
    } dmaIntStatus;
    union _pcixioDmaIntStatus dmaIntEnable;
    union _pcixioDmaIntStatus dmaIntClear;
    union _pcixioDmaIntStatus dmaIntSet;
    union _pcixioPciIntStatus
    {
        struct
        {
            unsigned int signaledTargetAbort : 1;
            unsigned int receivedTargetAbort : 1;
            unsigned int receivedMasterAbort : 1;
            unsigned int systemError : 1;
            unsigned int detectedParityErr : 1;
            unsigned int masterParityErr : 1;
            unsigned int base18Subword : 1;
            unsigned int base14Subword : 1;
            unsigned int base10Subword : 1;
            unsigned int pciErrFlag : 1;
            unsigned int : 1;
            unsigned int sErrObserved : 1;
            unsigned int : 2;
            unsigned int xioAckDone : 1;
            unsigned int pci1SlaveRdErr : 1;
            unsigned int pci1SlaveWrErr : 1;
            unsigned int pci2SlaveRdErr : 1;
            unsigned int pci2SlaveWrErr : 1;
            unsigned int : 1;
            unsigned int powerStateChanged : 1;
            unsigned int mmioSlaveRdErr : 1;
            unsigned int mmioSlaveWrErr : 1;
            unsigned int xioSlaveRdErr : 1;
            unsigned int xioSlaveWrErr : 1;
            unsigned int pciMasterRdErr : 1;
            unsigned int pciMasterWrErr : 1;
        } bits;
        unsigned int u32;
    } pciIntStatus;
    union _pcixioPciIntStatus pciIntEnable;
    union _pcixioPciIntStatus pciIntClear;
    union _pcixioPciIntStatus pciIntSet;
    unsigned char filler00ff0[0x00ffc-0x00ff0];
    tmBlockId_t blockId;
#define PCIXIO_MODULE_ID                 _RGD_Hex_Module_Id_Of_PciXio
#define PCIXIO_MAJOR_REV                          0
#define PCIXIO_MINOR_REV                          0
} tmhwPciXioRegs_NONVOLATILE_t;
typedef volatile tmhwPciXioRegs_NONVOLATILE_t *ptmhwPciXioRegs_t;

#if        defined(RGD_CHECK_OFFSETS)
#if        RGD_CHECK_OFFSETS

#define _RGD_offsetof(p, f) ((unsigned int) (&(((p))->f)))
#define _RGD_Check_Offset(s, f, o)                              \
    {                                                           \
        extern void _##s##_##f##_offset_not_##o(                \
            tp##s,                                              \
            unsigned int                                        \
        );                                                      \
        (void) (                                                \
            (_RGD_offsetof(pRegs, f) == 0x##o)                  \
            ? 0                                                 \
            : (                                                 \
                _##s##_##f##_offset_not_##o(                    \
                    pRegs,                                      \
                    _RGD_offsetof(pRegs, f)                     \
                ),                                              \
                0                                               \
            )                                                   \
        );                                                      \
    }                                                           \

#pragma optimize("agpsw", on)

#if        defined(__cplusplus)
extern "C" {
void _RGD_CPP_Check_tmhwPciXioRegs_Offsets(ptmhwPciXioRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwPciXioRegs_Offsets(ptmhwPciXioRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwPciXioRegs, setup,                         00000010);
    _RGD_Check_Offset(tmhwPciXioRegs, ctrl,                          00000014);
    _RGD_Check_Offset(tmhwPciXioRegs, base1Lo,                       00000018);
    _RGD_Check_Offset(tmhwPciXioRegs, base1Hi,                       0000001c);
    _RGD_Check_Offset(tmhwPciXioRegs, base2Lo,                       00000020);
    _RGD_Check_Offset(tmhwPciXioRegs, base2Hi,                       00000024);
    _RGD_Check_Offset(tmhwPciXioRegs, readLifetime,                  00000028);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterAddr,                  0000002c);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterWriteData,             00000030);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterReadData,              00000034);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterCtrl,                  00000038);
    _RGD_Check_Offset(tmhwPciXioRegs, unlock,                        0000003c);
    _RGD_Check_Offset(tmhwPciXioRegs, configDevAndVendorId,          00000040);
    _RGD_Check_Offset(tmhwPciXioRegs, configCmdAndStatus,            00000044);
    _RGD_Check_Offset(tmhwPciXioRegs, configClassAndRev,             00000048);
    _RGD_Check_Offset(tmhwPciXioRegs, configLatencyAndCache,         0000004c);
    _RGD_Check_Offset(tmhwPciXioRegs, configBase10,                  00000050);
    _RGD_Check_Offset(tmhwPciXioRegs, configBase14,                  00000054);
    _RGD_Check_Offset(tmhwPciXioRegs, configBase18,                  00000058);
    _RGD_Check_Offset(tmhwPciXioRegs, configSubsystemIds,            0000006c);
    _RGD_Check_Offset(tmhwPciXioRegs, configCapsPtr,                 00000074);
    _RGD_Check_Offset(tmhwPciXioRegs, configMisc,                    0000007c);
    _RGD_Check_Offset(tmhwPciXioRegs, configPowerMgmtCaps,           00000080);
    _RGD_Check_Offset(tmhwPciXioRegs, configPowerMgmtCtrl,           00000084);
    _RGD_Check_Offset(tmhwPciXioRegs, configPciIo,                   00000088);
    _RGD_Check_Offset(tmhwPciXioRegs, slaveDtlTuning,                0000008c);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaDtlTuning,                  00000090);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaPciAddr,                 00000800);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaInternalAddr,            00000804);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaSize,                    00000808);
    _RGD_Check_Offset(tmhwPciXioRegs, xioDmaCtrl,                    0000080c);
    _RGD_Check_Offset(tmhwPciXioRegs, xioCtrl,                       00000810);
    _RGD_Check_Offset(tmhwPciXioRegs, xioSelect,                     00000814);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpAddr,                     00000820);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpWriteData,                00000824);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpReadData,                 00000828);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpCtrl,                     0000082c);
    _RGD_Check_Offset(tmhwPciXioRegs, nandFlashControls,             00000830);
    _RGD_Check_Offset(tmhwPciXioRegs, xioSelectMore,                 00000834);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntStatus,                00000fb0);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntEnable,                00000fb4);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntClear,                 00000fb8);
    _RGD_Check_Offset(tmhwPciXioRegs, xioGpIntSet,                   00000fbc);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntStatus,             00000fc0);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntEnable,             00000fc4);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntClear,              00000fc8);
    _RGD_Check_Offset(tmhwPciXioRegs, gpMasterIntSet,                00000fcc);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntStatus,                  00000fd0);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntEnable,                  00000fd4);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntClear,                   00000fd8);
    _RGD_Check_Offset(tmhwPciXioRegs, dmaIntSet,                     00000fdc);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntStatus,                  00000fe0);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntEnable,                  00000fe4);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntClear,                   00000fe8);
    _RGD_Check_Offset(tmhwPciXioRegs, pciIntSet,                     00000fec);
    _RGD_Check_Offset(tmhwPciXioRegs, blockId,                       00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWPCIXIOA051_RGD_SEEN)

#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
