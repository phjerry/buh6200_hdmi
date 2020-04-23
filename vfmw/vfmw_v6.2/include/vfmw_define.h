#ifndef __BASETYPE_DEFS__ /*  #ifdef VFMW_BINARY_SUPPORT should be exclusive to v5 */
#define __BASETYPE_DEFS__

#define H264_ENABLE
#define MVC_ENABLE
#define MPEG2_ENABLE
#define AVS_ENABLE
#define AVS2_ENABLE
#define MPEG4_ENABLE
#define REAL8_ENABLE
#define REAL9_ENABLE
#define VC1_ENABLE
#define DIVX3_ENABLE
#define VP6_ENABLE
#define VP8_ENABLE
#define HEVC_ENABLE
#define VP9_ENABLE

#define VFMW_INT_MAX  ((hi_s32)(~0U >> 1))
#define VFMW_INT_MIN  (-VFMW_INT_MAX - 1)
#define VFMW_UINT_MAX (~0U)
#define VFMW_INT16_MAX  32767
#define VFMW_INT16_MIN  (-VFMW_INT16_MAX - 1)

#define MAX(a, b)       (((a) < (b)) ? (b) : (a))
#define MIN(a, b)       (((a) > (b)) ? (b) : (a))
#define ABS(x)          (((x) < 0) ? -(x) : (x))
#define MEDIAN(a, b, c) ((a) + (b) + (c)-MIN((a), MIN((b), (c))) - MAX((a), MAX((b), (c))))

#define CLIP1(high, x)      (MAX(MIN((x), high), 0))
#define CLIP3(low, high, x) (MAX(MIN((x), high), low))
#define CLIP255(x)          (MAX(MIN((x), 255), 0))

#define VFMW_ALIGN_UP(val, align) (((val) + (align) - 1) / (align) * (align)) /* for s/u32/64 */
#define VFMW_ALIGN_DN(val, align) ((val) & (~((align)-1)))

#define SWAP_XOR(a, b) \
    {                  \
        (a) ^= (b);    \
        (b) ^= (a);    \
        (a) ^= (b);    \
    }

#define CHECK_SINT_PLUS_OVERFLOW(a, b) ((((b > 0) && (a > (VFMW_INT_MAX - b)))    \
                                        || ((b < 0) && (a < (VFMW_INT_MIN - b)))) ? 1 : 0)

#define CHECK_UINT_PLUS_OVERFLOW(a, b) (((VFMW_UINT_MAX - a) < b) ? 1 : 0)

#define VFMW_LAST_IMG_ID(x) ((x) + 2)

#endif /*   __PUBLIC_H__  */

