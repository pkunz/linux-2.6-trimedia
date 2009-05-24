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
#if        !defined(_TMHWCLOCKA063_RGD_SEEN)
#define             _TMHWCLOCKA063_RGD_SEEN
#define _RGD_Module_Id_Of_Clk              A063
#define _RGD_Hex_Module_Id_Of_Clk          0xA063
#define _RGD_Module_Name_Of_A063           Clk
typedef struct _tmhwClockRegs
{
    union _clkPllCtrl0
    {
        struct
        {
            unsigned int : 2;
            unsigned int AckOff : 1;
            unsigned int pllLck : 1;
            unsigned int pllAdj : 4;
            unsigned int : 3;
            unsigned int paramN : 9;
            unsigned int : 2;
            unsigned int paramM : 6;
            unsigned int paramP : 2;
            unsigned int pwrDown : 1;
            unsigned int bypass : 1;
        } bits;
        unsigned int u32;
    } clkPllCtrl0;
    union _clkPllCtrl1
    {
        struct
        {
            unsigned int : 2;
            unsigned int AckOff : 1;
            unsigned int pllLck : 1;
            unsigned int pllAdj : 4;
            unsigned int : 3;
            unsigned int paramN : 9;
            unsigned int : 2;
            unsigned int paramM : 6;
            unsigned int paramP : 2;
            unsigned int pwrDown : 1;
            unsigned int bypass : 1;
        } bits;
        unsigned int u32;
    } clkPllCtrl1;
    union _clkPllCtrl2
    {
        struct
        {
            unsigned int : 2;
            unsigned int AckOff : 1;
            unsigned int pllLck : 1;
            unsigned int pllAdj : 4;
            unsigned int : 3;
            unsigned int paramN : 9;
            unsigned int : 2;
            unsigned int paramM : 6;
            unsigned int : 2;
            unsigned int pwrDown : 1;
            unsigned int bypass : 1;
        } bits;
        unsigned int u32;
    } clkPllCtrl2;
    union _clkPll17GHzCtrl
    {
        struct
        {
            unsigned : 29;
            unsigned int pwrDown : 1;
            unsigned int : 2;
        } bits;
        unsigned int u32;
    } pll17GHzCtrl;
    UInt32 ddsCtrl[9];
    union _clkPwrDownCtrl
    {
        struct
        {
            unsigned int : 23;
            unsigned int pd_192 : 1;
            unsigned int pd_144 : 1;
            unsigned int pd_123 : 1;
            unsigned int pd_108 : 1;
            unsigned int pd_96 : 1;
            unsigned int pd_86 : 1;
            unsigned int pd_78 : 1;
            unsigned int pd_66 : 1;
            unsigned int pd_58 : 1;
        } bits;
        unsigned int u32;
    } pwrDownCtrl;
    unsigned char filler00038[0x00100-0x00038];
    union  {
        UInt32 clockCtlRegsU32[259];
#define CLK_TM_CTL_OFFSET                     0x100
#define CLK_MEM_CTL_OFFSET                    0x104
#define CLK_D2D_CTL_OFFSET                    0x108
#define CLK_PCI_CTL_OFFSET                    0x10C
#define CLK_MBS_CTL_OFFSET                    0x110
#define CLK_TSTAMP_CTL_OFFSET                 0x114
#define CLK_LAN_CTL_OFFSET                    0x118
#define CLK_LAN_RX_CTL_OFFSET                 0x11C
#define CLK_LAN_TX_CTL_OFFSET                 0x120
#define CLK_IIC_CTL_OFFSET                    0x124
#define CLK_DVDD_CTL_OFFSET                   0x128
#define CLK_DTL_MMIO_CTL_OFFSET               0x12C
#define CLK_QVCP_OUT_CTL_OFFSET               0x200
#define CLK_QVCP_PIX_CTL_OFFSET               0x204
#define CLK_QVCP_PROC_CTL_OFFSET              0x208
#define CLK_LCD_TSTAMP_CTL_OFFSET             0x20C
#define CLK_VIP_CTL_OFFSET                    0x210
#define CLK_VLD_CTL_OFFSET                    0x214
#define CLK_AIOSCLK_CTL_OFFSET                0x300
#define CLK_AISCK_CTL_OFFSET                  0x304
#define CLK_AOOSCLK_CTL_OFFSET                0x308
#define CLK_AOSCK_CTL_OFFSET                  0x30C
#define CLK_SPDO_CTL_OFFSET                   0x310
#define CLK_SPDI_CTL_OFFSET                   0x314
#define CLK_GPIOQ4_CTL_OFFSET                 0x400
#define CLK_GPIOQ5_CTL_OFFSET                 0x404
#define CLK_GPIOQ6Q12_CTL_OFFSET              0x408
#define CLK_GPIOQ13_CTL_OFFSET                0x40C
#define CLK_GPIOQ14_CTL_OFFSET                0x410
#define CLK_FGPO_CTL_OFFSET                   0x414
#define CLK_FGPI_CTL_OFFSET                   0x418
#define CLK_STRETCHER_CTL_OFFSET              0x500
#define CLK_WAKEUP_CTL_OFFSET                 0x504
#define CLK_BS0                                   0
#define CLK_BS1                                   1
#define CLK_BS3                                   3
#define CLK_BS4                                   4
#define CLK_BC1                                   1
#define CLK_BC2                                   2
#define CLK_BC3                                   3
#define CLK_BC4                                   4
#define CLK_BC5                                   5
        struct  {
            union _clkTmCtl
            {
                struct
                {
                    unsigned int : 27;
                    unsigned int clockNoStretch : 1;
                    unsigned int clockPowerdown : 1;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkTmCtl;
            union _clkMemCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkMemCtl;
            union _clkD2dCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockFuncSelect : 3;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkD2dCtl;
            union _clkPciCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkPciCtl;
            union _clkMbsCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockFuncSelect : 3;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkMbsCtl;
            union _clkTstampCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkTstampCtl;
            union _clkLanCtl
            {
                struct
                {
                    unsigned int : 27;
                    unsigned int clockFuncSelect : 2;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkLanCtl;
            union _clkLanRxCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkLanRxCtl;
            union _clkLanTxCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkLanTxCtl;
            union _clkIicCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkIicCtl;
            union _clkDvddCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockFuncSelect : 3;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkDvddCtl;
            union _clkDtlMmioCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockFuncSelect : 3;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkDtlMmioCtl;
            unsigned char filler00130[0x00200-0x00130];
            union _clkQvcpOutCtl
            {
                struct
                {
                    unsigned int : 28;
                    unsigned int clockOutEna : 1;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkQvcpOutCtl;
            union _clkQvcpPixCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockFuncSelect : 3;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkQvcpPixCtl;
            union _clkQvcpProcCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockFuncSelect : 3;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkQvcpProcCtl;
            union _clkLcdTstampCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkLcdTstampCtl;
            union _clkVipCtl
            {
                struct
                {
                    unsigned int : 28;
                    unsigned int clockOutEna : 1;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkVipCtl;
            union _clkVldCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockFuncSelect : 3;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkVldCtl;
            unsigned char filler00218[0x00300-0x00218];
            union _clkAiOsclkCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkAiOsclkCtl;
            union _clkAiSckCtl
            {
                struct
                {
                    unsigned int : 30;
                    unsigned int clockSelect : 1;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkAiSckCtl;
            union _clkAoOsclkCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkAoOsclkCtl;
            union _clkAoSckCtl
            {
                struct
                {
                    unsigned int : 30;
                    unsigned int clockSelect : 1;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkAoSckCtl;
            union _clkSpdoCtl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkSpdoCtl;
            union _clkSpdiCtl
            {
                struct
                {
                    unsigned int : 28;
                    unsigned int clockFuncSelect : 1;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkSpdiCtl;
            unsigned char filler00318[0x00400-0x00318];
            union _clkGpioQ4Ctl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkGpioQ4Ctl;
            union _clkGpioQ5Ctl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkGpioQ5Ctl;
            union _clkGpioQ6Q12Ctl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkGpioQ6Q12Ctl;
            union _clkGpioQ13Ctl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkGpioQ13Ctl;
            union _clkGpioQ14Ctl
            {
                struct
                {
                    unsigned int : 29;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkGpioQ14Ctl;
            unsigned char filler00414[0x00418-0x00414];
            union _clkFgpoCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockOutEna : 1;
                    unsigned int clockFuncSelect : 2;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkFgpoCtl;
            union _clkFgpiCtl
            {
                struct
                {
                    unsigned int : 26;
                    unsigned int clockOutEna : 1;
                    unsigned int clockFuncSelect : 2;
                    unsigned int clockSelect : 2;
                    unsigned int en : 1;
                } bits;
                unsigned int u32;
            } clkFgpiCtl;
            unsigned char filler00420[0x00500-0x00420];
            union _clkStretcherCtl
            {
                struct
                {
                    unsigned int count : 32;
                } bits;
                unsigned int u32;
            } clkStretcherCtl;
            union _clkWakeUpCtl
            {
                struct
                {
                    unsigned int count : 30;
                    unsigned int extEnable : 1;
                    unsigned int gpioEnable : 1;
                } bits;
                unsigned int u32;
            } clkWakeUpCtl;
        } clkCtls ;
    } clkOutCtl ;
    unsigned char filler0050c[0x00fe0-0x0050c];
    union _clkIntStatus
    {
        struct
        {
            unsigned int voClk2Pres : 1;
            unsigned int viClk2Pres : 1;
            unsigned int aoSckClkPres : 1;
            unsigned int aiSckClkPres : 1;
            unsigned int viClk1Pres : 1;
            unsigned int : 22;
            unsigned int voClk2Int : 1;
            unsigned int viClk2Int : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int viClk1Int : 1;
        } bits;
        unsigned int u32;
    } clkIntStatus;
    union _clkIntEnable
    {
        struct
        {
            unsigned int : 27;
            unsigned int voClk2Int : 1;
            unsigned int viClk2Int : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int viClk1Int : 1;
        } bits;
        unsigned int u32;
    } clkIntEnable;
    union _clkIntClear
    {
        struct
        {
            unsigned int : 27;
            unsigned int voClk2Int : 1;
            unsigned int viClk2Int : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int viClk1Int : 1;
        } bits;
        unsigned int u32;
    } clkIntClear;
    union _clkIntSet
    {
        struct
        {
            unsigned int : 27;
            unsigned int voClk2Int : 1;
            unsigned int viClk2Int : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int viClk1Int : 1;
        } bits;
        unsigned int u32;
    } clkIntSet;
} tmhwClockRegs_NONVOLATILE_t;
typedef volatile tmhwClockRegs_NONVOLATILE_t *ptmhwClockRegs_t;

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
void _RGD_CPP_Check_tmhwClockRegs_Offsets(ptmhwClockRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwClockRegs_Offsets(ptmhwClockRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwClockRegs, clkPllCtrl0,                   00000000);
    _RGD_Check_Offset(tmhwClockRegs, clkPllCtrl1,                   00000004);
    _RGD_Check_Offset(tmhwClockRegs, clkPllCtrl2,                   00000008);
    _RGD_Check_Offset(tmhwClockRegs, pll17GHzCtrl,                  0000000c);
    _RGD_Check_Offset(tmhwClockRegs, ddsCtrl,                       00000010);
    _RGD_Check_Offset(tmhwClockRegs, pwrDownCtrl,                   00000034);
    _RGD_Check_Offset(tmhwClockRegs, clkOutCtl,                     00000100);
    _RGD_Check_Offset(tmhwClockRegs, clkIntStatus,                  00000fe0);
    _RGD_Check_Offset(tmhwClockRegs, clkIntEnable,                  00000fe4);
    _RGD_Check_Offset(tmhwClockRegs, clkIntClear,                   00000fe8);
    _RGD_Check_Offset(tmhwClockRegs, clkIntSet,                     00000fec);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWCLOCKA063_RGD_SEEN)

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
#if        !defined(_TMHWCLOCKA063_RGD_SEEN)
#define             _TMHWCLOCKA063_RGD_SEEN
#define _RGD_Module_Id_Of_Clk              A063
#define _RGD_Hex_Module_Id_Of_Clk          0xA063
#define _RGD_Module_Name_Of_A063           Clk
typedef struct _tmhwClockRegs
{
    union _clkPllCtrl0
    {
        struct
        {
            unsigned int bypass : 1;
            unsigned int pwrDown : 1;
            unsigned int paramP : 2;
            unsigned int paramM : 6;
            unsigned int : 2;
            unsigned int paramN : 9;
            unsigned int : 3;
            unsigned int pllAdj : 4;
            unsigned int pllLck : 1;
            unsigned int AckOff : 1;
        } bits;
        unsigned int u32;
    } clkPllCtrl0;
    union _clkPllCtrl1
    {
        struct
        {
            unsigned int bypass : 1;
            unsigned int pwrDown : 1;
            unsigned int paramP : 2;
            unsigned int paramM : 6;
            unsigned int : 2;
            unsigned int paramN : 9;
            unsigned int : 3;
            unsigned int pllAdj : 4;
            unsigned int pllLck : 1;
            unsigned int AckOff : 1;
        } bits;
        unsigned int u32;
    } clkPllCtrl1;
    union _clkPllCtrl2
    {
        struct
        {
            unsigned int bypass : 1;
            unsigned int pwrDown : 1;
            unsigned int : 2;
            unsigned int paramM : 6;
            unsigned int : 2;
            unsigned int paramN : 9;
            unsigned int : 3;
            unsigned int pllAdj : 4;
            unsigned int pllLck : 1;
            unsigned int AckOff : 1;
        } bits;
        unsigned int u32;
    } clkPllCtrl2;
    union _clkPll17GHzCtrl
    {
        struct
        {
            unsigned int : 2;
            unsigned int pwrDown : 1;
        } bits;
        unsigned int u32;
    } pll17GHzCtrl;
    UInt32 ddsCtrl[9];
    union _clkPwrDownCtrl
    {
        struct
        {
            unsigned int pd_58 : 1;
            unsigned int pd_66 : 1;
            unsigned int pd_78 : 1;
            unsigned int pd_86 : 1;
            unsigned int pd_96 : 1;
            unsigned int pd_108 : 1;
            unsigned int pd_123 : 1;
            unsigned int pd_144 : 1;
            unsigned int pd_192 : 1;
        } bits;
        unsigned int u32;
    } pwrDownCtrl;
    unsigned char filler00038[0x00100-0x00038];
    union  {
        UInt32 clockCtlRegsU32[259];
#define CLK_TM_CTL_OFFSET                     0x100
#define CLK_MEM_CTL_OFFSET                    0x104
#define CLK_D2D_CTL_OFFSET                    0x108
#define CLK_PCI_CTL_OFFSET                    0x10C
#define CLK_MBS_CTL_OFFSET                    0x110
#define CLK_TSTAMP_CTL_OFFSET                 0x114
#define CLK_LAN_CTL_OFFSET                    0x118
#define CLK_LAN_RX_CTL_OFFSET                 0x11C
#define CLK_LAN_TX_CTL_OFFSET                 0x120
#define CLK_IIC_CTL_OFFSET                    0x124
#define CLK_DVDD_CTL_OFFSET                   0x128
#define CLK_DTL_MMIO_CTL_OFFSET               0x12C
#define CLK_QVCP_OUT_CTL_OFFSET               0x200
#define CLK_QVCP_PIX_CTL_OFFSET               0x204
#define CLK_QVCP_PROC_CTL_OFFSET              0x208
#define CLK_LCD_TSTAMP_CTL_OFFSET             0x20C
#define CLK_VIP_CTL_OFFSET                    0x210
#define CLK_VLD_CTL_OFFSET                    0x214
#define CLK_AIOSCLK_CTL_OFFSET                0x300
#define CLK_AISCK_CTL_OFFSET                  0x304
#define CLK_AOOSCLK_CTL_OFFSET                0x308
#define CLK_AOSCK_CTL_OFFSET                  0x30C
#define CLK_SPDO_CTL_OFFSET                   0x310
#define CLK_SPDI_CTL_OFFSET                   0x314
#define CLK_GPIOQ4_CTL_OFFSET                 0x400
#define CLK_GPIOQ5_CTL_OFFSET                 0x404
#define CLK_GPIOQ6Q12_CTL_OFFSET              0x408
#define CLK_GPIOQ13_CTL_OFFSET                0x40C
#define CLK_GPIOQ14_CTL_OFFSET                0x410
#define CLK_FGPO_CTL_OFFSET                   0x414
#define CLK_FGPI_CTL_OFFSET                   0x418
#define CLK_STRETCHER_CTL_OFFSET              0x500
#define CLK_WAKEUP_CTL_OFFSET                 0x504
#define CLK_BS0                                   0
#define CLK_BS1                                   1
#define CLK_BS3                                   3
#define CLK_BS4                                   4
#define CLK_BC1                                   1
#define CLK_BC2                                   2
#define CLK_BC3                                   3
#define CLK_BC4                                   4
#define CLK_BC5                                   5
        struct  {
            union _clkTmCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockPowerdown : 1;
                    unsigned int clockNoStretch : 1;
                } bits;
                unsigned int u32;
            } clkTmCtl;
            union _clkMemCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkMemCtl;
            union _clkD2dCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 3;
                } bits;
                unsigned int u32;
            } clkD2dCtl;
            union _clkPciCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkPciCtl;
            union _clkMbsCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 3;
                } bits;
                unsigned int u32;
            } clkMbsCtl;
            union _clkTstampCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkTstampCtl;
            union _clkLanCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 2;
                } bits;
                unsigned int u32;
            } clkLanCtl;
            union _clkLanRxCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkLanRxCtl;
            union _clkLanTxCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkLanTxCtl;
            union _clkIicCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkIicCtl;
            union _clkDvddCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 3;
                } bits;
                unsigned int u32;
            } clkDvddCtl;
            union _clkDtlMmioCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 3;
                } bits;
                unsigned int u32;
            } clkDtlMmioCtl;
            unsigned char filler00130[0x00200-0x00130];
            union _clkQvcpOutCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockOutEna : 1;
                } bits;
                unsigned int u32;
            } clkQvcpOutCtl;
            union _clkQvcpPixCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 3;
                } bits;
                unsigned int u32;
            } clkQvcpPixCtl;
            union _clkQvcpProcCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 3;
                } bits;
                unsigned int u32;
            } clkQvcpProcCtl;
            union _clkLcdTstampCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkLcdTstampCtl;
            union _clkVipCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockOutEna : 1;
                } bits;
                unsigned int u32;
            } clkVipCtl;
            union _clkVldCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 3;
                } bits;
                unsigned int u32;
            } clkVldCtl;
            unsigned char filler00218[0x00300-0x00218];
            union _clkAiOsclkCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkAiOsclkCtl;
            union _clkAiSckCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 1;
                } bits;
                unsigned int u32;
            } clkAiSckCtl;
            union _clkAoOsclkCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkAoOsclkCtl;
            union _clkAoSckCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 1;
                } bits;
                unsigned int u32;
            } clkAoSckCtl;
            union _clkSpdoCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkSpdoCtl;
            union _clkSpdiCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 1;
                } bits;
                unsigned int u32;
            } clkSpdiCtl;
            unsigned char filler00318[0x00400-0x00318];
            union _clkGpioQ4Ctl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkGpioQ4Ctl;
            union _clkGpioQ5Ctl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkGpioQ5Ctl;
            union _clkGpioQ6Q12Ctl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkGpioQ6Q12Ctl;
            union _clkGpioQ13Ctl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkGpioQ13Ctl;
            union _clkGpioQ14Ctl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                } bits;
                unsigned int u32;
            } clkGpioQ14Ctl;
            unsigned char filler00414[0x00418-0x00414];
            union _clkFgpoCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 2;
                    unsigned int clockOutEna : 1;
                } bits;
                unsigned int u32;
            } clkFgpoCtl;
            union _clkFgpiCtl
            {
                struct
                {
                    unsigned int en : 1;
                    unsigned int clockSelect : 2;
                    unsigned int clockFuncSelect : 2;
                    unsigned int clockOutEna : 1;
                } bits;
                unsigned int u32;
            } clkFgpiCtl;
            unsigned char filler00420[0x00500-0x00420];
            union _clkStretcherCtl
            {
                struct
                {
                    unsigned int count : 32;
                } bits;
                unsigned int u32;
            } clkStretcherCtl;
            union _clkWakeUpCtl
            {
                struct
                {
                    unsigned int gpioEnable : 1;
                    unsigned int extEnable : 1;
                    unsigned int count : 30;
                } bits;
                unsigned int u32;
            } clkWakeUpCtl;
        } clkCtls ;
    } clkOutCtl ;
    unsigned char filler0050c[0x00fe0-0x0050c];
    union _clkIntStatus
    {
        struct
        {
            unsigned int viClk1Int : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int viClk2Int : 1;
            unsigned int voClk2Int : 1;
            unsigned int : 22;
            unsigned int viClk1Pres : 1;
            unsigned int aiSckClkPres : 1;
            unsigned int aoSckClkPres : 1;
            unsigned int viClk2Pres : 1;
            unsigned int voClk2Pres : 1;
        } bits;
        unsigned int u32;
    } clkIntStatus;
    union _clkIntEnable
    {
        struct
        {
            unsigned int viClk1Int : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int viClk2Int : 1;
            unsigned int voClk2Int : 1;
        } bits;
        unsigned int u32;
    } clkIntEnable;
    union _clkIntClear
    {
        struct
        {
            unsigned int viClk1Int : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int viClk2Int : 1;
            unsigned int voClk2Int : 1;
        } bits;
        unsigned int u32;
    } clkIntClear;
    union _clkIntSet
    {
        struct
        {
            unsigned int viClk1Int : 1;
            unsigned int aiSckClkInt : 1;
            unsigned int aoSckClkInt : 1;
            unsigned int viClk2Int : 1;
            unsigned int voClk2Int : 1;
        } bits;
        unsigned int u32;
    } clkIntSet;
} tmhwClockRegs_NONVOLATILE_t;
typedef volatile tmhwClockRegs_NONVOLATILE_t *ptmhwClockRegs_t;

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
void _RGD_CPP_Check_tmhwClockRegs_Offsets(ptmhwClockRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwClockRegs_Offsets(ptmhwClockRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwClockRegs, clkPllCtrl0,                   00000000);
    _RGD_Check_Offset(tmhwClockRegs, clkPllCtrl1,                   00000004);
    _RGD_Check_Offset(tmhwClockRegs, clkPllCtrl2,                   00000008);
    _RGD_Check_Offset(tmhwClockRegs, pll17GHzCtrl,                  0000000c);
    _RGD_Check_Offset(tmhwClockRegs, ddsCtrl,                       00000010);
    _RGD_Check_Offset(tmhwClockRegs, pwrDownCtrl,                   00000034);
    _RGD_Check_Offset(tmhwClockRegs, clkOutCtl,                     00000100);
    _RGD_Check_Offset(tmhwClockRegs, clkIntStatus,                  00000fe0);
    _RGD_Check_Offset(tmhwClockRegs, clkIntEnable,                  00000fe4);
    _RGD_Check_Offset(tmhwClockRegs, clkIntClear,                   00000fe8);
    _RGD_Check_Offset(tmhwClockRegs, clkIntSet,                     00000fec);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWCLOCKA063_RGD_SEEN)

#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
