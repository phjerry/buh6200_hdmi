/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d hal hwc
 * Create: 2019-11-05
 */

#include "drv_gfx2d_hal_features.h"
#include "drv_gfx2d_debug.h"

typedef union {
    struct {
        hi_u32 support_fmt_rgb444 : 1;            /* bit 1 */
        hi_u32 support_fmt_bgr444 : 1;            /* bit 2 */
        hi_u32 support_fmt_rgb555 : 1;            /* bit 3 */
        hi_u32 support_fmt_bgr555 : 1;            /* bit 4 */
        hi_u32 support_fmt_rgb565 : 1;            /* bit 5 */
        hi_u32 support_fmt_bgr565 : 1;            /* bit 6 */
        hi_u32 support_fmt_rgb888 : 1;            /* bit 7 */
        hi_u32 support_fmt_bgr888 : 1;            /* bit 8 */
        hi_u32 support_fmt_krgb888 : 1;           /* bit 9 */
        hi_u32 support_fmt_kbgr888 : 1;           /* bit 10 */
        hi_u32 support_fmt_argb4444 : 1;          /* bit 11 */
        hi_u32 support_fmt_abgr4444 : 1;          /* bit 12 */
        hi_u32 support_fmt_rgba4444 : 1;          /* bit 13 */
        hi_u32 support_fmt_bgra4444 : 1;          /* bit 14 */
        hi_u32 support_fmt_argb1555 : 1;          /* bit 15 */
        hi_u32 support_fmt_abgr1555 : 1;          /* bit 16 */
        hi_u32 support_fmt_rgba1555 : 1;          /* bit 17 */
        hi_u32 support_fmt_bgra1555 : 1;          /* bit 18 */
        hi_u32 support_fmt_argb8565 : 1;          /* bit 19 */
        hi_u32 support_fmt_abgr8565 : 1;          /* bit 20 */
        hi_u32 support_fmt_rgba8565 : 1;          /* bit 21 */
        hi_u32 support_fmt_bgra8565 : 1;          /* bit 22 */
        hi_u32 support_fmt_argb8888 : 1;          /* bit 23 */
        hi_u32 support_fmt_abgr8888 : 1;          /* bit 24 */
        hi_u32 support_fmt_rgba8888 : 1;          /* bit 25 */
        hi_u32 support_fmt_bgra8888 : 1;          /* bit 26 */
        hi_u32 support_fmt_argb2101010 : 1;       /* bit 27 */
        hi_u32 support_fmt_argb10101010 : 1;      /* bit 28 */
        hi_u32 support_fmt_fp16 : 1;              /* bit 29 */
        hi_u32 support_fmt_clut1 : 1;             /* bit 30 */
        hi_u32 support_fmt_clut2 : 1;             /* bit 31 */
        hi_u32 support_fmt_clut4 : 1;             /* bit 32 */
        hi_u32 support_fmt_clut8 : 1;             /* bit 33 */
        hi_u32 support_fmt_aclut44 : 1;           /* bit 34 */
        hi_u32 support_fmt_aclut88 : 1;           /* bit 35 */
        hi_u32 support_fmt_yuv888 : 1;            /* bit 36 */
        hi_u32 support_fmt_ayuv8888 : 1;          /* bit 37 */
        hi_u32 support_fmt_yuyv422 : 1;           /* bit 38 */
        hi_u32 support_fmt_yvyu422 : 1;           /* bit 39 */
        hi_u32 support_fmt_uyvy422 : 1;           /* bit 40 */
        hi_u32 support_fmt_yyuv422 : 1;           /* bit 41 */
        hi_u32 support_fmt_vyuy422 : 1;           /* bit 42 */
        hi_u32 support_fmt_vuyy422 : 1;           /* bit 43 */
        hi_u32 support_fmt_semiplanar400 : 1;     /* bit 44 */
        hi_u32 support_fmt_semiplanar420uv : 1;   /* bit 45 */
        hi_u32 support_fmt_semiplanar420vu : 1;   /* bit 46 */
        hi_u32 support_fmt_semiplanar422uv_h : 1; /* bit 47 */
        hi_u32 support_fmt_semiplanar422vu_h : 1; /* bit 48 */
        hi_u32 support_fmt_semiplanar422uv_v : 1; /* bit 49 */
        hi_u32 support_fmt_semiplanar422vu_v : 1; /* bit 50 */
        hi_u32 support_fmt_semiplanar444uv : 1;   /* bit 51 */
        hi_u32 support_fmt_semiplanar444vu : 1;   /* bit 52 */
        hi_u32 support_fmt_planar400 : 1;         /* bit 53 */
        hi_u32 support_fmt_planar420 : 1;         /* bit 54 */
        hi_u32 support_fmt_planar411 : 1;         /* bit 55 */
        hi_u32 support_fmt_planar410 : 1;         /* bit 56 */
        hi_u32 support_fmt_planar422h : 1;        /* bit 57 */
        hi_u32 support_fmt_planar422v : 1;        /* bit 58 */
        hi_u32 support_fmt_planar444 : 1;         /* bit 59 */
        hi_u32 reserved : 5;                      /* 5: bit 60 - 64 */
    } bits;

    hi_u64 all_bits;
} gfx2d_support_fmts;

typedef union {
    struct {
        hi_u32 support_compress_none : 1; /* bit 1 */
        hi_u32 support_compress_hfbc : 1; /* bit 2 */
        hi_u32 support_compress_afbc : 1; /* bit 3 */
        hi_u32 reserved : 29;             /* 29: bit 4 - 32 */
    } bits;

    hi_u32 all_bits;
} gfx2d_support_compress_types;

typedef union {
    struct {
        hi_u32 support_sdr : 1;   /* bit 1 */
        hi_u32 support_hdr10 : 1; /* bit 2 */
        hi_u32 support_hlg : 1;   /* bit 3 */
        hi_u32 reserved : 29;     /* 29: bit 4 - 32 */
    } bits;

    hi_u32 all_bits;
} gfx2d_support_xdr_types;

typedef union {
    struct {
        hi_u32 support_bt709 : 1;  /* bit 1 */
        hi_u32 support_bt2020 : 1; /* bit 2 */
        hi_u32 reserved : 30;      /* 30: bit 3 - 32 */
    } bits;

    hi_u32 all_bits;
} gfx2d_support_color_spaces;

typedef struct {
    gfx2d_support_fmts support_fmts;
    gfx2d_support_compress_types support_compress_types;
    gfx2d_support_xdr_types support_xdr_types;
    gfx2d_support_color_spaces support_color_space_types;
    hi_bool support_csc;
    hi_bool support_resize;
} gfx2d_support_features;

gfx2d_support_features g_gfx2d_support_features[HWC_LAYER_BUTT] = {
    /* src2 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,
        .support_fmts.bits.support_fmt_clut1 = 0,   /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_clut2 = 0,   /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_clut4 = 0,   /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_clut8 = 0,   /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_aclut44 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_aclut88 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_yuv888 = 1,
        .support_fmts.bits.support_fmt_ayuv8888 = 1,
        .support_fmts.bits.support_fmt_yuyv422 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_yvyu422 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_uyvy422 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_yyuv422 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_vyuy422 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_vuyy422 = 0, /* 1 */
        .support_fmts.bits.support_fmt_semiplanar400 = 1,
        .support_fmts.bits.support_fmt_semiplanar420uv = 1,
        .support_fmts.bits.support_fmt_semiplanar420vu = 1,
        .support_fmts.bits.support_fmt_semiplanar422uv_h = 1,
        .support_fmts.bits.support_fmt_semiplanar422vu_h = 1,
        .support_fmts.bits.support_fmt_semiplanar422uv_v = 1,
        .support_fmts.bits.support_fmt_semiplanar422vu_v = 1,
        .support_fmts.bits.support_fmt_semiplanar444uv = 1,
        .support_fmts.bits.support_fmt_semiplanar444vu = 1,

        .support_compress_types.bits.support_compress_none = 1,

        .support_xdr_types.bits.support_sdr = 1,

        .support_color_space_types.bits.support_bt709 = 1,

        .support_csc = 1,

        .support_resize = 1,
    },
    /* src 1 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,
        .support_fmts.bits.support_fmt_argb2101010 = 1,
        .support_fmts.bits.support_fmt_fp16 = 0, /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_yuv888 = 1,
        .support_fmts.bits.support_fmt_ayuv8888 = 1,

        .support_compress_types.bits.support_compress_none = 1,
        .support_compress_types.bits.support_compress_afbc = 0, /* can be 1, currently not support */

        .support_xdr_types.bits.support_sdr = 1,
        .support_xdr_types.bits.support_hdr10 = 0, /* can be 1, currently not support */
        .support_xdr_types.bits.support_hlg = 0,   /* can be 1, currently not support */

        .support_color_space_types.bits.support_bt709 = 1,
        .support_color_space_types.bits.support_bt2020 = 0, /* can be 1, currently not support */

        .support_csc = 1,

        .support_resize = 1,
    },
    /* src 3 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,
        .support_fmts.bits.support_fmt_argb2101010 = 1,
        .support_fmts.bits.support_fmt_fp16 = 0, /* can be 1, currently not support */

        .support_compress_types.bits.support_compress_none = 1,
        .support_compress_types.bits.support_compress_afbc = 0, /* can be 1, currently not support */

        .support_xdr_types.bits.support_sdr = 1,
        .support_xdr_types.bits.support_hdr10 = 0, /* can be 1, currently not support */
        .support_xdr_types.bits.support_hlg = 0,   /* can be 1, currently not support */

        .support_color_space_types.bits.support_bt709 = 1,
        .support_color_space_types.bits.support_bt2020 = 0, /* can be 1, currently not support */

        .support_csc = 1,

        .support_resize = 1,
    },
    /* src4 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,
        .support_fmts.bits.support_fmt_argb2101010 = 1,
        .support_fmts.bits.support_fmt_fp16 = 0,     /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_yuv888 = 0,   /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_ayuv8888 = 0, /* can be 1, currently not support */

        .support_compress_types.bits.support_compress_none = 1,
        .support_compress_types.bits.support_compress_afbc = 0, /* can be 1, currently not support */

        .support_xdr_types.bits.support_sdr = 1,
        .support_xdr_types.bits.support_hdr10 = 0, /* can be 1, currently not support */
        .support_xdr_types.bits.support_hlg = 0,   /* can be 1, currently not support */

        .support_color_space_types.bits.support_bt709 = 1,
        .support_color_space_types.bits.support_bt2020 = 0, /* can be 1, currently not support */
    },
    /* src5 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,
        .support_fmts.bits.support_fmt_argb2101010 = 1,
        .support_fmts.bits.support_fmt_fp16 = 0,     /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_yuv888 = 0,   /* can be 1, currently not support */
        .support_fmts.bits.support_fmt_ayuv8888 = 0, /* can be 1, currently not support */

        .support_compress_types.bits.support_compress_none = 1,

        .support_xdr_types.bits.support_sdr = 1,
        .support_xdr_types.bits.support_hdr10 = 0, /* can be 1, currently not support */
        .support_xdr_types.bits.support_hlg = 0,   /* can be 1, currently not support */

        .support_color_space_types.bits.support_bt709 = 1,
        .support_color_space_types.bits.support_bt2020 = 0, /* can be 1, currently not support */
    },
    /* src6 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,

        .support_compress_types.bits.support_compress_none = 1,

        .support_xdr_types.bits.support_sdr = 1,

        .support_color_space_types.bits.support_bt709 = 1,
    },
    /* src7 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,

        .support_compress_types.bits.support_compress_none = 1,

        .support_xdr_types.bits.support_sdr = 1,

        .support_color_space_types.bits.support_bt709 = 1,
    },
    /* src8 */
    {
        .support_fmts.bits.support_fmt_rgb444 = 1,
        .support_fmts.bits.support_fmt_bgr444 = 1,
        .support_fmts.bits.support_fmt_rgb555 = 1,
        .support_fmts.bits.support_fmt_bgr555 = 1,
        .support_fmts.bits.support_fmt_rgb565 = 1,
        .support_fmts.bits.support_fmt_bgr565 = 1,
        .support_fmts.bits.support_fmt_rgb888 = 1,
        .support_fmts.bits.support_fmt_bgr888 = 1,
        .support_fmts.bits.support_fmt_krgb888 = 1,
        .support_fmts.bits.support_fmt_kbgr888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_abgr4444 = 1,
        .support_fmts.bits.support_fmt_rgba4444 = 1,
        .support_fmts.bits.support_fmt_bgra4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_abgr1555 = 1,
        .support_fmts.bits.support_fmt_rgba1555 = 1,
        .support_fmts.bits.support_fmt_bgra1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_abgr8565 = 1,
        .support_fmts.bits.support_fmt_rgba8565 = 1,
        .support_fmts.bits.support_fmt_bgra8565 = 1,
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_abgr8888 = 1,
        .support_fmts.bits.support_fmt_rgba8888 = 1,
        .support_fmts.bits.support_fmt_bgra8888 = 1,

        .support_compress_types.bits.support_compress_none = 1,

        .support_xdr_types.bits.support_sdr = 1,

        .support_color_space_types.bits.support_bt709 = 1,
    },
};

hwc_layer_id select_layer_id_from_features(drv_gfx2d_compose_msg *compose, hi_u8 *ref, hi_u32 ref_len)
{
    hi_u32 j;

    if (ref_len < HWC_LAYER_BUTT) {
        return HWC_LAYER_BUTT;
    }
    for (j = HWC_LAYER_G6; j >= HWC_LAYER_VHD; j--) {
        if ((((g_gfx2d_support_features[j - 1].support_fmts.all_bits >> compose->compose_surface.format) & 0x1) ==
             0x1) &&
            (((g_gfx2d_support_features[j - 1].support_compress_types.all_bits >>
               compose->compose_surface.compress_msg.compress_type) &
              0x1) == 0x1) &&
            (((g_gfx2d_support_features[j - 1].support_xdr_types.all_bits >> compose->compose_surface.xdr_mode) &
              0x1) == 0x1) &&
            (((g_gfx2d_support_features[j - 1].support_color_space_types.all_bits >>
               compose->compose_surface.color_space) &
              0x1) == 0x1) &&
            (!((compose->opt.resize.resize_en == HI_TRUE) &&
               (g_gfx2d_support_features[j - 1].support_resize == HI_FALSE)))) {
            if (ref[j] == 0) {
                return j;
            }
        }
    }
    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->compose_surface.format);
    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->compose_surface.compress_msg.compress_type);
    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->compose_surface.xdr_mode);
    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->compose_surface.color_space);
    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->opt.resize.resize_en);
    for (j = HWC_LAYER_G6; j >= HWC_LAYER_VHD; j--) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, ref[j]);
    }
    return HWC_LAYER_BUTT;
}
