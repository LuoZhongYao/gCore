/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2015 八月 06
 ************************************************/
#ifndef __UTILS_H__
#define __UTILS_H__

#include <csrtypes.h>

#ifndef __unused
#define __unused    __attribute__((unused))
#endif

typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef int8   s8;
typedef int16  s16;
typedef int32  s32;
typedef u32    b16;

u8 atou(char ch);
s32 strtol( char *nptr, char **endptr, int base);
u32 strtoul( char *nptr, char **endptr, int base);
void *memmem(void *haystack,u16 haystacklen,
        const void *needle,u16 needlelen);

#define B16_U16(u)   (u16)((((u) >> 16) | ((u) << 8)))

#define false   FALSE
#define true    TRUE
#define INST_MSG(TYPE)      TYPE *msg = (TYPE*)message

#define FALSE_RETURN(e,v)    do{\
    if(!(e)){\
        return v;\
    }}while(0)

#define TRUE_RETURN(e,v)     do{\
    if((e)){\
        return v;\
    }}while(0)

#define TEST_AND_FREE(x)     do{\
    if((x)){\
        free((x));\
    }}while(0)

#define DUMMY

#define BUILD_BUG_ON_ZERO(e) (sizeof(char[1 - 2 * !!(e)]) - 1)

#define __must_be_array(a) \
  BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#define NR_N(n)         _NR_N(n)
#define QR_N(n)         _QR_N(n)
#define _NR_N(n)        n ## 9,n ## 8,n ## 7,n ## 6,n ## 5,n ## 4,n ## 3,n ## 2,n ## 1,n ## 0
#define _QR_N(n)        n ## 1,n ## 1,n ## 1,n ## 1,n ## 1,n ## 1,n ## 1,n ## 1,n ## 1,n ## 0
#define PP_RSEQ_N()     NR_N(6), NR_N(5), NR_N(4), NR_N(3), NR_N(2), NR_N(1), NR_N(),
#define PP_QSEQ_N()     QR_N( ), QR_N( ), QR_N( ), QR_N( ), QR_N( ), QR_N( ), QR_N()
#define PP_ARG_N( \
         _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
        _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
        _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
        _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
        _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
        _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
        _61,_62,_63,_64,_65,_66,_67,_68,_69,_70,N,...) N

#define PP_NARG_(...)   PP_ARG_N(__VA_ARGS__)
#define PP_NARG(...)    PP_NARG_(,##__VA_ARGS__,PP_RSEQ_N())
#define PP_QARG(...)    PP_NARG_(,##__VA_ARGS__,PP_QSEQ_N())

#endif

