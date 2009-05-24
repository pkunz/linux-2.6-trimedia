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
#if        !defined(_TMDLHWSEM0126_MIO_SEEN)
#define             _TMDLHWSEM0126_MIO_SEEN
#define _RGD_Module_Id_Of_GLOBAL_REG1      0126
#define _RGD_Hex_Module_Id_Of_GLOBAL_REG1   0x0126
#define _RGD_Module_Name_Of_0126           GLOBAL_REG1
typedef struct _tmdlHwSemRegs
{
#define NUM_OF_HWSEM                             16
    unsigned char filler00000[0x00800-0x00000];
    union _global_reg1Semaphore
    {
        struct
        {
            unsigned int : 20;
            unsigned int semaphore : 12;
        } bits;
        unsigned int u32;
    } semaphore[16];
} tmdlHwSemRegs_NONVOLATILE_t;
typedef volatile tmdlHwSemRegs_NONVOLATILE_t *ptmdlHwSemRegs_t;

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
void _RGD_CPP_Check_tmdlHwSemRegs_Offsets(ptmdlHwSemRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmdlHwSemRegs_Offsets(ptmdlHwSemRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmdlHwSemRegs, semaphore,                     00000800);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMDLHWSEM0126_MIO_SEEN)

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
#if        !defined(_TMDLHWSEM0126_MIO_SEEN)
#define             _TMDLHWSEM0126_MIO_SEEN
#define _RGD_Module_Id_Of_GLOBAL_REG1      0126
#define _RGD_Hex_Module_Id_Of_GLOBAL_REG1   0x0126
#define _RGD_Module_Name_Of_0126           GLOBAL_REG1
typedef struct _tmdlHwSemRegs
{
#define NUM_OF_HWSEM                             16
    unsigned char filler00000[0x00800-0x00000];
    union _global_reg1Semaphore
    {
        struct
        {
            unsigned int semaphore : 12;
        } bits;
        unsigned int u32;
    } semaphore[16];
} tmdlHwSemRegs_NONVOLATILE_t;
typedef volatile tmdlHwSemRegs_NONVOLATILE_t *ptmdlHwSemRegs_t;

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
void _RGD_CPP_Check_tmdlHwSemRegs_Offsets(ptmdlHwSemRegs_t pRegs)
#else   // defined(__cplusplus)
void _RGD_C_Check_tmdlHwSemRegs_Offsets(ptmdlHwSemRegs_t pRegs)
#endif  // defined(__cplusplus)
{
    _RGD_Check_Offset(tmdlHwSemRegs, semaphore,                     00000800);
}
#pragma optimize("", on)

#if        defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#undef  _RGD_offsetof
#undef  _RGD_Check_Offset

#endif  // RGD_CHECK_OFFSETS
#endif  // defined(RGD_CHECK_OFFSETS)

#endif  // !defined(_TMDLHWSEM0126_MIO_SEEN)

#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
