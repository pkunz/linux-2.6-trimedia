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
#if        !defined(_TMHWMMIARB1010RGD_RGD_SEEN)
#define             _TMHWMMIARB1010RGD_RGD_SEEN
typedef union
{
    struct {
     UInt32 : 22 ;
     UInt32 grantType : 2 ;
     UInt32 : 3 ;
     UInt32 agentId : 5 ;
    } bits;
    unsigned int u32;
} tmhwMmiArb1010List_rt ;
typedef union
{
    struct {
     UInt32 : 3 ;
     UInt32 rrLastEntries : 5 ;
     UInt32 : 3 ;
     UInt32 rrFirstEntries : 5 ;
     UInt32 : 3 ;
     UInt32 priorityEntries : 5 ;
     UInt32 tdmaEntries : 8 ;
    } bits;
    unsigned int u32;
} tmhwMmiArb1010Entries_rt ;
typedef union
{
    struct {
     UInt32 : 30 ;
     UInt32 mode : 2 ;
    } bits;
    unsigned int u32;
} tmhwMmiArb1010Mode_rt ;
#define _RGD_Module_Id_Of_MMIARB1010       1010
#define _RGD_Hex_Module_Id_Of_MMIARB1010   0x1010
#define _RGD_Module_Name_Of_1010           MMIARB1010
typedef struct _tmhwMmiArb1010Regs
{
    tmhwMmiArb1010List_rt tdmaListA[128];
    tmhwMmiArb1010List_rt priorityListA[32];
    tmhwMmiArb1010List_rt rrFirstListA[32];
    tmhwMmiArb1010List_rt rrLastListA[32];
    unsigned char filler00380[0x00400-0x00380];
    tmhwMmiArb1010List_rt tdmaListB[128];
    tmhwMmiArb1010List_rt priorityListB[32];
    tmhwMmiArb1010List_rt rrFirstListB[32];
    tmhwMmiArb1010List_rt rrLastListB[32];
    unsigned char filler00780[0x00800-0x00780];
    tmhwMmiArb1010Entries_rt nrEntriesA;
    tmhwMmiArb1010Entries_rt nrEntriesB;
    unsigned char filler00808[0x00900-0x00808];
    tmhwMmiArb1010Mode_rt control;
    tmhwMmiArb1010Mode_rt status;
    unsigned char filler00908[0x00ffc-0x00908];
    tmBlockId_t moduleId;
} tmhwMmiArb1010Regs_NONVOLATILE_t;
typedef volatile tmhwMmiArb1010Regs_NONVOLATILE_t *ptmhwMmiArb1010Regs_t;

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
void _RGD_CPP_Check_tmhwMmiArb1010Regs_Offsets(ptmhwMmiArb1010Regs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwMmiArb1010Regs_Offsets(ptmhwMmiArb1010Regs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwMmiArb1010Regs, tdmaListA,                     00000000);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, priorityListA,                 00000200);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrFirstListA,                  00000280);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrLastListA,                   00000300);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, tdmaListB,                     00000400);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, priorityListB,                 00000600);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrFirstListB,                  00000680);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrLastListB,                   00000700);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, nrEntriesA,                    00000800);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, nrEntriesB,                    00000804);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, control,                       00000900);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, status,                        00000904);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, moduleId,                      00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWMMIARB1010RGD_RGD_SEEN)

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
#if        !defined(_TMHWMMIARB1010RGD_RGD_SEEN)
#define             _TMHWMMIARB1010RGD_RGD_SEEN
typedef union
{
    struct {
     UInt32 agentId : 5 ;
     UInt32 : 3 ;
     UInt32 grantType : 2 ;
     UInt32 : 22 ;
    } bits;
    unsigned int u32;
} tmhwMmiArb1010List_rt ;
typedef union
{
    struct {
     UInt32 tdmaEntries : 8 ;
     UInt32 priorityEntries : 5 ;
     UInt32 : 3 ;
     UInt32 rrFirstEntries : 5 ;
     UInt32 : 3 ;
     UInt32 rrLastEntries : 5 ;
     UInt32 : 3 ;
    } bits;
    unsigned int u32;
} tmhwMmiArb1010Entries_rt ;
typedef union
{
    struct {
     UInt32 mode : 2 ;
     UInt32 : 30 ;
    } bits;
    unsigned int u32;
} tmhwMmiArb1010Mode_rt ;
#define _RGD_Module_Id_Of_MMIARB1010       1010
#define _RGD_Hex_Module_Id_Of_MMIARB1010   0x1010
#define _RGD_Module_Name_Of_1010           MMIARB1010
typedef struct _tmhwMmiArb1010Regs
{
    tmhwMmiArb1010List_rt tdmaListA[128];
    tmhwMmiArb1010List_rt priorityListA[32];
    tmhwMmiArb1010List_rt rrFirstListA[32];
    tmhwMmiArb1010List_rt rrLastListA[32];
    unsigned char filler00380[0x00400-0x00380];
    tmhwMmiArb1010List_rt tdmaListB[128];
    tmhwMmiArb1010List_rt priorityListB[32];
    tmhwMmiArb1010List_rt rrFirstListB[32];
    tmhwMmiArb1010List_rt rrLastListB[32];
    unsigned char filler00780[0x00800-0x00780];
    tmhwMmiArb1010Entries_rt nrEntriesA;
    tmhwMmiArb1010Entries_rt nrEntriesB;
    unsigned char filler00808[0x00900-0x00808];
    tmhwMmiArb1010Mode_rt control;
    tmhwMmiArb1010Mode_rt status;
    unsigned char filler00908[0x00ffc-0x00908];
    tmBlockId_t moduleId;
} tmhwMmiArb1010Regs_NONVOLATILE_t;
typedef volatile tmhwMmiArb1010Regs_NONVOLATILE_t *ptmhwMmiArb1010Regs_t;

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
void _RGD_CPP_Check_tmhwMmiArb1010Regs_Offsets(ptmhwMmiArb1010Regs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmhwMmiArb1010Regs_Offsets(ptmhwMmiArb1010Regs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmhwMmiArb1010Regs, tdmaListA,                     00000000);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, priorityListA,                 00000200);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrFirstListA,                  00000280);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrLastListA,                   00000300);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, tdmaListB,                     00000400);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, priorityListB,                 00000600);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrFirstListB,                  00000680);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, rrLastListB,                   00000700);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, nrEntriesA,                    00000800);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, nrEntriesB,                    00000804);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, control,                       00000900);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, status,                        00000904);
    _RGD_Check_Offset(tmhwMmiArb1010Regs, moduleId,                      00000ffc);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMHWMMIARB1010RGD_RGD_SEEN)

#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
