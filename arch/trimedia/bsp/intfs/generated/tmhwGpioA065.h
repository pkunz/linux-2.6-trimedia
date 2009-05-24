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
#if        !defined(_TMHWGPIO010F_RGD_SEEN)
#define             _TMHWGPIO010F_RGD_SEEN
#define _RGD_Module_Id_Of_Gpio             010F
#define _RGD_Hex_Module_Id_Of_Gpio         0x010F
#define _RGD_Module_Name_Of_010F           Gpio
typedef struct _tmhwGpioRegs
{
    UInt32 mode[4];
#define GPIO_MODE_RETAIN                          0
#define GPIO_MODE_PRIMARY_FUNCTION                1
#define GPIO_MODE_IO_FUNCTION                     2
#define GPIO_MODE_OPEN_DRAIN                      3
#define GPIO_MODE_BITSIZE                         2
    union _gpioCtrl
    {
        struct
        {
            unsigned int mask : 16;
            unsigned int data : 16;
        } bits;
        unsigned int u32;
    } ctrl[4];
    union _gpioIntSignal
    {
        struct
        {
            unsigned int : 20;
            unsigned int lastWordQ3 : 1;
            unsigned int lastWordQ2 : 1;
            unsigned int lastWordQ1 : 1;
            unsigned int lastWordQ0 : 1;
            unsigned int vip1EowVbi : 1;
            unsigned int vip1EowVid : 1;
            unsigned int spdiTstamp2 : 1;
            unsigned int spdiTstamp1 : 1;
            unsigned int spdoTstamp : 1;
            unsigned int ai1Tstamp : 1;
            unsigned int ao1Tstamp : 1;
            unsigned int qvcpTstamp : 1;
        } bits;
        unsigned int u32;
    } intSignal;
    union _gpioEventFifo
    {
        struct
        {
            unsigned int : 1;
            unsigned int enEvTStamp : 1;
#define GPIO_EV_TSTAMP_DISABLE                    0
#define GPIO_EV_TSTAMP_ENABLE                     1
            unsigned int enIRCarrier : 1;
#define GPIO_IR_CARRIER_DISABLE                   0
#define GPIO_IR_CARRIER_ENABLE                    1
            unsigned int enIRFilter : 1;
#define GPIO_IR_FILTER_DISABLE                    0
#define GPIO_IR_FILTER_ENABLE                     1
            unsigned int enClockSel : 2;
#define GPIO_CLOCK_SEL_DISABLE                    0
#define GPIO_CLOCK_SEL_POS_EDGE                   1
#define GPIO_CLOCK_SEL_NEG_EDGE                   3
            unsigned int enPatGenClk : 2;
#define GPIO_DISABLE_PAT_GEN_CLK                  0
#define GPIO_ENABLE_PAT_GEN_CLK                   1
#define GPIO_ENABLE_PAT_GEN_CLK_INV               3
            unsigned int unused : 2;
            unsigned int enDDSSource : 1;
            unsigned int srcClkSel : 3;
#define GPIO_SOURCE_GPIO_0                        0
#define GPIO_SOURCE_GPIO_1                        1
#define GPIO_SOURCE_GPIO_2                        2
#define GPIO_SOURCE_GPIO_3                        3
#define GPIO_SOURCE_GPIO_4                        4
#define GPIO_SOURCE_GPIO_5                        5
#define GPIO_SOURCE_GPIO_6                        6
            unsigned int enIoSel : 2;
#define GPIO_SAMPLE_1_BIT                         0
#define GPIO_SAMPLE_2_BIT                         1
#define GPIO_SAMPLE_4_BIT                         2
            unsigned int interval : 12;
            unsigned int eventMode : 2;
#define GPIO_EVENT_EDGE_NONE                      0
#define GPIO_EVENT_EDGE_NEG                       1
#define GPIO_EVENT_EDGE_POS                       2
#define GPIO_EVENT_EDGE_EITHER                    3
            unsigned int fifoMode : 2;
#define GPIO_EVENT_FIFO_TIMESTAMP                 0
#define GPIO_EVENT_FIFO_SIG_SAMPLE                1
#define GPIO_EVENT_FIFO_PATGEN_TS                 2
#define GPIO_EVENT_FIFO_PATGEN_SPL                3
        } bits;
        unsigned int u32;
    } eventFifo[4];
    union _gpioEventTsu
    {
        struct
        {
            unsigned int : 22;
            unsigned int sourceSel : 8;
            unsigned int eventMode : 2;
        } bits;
        unsigned int u32;
    } eventTsu[12];
    union _gpioPinSel
    {
        struct
        {
            unsigned int ioSel3 : 8;
            unsigned int ioSel2 : 8;
            unsigned int ioSel1 : 8;
            unsigned int ioSel0 : 8;
        } bits;
        unsigned int u32;
    } pinSel[4];
    union _gpioPgBufCtrl
    {
        struct
        {
            unsigned int : 14;
            unsigned int bufLen : 18;
        } bits;
        unsigned int u32;
    } pgBufCtrl[4];
    UInt32 basePtr[2][4];
#define GPIO_DMA_BUF_1                            0
#define GPIO_DMA_BUF_2                            1
    union _gpioBufSize
    {
        struct
        {
            unsigned int : 18;
            unsigned int size : 14;
        } bits;
        unsigned int u32;
    } bufSize[4];
    union  {
        union _gpioPatGen
        {
            struct
            {
                unsigned int : 9;
                unsigned int dutyCycle : 2;
#define GPIO_SUBC_DUTY_33                         0
#define GPIO_SUBC_DUTY_50                         1
#define GPIO_SUBC_DUTY_66                         2
                unsigned int carrDiv : 5;
                unsigned int freqDiv : 16;
            } bits;
            unsigned int u32;
        } patGen;
        union _gpioSampling
        {
            struct
            {
                unsigned int : 13;
                unsigned int irFilter : 3;
                unsigned int freqDiv : 16;
            } bits;
            unsigned int u32;
        } sampling;
    } divider[4] ;
    union _gpioTsUnit
    {
        struct
        {
            unsigned int direction : 1;
#define GPIO_EDGE_RISING                          0
#define GPIO_EDGE_FALLING                         1
            unsigned int counter : 31;
        } bits;
        unsigned int u32;
    } tsUnit[12];
    union _gpioTimeCtrl
    {
        struct
        {
            unsigned int : 1;
            unsigned int timeCounter : 31;
        } bits;
        unsigned int u32;
    } TimeCtrl;
    union _gpioTimerIOSel
    {
        struct
        {
            unsigned int : 16;
            unsigned int ioSel1 : 8;
            unsigned int ioSel0 : 8;
        } bits;
        unsigned int u32;
    } timerIOSel;
    union _gpioVicIntStat
    {
        struct
        {
            unsigned int : 27;
            unsigned int tsuORed : 1;
            unsigned int fifoQueue3 : 1;
            unsigned int fifoQueue2 : 1;
            unsigned int fifoQueue1 : 1;
            unsigned int fifoQueue0 : 1;
        } bits;
        unsigned int u32;
    } vicIntStat;
    union _gpioDdsOutSel
    {
        struct
        {
            unsigned int : 29;
            unsigned int enDdsOutOnGpioPin14 : 1;
            unsigned int enDdsOutOnGpioPin13 : 1;
            unsigned int enDdsOutOnGpioPin12 : 1;
        } bits;
        unsigned int u32;
    } ddsOutSel;
    unsigned char filler00104[0x00fa0-0x00104];
    struct  {
        union _gpioStatus
        {
            struct
            {
                unsigned int validPtr : 18;
                unsigned int : 10;
                unsigned int intOE : 1;
                unsigned int fifoOE : 1;
                unsigned int buf2Rdy : 1;
                unsigned int buf1Rdy : 1;
            } bits;
            unsigned int u32;
        } status;
        union _gpioEnable
        {
            struct
            {
                unsigned int : 28;
                unsigned int intOE : 1;
                unsigned int fifoOE : 1;
                unsigned int buf2Rdy : 1;
                unsigned int buf1Rdy : 1;
            } bits;
            unsigned int u32;
        } enable;
        union _gpioClear
        {
            struct
            {
                unsigned int : 28;
                unsigned int intOE : 1;
                unsigned int fifoOE : 1;
                unsigned int buf2Rdy : 1;
                unsigned int buf1Rdy : 1;
            } bits;
            unsigned int u32;
        } clear;
        union _gpioSet
        {
            struct
            {
                unsigned int : 28;
                unsigned int intOE : 1;
                unsigned int fifoOE : 1;
                unsigned int buf2Rdy : 1;
                unsigned int buf1Rdy : 1;
            } bits;
            unsigned int u32;
        } set;
    } fifoInt[4] ;
    union _gpioTsuIntStatus
    {
        struct
        {
            unsigned int : 8;
            unsigned int intOE : 12;
            unsigned int dataValid : 12;
        } bits;
        unsigned int u32;
    } tsuIntStatus;
    union _gpioTsuIntEnable
    {
        struct
        {
            unsigned int : 8;
            unsigned int intOE : 12;
            unsigned int dataValid : 12;
        } bits;
        unsigned int u32;
    } tsuIntEnable;
    union _gpioTsuIntClear
    {
        struct
        {
            unsigned int : 8;
            unsigned int intOE : 12;
            unsigned int dataValid : 12;
        } bits;
        unsigned int u32;
    } tsuIntClear;
    union _gpioTsuIntSet
    {
        struct
        {
            unsigned int : 8;
            unsigned int intOE : 12;
            unsigned int dataValid : 12;
        } bits;
        unsigned int u32;
    } tsuIntSet;
    unsigned char filler00ff0[0x00ff4-0x00ff0];
    union _gpioPowerDown
    {
        struct
        {
            unsigned int down : 1;
            unsigned int : 31;
        } bits;
        unsigned int u32;
    } powerDown;
    unsigned char filler00ff8[0x00ffc-0x00ff8];
    tmBlockId_t blockId;
} tmhwGpioRegs_NONVOLATILE_t;
typedef volatile tmhwGpioRegs_NONVOLATILE_t *ptmhwGpioRegs_t;

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
void _RGD_CPP_Check_tmhwGpioRegs_Offsets(ptmhwGpioRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwGpioRegs_Offsets(ptmhwGpioRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwGpioRegs, mode,                          00000000);
    _RGD_Check_Offset(tmhwGpioRegs, ctrl,                          00000010);
    _RGD_Check_Offset(tmhwGpioRegs, intSignal,                     00000020);
    _RGD_Check_Offset(tmhwGpioRegs, eventFifo,                     00000024);
    _RGD_Check_Offset(tmhwGpioRegs, eventTsu,                      00000034);
    _RGD_Check_Offset(tmhwGpioRegs, pinSel,                        00000064);
    _RGD_Check_Offset(tmhwGpioRegs, pgBufCtrl,                     00000074);
    _RGD_Check_Offset(tmhwGpioRegs, basePtr,                       00000084);
    _RGD_Check_Offset(tmhwGpioRegs, bufSize,                       000000a4);
    _RGD_Check_Offset(tmhwGpioRegs, divider,                       000000b4);
    _RGD_Check_Offset(tmhwGpioRegs, tsUnit,                        000000c4);
    _RGD_Check_Offset(tmhwGpioRegs, TimeCtrl,                      000000f4);
    _RGD_Check_Offset(tmhwGpioRegs, timerIOSel,                    000000f8);
    _RGD_Check_Offset(tmhwGpioRegs, vicIntStat,                    000000fc);
    _RGD_Check_Offset(tmhwGpioRegs, ddsOutSel,                     00000100);
    _RGD_Check_Offset(tmhwGpioRegs, fifoInt,                       00000fa0);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntStatus,                  00000fe0);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntEnable,                  00000fe4);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntClear,                   00000fe8);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntSet,                     00000fec);
    _RGD_Check_Offset(tmhwGpioRegs, powerDown,                     00000ff4);
    _RGD_Check_Offset(tmhwGpioRegs, blockId,                       00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWGPIO010F_RGD_SEEN)

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
#if        !defined(_TMHWGPIO010F_RGD_SEEN)
#define             _TMHWGPIO010F_RGD_SEEN
#define _RGD_Module_Id_Of_Gpio             010F
#define _RGD_Hex_Module_Id_Of_Gpio         0x010F
#define _RGD_Module_Name_Of_010F           Gpio
typedef struct _tmhwGpioRegs
{
    UInt32 mode[4];
#define GPIO_MODE_RETAIN                          0
#define GPIO_MODE_PRIMARY_FUNCTION                1
#define GPIO_MODE_IO_FUNCTION                     2
#define GPIO_MODE_OPEN_DRAIN                      3
#define GPIO_MODE_BITSIZE                         2
    union _gpioCtrl
    {
        struct
        {
            unsigned int data : 16;
            unsigned int mask : 16;
        } bits;
        unsigned int u32;
    } ctrl[4];
    union _gpioIntSignal
    {
        struct
        {
            unsigned int qvcpTstamp : 1;
            unsigned int ao1Tstamp : 1;
            unsigned int ai1Tstamp : 1;
            unsigned int spdoTstamp : 1;
            unsigned int spdiTstamp1 : 1;
            unsigned int spdiTstamp2 : 1;
            unsigned int vip1EowVid : 1;
            unsigned int vip1EowVbi : 1;
            unsigned int lastWordQ0 : 1;
            unsigned int lastWordQ1 : 1;
            unsigned int lastWordQ2 : 1;
            unsigned int lastWordQ3 : 1;
        } bits;
        unsigned int u32;
    } intSignal;
    union _gpioEventFifo
    {
        struct
        {
            unsigned int fifoMode : 2;
#define GPIO_EVENT_FIFO_TIMESTAMP                 0
#define GPIO_EVENT_FIFO_SIG_SAMPLE                1
#define GPIO_EVENT_FIFO_PATGEN_TS                 2
#define GPIO_EVENT_FIFO_PATGEN_SPL                3
            unsigned int eventMode : 2;
#define GPIO_EVENT_EDGE_NONE                      0
#define GPIO_EVENT_EDGE_NEG                       1
#define GPIO_EVENT_EDGE_POS                       2
#define GPIO_EVENT_EDGE_EITHER                    3
            unsigned int interval : 12;
            unsigned int enIoSel : 2;
#define GPIO_SAMPLE_1_BIT                         0
#define GPIO_SAMPLE_2_BIT                         1
#define GPIO_SAMPLE_4_BIT                         2
            unsigned int srcClkSel : 3;
#define GPIO_SOURCE_GPIO_0                        0
#define GPIO_SOURCE_GPIO_1                        1
#define GPIO_SOURCE_GPIO_2                        2
#define GPIO_SOURCE_GPIO_3                        3
#define GPIO_SOURCE_GPIO_4                        4
#define GPIO_SOURCE_GPIO_5                        5
#define GPIO_SOURCE_GPIO_6                        6
            unsigned int enDDSSource : 1;
            unsigned int unused : 2;
            unsigned int enPatGenClk : 2;
#define GPIO_DISABLE_PAT_GEN_CLK                  0
#define GPIO_ENABLE_PAT_GEN_CLK                   1
#define GPIO_ENABLE_PAT_GEN_CLK_INV               3
            unsigned int enClockSel : 2;
#define GPIO_CLOCK_SEL_DISABLE                    0
#define GPIO_CLOCK_SEL_POS_EDGE                   1
#define GPIO_CLOCK_SEL_NEG_EDGE                   3
            unsigned int enIRFilter : 1;
#define GPIO_IR_FILTER_DISABLE                    0
#define GPIO_IR_FILTER_ENABLE                     1
            unsigned int enIRCarrier : 1;
#define GPIO_IR_CARRIER_DISABLE                   0
#define GPIO_IR_CARRIER_ENABLE                    1
            unsigned int enEvTStamp : 1;
#define GPIO_EV_TSTAMP_DISABLE                    0
#define GPIO_EV_TSTAMP_ENABLE                     1
        } bits;
        unsigned int u32;
    } eventFifo[4];
    union _gpioEventTsu
    {
        struct
        {
            unsigned int eventMode : 2;
            unsigned int sourceSel : 8;
        } bits;
        unsigned int u32;
    } eventTsu[12];
    union _gpioPinSel
    {
        struct
        {
            unsigned int ioSel0 : 8;
            unsigned int ioSel1 : 8;
            unsigned int ioSel2 : 8;
            unsigned int ioSel3 : 8;
        } bits;
        unsigned int u32;
    } pinSel[4];
    union _gpioPgBufCtrl
    {
        struct
        {
            unsigned int bufLen : 18;
        } bits;
        unsigned int u32;
    } pgBufCtrl[4];
    UInt32 basePtr[2][4];
#define GPIO_DMA_BUF_1                            0
#define GPIO_DMA_BUF_2                            1
    union _gpioBufSize
    {
        struct
        {
            unsigned int size : 14;
        } bits;
        unsigned int u32;
    } bufSize[4];
    union  {
        union _gpioPatGen
        {
            struct
            {
                unsigned int freqDiv : 16;
                unsigned int carrDiv : 5;
                unsigned int dutyCycle : 2;
#define GPIO_SUBC_DUTY_33                         0
#define GPIO_SUBC_DUTY_50                         1
#define GPIO_SUBC_DUTY_66                         2
            } bits;
            unsigned int u32;
        } patGen;
        union _gpioSampling
        {
            struct
            {
                unsigned int freqDiv : 16;
                unsigned int irFilter : 3;
            } bits;
            unsigned int u32;
        } sampling;
    } divider[4] ;
    union _gpioTsUnit
    {
        struct
        {
            unsigned int counter : 31;
            unsigned int direction : 1;
#define GPIO_EDGE_RISING                          0
#define GPIO_EDGE_FALLING                         1
        } bits;
        unsigned int u32;
    } tsUnit[12];
    union _gpioTimeCtrl
    {
        struct
        {
            unsigned int timeCounter : 31;
        } bits;
        unsigned int u32;
    } TimeCtrl;
    union _gpioTimerIOSel
    {
        struct
        {
            unsigned int ioSel0 : 8;
            unsigned int ioSel1 : 8;
        } bits;
        unsigned int u32;
    } timerIOSel;
    union _gpioVicIntStat
    {
        struct
        {
            unsigned int fifoQueue0 : 1;
            unsigned int fifoQueue1 : 1;
            unsigned int fifoQueue2 : 1;
            unsigned int fifoQueue3 : 1;
            unsigned int tsuORed : 1;
        } bits;
        unsigned int u32;
    } vicIntStat;
    union _gpioDdsOutSel
    {
        struct
        {
            unsigned int enDdsOutOnGpioPin12 : 1;
            unsigned int enDdsOutOnGpioPin13 : 1;
            unsigned int enDdsOutOnGpioPin14 : 1;
        } bits;
        unsigned int u32;
    } ddsOutSel;
    unsigned char filler00104[0x00fa0-0x00104];
    struct  {
        union _gpioStatus
        {
            struct
            {
                unsigned int buf1Rdy : 1;
                unsigned int buf2Rdy : 1;
                unsigned int fifoOE : 1;
                unsigned int intOE : 1;
                unsigned int : 10;
                unsigned int validPtr : 18;
            } bits;
            unsigned int u32;
        } status;
        union _gpioEnable
        {
            struct
            {
                unsigned int buf1Rdy : 1;
                unsigned int buf2Rdy : 1;
                unsigned int fifoOE : 1;
                unsigned int intOE : 1;
            } bits;
            unsigned int u32;
        } enable;
        union _gpioClear
        {
            struct
            {
                unsigned int buf1Rdy : 1;
                unsigned int buf2Rdy : 1;
                unsigned int fifoOE : 1;
                unsigned int intOE : 1;
            } bits;
            unsigned int u32;
        } clear;
        union _gpioSet
        {
            struct
            {
                unsigned int buf1Rdy : 1;
                unsigned int buf2Rdy : 1;
                unsigned int fifoOE : 1;
                unsigned int intOE : 1;
            } bits;
            unsigned int u32;
        } set;
    } fifoInt[4] ;
    union _gpioTsuIntStatus
    {
        struct
        {
            unsigned int dataValid : 12;
            unsigned int intOE : 12;
        } bits;
        unsigned int u32;
    } tsuIntStatus;
    union _gpioTsuIntEnable
    {
        struct
        {
            unsigned int dataValid : 12;
            unsigned int intOE : 12;
        } bits;
        unsigned int u32;
    } tsuIntEnable;
    union _gpioTsuIntClear
    {
        struct
        {
            unsigned int dataValid : 12;
            unsigned int intOE : 12;
        } bits;
        unsigned int u32;
    } tsuIntClear;
    union _gpioTsuIntSet
    {
        struct
        {
            unsigned int dataValid : 12;
            unsigned int intOE : 12;
        } bits;
        unsigned int u32;
    } tsuIntSet;
    unsigned char filler00ff0[0x00ff4-0x00ff0];
    union _gpioPowerDown
    {
        struct
        {
            unsigned int : 31;
            unsigned int down : 1;
        } bits;
        unsigned int u32;
    } powerDown;
    unsigned char filler00ff8[0x00ffc-0x00ff8];
    tmBlockId_t blockId;
} tmhwGpioRegs_NONVOLATILE_t;
typedef volatile tmhwGpioRegs_NONVOLATILE_t *ptmhwGpioRegs_t;

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
void _RGD_CPP_Check_tmhwGpioRegs_Offsets(ptmhwGpioRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwGpioRegs_Offsets(ptmhwGpioRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwGpioRegs, mode,                          00000000);
    _RGD_Check_Offset(tmhwGpioRegs, ctrl,                          00000010);
    _RGD_Check_Offset(tmhwGpioRegs, intSignal,                     00000020);
    _RGD_Check_Offset(tmhwGpioRegs, eventFifo,                     00000024);
    _RGD_Check_Offset(tmhwGpioRegs, eventTsu,                      00000034);
    _RGD_Check_Offset(tmhwGpioRegs, pinSel,                        00000064);
    _RGD_Check_Offset(tmhwGpioRegs, pgBufCtrl,                     00000074);
    _RGD_Check_Offset(tmhwGpioRegs, basePtr,                       00000084);
    _RGD_Check_Offset(tmhwGpioRegs, bufSize,                       000000a4);
    _RGD_Check_Offset(tmhwGpioRegs, divider,                       000000b4);
    _RGD_Check_Offset(tmhwGpioRegs, tsUnit,                        000000c4);
    _RGD_Check_Offset(tmhwGpioRegs, TimeCtrl,                      000000f4);
    _RGD_Check_Offset(tmhwGpioRegs, timerIOSel,                    000000f8);
    _RGD_Check_Offset(tmhwGpioRegs, vicIntStat,                    000000fc);
    _RGD_Check_Offset(tmhwGpioRegs, ddsOutSel,                     00000100);
    _RGD_Check_Offset(tmhwGpioRegs, fifoInt,                       00000fa0);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntStatus,                  00000fe0);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntEnable,                  00000fe4);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntClear,                   00000fe8);
    _RGD_Check_Offset(tmhwGpioRegs, tsuIntSet,                     00000fec);
    _RGD_Check_Offset(tmhwGpioRegs, powerDown,                     00000ff4);
    _RGD_Check_Offset(tmhwGpioRegs, blockId,                       00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWGPIO010F_RGD_SEEN)

#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
