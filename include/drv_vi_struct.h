/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiSilicon VI drv struct declaration.
 * Author: ATV team
 * Create: 2019-11-23
 */

#ifndef __DRV_VI_STRUCT_H__
#define __DRV_VI_STRUCT_H__

#include "hi_drv_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_VI_MAX_NUM 2

typedef enum {
    HI_VI_TYPE_MAIN = 0, /* default */
    HI_VI_TYPE_SUB,      /* used for PIP or transcoding */

    HI_VI_TYPE_MAX
} hi_vi_type;

typedef enum {
    HI_VI_INTF_UNKNOW = 0,
    HI_VI_INTF_FVHDE,
    HI_VI_INTF_BT1120,

    HI_VI_INTF_MAX
} hi_vi_intf_mode;

typedef enum {
    HI_VI_ACCESS_UNKNOW = 0,
    HI_VI_ACCESS_TVD,
    HI_VI_ACCESS_HDDEC,
    HI_VI_ACCESS_HDMIRX_PORT0,
    HI_VI_ACCESS_HDMIRX_PORT1,
    HI_VI_ACCESS_HDMIRX_PORT2,
    HI_VI_ACCESS_HDMIRX_PORT3,

    HI_VI_ACCESS_MAX
} hi_vi_access_type;

typedef enum {
    HI_VI_QUANTIZATION_RANGE_AUTO = 0,
    HI_VI_QUANTIZATION_RANGE_LIMIT,
    HI_VI_QUANTIZATION_RANGE_FULL,

    HI_VI_QUANTIZATION_RANGE_MAX
} hi_vi_quantization_range;

typedef enum {
    HI_VI_OVERSAMPLE_1X = 0,
    HI_VI_OVERSAMPLE_2X,
    HI_VI_OVERSAMPLE_4X,

    HI_VI_OVERSAMPLE_MAX
} hi_vi_oversample;

typedef enum {
    HI_VI_PICTURE_MODE_VIDEO = 0, /** < pass vpss and pass csc */
    HI_VI_PICTURE_MODE_GAME,      /** < bypass vpss and pass csc */
    HI_VI_PICTURE_MODE_PC,        /** < bypass vpss and bypass csc */

    HI_VI_PICTURE_MODE_MAX
} hi_vi_picture_mode;

typedef struct {
    hi_bool non_std; /* not stand flag */
    hi_u32 height;   /* not stand height */
    hi_u32 v_freq;   /* not stand freq. 61050 means 61.05_hz */
} hi_vi_nonstd_info;

typedef struct {
    hi_drv_rect crop_rect;        /* input crop rect */
    hi_u32 width;                 /* frame width, 3D is single eye width */
    hi_u32 height;                /* frame height, interlace is two field height,
                                     progress is one field, 3D is single eye */
    hi_u32 frame_rate;            /* input frame rate [24_hz,85] */
    hi_drv_color_sys color_sys;   /* ATV/CVBS color system */
    hi_drv_field_mode field_mode; /* VI display filed mode */
    hi_bool interlace;            /* interlace or progress */
} hi_vi_config;

typedef struct {
    hi_vi_type vi_type;              /* most support two 2D path, one 3D path */
    hi_vi_intf_mode intf_mode;       /* vi interface timming mode */
    hi_vi_access_type access_type;   /* vi access type */
    hi_vi_oversample over_sample;    /* over sample mode, 1X 2X 4X */
    hi_vi_picture_mode picture_mode; /** <The UI picture mode */
    hi_vi_config config;             /** < vi config can be dynamic changed */

    hi_drv_source source_type;        /* input source type */
    hi_drv_color_descript color_descript;   /* color space */
    hi_drv_3d_type video_3d_type;     /* 2D/3D */
    hi_drv_pixel_format pixel_format; /* pix format RGB444,YUV444/422 */
    hi_drv_pixel_bitwidth bit_width;  /* bit width 8/10/12BIT */
    hi_u32 vblank;                    /* A blanking zone width, FP for dealing with 3D format */
    hi_bool secure;                   /* Support secure path */
} hi_vi_attr;

typedef struct {
    hi_bool thread_exist;
    hi_bool thread_run;

    hi_u32 thread_pid;
    hi_u32 thread_tid;

    hi_u32 que_thread_cnt;
    hi_u32 deq_thread_cnt;
    hi_u32 que_thread_cost;
    hi_u32 deq_thread_cost;
    hi_u32 que_win_try;
    hi_u32 que_win_ok;
    hi_u32 deq_win_try;
    hi_u32 deq_win_ok;
    hi_u32 acq_try;
    hi_u32 acq_ok;
    hi_u32 rel_try;
    hi_u32 rel_ok;
} hi_vi_dbg_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_VI_STRUCT_H__ */
