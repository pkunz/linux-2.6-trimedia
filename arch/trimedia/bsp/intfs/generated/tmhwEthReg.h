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
#if        !defined(_TMHWETHERNET3902_H_SEEN)
#define             _TMHWETHERNET3902_H_SEEN
#define _RGD_Module_Id_Of_ETH              3902
#define _RGD_Hex_Module_Id_Of_ETH          0x3902
#define _RGD_Module_Name_Of_3902           ETH
typedef struct _tmhwEthRegs
{
    union _ethMac1
    {
        struct
        {
            unsigned int : 16;
            unsigned int softReset : 1;
            unsigned int simulationReset : 1;
            unsigned int : 2;
            unsigned int resetPEMCSRx : 1;
            unsigned int resetPerFUN : 1;
            unsigned int resetPEMCSTx : 1;
            unsigned int resetRetFun : 1;
            unsigned int : 3;
            unsigned int loopBack : 1;
            unsigned int txFlowControl : 1;
            unsigned int rxFlowControl : 1;
            unsigned int passAllReceiveFrames : 1;
            unsigned int receiveEnable : 1;
        } bits;
        unsigned int u32;
    } mac1;
    union _ethMac2
    {
        struct
        {
            unsigned int : 17;
            unsigned int excessDefer : 1;
            unsigned int backPressure : 1;
            unsigned int noBackOff : 1;
            unsigned int : 2;
            unsigned int longPreambleEnforcement : 1;
            unsigned int purePreambleEnforcement : 1;
            unsigned int autoDetectPadEnable : 1;
            unsigned int vlanPadEnable : 1;
            unsigned int padCRCEnable : 1;
            unsigned int crcEnable : 1;
            unsigned int delayedCRC : 1;
            unsigned int hugeFrameEnable : 1;
            unsigned int frameLengthChecking : 1;
            unsigned int fullDuplex : 1;
        } bits;
        unsigned int u32;
    } mac2;
    union _ethIpgt
    {
        struct
        {
            unsigned int : 25;
            unsigned int BackTobackInterpacketGap : 7;
        } bits;
        unsigned int u32;
    } ipgt;
    union _ethIpgr
    {
        struct
        {
            unsigned int : 17;
            unsigned int nonBackToBackInterpacketGap1 : 7;
            unsigned int : 1;
            unsigned int nonBackToBackInterpacketGap2 : 7;
        } bits;
        unsigned int u32;
    } ipgr;
    union _ethClrt
    {
        struct
        {
            unsigned int : 18;
            unsigned int collisionWindow : 6;
            unsigned int : 4;
            unsigned int retransmissionMaximum : 4;
        } bits;
        unsigned int u32;
    } clrt;
    union _ethMaxf
    {
        struct
        {
            unsigned int : 16;
            unsigned int maxFrameLength : 16;
        } bits;
        unsigned int u32;
    } maxf;
    union _ethSupp
    {
        struct
        {
            unsigned int : 16;
            unsigned int resetPESMII : 1;
            unsigned int : 2;
            unsigned int phyMode : 1;
            unsigned int resetPERMII : 1;
            unsigned int : 2;
            unsigned int speed : 1;
            unsigned int resetPE100X : 1;
            unsigned int forceQuiet : 1;
            unsigned int noCipher : 1;
            unsigned int disableLinkFail : 1;
            unsigned int resetPE10T : 1;
            unsigned int : 1;
            unsigned int EnableJabberProtection : 1;
            unsigned int bitMode : 1;
        } bits;
        unsigned int u32;
    } supp;
    union _ethTest
    {
        struct
        {
            unsigned int : 29;
            unsigned int testBackPressure : 1;
            unsigned int testPause : 1;
            unsigned int shortcutPauseQuanta : 1;
        } bits;
        unsigned int u32;
    } test;
    union _ethMcfg
    {
        struct
        {
            unsigned int : 16;
            unsigned int resetMIImMgmt : 1;
            unsigned int : 10;
            unsigned int clockSelect : 3;
            unsigned int suppressPreamble : 1;
            unsigned int scanIncrement : 1;
        } bits;
        unsigned int u32;
    } mcfg;
    union _ethMcmd
    {
        struct
        {
            unsigned int : 30;
            unsigned int scan : 1;
            unsigned int read : 1;
        } bits;
        unsigned int u32;
    } mcmd;
    union _ethMadr
    {
        struct
        {
            unsigned int : 19;
            unsigned int phyAddress : 5;
            unsigned int : 3;
            unsigned int registerAddress : 5;
        } bits;
        unsigned int u32;
    } madr;
    union _ethMwtd
    {
        struct
        {
            unsigned int : 16;
            unsigned int writeData : 16;
        } bits;
        unsigned int u32;
    } mwtd;
    union _ethMrdd
    {
        struct
        {
            unsigned int : 16;
            unsigned int readData : 16;
        } bits;
        unsigned int u32;
    } mrdd;
    union _ethMind
    {
        struct
        {
            unsigned int : 29;
            unsigned int notValid : 1;
            unsigned int scanning : 1;
            unsigned int busy : 1;
        } bits;
        unsigned int u32;
    } mind;
    unsigned char filler00038[0x00040-0x00038];
    union _ethSa0
    {
        struct
        {
            unsigned int : 16;
            unsigned int stationAddress6 : 8;
            unsigned int stationAddress5 : 8;
        } bits;
        unsigned int u32;
    } sa0;
    union _ethSa1
    {
        struct
        {
            unsigned int : 16;
            unsigned int stationAddress4 : 8;
            unsigned int stationAddress3 : 8;
        } bits;
        unsigned int u32;
    } sa1;
    union _ethSa2
    {
        struct
        {
            unsigned int : 16;
            unsigned int stationAddress2 : 8;
            unsigned int stationAddress1 : 8;
        } bits;
        unsigned int u32;
    } sa2;
    unsigned char filler0004c[0x00100-0x0004c];
    union _ethCommand
    {
        struct
        {
            unsigned int : 20;
            unsigned int enableQos : 1;
            unsigned int fullDuplex : 1;
            unsigned int rMII : 1;
            unsigned int txFlowControl : 1;
            unsigned int passRxFilter : 1;
            unsigned int passRuntFrame : 1;
            unsigned int rxReset : 1;
            unsigned int txReset : 1;
            unsigned int regReset : 1;
            unsigned int txRtEnable : 1;
            unsigned int txEnable : 1;
            unsigned int rxEnable : 1;
        } bits;
        unsigned int u32;
    } command;
    union _ethStatus
    {
        struct
        {
            unsigned int : 29;
            unsigned int txRtStatus : 1;
            unsigned int txStatus : 1;
            unsigned int rxStatus : 1;
        } bits;
        unsigned int u32;
    } status;
    union _ethRxDescriptor
    {
        struct
        {
            unsigned int rxDescriptor : 32;
        } bits;
        unsigned int u32;
    } rxDescriptor;
    union _ethRxStatus
    {
        struct
        {
            unsigned int rxStatus : 32;
        } bits;
        unsigned int u32;
    } rxStatus;
    union _ethRxDescriptorNumber
    {
        struct
        {
            unsigned int : 16;
            unsigned int rxDescriptorNumber : 16;
        } bits;
        unsigned int u32;
    } rxDescriptorNumber;
    union _ethRxProduceIndex
    {
        struct
        {
            unsigned int : 16;
            unsigned int rxProduceIndex : 16;
        } bits;
        unsigned int u32;
    } rxProduceIndex;
    union _ethRxConsumeIndex
    {
        struct
        {
            unsigned int : 16;
            unsigned int rxConsumeIndex : 16;
        } bits;
        unsigned int u32;
    } rxConsumeIndex;
    union _ethTxDescriptor
    {
        struct
        {
            unsigned int txDescriptor : 32;
        } bits;
        unsigned int u32;
    } txDescriptor;
    union _ethTxStatus
    {
        struct
        {
            unsigned int txStatus : 32;
        } bits;
        unsigned int u32;
    } txStatus;
    union _ethTxDescriptorNumber
    {
        struct
        {
            unsigned int : 16;
            unsigned int txDescriptorNumber : 16;
        } bits;
        unsigned int u32;
    } txDescriptorNumber;
    union _ethTxProduceIndex
    {
        struct
        {
            unsigned int : 16;
            unsigned int txProduceIndex : 16;
        } bits;
        unsigned int u32;
    } txProduceIndex;
    union _ethTxConsumeIndex
    {
        struct
        {
            unsigned int : 16;
            unsigned int txConsumeIndex : 16;
        } bits;
        unsigned int u32;
    } txConsumeIndex;
    union _ethTxRtDescriptor
    {
        struct
        {
            unsigned int txRtDescriptor : 32;
        } bits;
        unsigned int u32;
    } txRtDescriptor;
    union _ethTxRtStatus
    {
        struct
        {
            unsigned int txRtStatus : 32;
        } bits;
        unsigned int u32;
    } txRtStatus;
    union _ethTxRtDescriptorNumber
    {
        struct
        {
            unsigned int : 16;
            unsigned int txRtDescriptorNumber : 16;
        } bits;
        unsigned int u32;
    } txRtDescriptorNumber;
    union _ethTxRtProduceIndex
    {
        struct
        {
            unsigned int : 16;
            unsigned int txRtProduceIndex : 16;
        } bits;
        unsigned int u32;
    } txRtProduceIndex;
    union _ethTxRtConsumeIndex
    {
        struct
        {
            unsigned int : 16;
            unsigned int txRtConsumeIndex : 16;
        } bits;
        unsigned int u32;
    } txRtConsumeIndex;
    union _ethBlockZone
    {
        struct
        {
            unsigned int : 16;
            unsigned int blockZone : 16;
        } bits;
        unsigned int u32;
    } blockZone;
    union _ethQosTimeout
    {
        struct
        {
            unsigned int : 16;
            unsigned int qosTimeout : 16;
        } bits;
        unsigned int u32;
    } qosTimeout;
    unsigned char filler0014c[0x00158-0x0014c];
    union _ethTsv0
    {
        struct
        {
            unsigned int tsv0 : 32;
        } bits;
        unsigned int u32;
    } tsv0;
    union _ethTsv1
    {
        struct
        {
            unsigned int tsv1 : 32;
        } bits;
        unsigned int u32;
    } tsv1;
    union _ethRsv
    {
        struct
        {
            unsigned int rsv : 32;
        } bits;
        unsigned int u32;
    } rsv;
    unsigned char filler00164[0x00170-0x00164];
    union _ethFlowControlCounter
    {
        struct
        {
            unsigned int pauseTimer : 16;
            unsigned int mirrorCounter : 16;
        } bits;
        unsigned int u32;
    } flowControlCounter;
    union _ethFlowControlStatus
    {
        struct
        {
            unsigned int : 16;
            unsigned int mirrirCounterCurrent : 16;
        } bits;
        unsigned int u32;
    } flowControlStatus;
    unsigned char filler00178[0x001fc-0x00178];
    union _ethGlobalTimestamp
    {
        struct
        {
            unsigned int globalTimestamp : 32;
        } bits;
        unsigned int u32;
    } globalTimestamp;
    union _ethRxFilterCtrl
    {
        struct
        {
            unsigned int : 18;
            unsigned int rxFilterEnWol : 1;
            unsigned int magicPacketEnWol : 1;
            unsigned int patternMatchEn : 4;
#define ETH_PATTERN_MATCH0                    0x100
#define ETH_PATTERN_MATCH1                    0x200
#define ETH_PATTERN_MATCH2                    0x400
#define ETH_PATTERN_MATCH3                    0x800
            unsigned int andOR : 1;
            unsigned int : 1;
            unsigned int acceptPerfectEn : 1;
            unsigned int acceptMulticastHashEn : 1;
            unsigned int acceptUnicastHashEn : 1;
            unsigned int acceptMulticastEn : 1;
            unsigned int acceptBroadcastEn : 1;
            unsigned int acceptUnicastEn : 1;
        } bits;
        unsigned int u32;
    } rxFilterCtrl;
    union _ethRxFilterWolStatus
    {
        struct
        {
            unsigned int : 23;
            unsigned int magicPacketWol : 1;
            unsigned int rxFilterWol : 1;
            unsigned int patternMatchWol : 1;
            unsigned int acceptPerfectWol : 1;
            unsigned int acceptMulticastHashWol : 1;
            unsigned int acceptUnicastHashWol : 1;
            unsigned int acceptMulticastWol : 1;
            unsigned int acceptBroadcastWol : 1;
            unsigned int acceptUnicastWol : 1;
        } bits;
        unsigned int u32;
    } rxFilterWolStatus;
    union _ethRxFilterWolClear
    {
        struct
        {
            unsigned int : 23;
            unsigned int magicPacketWolClr : 1;
            unsigned int rxFilterWolClr : 1;
            unsigned int patternMatchWolClr : 1;
            unsigned int acceptPerfectWolClr : 1;
            unsigned int acceptMulticastHashWolClr : 1;
            unsigned int acceptUnicastHashWolClr : 1;
            unsigned int acceptMulticastWolClr : 1;
            unsigned int acceptBroadcastWolClr : 1;
            unsigned int acceptUnicastWolClr : 1;
        } bits;
        unsigned int u32;
    } rxFilterWolClear;
    union _ethPatternMatchJoin
    {
        struct
        {
            unsigned int : 8;
            unsigned int join0123 : 4;
            unsigned int join123 : 4;
            unsigned int join012 : 4;
            unsigned int join23 : 4;
            unsigned int join12 : 4;
            unsigned int join01 : 4;
        } bits;
        unsigned int u32;
    } patternMatchJoin;
    union _ethHashFilterL
    {
        struct
        {
            unsigned int hashFilterL : 32;
        } bits;
        unsigned int u32;
    } hashFilterL;
    union _ethHashFilterH
    {
        struct
        {
            unsigned int hashFilterH : 32;
        } bits;
        unsigned int u32;
    } hashFilterH;
    unsigned char filler00218[0x00230-0x00218];
    union _ethPatternMatchMask0L
    {
        struct
        {
            unsigned int patternMatchMask0L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask0L;
    union _ethPatternMatchMask0H
    {
        struct
        {
            unsigned int patternMatchMask0H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask0H;
    union _ethPattenMatchCRC0
    {
        struct
        {
            unsigned int pattenMatchCRC0 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC0;
    union _ethPatternMatchSkip0
    {
        struct
        {
            unsigned int patternMatchSkip0 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip0;
    union _ethPatternMatchMask1L
    {
        struct
        {
            unsigned int patternMatchMask1L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask1L;
    union _ethPatternMatchMask1H
    {
        struct
        {
            unsigned int patternMatchMask1H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask1H;
    union _ethPattenMatchCRC1
    {
        struct
        {
            unsigned int pattenMatchCRC1 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC1;
    union _ethPatternMatchSkip1
    {
        struct
        {
            unsigned int patternMatchSkip1 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip1;
    union _ethPatternMatchMask2L
    {
        struct
        {
            unsigned int patternMatchMask2L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask2L;
    union _ethPatternMatchMask2H
    {
        struct
        {
            unsigned int patternMatchMask2H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask2H;
    union _ethPattenMatchCRC2
    {
        struct
        {
            unsigned int pattenMatchCRC2 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC2;
    union _ethPatternMatchSkip2
    {
        struct
        {
            unsigned int patternMatchSkip2 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip2;
    union _ethPatternMatchMask3L
    {
        struct
        {
            unsigned int patternMatchMask3L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask3L;
    union _ethPatternMatchMask3H
    {
        struct
        {
            unsigned int patternMatchMask3H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask3H;
    union _ethPattenMatchCRC3
    {
        struct
        {
            unsigned int pattenMatchCRC3 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC3;
    union _ethPatternMatchSkip3
    {
        struct
        {
            unsigned int patternMatchSkip3 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip3;
    unsigned char filler00270[0x00fe0-0x00270];
    union _ethIntStatus
    {
        struct
        {
            unsigned int : 18;
            unsigned int wakeupInt : 1;
            unsigned int softInt : 1;
            unsigned int txRtDoneInt : 1;
            unsigned int txRtFinishedInt : 1;
            unsigned int txRtErrorInt : 1;
            unsigned int txRtUnderrunInt : 1;
            unsigned int txDoneInt : 1;
            unsigned int txFinishedInt : 1;
            unsigned int txErrorInt : 1;
            unsigned int txUnderrunInt : 1;
            unsigned int rxDoneInt : 1;
            unsigned int rxFinishedInt : 1;
            unsigned int rxErrorInt : 1;
            unsigned int rxOverInt : 1;
        } bits;
        unsigned int u32;
    } intStatus;
    union _ethIntEnable
    {
        struct
        {
            unsigned int : 18;
            unsigned int wakeupIntEn : 1;
            unsigned int softIntEn : 1;
            unsigned int txRtDoneIntEn : 1;
            unsigned int txRtFinishedIntEn : 1;
            unsigned int txRtErrorIntEn : 1;
            unsigned int txRtUnderrunIntEn : 1;
            unsigned int txDoneIntEn : 1;
            unsigned int txFinishedIntEn : 1;
            unsigned int txErrorIntEn : 1;
            unsigned int txUnderrunIntEn : 1;
            unsigned int rxDoneIntEn : 1;
            unsigned int rxFinishedIntEn : 1;
            unsigned int rxErrorIntEn : 1;
            unsigned int rxOverIntEn : 1;
        } bits;
        unsigned int u32;
    } intEnable;
    union _ethIntClear
    {
        struct
        {
            unsigned int : 18;
            unsigned int wakeupIntClr : 1;
            unsigned int softIntClr : 1;
            unsigned int txRtDoneIntClr : 1;
            unsigned int txRtFinishedIntClr : 1;
            unsigned int txRtErrorIntClr : 1;
            unsigned int txRtUnderrunIntClr : 1;
            unsigned int txDoneIntClr : 1;
            unsigned int txFinishedIntClr : 1;
            unsigned int txErrorIntClr : 1;
            unsigned int txUnderrunIntClr : 1;
            unsigned int rxDoneIntClr : 1;
            unsigned int rxFinishedIntClr : 1;
            unsigned int rxErrorIntClr : 1;
            unsigned int rxOverIntClr : 1;
        } bits;
        unsigned int u32;
    } intClear;
    union _ethIntSet
    {
        struct
        {
            unsigned int : 18;
            unsigned int wakeupIntSet : 1;
            unsigned int softIntSet : 1;
            unsigned int txRtDoneIntSet : 1;
            unsigned int txRtFinishedIntSet : 1;
            unsigned int txRtErrorIntSet : 1;
            unsigned int txRtUnderrunIntSet : 1;
            unsigned int txDoneIntSet : 1;
            unsigned int txFinishedIntSet : 1;
            unsigned int txErrorIntSet : 1;
            unsigned int txUnderrunIntSet : 1;
            unsigned int rxDoneIntSet : 1;
            unsigned int rxFinishedIntSet : 1;
            unsigned int rxErrorIntSet : 1;
            unsigned int rxOverrunIntSet : 1;
        } bits;
        unsigned int u32;
    } intSet;
    unsigned char filler00ff0[0x00ff4-0x00ff0];
    union _ethPowerDown
    {
        struct
        {
            unsigned int powerDown : 1;
            unsigned int : 31;
        } bits;
        unsigned int u32;
    } powerDown;
    unsigned char filler00ff8[0x00ffc-0x00ff8];
    union _ethModuleId
    {
        struct
        {
            unsigned int moduleId : 16;
            unsigned int majRev : 4;
            unsigned int minRev : 4;
            unsigned int apertureSize : 8;
        } bits;
        unsigned int u32;
    } moduleId;
} tmhwEthRegs_NONVOLATILE_t;
typedef volatile tmhwEthRegs_NONVOLATILE_t *ptmhwEthRegs_t;

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
void _RGD_CPP_Check_tmhwEthRegs_Offsets(ptmhwEthRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwEthRegs_Offsets(ptmhwEthRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwEthRegs, mac1,                          00000000);
    _RGD_Check_Offset(tmhwEthRegs, mac2,                          00000004);
    _RGD_Check_Offset(tmhwEthRegs, ipgt,                          00000008);
    _RGD_Check_Offset(tmhwEthRegs, ipgr,                          0000000c);
    _RGD_Check_Offset(tmhwEthRegs, clrt,                          00000010);
    _RGD_Check_Offset(tmhwEthRegs, maxf,                          00000014);
    _RGD_Check_Offset(tmhwEthRegs, supp,                          00000018);
    _RGD_Check_Offset(tmhwEthRegs, test,                          0000001c);
    _RGD_Check_Offset(tmhwEthRegs, mcfg,                          00000020);
    _RGD_Check_Offset(tmhwEthRegs, mcmd,                          00000024);
    _RGD_Check_Offset(tmhwEthRegs, madr,                          00000028);
    _RGD_Check_Offset(tmhwEthRegs, mwtd,                          0000002c);
    _RGD_Check_Offset(tmhwEthRegs, mrdd,                          00000030);
    _RGD_Check_Offset(tmhwEthRegs, mind,                          00000034);
    _RGD_Check_Offset(tmhwEthRegs, sa0,                           00000040);
    _RGD_Check_Offset(tmhwEthRegs, sa1,                           00000044);
    _RGD_Check_Offset(tmhwEthRegs, sa2,                           00000048);
    _RGD_Check_Offset(tmhwEthRegs, command,                       00000100);
    _RGD_Check_Offset(tmhwEthRegs, status,                        00000104);
    _RGD_Check_Offset(tmhwEthRegs, rxDescriptor,                  00000108);
    _RGD_Check_Offset(tmhwEthRegs, rxStatus,                      0000010c);
    _RGD_Check_Offset(tmhwEthRegs, rxDescriptorNumber,            00000110);
    _RGD_Check_Offset(tmhwEthRegs, rxProduceIndex,                00000114);
    _RGD_Check_Offset(tmhwEthRegs, rxConsumeIndex,                00000118);
    _RGD_Check_Offset(tmhwEthRegs, txDescriptor,                  0000011c);
    _RGD_Check_Offset(tmhwEthRegs, txStatus,                      00000120);
    _RGD_Check_Offset(tmhwEthRegs, txDescriptorNumber,            00000124);
    _RGD_Check_Offset(tmhwEthRegs, txProduceIndex,                00000128);
    _RGD_Check_Offset(tmhwEthRegs, txConsumeIndex,                0000012c);
    _RGD_Check_Offset(tmhwEthRegs, txRtDescriptor,                00000130);
    _RGD_Check_Offset(tmhwEthRegs, txRtStatus,                    00000134);
    _RGD_Check_Offset(tmhwEthRegs, txRtDescriptorNumber,          00000138);
    _RGD_Check_Offset(tmhwEthRegs, txRtProduceIndex,              0000013c);
    _RGD_Check_Offset(tmhwEthRegs, txRtConsumeIndex,              00000140);
    _RGD_Check_Offset(tmhwEthRegs, blockZone,                     00000144);
    _RGD_Check_Offset(tmhwEthRegs, qosTimeout,                    00000148);
    _RGD_Check_Offset(tmhwEthRegs, tsv0,                          00000158);
    _RGD_Check_Offset(tmhwEthRegs, tsv1,                          0000015c);
    _RGD_Check_Offset(tmhwEthRegs, rsv,                           00000160);
    _RGD_Check_Offset(tmhwEthRegs, flowControlCounter,            00000170);
    _RGD_Check_Offset(tmhwEthRegs, flowControlStatus,             00000174);
    _RGD_Check_Offset(tmhwEthRegs, globalTimestamp,               000001fc);
    _RGD_Check_Offset(tmhwEthRegs, rxFilterCtrl,                  00000200);
    _RGD_Check_Offset(tmhwEthRegs, rxFilterWolStatus,             00000204);
    _RGD_Check_Offset(tmhwEthRegs, rxFilterWolClear,              00000208);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchJoin,              0000020c);
    _RGD_Check_Offset(tmhwEthRegs, hashFilterL,                   00000210);
    _RGD_Check_Offset(tmhwEthRegs, hashFilterH,                   00000214);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask0L,            00000230);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask0H,            00000234);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC0,               00000238);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip0,             0000023c);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask1L,            00000240);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask1H,            00000244);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC1,               00000248);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip1,             0000024c);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask2L,            00000250);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask2H,            00000254);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC2,               00000258);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip2,             0000025c);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask3L,            00000260);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask3H,            00000264);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC3,               00000268);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip3,             0000026c);
    _RGD_Check_Offset(tmhwEthRegs, intStatus,                     00000fe0);
    _RGD_Check_Offset(tmhwEthRegs, intEnable,                     00000fe4);
    _RGD_Check_Offset(tmhwEthRegs, intClear,                      00000fe8);
    _RGD_Check_Offset(tmhwEthRegs, intSet,                        00000fec);
    _RGD_Check_Offset(tmhwEthRegs, powerDown,                     00000ff4);
    _RGD_Check_Offset(tmhwEthRegs, moduleId,                      00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWETHERNET3902_H_SEEN)

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
#if        !defined(_TMHWETHERNET3902_H_SEEN)
#define             _TMHWETHERNET3902_H_SEEN
#define _RGD_Module_Id_Of_ETH              3902
#define _RGD_Hex_Module_Id_Of_ETH          0x3902
#define _RGD_Module_Name_Of_3902           ETH
typedef struct _tmhwEthRegs
{
    union _ethMac1
    {
        struct
        {
            unsigned int receiveEnable : 1;
            unsigned int passAllReceiveFrames : 1;
            unsigned int rxFlowControl : 1;
            unsigned int txFlowControl : 1;
            unsigned int loopBack : 1;
            unsigned int : 3;
            unsigned int resetRetFun : 1;
            unsigned int resetPEMCSTx : 1;
            unsigned int resetPerFUN : 1;
            unsigned int resetPEMCSRx : 1;
            unsigned int : 2;
            unsigned int simulationReset : 1;
            unsigned int softReset : 1;
        } bits;
        unsigned int u32;
    } mac1;
    union _ethMac2
    {
        struct
        {
            unsigned int fullDuplex : 1;
            unsigned int frameLengthChecking : 1;
            unsigned int hugeFrameEnable : 1;
            unsigned int delayedCRC : 1;
            unsigned int crcEnable : 1;
            unsigned int padCRCEnable : 1;
            unsigned int vlanPadEnable : 1;
            unsigned int autoDetectPadEnable : 1;
            unsigned int purePreambleEnforcement : 1;
            unsigned int longPreambleEnforcement : 1;
            unsigned int : 2;
            unsigned int noBackOff : 1;
            unsigned int backPressure : 1;
            unsigned int excessDefer : 1;
        } bits;
        unsigned int u32;
    } mac2;
    union _ethIpgt
    {
        struct
        {
            unsigned int BackTobackInterpacketGap : 7;
        } bits;
        unsigned int u32;
    } ipgt;
    union _ethIpgr
    {
        struct
        {
            unsigned int nonBackToBackInterpacketGap2 : 7;
            unsigned int : 1;
            unsigned int nonBackToBackInterpacketGap1 : 7;
        } bits;
        unsigned int u32;
    } ipgr;
    union _ethClrt
    {
        struct
        {
            unsigned int retransmissionMaximum : 4;
            unsigned int : 4;
            unsigned int collisionWindow : 6;
        } bits;
        unsigned int u32;
    } clrt;
    union _ethMaxf
    {
        struct
        {
            unsigned int maxFrameLength : 16;
        } bits;
        unsigned int u32;
    } maxf;
    union _ethSupp
    {
        struct
        {
            unsigned int bitMode : 1;
            unsigned int EnableJabberProtection : 1;
            unsigned int : 1;
            unsigned int resetPE10T : 1;
            unsigned int disableLinkFail : 1;
            unsigned int noCipher : 1;
            unsigned int forceQuiet : 1;
            unsigned int resetPE100X : 1;
            unsigned int speed : 1;
            unsigned int : 2;
            unsigned int resetPERMII : 1;
            unsigned int phyMode : 1;
            unsigned int : 2;
            unsigned int resetPESMII : 1;
        } bits;
        unsigned int u32;
    } supp;
    union _ethTest
    {
        struct
        {
            unsigned int shortcutPauseQuanta : 1;
            unsigned int testPause : 1;
            unsigned int testBackPressure : 1;
        } bits;
        unsigned int u32;
    } test;
    union _ethMcfg
    {
        struct
        {
            unsigned int scanIncrement : 1;
            unsigned int suppressPreamble : 1;
            unsigned int clockSelect : 3;
            unsigned int : 10;
            unsigned int resetMIImMgmt : 1;
        } bits;
        unsigned int u32;
    } mcfg;
    union _ethMcmd
    {
        struct
        {
            unsigned int read : 1;
            unsigned int scan : 1;
        } bits;
        unsigned int u32;
    } mcmd;
    union _ethMadr
    {
        struct
        {
            unsigned int registerAddress : 5;
            unsigned int : 3;
            unsigned int phyAddress : 5;
        } bits;
        unsigned int u32;
    } madr;
    union _ethMwtd
    {
        struct
        {
            unsigned int writeData : 16;
        } bits;
        unsigned int u32;
    } mwtd;
    union _ethMrdd
    {
        struct
        {
            unsigned int readData : 16;
        } bits;
        unsigned int u32;
    } mrdd;
    union _ethMind
    {
        struct
        {
            unsigned int busy : 1;
            unsigned int scanning : 1;
            unsigned int notValid : 1;
        } bits;
        unsigned int u32;
    } mind;
    unsigned char filler00038[0x00040-0x00038];
    union _ethSa0
    {
        struct
        {
            unsigned int stationAddress5 : 8;
            unsigned int stationAddress6 : 8;
        } bits;
        unsigned int u32;
    } sa0;
    union _ethSa1
    {
        struct
        {
            unsigned int stationAddress3 : 8;
            unsigned int stationAddress4 : 8;
        } bits;
        unsigned int u32;
    } sa1;
    union _ethSa2
    {
        struct
        {
            unsigned int stationAddress1 : 8;
            unsigned int stationAddress2 : 8;
        } bits;
        unsigned int u32;
    } sa2;
    unsigned char filler0004c[0x00100-0x0004c];
    union _ethCommand
    {
        struct
        {
            unsigned int rxEnable : 1;
            unsigned int txEnable : 1;
            unsigned int txRtEnable : 1;
            unsigned int regReset : 1;
            unsigned int txReset : 1;
            unsigned int rxReset : 1;
            unsigned int passRuntFrame : 1;
            unsigned int passRxFilter : 1;
            unsigned int txFlowControl : 1;
            unsigned int rMII : 1;
            unsigned int fullDuplex : 1;
            unsigned int enableQos : 1;
        } bits;
        unsigned int u32;
    } command;
    union _ethStatus
    {
        struct
        {
            unsigned int rxStatus : 1;
            unsigned int txStatus : 1;
            unsigned int txRtStatus : 1;
        } bits;
        unsigned int u32;
    } status;
    union _ethRxDescriptor
    {
        struct
        {
            unsigned int rxDescriptor : 32;
        } bits;
        unsigned int u32;
    } rxDescriptor;
    union _ethRxStatus
    {
        struct
        {
            unsigned int rxStatus : 32;
        } bits;
        unsigned int u32;
    } rxStatus;
    union _ethRxDescriptorNumber
    {
        struct
        {
            unsigned int rxDescriptorNumber : 16;
        } bits;
        unsigned int u32;
    } rxDescriptorNumber;
    union _ethRxProduceIndex
    {
        struct
        {
            unsigned int rxProduceIndex : 16;
        } bits;
        unsigned int u32;
    } rxProduceIndex;
    union _ethRxConsumeIndex
    {
        struct
        {
            unsigned int rxConsumeIndex : 16;
        } bits;
        unsigned int u32;
    } rxConsumeIndex;
    union _ethTxDescriptor
    {
        struct
        {
            unsigned int txDescriptor : 32;
        } bits;
        unsigned int u32;
    } txDescriptor;
    union _ethTxStatus
    {
        struct
        {
            unsigned int txStatus : 32;
        } bits;
        unsigned int u32;
    } txStatus;
    union _ethTxDescriptorNumber
    {
        struct
        {
            unsigned int txDescriptorNumber : 16;
        } bits;
        unsigned int u32;
    } txDescriptorNumber;
    union _ethTxProduceIndex
    {
        struct
        {
            unsigned int txProduceIndex : 16;
        } bits;
        unsigned int u32;
    } txProduceIndex;
    union _ethTxConsumeIndex
    {
        struct
        {
            unsigned int txConsumeIndex : 16;
        } bits;
        unsigned int u32;
    } txConsumeIndex;
    union _ethTxRtDescriptor
    {
        struct
        {
            unsigned int txRtDescriptor : 32;
        } bits;
        unsigned int u32;
    } txRtDescriptor;
    union _ethTxRtStatus
    {
        struct
        {
            unsigned int txRtStatus : 32;
        } bits;
        unsigned int u32;
    } txRtStatus;
    union _ethTxRtDescriptorNumber
    {
        struct
        {
            unsigned int txRtDescriptorNumber : 16;
        } bits;
        unsigned int u32;
    } txRtDescriptorNumber;
    union _ethTxRtProduceIndex
    {
        struct
        {
            unsigned int txRtProduceIndex : 16;
        } bits;
        unsigned int u32;
    } txRtProduceIndex;
    union _ethTxRtConsumeIndex
    {
        struct
        {
            unsigned int txRtConsumeIndex : 16;
        } bits;
        unsigned int u32;
    } txRtConsumeIndex;
    union _ethBlockZone
    {
        struct
        {
            unsigned int blockZone : 16;
        } bits;
        unsigned int u32;
    } blockZone;
    union _ethQosTimeout
    {
        struct
        {
            unsigned int qosTimeout : 16;
        } bits;
        unsigned int u32;
    } qosTimeout;
    unsigned char filler0014c[0x00158-0x0014c];
    union _ethTsv0
    {
        struct
        {
            unsigned int tsv0 : 32;
        } bits;
        unsigned int u32;
    } tsv0;
    union _ethTsv1
    {
        struct
        {
            unsigned int tsv1 : 32;
        } bits;
        unsigned int u32;
    } tsv1;
    union _ethRsv
    {
        struct
        {
            unsigned int rsv : 32;
        } bits;
        unsigned int u32;
    } rsv;
    unsigned char filler00164[0x00170-0x00164];
    union _ethFlowControlCounter
    {
        struct
        {
            unsigned int mirrorCounter : 16;
            unsigned int pauseTimer : 16;
        } bits;
        unsigned int u32;
    } flowControlCounter;
    union _ethFlowControlStatus
    {
        struct
        {
            unsigned int mirrirCounterCurrent : 16;
        } bits;
        unsigned int u32;
    } flowControlStatus;
    unsigned char filler00178[0x001fc-0x00178];
    union _ethGlobalTimestamp
    {
        struct
        {
            unsigned int globalTimestamp : 32;
        } bits;
        unsigned int u32;
    } globalTimestamp;
    union _ethRxFilterCtrl
    {
        struct
        {
            unsigned int acceptUnicastEn : 1;
            unsigned int acceptBroadcastEn : 1;
            unsigned int acceptMulticastEn : 1;
            unsigned int acceptUnicastHashEn : 1;
            unsigned int acceptMulticastHashEn : 1;
            unsigned int acceptPerfectEn : 1;
            unsigned int : 1;
            unsigned int andOR : 1;
            unsigned int patternMatchEn : 4;
#define ETH_PATTERN_MATCH0                    0x100
#define ETH_PATTERN_MATCH1                    0x200
#define ETH_PATTERN_MATCH2                    0x400
#define ETH_PATTERN_MATCH3                    0x800
            unsigned int magicPacketEnWol : 1;
            unsigned int rxFilterEnWol : 1;
        } bits;
        unsigned int u32;
    } rxFilterCtrl;
    union _ethRxFilterWolStatus
    {
        struct
        {
            unsigned int acceptUnicastWol : 1;
            unsigned int acceptBroadcastWol : 1;
            unsigned int acceptMulticastWol : 1;
            unsigned int acceptUnicastHashWol : 1;
            unsigned int acceptMulticastHashWol : 1;
            unsigned int acceptPerfectWol : 1;
            unsigned int patternMatchWol : 1;
            unsigned int rxFilterWol : 1;
            unsigned int magicPacketWol : 1;
        } bits;
        unsigned int u32;
    } rxFilterWolStatus;
    union _ethRxFilterWolClear
    {
        struct
        {
            unsigned int acceptUnicastWolClr : 1;
            unsigned int acceptBroadcastWolClr : 1;
            unsigned int acceptMulticastWolClr : 1;
            unsigned int acceptUnicastHashWolClr : 1;
            unsigned int acceptMulticastHashWolClr : 1;
            unsigned int acceptPerfectWolClr : 1;
            unsigned int patternMatchWolClr : 1;
            unsigned int rxFilterWolClr : 1;
            unsigned int magicPacketWolClr : 1;
        } bits;
        unsigned int u32;
    } rxFilterWolClear;
    union _ethPatternMatchJoin
    {
        struct
        {
            unsigned int join01 : 4;
            unsigned int join12 : 4;
            unsigned int join23 : 4;
            unsigned int join012 : 4;
            unsigned int join123 : 4;
            unsigned int join0123 : 4;
        } bits;
        unsigned int u32;
    } patternMatchJoin;
    union _ethHashFilterL
    {
        struct
        {
            unsigned int hashFilterL : 32;
        } bits;
        unsigned int u32;
    } hashFilterL;
    union _ethHashFilterH
    {
        struct
        {
            unsigned int hashFilterH : 32;
        } bits;
        unsigned int u32;
    } hashFilterH;
    unsigned char filler00218[0x00230-0x00218];
    union _ethPatternMatchMask0L
    {
        struct
        {
            unsigned int patternMatchMask0L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask0L;
    union _ethPatternMatchMask0H
    {
        struct
        {
            unsigned int patternMatchMask0H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask0H;
    union _ethPattenMatchCRC0
    {
        struct
        {
            unsigned int pattenMatchCRC0 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC0;
    union _ethPatternMatchSkip0
    {
        struct
        {
            unsigned int patternMatchSkip0 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip0;
    union _ethPatternMatchMask1L
    {
        struct
        {
            unsigned int patternMatchMask1L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask1L;
    union _ethPatternMatchMask1H
    {
        struct
        {
            unsigned int patternMatchMask1H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask1H;
    union _ethPattenMatchCRC1
    {
        struct
        {
            unsigned int pattenMatchCRC1 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC1;
    union _ethPatternMatchSkip1
    {
        struct
        {
            unsigned int patternMatchSkip1 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip1;
    union _ethPatternMatchMask2L
    {
        struct
        {
            unsigned int patternMatchMask2L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask2L;
    union _ethPatternMatchMask2H
    {
        struct
        {
            unsigned int patternMatchMask2H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask2H;
    union _ethPattenMatchCRC2
    {
        struct
        {
            unsigned int pattenMatchCRC2 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC2;
    union _ethPatternMatchSkip2
    {
        struct
        {
            unsigned int patternMatchSkip2 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip2;
    union _ethPatternMatchMask3L
    {
        struct
        {
            unsigned int patternMatchMask3L : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask3L;
    union _ethPatternMatchMask3H
    {
        struct
        {
            unsigned int patternMatchMask3H : 32;
        } bits;
        unsigned int u32;
    } patternMatchMask3H;
    union _ethPattenMatchCRC3
    {
        struct
        {
            unsigned int pattenMatchCRC3 : 32;
        } bits;
        unsigned int u32;
    } pattenMatchCRC3;
    union _ethPatternMatchSkip3
    {
        struct
        {
            unsigned int patternMatchSkip3 : 32;
        } bits;
        unsigned int u32;
    } patternMatchSkip3;
    unsigned char filler00270[0x00fe0-0x00270];
    union _ethIntStatus
    {
        struct
        {
            unsigned int rxOverInt : 1;
            unsigned int rxErrorInt : 1;
            unsigned int rxFinishedInt : 1;
            unsigned int rxDoneInt : 1;
            unsigned int txUnderrunInt : 1;
            unsigned int txErrorInt : 1;
            unsigned int txFinishedInt : 1;
            unsigned int txDoneInt : 1;
            unsigned int txRtUnderrunInt : 1;
            unsigned int txRtErrorInt : 1;
            unsigned int txRtFinishedInt : 1;
            unsigned int txRtDoneInt : 1;
            unsigned int softInt : 1;
            unsigned int wakeupInt : 1;
        } bits;
        unsigned int u32;
    } intStatus;
    union _ethIntEnable
    {
        struct
        {
            unsigned int rxOverIntEn : 1;
            unsigned int rxErrorIntEn : 1;
            unsigned int rxFinishedIntEn : 1;
            unsigned int rxDoneIntEn : 1;
            unsigned int txUnderrunIntEn : 1;
            unsigned int txErrorIntEn : 1;
            unsigned int txFinishedIntEn : 1;
            unsigned int txDoneIntEn : 1;
            unsigned int txRtUnderrunIntEn : 1;
            unsigned int txRtErrorIntEn : 1;
            unsigned int txRtFinishedIntEn : 1;
            unsigned int txRtDoneIntEn : 1;
            unsigned int softIntEn : 1;
            unsigned int wakeupIntEn : 1;
        } bits;
        unsigned int u32;
    } intEnable;
    union _ethIntClear
    {
        struct
        {
            unsigned int rxOverIntClr : 1;
            unsigned int rxErrorIntClr : 1;
            unsigned int rxFinishedIntClr : 1;
            unsigned int rxDoneIntClr : 1;
            unsigned int txUnderrunIntClr : 1;
            unsigned int txErrorIntClr : 1;
            unsigned int txFinishedIntClr : 1;
            unsigned int txDoneIntClr : 1;
            unsigned int txRtUnderrunIntClr : 1;
            unsigned int txRtErrorIntClr : 1;
            unsigned int txRtFinishedIntClr : 1;
            unsigned int txRtDoneIntClr : 1;
            unsigned int softIntClr : 1;
            unsigned int wakeupIntClr : 1;
        } bits;
        unsigned int u32;
    } intClear;
    union _ethIntSet
    {
        struct
        {
            unsigned int rxOverrunIntSet : 1;
            unsigned int rxErrorIntSet : 1;
            unsigned int rxFinishedIntSet : 1;
            unsigned int rxDoneIntSet : 1;
            unsigned int txUnderrunIntSet : 1;
            unsigned int txErrorIntSet : 1;
            unsigned int txFinishedIntSet : 1;
            unsigned int txDoneIntSet : 1;
            unsigned int txRtUnderrunIntSet : 1;
            unsigned int txRtErrorIntSet : 1;
            unsigned int txRtFinishedIntSet : 1;
            unsigned int txRtDoneIntSet : 1;
            unsigned int softIntSet : 1;
            unsigned int wakeupIntSet : 1;
        } bits;
        unsigned int u32;
    } intSet;
    unsigned char filler00ff0[0x00ff4-0x00ff0];
    union _ethPowerDown
    {
        struct
        {
            unsigned int : 31;
            unsigned int powerDown : 1;
        } bits;
        unsigned int u32;
    } powerDown;
    unsigned char filler00ff8[0x00ffc-0x00ff8];
    union _ethModuleId
    {
        struct
        {
            unsigned int apertureSize : 8;
            unsigned int minRev : 4;
            unsigned int majRev : 4;
            unsigned int moduleId : 16;
        } bits;
        unsigned int u32;
    } moduleId;
} tmhwEthRegs_NONVOLATILE_t;
typedef volatile tmhwEthRegs_NONVOLATILE_t *ptmhwEthRegs_t;

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
void _RGD_CPP_Check_tmhwEthRegs_Offsets(ptmhwEthRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwEthRegs_Offsets(ptmhwEthRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwEthRegs, mac1,                          00000000);
    _RGD_Check_Offset(tmhwEthRegs, mac2,                          00000004);
    _RGD_Check_Offset(tmhwEthRegs, ipgt,                          00000008);
    _RGD_Check_Offset(tmhwEthRegs, ipgr,                          0000000c);
    _RGD_Check_Offset(tmhwEthRegs, clrt,                          00000010);
    _RGD_Check_Offset(tmhwEthRegs, maxf,                          00000014);
    _RGD_Check_Offset(tmhwEthRegs, supp,                          00000018);
    _RGD_Check_Offset(tmhwEthRegs, test,                          0000001c);
    _RGD_Check_Offset(tmhwEthRegs, mcfg,                          00000020);
    _RGD_Check_Offset(tmhwEthRegs, mcmd,                          00000024);
    _RGD_Check_Offset(tmhwEthRegs, madr,                          00000028);
    _RGD_Check_Offset(tmhwEthRegs, mwtd,                          0000002c);
    _RGD_Check_Offset(tmhwEthRegs, mrdd,                          00000030);
    _RGD_Check_Offset(tmhwEthRegs, mind,                          00000034);
    _RGD_Check_Offset(tmhwEthRegs, sa0,                           00000040);
    _RGD_Check_Offset(tmhwEthRegs, sa1,                           00000044);
    _RGD_Check_Offset(tmhwEthRegs, sa2,                           00000048);
    _RGD_Check_Offset(tmhwEthRegs, command,                       00000100);
    _RGD_Check_Offset(tmhwEthRegs, status,                        00000104);
    _RGD_Check_Offset(tmhwEthRegs, rxDescriptor,                  00000108);
    _RGD_Check_Offset(tmhwEthRegs, rxStatus,                      0000010c);
    _RGD_Check_Offset(tmhwEthRegs, rxDescriptorNumber,            00000110);
    _RGD_Check_Offset(tmhwEthRegs, rxProduceIndex,                00000114);
    _RGD_Check_Offset(tmhwEthRegs, rxConsumeIndex,                00000118);
    _RGD_Check_Offset(tmhwEthRegs, txDescriptor,                  0000011c);
    _RGD_Check_Offset(tmhwEthRegs, txStatus,                      00000120);
    _RGD_Check_Offset(tmhwEthRegs, txDescriptorNumber,            00000124);
    _RGD_Check_Offset(tmhwEthRegs, txProduceIndex,                00000128);
    _RGD_Check_Offset(tmhwEthRegs, txConsumeIndex,                0000012c);
    _RGD_Check_Offset(tmhwEthRegs, txRtDescriptor,                00000130);
    _RGD_Check_Offset(tmhwEthRegs, txRtStatus,                    00000134);
    _RGD_Check_Offset(tmhwEthRegs, txRtDescriptorNumber,          00000138);
    _RGD_Check_Offset(tmhwEthRegs, txRtProduceIndex,              0000013c);
    _RGD_Check_Offset(tmhwEthRegs, txRtConsumeIndex,              00000140);
    _RGD_Check_Offset(tmhwEthRegs, blockZone,                     00000144);
    _RGD_Check_Offset(tmhwEthRegs, qosTimeout,                    00000148);
    _RGD_Check_Offset(tmhwEthRegs, tsv0,                          00000158);
    _RGD_Check_Offset(tmhwEthRegs, tsv1,                          0000015c);
    _RGD_Check_Offset(tmhwEthRegs, rsv,                           00000160);
    _RGD_Check_Offset(tmhwEthRegs, flowControlCounter,            00000170);
    _RGD_Check_Offset(tmhwEthRegs, flowControlStatus,             00000174);
    _RGD_Check_Offset(tmhwEthRegs, globalTimestamp,               000001fc);
    _RGD_Check_Offset(tmhwEthRegs, rxFilterCtrl,                  00000200);
    _RGD_Check_Offset(tmhwEthRegs, rxFilterWolStatus,             00000204);
    _RGD_Check_Offset(tmhwEthRegs, rxFilterWolClear,              00000208);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchJoin,              0000020c);
    _RGD_Check_Offset(tmhwEthRegs, hashFilterL,                   00000210);
    _RGD_Check_Offset(tmhwEthRegs, hashFilterH,                   00000214);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask0L,            00000230);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask0H,            00000234);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC0,               00000238);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip0,             0000023c);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask1L,            00000240);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask1H,            00000244);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC1,               00000248);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip1,             0000024c);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask2L,            00000250);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask2H,            00000254);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC2,               00000258);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip2,             0000025c);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask3L,            00000260);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchMask3H,            00000264);
    _RGD_Check_Offset(tmhwEthRegs, pattenMatchCRC3,               00000268);
    _RGD_Check_Offset(tmhwEthRegs, patternMatchSkip3,             0000026c);
    _RGD_Check_Offset(tmhwEthRegs, intStatus,                     00000fe0);
    _RGD_Check_Offset(tmhwEthRegs, intEnable,                     00000fe4);
    _RGD_Check_Offset(tmhwEthRegs, intClear,                      00000fe8);
    _RGD_Check_Offset(tmhwEthRegs, intSet,                        00000fec);
    _RGD_Check_Offset(tmhwEthRegs, powerDown,                     00000ff4);
    _RGD_Check_Offset(tmhwEthRegs, moduleId,                      00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWETHERNET3902_H_SEEN)

#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
