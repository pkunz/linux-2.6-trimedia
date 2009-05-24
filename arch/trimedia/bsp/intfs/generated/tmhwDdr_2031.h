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
#if        !defined(_TMHWDDR2031_REGS_RGD_SEEN)
#define             _TMHWDDR2031_REGS_RGD_SEEN
#define _RGD_Module_Id_Of_ddr              2031
#define _RGD_Hex_Module_Id_Of_ddr          0x2031
#define _RGD_Module_Name_Of_2031           ddr
typedef struct _tmhwDdrRegs
{
    union _ddrCtl
    {
        struct
        {
            unsigned int haltModeStatus : 1;
            unsigned int haltAutoStatus : 1;
            unsigned int : 14;
            unsigned int haltCtl : 1;
            unsigned int haltAutoOn : 1;
            unsigned int warmStart : 1;
            unsigned int : 9;
            unsigned int dqsPerByte : 1;
            unsigned int dqBusLowHalf : 1;
            unsigned int specAutoPrecharge : 1;
            unsigned int ddrStart : 1;
        } bits;
        unsigned int u32;
    } ddrCtl;
    union _ddrBankSwitch
    {
        struct
        {
            unsigned int : 14;
            unsigned int ddrStride2048 : 1;
            unsigned int ddrStride1024 : 1;
            unsigned int : 12;
            unsigned int bankSwitchColumn : 4;
        } bits;
        unsigned int u32;
    } ddrBankSwitch;
    UInt32 ddrAutoHaltLimit;
    unsigned char filler0000c[0x00010-0x0000c];
    union _ddrRank0AddrLo
    {
        struct
        {
            unsigned int : 4;
            unsigned int rank0AddrLo : 28;
        } bits;
        unsigned int u32;
    } ddrRank0AddrLo;
    union _ddrRank0AddrHi
    {
        struct
        {
            unsigned int : 4;
            unsigned int rank0AddrHi : 28;
        } bits;
        unsigned int u32;
    } ddrRank0AddrHi;
    union _ddrRank1AddrHi
    {
        struct
        {
            unsigned int : 4;
            unsigned int rank1AddrHi : 28;
        } bits;
        unsigned int u32;
    } ddrRank1AddrHi;
    unsigned char filler0001c[0x00040-0x0001c];
    union _ddrRegion1Base
    {
        struct
        {
            unsigned : 4;
            unsigned int region1Base : 12;
            unsigned int : 16;
        } bits;
        unsigned int u32;
    } ddrRegion1Base;
    union _ddrRegion1Mask
    {
        struct
        {
            unsigned : 4;
            unsigned int region1Mask : 12;
            unsigned int : 16;
        } bits;
        unsigned int u32;
    } ddrRegion1Mask;
    union _ddrBankSwitchRegion1
    {
        struct
        {
            unsigned int region1Enab : 1;
            unsigned int : 13;
            unsigned int ddrStride2048 : 1;
            unsigned int ddrStride1024 : 1;
            unsigned int : 12;
            unsigned int bankSwitchColumn : 4;
        } bits;
        unsigned int u32;
    } ddrBankSwitchRegion1;
    unsigned char filler0004c[0x00080-0x0004c];
    union _ddrModeReg
    {
        struct
        {
            unsigned int : 19;
            unsigned int casLatBurstLen : 13;
        } bits;
        unsigned int u32;
    } ddrModeReg;
    union _ddrExtModeReg
    {
        struct
        {
            unsigned int : 19;
            unsigned int ddrExtMode : 13;
        } bits;
        unsigned int u32;
    } ddrExtModeReg;
    union _ddrPrechargeCtl
    {
        struct
        {
            unsigned int : 28;
            unsigned int ddrPrecharge : 4;
        } bits;
        unsigned int u32;
    } ddrPrechargeCtl;
    unsigned char filler0008c[0x000c0-0x0008c];
    union _ddrRank0RowWidth
    {
        struct
        {
            unsigned int : 28;
            unsigned int ddrRowWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank0RowWidth;
    union _ddrRank0ColumnWidth
    {
        struct
        {
            unsigned int : 28;
            unsigned int ddrColumnWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank0ColumnWidth;
    unsigned char filler000c8[0x000d0-0x000c8];
    union _ddrRank1RowWidth
    {
        struct
        {
            unsigned int : 28;
            unsigned int ddrRowWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank1RowWidth;
    union _ddrRank1ColumnWidth
    {
        struct
        {
            unsigned int : 28;
            unsigned int ddrColumnWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank1ColumnWidth;
    unsigned char filler000d8[0x00100-0x000d8];
    union _ddrTimeRasToCas
    {
        struct
        {
            unsigned int : 12;
            unsigned int rasToCasDelay : 4;
            unsigned int : 12;
            unsigned int activeToReadDelay : 4;
        } bits;
        unsigned int u32;
    } ddrTimeRasToCas;
    union _ddrTimeBetweenBankCmds
    {
        struct
        {
            unsigned int : 28;
            unsigned int activeCmdMinTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeBetweenBankCmds;
    union _ddrTimeWriteToRead
    {
        struct
        {
            unsigned int : 28;
            unsigned int writeToReadDelay : 4;
        } bits;
        unsigned int u32;
    } ddrTimeWriteToRead;
    union _ddrTimeWriteRecovery
    {
        struct
        {
            unsigned int : 28;
            unsigned int writeRecoverTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeWriteRecovery;
    union _ddrTimePrecharge
    {
        struct
        {
            unsigned int : 28;
            unsigned int prechargeCmdTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimePrecharge;
    union _ddrTimeActiveToPrecharge
    {
        struct
        {
            unsigned int : 28;
            unsigned int activeToPrechargeDelay : 4;
        } bits;
        unsigned int u32;
    } ddrTimeActiveToPrecharge;
    unsigned char filler00118[0x0011c-0x00118];
    union _ddrTimeBankToBank
    {
        struct
        {
            unsigned int : 28;
            unsigned int activeBankA2BTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeBankToBank;
    union _ddrTimeAutoRefresh
    {
        struct
        {
            unsigned int : 28;
            unsigned int autoRefreshCmdTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeAutoRefresh;
    union _ddrTimeLoadModeReg
    {
        struct
        {
            unsigned int : 28;
            unsigned int loadModeRegTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeLoadModeReg;
    union _ddrTimeCasReadLatency
    {
        struct
        {
            unsigned int : 28;
            unsigned int casReadLatency : 4;
        } bits;
        unsigned int u32;
    } ddrTimeCasReadLatency;
    union _ddrTimeRefreshCycles
    {
        struct
        {
            unsigned int : 16;
            unsigned int refreshCycles : 16;
        } bits;
        unsigned int u32;
    } ddrTimeRefreshCycles;
    unsigned char filler00130[0x00180-0x00130];
    union _ddrArbCtl
    {
        struct
        {
            unsigned int cpuDecrInDmaWin : 1;
#define TMHW_DDR2031_ARBCTL_DFLT         0x00010001
            unsigned int cpuSoftRealTime : 1;
            unsigned int cpuBacklogBuffer : 1;
            unsigned int cpuDynamicRatios : 1;
            unsigned int : 10;
            unsigned int cpuPreemptPolicy : 2;
            unsigned int : 14;
            unsigned int dmaPreemptPolicy : 2;
#define TMHW_DDR2031_PREEMPT_NONE                 0
#define TMHW_DDR2031_PREEMPT_ONSTART              1
#define TMHW_DDR2031_PREEMPT_IMMEDIATE            3
        } bits;
        unsigned int u32;
    } ddrArbCtl;
    union _ddrArbDmaWindow
    {
        struct
        {
            unsigned int : 16;
            unsigned int dmaArbWindow : 16;
#define TMHW_DDR2031_DMAWIN_DFLT         0x0000003F
        } bits;
        unsigned int u32;
    } ddrArbDmaWindow;
    union _ddrArbCpuWindow
    {
        struct
        {
            unsigned int : 16;
            unsigned int cpuArbWindow : 16;
#define TMHW_DDR2031_CPUWIN_DFLT         0x0000003F
        } bits;
        unsigned int u32;
    } ddrArbCpuWindow;
    unsigned char filler0018c[0x001c0-0x0018c];
    struct  {
        union _ddrArbCpuLimit
        {
            struct
            {
                unsigned int cpuVersusDma : 1;
                unsigned int cpuPrioHigh : 1;
                unsigned int : 14;
                unsigned int cpuArbLimit : 16;
#define TMHW_DDR2031_CPULIM_DFLT         0x00007FFF
            } bits;
            unsigned int u32;
        } ddrArbCpuLimit;
        union _ddrArbCpuRatio
        {
            struct
            {
                unsigned int : 16;
                unsigned int cpuArbRatio : 16;
#define TMHW_DDR2031_CPURATIO_DFLT       0x00000004
            } bits;
            unsigned int u32;
        } ddrArbCpuRatio;
        union _ddrArbCpuClip
        {
            struct
            {
                unsigned int : 16;
                unsigned int cpuArbClip : 16;
#define TMHW_DDR2031_CPUCLIP_DFLT        0x0000FFFF
            } bits;
            unsigned int u32;
        } ddrArbCpuClip;
        union _ddrArbCpuDecr
        {
            struct
            {
                unsigned int : 16;
                unsigned int cpuArbDecr : 16;
#define TMHW_DDR2031_CPUDECR_DFLT        0x00000001
#define TMHW_DDR2031_CPU1LIM_DFLT        0x00007FFF
#define TMHW_DDR2031_CPU1RATIO_DFLT      0x00000004
#define TMHW_DDR2031_CPU1CLIP_DFLT       0x0000FFFF
#define TMHW_DDR2031_CPU1DECR_DFLT       0x00000001
#define TMHW_DDR2031_CPU2LIM_DFLT        0x00007FFF
#define TMHW_DDR2031_CPU2RATIO_DFLT      0x00000004
#define TMHW_DDR2031_CPU2CLIP_DFLT       0x0000FFFF
#define TMHW_DDR2031_CPU2DECR_DFLT       0x00000001
#define TMHW_DDR2031_CPU3LIM_DFLT        0x00007FFF
#define TMHW_DDR2031_CPU3RATIO_DFLT      0x00000004
#define TMHW_DDR2031_CPU3CLIP_DFLT       0x0000FFFF
#define TMHW_DDR2031_CPU3DECR_DFLT       0x00000001
            } bits;
            unsigned int u32;
        } ddrArbCpuDecr;
    } tmDdrCpuArb[4] ;
    UInt32 ddrPerfMtl0ReadValid;
    UInt32 ddrPerfMtl0WriteAccept;
    UInt32 ddrPerfMtl1ReadValid;
    UInt32 ddrPerfMtl1WriteAccept;
    UInt32 ddrPerfMtl2ReadValid;
    UInt32 ddrPerfMtl2WriteAccept;
    UInt32 ddrPerfMtl3ReadValid;
    UInt32 ddrPerfMtl3WriteAccept;
    unsigned char filler00220[0x00240-0x00220];
    UInt32 ddrPerfIdleCycles;
    unsigned char filler00244[0x00280-0x00244];
    union _ddrErrValid
    {
        struct
        {
            unsigned int : 31;
            unsigned int ddrErrorValid : 1;
        } bits;
        unsigned int u32;
    } ddrErrValid;
    union _ddrErrMtlPort
    {
        struct
        {
            unsigned int : 30;
            unsigned int ddrMtlPortId : 2;
        } bits;
        unsigned int u32;
    } ddrErrMtlPort;
    UInt32 ddrErrMtlCmdAddr;
    union _ddrErrMtlCmdRead
    {
        struct
        {
            unsigned int : 31;
            unsigned int ddrMtlCmdRead : 1;
        } bits;
        unsigned int u32;
    } ddrErrMtlCmdRead;
    union _ddrErrMtlCmdId
    {
        struct
        {
            unsigned int : 22;
            unsigned int ddrMtlCmdId : 10;
        } bits;
        unsigned int u32;
    } ddrErrMtlCmdId;
    unsigned char filler00294[0x00ffc-0x00294];
    tmBlockId_t BlockId;
} tmhwDdrRegs_NONVOLATILE_t;
typedef volatile tmhwDdrRegs_NONVOLATILE_t *ptmhwDdrRegs_t;

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
void _RGD_CPP_Check_tmhwDdrRegs_Offsets(ptmhwDdrRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwDdrRegs_Offsets(ptmhwDdrRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwDdrRegs, ddrCtl,                        00000000);
    _RGD_Check_Offset(tmhwDdrRegs, ddrBankSwitch,                 00000004);
    _RGD_Check_Offset(tmhwDdrRegs, ddrAutoHaltLimit,              00000008);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0AddrLo,                00000010);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0AddrHi,                00000014);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank1AddrHi,                00000018);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRegion1Base,                00000040);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRegion1Mask,                00000044);
    _RGD_Check_Offset(tmhwDdrRegs, ddrBankSwitchRegion1,          00000048);
    _RGD_Check_Offset(tmhwDdrRegs, ddrModeReg,                    00000080);
    _RGD_Check_Offset(tmhwDdrRegs, ddrExtModeReg,                 00000084);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPrechargeCtl,               00000088);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0RowWidth,              000000c0);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0ColumnWidth,           000000c4);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank1RowWidth,              000000d0);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank1ColumnWidth,           000000d4);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeRasToCas,               00000100);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeBetweenBankCmds,        00000104);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeWriteToRead,            00000108);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeWriteRecovery,          0000010c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimePrecharge,              00000110);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeActiveToPrecharge,      00000114);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeBankToBank,             0000011c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeAutoRefresh,            00000120);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeLoadModeReg,            00000124);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeCasReadLatency,         00000128);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeRefreshCycles,          0000012c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrArbCtl,                     00000180);
    _RGD_Check_Offset(tmhwDdrRegs, ddrArbDmaWindow,               00000184);
    _RGD_Check_Offset(tmhwDdrRegs, ddrArbCpuWindow,               00000188);
    _RGD_Check_Offset(tmhwDdrRegs, tmDdrCpuArb,                   000001c0);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl0ReadValid,          00000200);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl0WriteAccept,        00000204);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl1ReadValid,          00000208);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl1WriteAccept,        0000020c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl2ReadValid,          00000210);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl2WriteAccept,        00000214);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl3ReadValid,          00000218);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl3WriteAccept,        0000021c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfIdleCycles,             00000240);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrValid,                   00000280);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlPort,                 00000284);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlCmdAddr,              00000288);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlCmdRead,              0000028c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlCmdId,                00000290);
    _RGD_Check_Offset(tmhwDdrRegs, BlockId,                       00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWDDR2031_REGS_RGD_SEEN)

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
#if        !defined(_TMHWDDR2031_REGS_RGD_SEEN)
#define             _TMHWDDR2031_REGS_RGD_SEEN
#define _RGD_Module_Id_Of_ddr              2031
#define _RGD_Hex_Module_Id_Of_ddr          0x2031
#define _RGD_Module_Name_Of_2031           ddr
typedef struct _tmhwDdrRegs
{
    union _ddrCtl
    {
        struct
        {
            unsigned int ddrStart : 1;
            unsigned int specAutoPrecharge : 1;
            unsigned int dqBusLowHalf : 1;
            unsigned int dqsPerByte : 1;
            unsigned int : 9;
            unsigned int warmStart : 1;
            unsigned int haltAutoOn : 1;
            unsigned int haltCtl : 1;
            unsigned int : 14;
            unsigned int haltAutoStatus : 1;
            unsigned int haltModeStatus : 1;
        } bits;
        unsigned int u32;
    } ddrCtl;
    union _ddrBankSwitch
    {
        struct
        {
            unsigned int bankSwitchColumn : 4;
            unsigned int : 12;
            unsigned int ddrStride1024 : 1;
            unsigned int ddrStride2048 : 1;
        } bits;
        unsigned int u32;
    } ddrBankSwitch;
    UInt32 ddrAutoHaltLimit;
    unsigned char filler0000c[0x00010-0x0000c];
    union _ddrRank0AddrLo
    {
        struct
        {
            unsigned int rank0AddrLo : 28;
        } bits;
        unsigned int u32;
    } ddrRank0AddrLo;
    union _ddrRank0AddrHi
    {
        struct
        {
            unsigned int rank0AddrHi : 28;
        } bits;
        unsigned int u32;
    } ddrRank0AddrHi;
    union _ddrRank1AddrHi
    {
        struct
        {
            unsigned int rank1AddrHi : 28;
        } bits;
        unsigned int u32;
    } ddrRank1AddrHi;
    unsigned char filler0001c[0x00040-0x0001c];
    union _ddrRegion1Base
    {
        struct
        {
            unsigned int : 16;
            unsigned int region1Base : 12;
        } bits;
        unsigned int u32;
    } ddrRegion1Base;
    union _ddrRegion1Mask
    {
        struct
        {
            unsigned int : 16;
            unsigned int region1Mask : 12;
        } bits;
        unsigned int u32;
    } ddrRegion1Mask;
    union _ddrBankSwitchRegion1
    {
        struct
        {
            unsigned int bankSwitchColumn : 4;
            unsigned int : 12;
            unsigned int ddrStride1024 : 1;
            unsigned int ddrStride2048 : 1;
            unsigned int : 13;
            unsigned int region1Enab : 1;
        } bits;
        unsigned int u32;
    } ddrBankSwitchRegion1;
    unsigned char filler0004c[0x00080-0x0004c];
    union _ddrModeReg
    {
        struct
        {
            unsigned int casLatBurstLen : 13;
        } bits;
        unsigned int u32;
    } ddrModeReg;
    union _ddrExtModeReg
    {
        struct
        {
            unsigned int ddrExtMode : 13;
        } bits;
        unsigned int u32;
    } ddrExtModeReg;
    union _ddrPrechargeCtl
    {
        struct
        {
            unsigned int ddrPrecharge : 4;
        } bits;
        unsigned int u32;
    } ddrPrechargeCtl;
    unsigned char filler0008c[0x000c0-0x0008c];
    union _ddrRank0RowWidth
    {
        struct
        {
            unsigned int ddrRowWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank0RowWidth;
    union _ddrRank0ColumnWidth
    {
        struct
        {
            unsigned int ddrColumnWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank0ColumnWidth;
    unsigned char filler000c8[0x000d0-0x000c8];
    union _ddrRank1RowWidth
    {
        struct
        {
            unsigned int ddrRowWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank1RowWidth;
    union _ddrRank1ColumnWidth
    {
        struct
        {
            unsigned int ddrColumnWidth : 4;
        } bits;
        unsigned int u32;
    } ddrRank1ColumnWidth;
    unsigned char filler000d8[0x00100-0x000d8];
    union _ddrTimeRasToCas
    {
        struct
        {
            unsigned int activeToReadDelay : 4;
            unsigned int : 12;
            unsigned int rasToCasDelay : 4;
        } bits;
        unsigned int u32;
    } ddrTimeRasToCas;
    union _ddrTimeBetweenBankCmds
    {
        struct
        {
            unsigned int activeCmdMinTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeBetweenBankCmds;
    union _ddrTimeWriteToRead
    {
        struct
        {
            unsigned int writeToReadDelay : 4;
        } bits;
        unsigned int u32;
    } ddrTimeWriteToRead;
    union _ddrTimeWriteRecovery
    {
        struct
        {
            unsigned int writeRecoverTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeWriteRecovery;
    union _ddrTimePrecharge
    {
        struct
        {
            unsigned int prechargeCmdTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimePrecharge;
    union _ddrTimeActiveToPrecharge
    {
        struct
        {
            unsigned int activeToPrechargeDelay : 4;
        } bits;
        unsigned int u32;
    } ddrTimeActiveToPrecharge;
    unsigned char filler00118[0x0011c-0x00118];
    union _ddrTimeBankToBank
    {
        struct
        {
            unsigned int activeBankA2BTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeBankToBank;
    union _ddrTimeAutoRefresh
    {
        struct
        {
            unsigned int autoRefreshCmdTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeAutoRefresh;
    union _ddrTimeLoadModeReg
    {
        struct
        {
            unsigned int loadModeRegTime : 4;
        } bits;
        unsigned int u32;
    } ddrTimeLoadModeReg;
    union _ddrTimeCasReadLatency
    {
        struct
        {
            unsigned int casReadLatency : 4;
        } bits;
        unsigned int u32;
    } ddrTimeCasReadLatency;
    union _ddrTimeRefreshCycles
    {
        struct
        {
            unsigned int refreshCycles : 16;
        } bits;
        unsigned int u32;
    } ddrTimeRefreshCycles;
    unsigned char filler00130[0x00180-0x00130];
    union _ddrArbCtl
    {
        struct
        {
            unsigned int dmaPreemptPolicy : 2;
#define TMHW_DDR2031_PREEMPT_NONE                 0
#define TMHW_DDR2031_PREEMPT_ONSTART              1
#define TMHW_DDR2031_PREEMPT_IMMEDIATE            3
            unsigned int : 14;
            unsigned int cpuPreemptPolicy : 2;
            unsigned int : 10;
            unsigned int cpuDynamicRatios : 1;
            unsigned int cpuBacklogBuffer : 1;
            unsigned int cpuSoftRealTime : 1;
            unsigned int cpuDecrInDmaWin : 1;
#define TMHW_DDR2031_ARBCTL_DFLT         0x00010001
        } bits;
        unsigned int u32;
    } ddrArbCtl;
    union _ddrArbDmaWindow
    {
        struct
        {
            unsigned int dmaArbWindow : 16;
#define TMHW_DDR2031_DMAWIN_DFLT         0x0000003F
        } bits;
        unsigned int u32;
    } ddrArbDmaWindow;
    union _ddrArbCpuWindow
    {
        struct
        {
            unsigned int cpuArbWindow : 16;
#define TMHW_DDR2031_CPUWIN_DFLT         0x0000003F
        } bits;
        unsigned int u32;
    } ddrArbCpuWindow;
    unsigned char filler0018c[0x001c0-0x0018c];
    struct  {
        union _ddrArbCpuLimit
        {
            struct
            {
                unsigned int cpuArbLimit : 16;
#define TMHW_DDR2031_CPULIM_DFLT         0x00007FFF
                unsigned int : 14;
                unsigned int cpuPrioHigh : 1;
                unsigned int cpuVersusDma : 1;
            } bits;
            unsigned int u32;
        } ddrArbCpuLimit;
        union _ddrArbCpuRatio
        {
            struct
            {
                unsigned int cpuArbRatio : 16;
#define TMHW_DDR2031_CPURATIO_DFLT       0x00000004
            } bits;
            unsigned int u32;
        } ddrArbCpuRatio;
        union _ddrArbCpuClip
        {
            struct
            {
                unsigned int cpuArbClip : 16;
#define TMHW_DDR2031_CPUCLIP_DFLT        0x0000FFFF
            } bits;
            unsigned int u32;
        } ddrArbCpuClip;
        union _ddrArbCpuDecr
        {
            struct
            {
                unsigned int cpuArbDecr : 16;
#define TMHW_DDR2031_CPUDECR_DFLT        0x00000001
#define TMHW_DDR2031_CPU1LIM_DFLT        0x00007FFF
#define TMHW_DDR2031_CPU1RATIO_DFLT      0x00000004
#define TMHW_DDR2031_CPU1CLIP_DFLT       0x0000FFFF
#define TMHW_DDR2031_CPU1DECR_DFLT       0x00000001
#define TMHW_DDR2031_CPU2LIM_DFLT        0x00007FFF
#define TMHW_DDR2031_CPU2RATIO_DFLT      0x00000004
#define TMHW_DDR2031_CPU2CLIP_DFLT       0x0000FFFF
#define TMHW_DDR2031_CPU2DECR_DFLT       0x00000001
#define TMHW_DDR2031_CPU3LIM_DFLT        0x00007FFF
#define TMHW_DDR2031_CPU3RATIO_DFLT      0x00000004
#define TMHW_DDR2031_CPU3CLIP_DFLT       0x0000FFFF
#define TMHW_DDR2031_CPU3DECR_DFLT       0x00000001
            } bits;
            unsigned int u32;
        } ddrArbCpuDecr;
    } tmDdrCpuArb[4] ;
    UInt32 ddrPerfMtl0ReadValid;
    UInt32 ddrPerfMtl0WriteAccept;
    UInt32 ddrPerfMtl1ReadValid;
    UInt32 ddrPerfMtl1WriteAccept;
    UInt32 ddrPerfMtl2ReadValid;
    UInt32 ddrPerfMtl2WriteAccept;
    UInt32 ddrPerfMtl3ReadValid;
    UInt32 ddrPerfMtl3WriteAccept;
    unsigned char filler00220[0x00240-0x00220];
    UInt32 ddrPerfIdleCycles;
    unsigned char filler00244[0x00280-0x00244];
    union _ddrErrValid
    {
        struct
        {
            unsigned int ddrErrorValid : 1;
        } bits;
        unsigned int u32;
    } ddrErrValid;
    union _ddrErrMtlPort
    {
        struct
        {
            unsigned int ddrMtlPortId : 2;
        } bits;
        unsigned int u32;
    } ddrErrMtlPort;
    UInt32 ddrErrMtlCmdAddr;
    union _ddrErrMtlCmdRead
    {
        struct
        {
            unsigned int ddrMtlCmdRead : 1;
        } bits;
        unsigned int u32;
    } ddrErrMtlCmdRead;
    union _ddrErrMtlCmdId
    {
        struct
        {
            unsigned int ddrMtlCmdId : 10;
        } bits;
        unsigned int u32;
    } ddrErrMtlCmdId;
    unsigned char filler00294[0x00ffc-0x00294];
    tmBlockId_t BlockId;
} tmhwDdrRegs_NONVOLATILE_t;
typedef volatile tmhwDdrRegs_NONVOLATILE_t *ptmhwDdrRegs_t;

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
void _RGD_CPP_Check_tmhwDdrRegs_Offsets(ptmhwDdrRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwDdrRegs_Offsets(ptmhwDdrRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwDdrRegs, ddrCtl,                        00000000);
    _RGD_Check_Offset(tmhwDdrRegs, ddrBankSwitch,                 00000004);
    _RGD_Check_Offset(tmhwDdrRegs, ddrAutoHaltLimit,              00000008);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0AddrLo,                00000010);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0AddrHi,                00000014);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank1AddrHi,                00000018);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRegion1Base,                00000040);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRegion1Mask,                00000044);
    _RGD_Check_Offset(tmhwDdrRegs, ddrBankSwitchRegion1,          00000048);
    _RGD_Check_Offset(tmhwDdrRegs, ddrModeReg,                    00000080);
    _RGD_Check_Offset(tmhwDdrRegs, ddrExtModeReg,                 00000084);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPrechargeCtl,               00000088);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0RowWidth,              000000c0);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank0ColumnWidth,           000000c4);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank1RowWidth,              000000d0);
    _RGD_Check_Offset(tmhwDdrRegs, ddrRank1ColumnWidth,           000000d4);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeRasToCas,               00000100);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeBetweenBankCmds,        00000104);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeWriteToRead,            00000108);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeWriteRecovery,          0000010c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimePrecharge,              00000110);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeActiveToPrecharge,      00000114);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeBankToBank,             0000011c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeAutoRefresh,            00000120);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeLoadModeReg,            00000124);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeCasReadLatency,         00000128);
    _RGD_Check_Offset(tmhwDdrRegs, ddrTimeRefreshCycles,          0000012c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrArbCtl,                     00000180);
    _RGD_Check_Offset(tmhwDdrRegs, ddrArbDmaWindow,               00000184);
    _RGD_Check_Offset(tmhwDdrRegs, ddrArbCpuWindow,               00000188);
    _RGD_Check_Offset(tmhwDdrRegs, tmDdrCpuArb,                   000001c0);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl0ReadValid,          00000200);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl0WriteAccept,        00000204);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl1ReadValid,          00000208);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl1WriteAccept,        0000020c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl2ReadValid,          00000210);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl2WriteAccept,        00000214);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl3ReadValid,          00000218);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfMtl3WriteAccept,        0000021c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrPerfIdleCycles,             00000240);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrValid,                   00000280);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlPort,                 00000284);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlCmdAddr,              00000288);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlCmdRead,              0000028c);
    _RGD_Check_Offset(tmhwDdrRegs, ddrErrMtlCmdId,                00000290);
    _RGD_Check_Offset(tmhwDdrRegs, BlockId,                       00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWDDR2031_REGS_RGD_SEEN)

#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
