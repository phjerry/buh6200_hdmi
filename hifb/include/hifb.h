/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: extended interface based on fb.h, define 110 111 cmd in hifb_liteos.h
 * Create: 2019-06-29
 */

#ifndef __HIFB_H__
#define __HIFB_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* * \addtogroup  HIFB      */
/* * @{ */ /* * <!-- [HIFB] */

#define HIFB_IOC_TYPE 'F'
#define HIFB_LOWPOWER_PARA_NUM 17

#define HIFBIOGET_SHOW _IOR(HIFB_IOC_TYPE, 90, hi_bool)
#define HIFBIOPUT_SHOW _IOW(HIFB_IOC_TYPE, 91, hi_bool)

#define HIFBIOGET_DECOMPRESS _IOR(HIFB_IOC_TYPE, 92, hi_bool)
#define HIFBIOPUT_DECOMPRESS _IOW(HIFB_IOC_TYPE, 93, hi_bool)

#define HIFBIOGET_COLORKEY _IOR(HIFB_IOC_TYPE, 94, hifb_colorkey)
#define HIFBIOPUT_COLORKEY _IOW(HIFB_IOC_TYPE, 95, hifb_colorkey)

#define HIFBIOGET_ALPHA _IOR(HIFB_IOC_TYPE, 96, hifb_alpha)
#define HIFBIOPUT_ALPHA _IOW(HIFB_IOC_TYPE, 97, hifb_alpha)

#define HIFBIOGET_SCREENPOS _IOR(HIFB_IOC_TYPE, 98, hifb_position)
#define HIFBIOPUT_SCREENPOS _IOW(HIFB_IOC_TYPE, 99, hifb_position)

#define HIFBIOGET_VSCREENINFO _IOR(HIFB_IOC_TYPE, 112, hifb_vscreeninfo)
#define HIFBIOPUT_VSCREENINFO _IOW(HIFB_IOC_TYPE, 113, hifb_vscreeninfo)

#define HIFBIOGET_LAYERINFO _IOR(HIFB_IOC_TYPE, 114, hifb_layerinfo)
#define HIFBIOPUT_LAYERINFO _IOW(HIFB_IOC_TYPE, 115, hifb_layerinfo)

#define HIFBIO_SCROLLTEXT_CREATE _IOW(HIFB_IOC_TYPE, 130, hifb_scrolltext_attr)
#define HIFBIO_SCROLLTEXT_FILL _IOW(HIFB_IOC_TYPE, 131, hifb_scrolltext_data)
#define HIFBIO_SCROLLTEXT_PAUSE _IOW(HIFB_IOC_TYPE, 132, hi_u32)
#define HIFBIO_SCROLLTEXT_RESUME _IOW(HIFB_IOC_TYPE, 133, hi_u32)
#define HIFBIO_SCROLLTEXT_DESTORY _IOW(HIFB_IOC_TYPE, 134, hi_u32)

#define HIFBIOPUT_STEREODEPTH _IOW(HIFB_IOC_TYPE, 150, hi_s32)
#define HIFBIOGET_STEREODEPTH _IOR(HIFB_IOC_TYPE, 151, hi_s32)

#define HIFBIOPUT_REFRESH _IOW(HIFB_IOC_TYPE, 170, hifb_flush_info)
#define HIFBIOGET_VBLANK _IOR(HIFB_IOC_TYPE, 171, hifb_vblank_info)

#define HIFBIOGET_DMABUF _IOWR(HIFB_IOC_TYPE, 172, hifb_dmabuf_export)

/* * @} */ /* * <!-- ==== Macro Definition end ==== */

/* * \addtogroup     HIFB   */
/* * @{ */ /* * <!-- [HIFB] */

/* color format */
typedef enum {
    HIFB_COLOR_FMT_RGB565 = 0x0,
    HIFB_COLOR_FMT_RGB888,
    HIFB_COLOR_FMT_ARGB4444,
    HIFB_COLOR_FMT_ARGB1555,
    HIFB_COLOR_FMT_ARGB8888,

    HIFB_COLOR_FMT_RGBA4444 = 0x10,
    HIFB_COLOR_FMT_RGBA5551,
    HIFB_COLOR_FMT_RGBA8888,

    HIFB_COLOR_FMT_ABGR4444 = 0x20,
    HIFB_COLOR_FMT_ABGR1555,
    HIFB_COLOR_FMT_ABGR8888,

    HIFB_COLOR_FMT_BGR565 = 0x30,
    HIFB_COLOR_FMT_BGR888,
    HIFB_COLOR_FMT_BGRA4444,
    HIFB_COLOR_FMT_BGRA5551,
    HIFB_COLOR_FMT_BGRA8888,

    HIFB_COLOR_FMT_CLUT1 = 0x40,
    HIFB_COLOR_FMT_CLUT2,
    HIFB_COLOR_FMT_CLUT4,
    HIFB_COLOR_FMT_CLUT8,
    HIFB_COLOR_FMT_ACLUT44,
    HIFB_COLOR_FMT_ACLUT88,

    HIFB_COLOR_FMT_FP16 = 0x50,
    HIFB_COLOR_FMT_ARGB2101010,
    HIFB_COLOR_FMT_ARGB10101010,

    HIFB_COLOR_FMT_MAX
} hifb_color_fmt;

/* updata buffer mode */
typedef enum {
    HIFB_BUFFER_NONE = 0x0, /* no display buffer, display used canvas buffer */
    HIFB_BUFFER_ONE,        /* one display buffer in hifb */
    HIFB_BUFFER_DOUBLE,     /* two display buffer in hifb */
    HIFB_BUFFER_MAX
} hifb_buffer_mode;

typedef enum {
    HIFB_HDR_SDR = 0,
    HIFB_HDR_HDR10,
    HIFB_HDR_HLG,
    HIFB_HDR_MAX
} hifb_xdr_mode;

typedef enum {
    HIFB_COLOR_SPACE_BT709,
    HIFB_COLOR_SPACE_BT2020,
    HIFB_COLOR_SPACE_MAX
} hifb_color_space;

typedef enum {
    HIFB_CMP_NONE = 0,
    HIFB_CMP_HFBC,
    HIFB_CMP_AFBC,
    HIFB_CMP_MAX
} hifb_compress_mode;

/* rect information */
typedef struct {
    hi_s32 x, y;
    hi_s32 w, h;
} hifb_rect;

/* colorkey information */
typedef struct {
    hi_bool enable;
    hi_u32 key_value;
} hifb_colorkey;

/* alpha information */
typedef struct {
    hi_bool pixel_alpha_en;
    hi_bool global_alpha_en;
    hi_u8 alpha0; /* be used in argb1555, pixel alpha = 0 will be replace by alpha0 value */
    hi_u8 alpha1; /* be used in argb1555, pixel alpha = 1 will be replace by alpha1 value */
    hi_u8 global_alpha;
} hifb_alpha;

/* display position, be used for cursor */
typedef struct {
    hi_s32 x;
    hi_s32 y;
} hifb_position;

/* input canvas information */
typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 mem_size;
    hi_mem_handle mem_handle;
    hifb_color_fmt fmt;
    hifb_buffer_mode buf_mode;
} hifb_vscreeninfo;

/* scrolltext information */
typedef struct {
    hi_u32 handle;
    hi_u32 cache_num;
    hifb_rect rect;
    hifb_color_fmt fmt;
} hifb_scrolltext_attr;

typedef struct {
    hi_u32 handle;
    hi_u32 stride;
    hi_u32 mem_size;
    hi_mem_handle mem_handle;
} hifb_scrolltext_data;

/* refresh surface info */
typedef struct {
    hifb_rect updata_rect;
} hifb_flush_info;

/* vblank information */
typedef enum {
    HIFB_VSYNC_UPDATA, /* updata interrupt */
    HIFB_VSYNC_FINISH, /* finish interrupt */
    HIFB_VSYNC_MAX
} hifb_vsync_type;

typedef struct {
    hi_u64 tv_sec;  /* seconds */
    hi_u64 tv_msec; /* milliseconds */
    hi_u64 tv_usec; /* microseconds */
} hifb_timeval;

typedef struct {
    hifb_timeval time_val;
    hifb_vsync_type vsync_type;
} hifb_vblank_info;

/* layer information, only used in need sync scen */
typedef struct {
    hi_bool enable;
    hi_u32 mem_size;
    hi_mem_handle mem_handle;
    hi_handle win_handle;
    hifb_rect video_rect;
    hifb_rect output_rect;
} hifb_sync_info;

typedef struct {
    hi_bool enable;
    hi_u32 lowpower_value[HIFB_LOWPOWER_PARA_NUM];
} hifb_lowpower_info;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 cmp_stride;
    hi_u32 mem_size;
    hi_mem_handle mem_handle;
    hifb_color_fmt fmt;
} hifb_surface;

typedef struct {
    hi_bool is_premulti;
    hi_bool is_stereo;
    hi_u32 alpha_sum;
    hi_s32 acquire_fd;
    hi_s32 release_fd;
    hifb_surface surface;
    hifb_xdr_mode xdr_mode;
    hifb_color_space color_space;
    hifb_compress_mode compress_mode;
    hifb_lowpower_info lowpower_info; /* lowpower info from gfx2d */
    hifb_sync_info sync_info;         /* used in video and ui sync scen */
} hifb_layerinfo;

typedef struct {
    hi_mem_handle mem_handle;
    hi_u32 flags;
} hifb_dmabuf_export;

/* * @} */ /* * <!-- ==== Structure Definition end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HIFB_H__ */
