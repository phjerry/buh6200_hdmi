/******************************************************************************
  Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
  File Name     : hi_math.h
  Version       : Initial Draft
  Author        :
  Created       : 2009/04/09
  Last Modified :
  Description   : mathematical functions.
  Function List :
  History       :
  1.Date        : 2009/04/09
    Author      :
    Modification: Created file

  2.Date        : 2009/05/04
    Author      :
    Modification: Add ENDIAN and value_between.

  3.Date        : 2009/05/21
    Author      :
    Modification: Add macro of FRACTION operation and cmp.

******************************************************************************/
#ifndef __HI_MATH_H__
#define __HI_MATH_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
/*
 * hi_abs(x)               absolute value of x
 * hi_sign(x)              sign of x
 * hi_cmp(x,y)             0 if x==y; 1 if x>y; -1 if x<y
 */
#define hi_abs(x)    ((x) >= 0 ? (x) : (-(x)))
#define hi_sign(x)   ((x) >= 0 ? 1 : (-1))
#define hi_cmp(x, y) (((x) == (y)) ? 0 : (((x) > (y)) ? 1 : (-1)))
/*
 * max2(x,y)              maximum of x and y
 * min2(x,y)              minimum of x and y
 * max3(x,y,z)            maximum of x, y and z
 * min3(x,y,z)            minimun of x, y and z
 * median(x,y,z)          median of x,y,z
 * mean2(x,y)             mean of x,y
 */
#define max2(x, y)      ((x) > (y) ? (x) : (y))
#define min2(x, y)      ((x) < (y) ? (x) : (y))
#define max3(x, y, z)   ((x) > (y) ? max2(x, z) : max2(y, z))
#define min3(x, y, z)   ((x) < (y) ? min2(x, z) : min2(y, z))
#define median(x, y, z) (((x) + (y) + (z) - max3(x, y, z)) - min3(x, y, z))
#define mean2(x, y)     (((x) + (y)) >> 1)
/*
 * clip3(x,min,max)       clip x within [min,max]
 * wrap_max(x,max,min)    wrap to min if x equal max
 * wrap_min(x,min,max)    wrap to max if x equal min
 * value_between(x,min.max)   True if x is between [min,max] inclusively.
 */
#define clip3(x, min, max)         ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define wrap_max(x, max, min)      ((x) >= (max) ? (min) : (x))
#define wrap_min(x, min, max)      ((x) <= (min) ? (max) : (x))
#define value_between(x, min, max) (((x) >= (min)) && ((x) <= (max)))
/*
 * multi_of_2_power(x,a)  whether x is multiple of a(a must be power of 2)
 * ceiling_2_power(x,a)   ceiling x to multiple of a(a must be power of 2)
 * floor_2_power(x,a)     floor x to multiple of a(a must be power of 2)
 * hialign(x, a)            align x to multiple of a
 *
 * Example:
 * ceiling_2_power(5,4) result is 8
 * floor_2_power(5,4) result is 4
 */
#define multi_of_2_power(x, a) (!((x) & ((a) - 1)))
#define ceiling_2_power(x, a)  (((x) + ((a) - 1)) & (~((a) - 1)))
#define floor_2_power(x, a)    ((x) & (~((a) - 1)))
#define hialign(x, a)          ((a) * (((x) + (a) - 1) / (a)))
#define hiceiling(x, a)        (((x) + (a) - 1) / (a))
#define hialign_up(x, a)       ((((x) + (a) - 1) / (a)) * (a))
#define hialign_down(x, a)     ((x) - (x) % (a))
#define hialign_closest(x, divisor) ({ \
    typeof(x) __x = x; \
    typeof(divisor) __d = divisor; \
    (((typeof(x))-1) > 0 || \
    ((typeof(divisor))-1) > 0 || (__x) > 0) ? \
        (((__x) + ((__d) / 2)) / (__d)) : \
        (((__x) - ((__d) / 2)) / (__d)); })

#if defined(HI_DDRC_CHANNEL_DUAL)
#define hi_sys_get_stride(w) ((((w) % 256) == 0) ? (w) : (((((w) / 256) % 2) == 0) ? \
    ((((w) / 256) + 1) * 256) : ((((w) / 256) + 2) * 256)))
#elif defined(HI_DDRC_CHANNEL_SINGLE)
#define hi_sys_get_stride(w) (hialign(w, 64))
#else
#define hi_sys_get_stride(w) (((w) + 63) / 64 * 64)
#endif
/*
 * Get the span between two unsinged number, such as
 * span(HI_U32, 100, 200) is 200 - 100 = 100
 * span(HI_U32, 200, 100) is 0xFFFFFFFF - 200 + 100
 * span(HI_U64, 200, 100) is 0xFFFFFFFFFFFFFFFF - 200 + 100
 */
#define span(type, begin, end) \
    do { \
        type b = (begin); \
        type e = (end); \
        (type)((b >= e) ? (b - e) : (b + ((~((type)0)) - e))); \
    } while (0)
/*
 * endian32(x,y)              little endian <---> big endian
 * is_little_end()            whether the system is little end mode
 */
#define endian32(x)               \
    (((x) << 24) |  \
        (((x) & 0x0000ff00) << 8) |  \
        (((x) & 0x00ff0000) >> 8) |  \
        (((x) >> 24) & 0x000000ff))

typedef union un_end_test {
    hi_char ctest[4]; /* 4 表示使用4个字节长度来保存用于计算大小端的数值 */
    hi_u32 test;
} end_test_info;

__inline static hi_bool is_little_end(void)
{
    end_test_info end_test;

    end_test.ctest[0] = 0x01; /* 0 表示第1个字节位置 */
    end_test.ctest[1] = 0x02; /* 1 表示第2个字节位置 */
    end_test.ctest[2] = 0x03; /* 2 表示第3个字节位置 */
    end_test.ctest[3] = 0x04; /* 3 表示第4个字节位置 */

    return (end_test.test > 0x01020304) ? (HI_TRUE) : (HI_FALSE);
}

/*
 * fraction32(de,nu)          fraction: nu(minator) / de(nominator).
 * numerator32(x)              of x(x is fraction)
 * denominator32(x)           Denominator of x(x is fraction)
 * represent fraction in 32 bit. LSB 16 is numerator, MSB 16 is denominator
 * It is integer if denominator is 0.
 */
#define fraction32(de, nu) (((de) << 16) | (nu))
#define numerator32(x)     ((x) & 0xffff)
#define denominator32(x)   ((x) >> 16)

/******************************************************************************
* rgb(r,g,b)    assemble the r,g,b to 24bit color
* rgb_r(c)      get RED   from 24bit color
* rgb_g(c)      get GREEN from 24bit color
* rgb_b(c)      get BLUE  from 24bit color
******************************************************************************/
#define rgb(r, g, b) ((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define rgb_r(c)     (((c) & 0xff0000) >> 16)
#define rgb_g(c)     (((c) & 0xff00) >> 8)
#define rgb_b(c)     ((c) & 0xff)
/*
 * yuv(y,u,v)    assemble the y,u,v to 24bit color
 * yuv_y(c)      get Y from 24bit color
 * yuv_u(c)      get U from 24bit color
 * yuv_v(c)      get V from 24bit color
 */
#define yuv(y, u, v) ((((y) & 0xff) << 16) | (((u) & 0xff) << 8) | ((v) & 0xff))
#define yuv_y(c)     (((c) & 0xff0000) >> 16)
#define yuv_u(c)     (((c) & 0xff00) >> 8)
#define yuv_v(c)     ((c) & 0xff)

/******************************************************************************
* rgb2yc(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
* rgb2yuv(rgb_data, *yuv)             convert rgb to yuv
******************************************************************************/
__inline static hi_void rgb2yc(hi_u8 r, hi_u8 g, hi_u8 b, hi_u8 *py, hi_u8 *pcb, hi_u8 *pcr)
{
    /* Y */
    *py = (hi_u8)(((r * 66 + g * 129 + b * 25) >> 8) + 16); /* 66 129 25 8 16 表示rgb转yc格式标准公式的参数 */

    /* Cb */
    *pcb = (hi_u8)((((b * 112 - r * 38) - g * 74) >> 8) + 128); /* 112 38 74 8 128 表示rgb转yc格式标准公式的参数 */

    /* Cr */
    *pcr = (hi_u8)((((r * 112 - g * 94) - b * 18) >> 8) + 128); /* 112 94 18 8 128 表示rgb转yc格式标准公式的参数 */
}

__inline static hi_u32 rgb2yuv(hi_u32 rgb_data)
{
    hi_u8 y, u, v;

    rgb2yc(rgb_r(rgb_data), rgb_g(rgb_data), rgb_b(rgb_data), &y, &u, &v);

    return yuv(y, u, v);
}

/******************************************************************************
* GetYCFromRGB(rgb_data, *y, *cbcr)    convert rgb to yyyy, uvuv,
******************************************************************************/
__inline static hi_void get_yc_from_rgb(hi_u32 rgb_data, hi_u32 *py, hi_u32 *pc)
{
    hi_u8 y, cb, cr;
    hi_u32 color_y, color_c;

    rgb2yc(rgb_r(rgb_data), rgb_g(rgb_data), rgb_b(rgb_data), &y, &cb, &cr);

    color_y = ((y & 0xFF) << 24) + ((y & 0xFF) << 16) + ((y & 0xFF) << 8) + (y & 0xFF); /* 24 16 18 8 表示计算color_y偏移位数 */

    color_c = ((cb & 0xFF) << 24) + ((cb & 0xFF) << 8); /* 24 8 表示计算color_c偏移位数 */

    color_c = color_c + ((cr & 0xFF) << 16) + (cr & 0xFF); /* 16 表示计算color_c偏移位数 */

    *py = color_y;
    *pc = color_c;
}

/*******************************************************************************
* fps_control Useing Sample:
*  fps_ctrl g_stFpsCtrl;
*
*  Take 12 frame uniform in 25.
*  call like this : init_fps(&g_stFpsCtrl, 25, 12)
*  if fps_control(&g_stFpsCtrl) return true
*  then printf "Yes, this frmae should be token"
*
*******************************************************************************/
typedef struct hi_fps_ctrl {
    hi_u32 ffps;    /* Full frame rate */
    hi_u32 tfps;    /* Target frame rate */
    hi_u32 frm_key; /* update key frame */
} fps_ctrl;

__inline static hi_void init_fps(fps_ctrl *frm_cttl_ptr, hi_u32 full_fps, hi_u32 tag_fps)
{
    frm_cttl_ptr->ffps = full_fps;
    frm_cttl_ptr->tfps = tag_fps;
    frm_cttl_ptr->frm_key = 0;
}

__inline static hi_bool fps_control(fps_ctrl *frm_cttl_ptr)
{
    hi_bool ret = HI_FALSE;

    frm_cttl_ptr->frm_key += frm_cttl_ptr->tfps;
    if (frm_cttl_ptr->frm_key >= frm_cttl_ptr->ffps) {
        frm_cttl_ptr->frm_key -= frm_cttl_ptr->ffps;
        ret = HI_TRUE;
    }

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_MATH_H__ */

