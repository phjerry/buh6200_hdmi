/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: 用户态和内核态交付的数据结构类型
 * Author: sdk
 * Create: 2019-01-10
 */

#ifndef __DRV_TDE_TYPE_H__
#define __DRV_TDE_TYPE_H__

#include "hi_type.h"
#ifndef HI_BUILD_IN_BOOT
#include "hi_module.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hi_void (*drv_tde_func_callback)(hi_void *para1, hi_void *para2);

typedef enum {
    HI_TDE_COLOR_FMT_RGB444 = 0,
    HI_TDE_COLOR_FMT_BGR444,
    HI_TDE_COLOR_FMT_RGB555,
    HI_TDE_COLOR_FMT_BGR555,
    HI_TDE_COLOR_FMT_RGB565,
    HI_TDE_COLOR_FMT_BGR565,
    HI_TDE_COLOR_FMT_RGB888,
    HI_TDE_COLOR_FMT_BGR888,
    HI_TDE_COLOR_FMT_ARGB4444,
    HI_TDE_COLOR_FMT_ABGR4444,
    HI_TDE_COLOR_FMT_RGBA4444,
    HI_TDE_COLOR_FMT_BGRA4444,
    HI_TDE_COLOR_FMT_ARGB1555,
    HI_TDE_COLOR_FMT_ABGR1555,
    HI_TDE_COLOR_FMT_RGBA1555,
    HI_TDE_COLOR_FMT_BGRA1555,
    HI_TDE_COLOR_FMT_ARGB8565,
    HI_TDE_COLOR_FMT_ABGR8565,
    HI_TDE_COLOR_FMT_RGBA8565,
    HI_TDE_COLOR_FMT_BGRA8565,
    HI_TDE_COLOR_FMT_ARGB8888,
    HI_TDE_COLOR_FMT_ABGR8888,
    HI_TDE_COLOR_FMT_RGBA8888,
    HI_TDE_COLOR_FMT_BGRA8888,
    HI_TDE_COLOR_FMT_RABG8888,
    HI_TDE_COLOR_FMT_ARGB2101010,
    HI_TDE_COLOR_FMT_ARGB10101010,
    HI_TDE_COLOR_FMT_FP16,
    HI_TDE_COLOR_FMT_CLUT1,
    HI_TDE_COLOR_FMT_CLUT2,
    HI_TDE_COLOR_FMT_CLUT4,
    HI_TDE_COLOR_FMT_CLUT8,
    HI_TDE_COLOR_FMT_ACLUT44,
    HI_TDE_COLOR_FMT_ACLUT88,
    HI_TDE_COLOR_FMT_A1,
    HI_TDE_COLOR_FMT_A8,
    HI_TDE_COLOR_FMT_YCbCr888,
    HI_TDE_COLOR_FMT_AYCbCr8888,
    HI_TDE_COLOR_FMT_YCbCr422,
    HI_TDE_COLOR_FMT_byte,
    HI_TDE_COLOR_FMT_halfword,
    HI_TDE_COLOR_FMT_JPG_YCbCr400MBP,
    HI_TDE_COLOR_FMT_JPG_YCbCr422MBHP,
    HI_TDE_COLOR_FMT_JPG_YCbCr422MBVP,
    HI_TDE_COLOR_FMT_MP1_YCbCr420MBP,
    HI_TDE_COLOR_FMT_MP2_YCbCr420MBP,
    HI_TDE_COLOR_FMT_MP2_YCbCr420MBI,
    HI_TDE_COLOR_FMT_JPG_YCbCr420MBP,
    HI_TDE_COLOR_FMT_JPG_YCbCr444MBP,
    HI_TDE_COLOR_FMT_MAX
} hi_tde_color_fmt;

typedef enum {
    HI_TDE_MB_COLOR_FMT_JPG_YCbCr400MBP = 0,
    HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBHP,
    HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBVP,
    HI_TDE_MB_COLOR_FMT_MP1_YCbCr420MBP,
    HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBP,
    HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBI,
    HI_TDE_MB_COLOR_FMT_JPG_YCbCr420MBP,
    HI_TDE_MB_COLOR_FMT_JPG_YCbCr444MBP,
    HI_TDE_MB_COLOR_FMT_MAX
} hi_tde_mb_color_fmt;

typedef struct {
    hi_u64 phy_addr;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle y_mem_handle;
#endif
    hi_tde_color_fmt color_fmt;
    hi_u32 height;
    hi_u32 width;
    hi_u32 stride;
    hi_u64 clut_phy_addr;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle clut_mem_handle;
#endif
    hi_bool is_ycbcr_clut;
    hi_bool alpha_max_is_255;
    hi_bool support_alpha_ext_1555;
    hi_u8 alpha0;
    hi_u8 alpha1;
    hi_u64 cbcr_phy_addr;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle cbcr_mem_handle;
#endif
    hi_u32 cbcr_stride;
} hi_tde_surface;

typedef struct {
    hi_u64 y_addr;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle y_mem_handle;
#endif
    hi_u32 y_width;
    hi_u32 y_height;
    hi_u32 y_stride;
    hi_u64 cbcr_phy_addr;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle cbcr_mem_handle;
#endif
    hi_u32 cbcr_stride;
    hi_tde_mb_color_fmt mb_color_fmt;
} hi_tde_mb_surface;

typedef struct {
    hi_s32 pos_x;
    hi_s32 pos_y;
    hi_u32 width;
    hi_u32 height;
} hi_tde_rect;

typedef enum {
    HI_TDE_ALPHA_BLENDING_NONE = 0x0,
    HI_TDE_ALPHA_BLENDING_BLEND = 0x1,
    /* Alpha blending */ /* <CNcomment:Alpha混合 */
    HI_TDE_ALPHA_BLENDING_ROP = 0x2,
    /* ROP blending */ /* <CNcomment:进行rop叠加 */
    HI_TDE_ALPHA_BLENDING_COLORIZE = 0x4,
    /* Colorize operation */                                 /* <CNcomment:进行Colorize操作 */
    HI_TDE_ALPHA_BLENDING_MAX = 0x8 /* End of enumeration */ /* <CNcomment:枚举结束 */
} hi_tde_alpha_blending;

typedef enum {
    HI_TDE_ROP_BLACK = 0,   /* Blackness */
    HI_TDE_ROP_NOTMERGEPEN, /* ~(S2 | S1) */
    HI_TDE_ROP_MASKNOTPEN,  /* ~S2&S1    */
    HI_TDE_ROP_NOTCOPYPEN,  /* ~S2       */
    HI_TDE_ROP_MASKPENNOT,  /* S2&~S1    */
    HI_TDE_ROP_NOT,         /* ~S1       */
    HI_TDE_ROP_XORPEN,      /* S2^S1     */
    HI_TDE_ROP_NOTMASKPEN,  /* ~(S2 & S1) */
    HI_TDE_ROP_MASKPEN,     /* S2&S1     */
    HI_TDE_ROP_NOTXORPEN,   /* ~(S2^S1)  */
    HI_TDE_ROP_NOP,         /* S1        */
    HI_TDE_ROP_MERGENOTPEN, /* ~S2|S1    */
    HI_TDE_ROP_COPYPEN,     /* S2        */
    HI_TDE_ROP_MERGEPENNOT, /* S2|~S1    */
    HI_TDE_ROP_MERGEPEN,    /* S2|S1     */
    HI_TDE_ROP_WHITE,       /* Whiteness */
    HI_TDE_ROP_MAX
} hi_tde_rop_mode;

typedef enum {
    HI_TDE_MIRROR_NONE = 0,
    /* No mirror */ /* <CNcomment:不进行镜像 */
    HI_TDE_MIRROR_HORIZONTAL,
    /* Horizontal mirror */ /* <CNcomment:水平镜像 */
    HI_TDE_MIRROR_VERTICAL,
    /* Vertical mirror */ /* <CNcomment:垂直镜像 */
    HI_TDE_MIRROR_BOTH,
    /* Horizontal and vertical mirror */ /* <CNcomment:垂直和水平镜像 */
    HI_TDE_MIRROR_MAX
} hi_tde_mirror_mode;

typedef enum {
    HI_TDE_CLIP_MODE_NONE = 0,
    HI_TDE_CLIP_MODE_INSIDE,
    HI_TDE_CLIP_MODE_OUTSIDE,
    HI_TDE_CLIP_MODE_MAX
} hi_tde_clip_mode;

typedef enum {
    HI_TDE_MB_RESIZE_NONE = 0,
    HI_TDE_MB_RESIZE_QUALITY_LOW,
    HI_TDE_MB_RESIZE_QUALITY_MIDDLE,
    HI_TDE_MB_RESIZE_QUALITY_HIGH,
    HI_TDE_MB_RESIZE_MAX
} hi_tde_mb_resize;

typedef struct {
    hi_u32 color_value;
    hi_tde_color_fmt color_fmt;
} hi_tde_fill_color;

typedef enum {
    HI_TDE_COLOR_KEY_MODE_NONE = 0,
    HI_TDE_COLOR_KEY_MODE_FOREGROUND,
    HI_TDE_COLOR_KEY_MODE_BACKGROUND,
    HI_TDE_COLOR_KEY_MODE_MAX
} hi_tde_color_key_mode;

typedef struct {
    hi_u8 component_min; /* Minimum value of a component */ /* <CNcomment:分量最小值 */
    hi_u8 component_max; /* Maximum value of a component */ /* <CNcomment:分量最大值 */
    hi_u8 is_component_out;
    hi_u8 is_component_ignore; /* Whether to ignore a component */ /* <CNcomment:分量是否忽略 */
    hi_u8 component_mask; /* Component mask */                     /* <CNcomment:分量掩码 */
    hi_u8 component_reserved;
    hi_u8 component_reserved1;
    hi_u8 component_reserved2;
} hi_tde_color_key_comp;

typedef union {
    struct {
        hi_tde_color_key_comp alpha; /* alpha component */ /* <CNcomment:alpha 分量信息 */
        hi_tde_color_key_comp red; /* red component */     /* <CNcomment:红色分量信息 */
        hi_tde_color_key_comp green; /* green component */ /* <CNcomment:绿色分量信息 */
        hi_tde_color_key_comp blue; /* blue component */   /* <CNcomment:蓝色分量信息 */
    } argb_color_key;
    struct {
        hi_tde_color_key_comp alpha; /* alpha component */ /* <CNcomment:alpha 分量信息 */
        hi_tde_color_key_comp y; /* y component */         /* <CNcomment:Y分量信息 */
        hi_tde_color_key_comp cb; /* cb component */       /* <CNcomment:Cb分量信息 */
        hi_tde_color_key_comp cr; /* cr component */       /* <CNcomment:Cr分量信息 */
    } ycbcr_color_key;
    struct {
        hi_tde_color_key_comp alpha; /* alpha component */ /**<CNcomment:alpha 分量信息 */
        hi_tde_color_key_comp clut; /* clut component */   /**<CNcomment:调色板分量信息 */
    } clut_color_key;
} hi_tde_color_key;

typedef enum {
    HI_TDE_OUT_ALPHA_FROM_NORM = 0,
    HI_TDE_OUT_ALPHA_FROM_BACKGROUND,
    /* Output from the background bitmap */ /* <CNcomment:来源于背景位图 */
    HI_TDE_OUT_ALPHA_FROM_FOREGROUND,
    /* Output from the foreground bitmap */ /* <CNcomment:来源于前景位图 */
    HI_TDE_OUT_ALPHA_FROM_GLOBALALPHA,
    /* Output from the global alpha */ /* <CNcomment:来源于全局alpha */
    HI_TDE_OUT_ALPHA_FROM_MAX
} hi_tde_out_alpha_from;

typedef enum {
    HI_TDE_FILTER_MODE_COLOR = 0,
    /* Filter the color */ /* <CNcomment:对颜色进行滤波 */
    HI_TDE_FILTER_MODE_ALPHA,
    /* Filter the alpha channel */ /* <CNcomment:对alpha通道滤波 */
    HI_TDE_FILTER_MODE_BOTH,
    /* Filter the color and alpha channel */ /* <CNcomment:对颜色和alpha通道同时滤波 */
    HI_TDE_FILTER_MODE_NONE,
    /* No filter */ /* <CNcomment:不进行滤波 */
    HI_TDE_FILTER_MODE_MAX
} hi_tde_filter_mode;

typedef enum {
    HI_TDE_DEFLICKER_MODE_NONE = 0,
    HI_TDE_DEFLICKER_MODE_RGB,
    HI_TDE_DEFLICKER_MODE_BOTH,
    HI_TDE_DEFLICKER_MODE_MAX
} hi_tde_deflicker_mode;

typedef enum {
    HI_TDE_BLEND_ZERO = 0x0,
    HI_TDE_BLEND_ONE,
    HI_TDE_BLEND_SRC2COLOR,
    HI_TDE_BLEND_INVSRC2COLOR,
    HI_TDE_BLEND_SRC2ALPHA,
    HI_TDE_BLEND_INVSRC2ALPHA,
    HI_TDE_BLEND_SRC1COLOR,
    HI_TDE_BLEND_INVSRC1COLOR,
    HI_TDE_BLEND_SRC1ALPHA,
    HI_TDE_BLEND_INVSRC1ALPHA,
    HI_TDE_BLEND_SRC2ALPHASAT,
    HI_TDE_BLEND_MAX
} hi_tde_blend_mode;

typedef enum {
    HI_TDE_BLEND_CMD_NONE = 0x0, /* fs: sa      fd: 1.0-sa */
    HI_TDE_BLEND_CMD_CLEAR,      /* fs: 0.0     fd: 0.0    */
    HI_TDE_BLEND_CMD_SRC,        /* fs: 1.0     fd: 0.0    */
    HI_TDE_BLEND_CMD_SRCOVER,    /* fs: 1.0     fd: 1.0-sa */
    HI_TDE_BLEND_CMD_DSTOVER,    /* fs: 1.0-da  fd: 1.0    */
    HI_TDE_BLEND_CMD_SRCIN,      /* fs: da      fd: 0.0    */
    HI_TDE_BLEND_CMD_DSTIN,      /* fs: 0.0     fd: sa     */
    HI_TDE_BLEND_CMD_SRCOUT,     /* fs: 1.0-da  fd: 0.0    */
    HI_TDE_BLEND_CMD_DSTOUT,     /* fs: 0.0     fd: 1.0-sa */
    HI_TDE_BLEND_CMD_SRCATOP,    /* fs: da      fd: 1.0-sa */
    HI_TDE_BLEND_CMD_DSTATOP,    /* fs: 1.0-da  fd: sa     */
    HI_TDE_BLEND_CMD_ADD,        /* fs: 1.0     fd: 1.0    */
    HI_TDE_BLEND_CMD_XOR,        /* fs: 1.0-da  fd: 1.0-sa */
    HI_TDE_BLEND_CMD_DST,        /* fs: 0.0     fd: 1.0    */
    HI_TDE_BLEND_CMD_CONFIG,
    /* You can set the parameteres */ /* <CNcomment:用户自己配置参数 */
    HI_TDE_BLEND_CMD_MAX
} hi_tde_blend_cmd;

typedef struct {
    hi_bool global_alpha_en;
    hi_bool pixel_alpha_en;
    hi_bool src1_alpha_premulti;
    hi_bool src2_alpha_premulti;
    hi_tde_blend_cmd blend_cmd;
    hi_tde_blend_mode src1_blend_mode;
    hi_tde_blend_mode src2_blend_mode;
} hi_tde_blend_opt;

typedef struct {
    hi_bool src_csc_user_en;
    hi_bool src_csc_param_reload_en;
    hi_bool dst_csc_user_en;
    hi_bool dst_csc_param_reload_en;
    hi_u64 src_csc_param_addr;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle src_csc_param_mem_handle;
#endif
    hi_u32 src_csc_param_size;
    hi_u64 dst_csc_param_addr;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle dst_csc_param_mem_handle;
#endif
    hi_u32 dst_csc_param_size;
} hi_tde_csc_opt;

typedef struct {
    hi_tde_alpha_blending alpha_blending_cmd;
    hi_tde_rop_mode rop_color;
    hi_tde_rop_mode rop_alpha;
    hi_tde_color_key_mode color_key_mode;
    hi_tde_color_key color_key_value;
    hi_tde_clip_mode clip_mode;
    hi_tde_rect clip_rect;
    hi_tde_deflicker_mode deflicker_mode;
    hi_bool resize;
    hi_tde_filter_mode filter_mode;
    hi_tde_mirror_mode mirror;
    hi_bool clut_reload;
    hi_u8 global_alpha;
    hi_tde_out_alpha_from out_alpha_from;
    hi_u32 color_resize;
    hi_tde_blend_opt blend_opt;
    hi_tde_csc_opt csc_opt;
} hi_tde_opt;

typedef struct {
    hi_tde_clip_mode clip_mode;
    hi_tde_rect clip_rect;
    hi_bool is_deflicker;
    hi_tde_mb_resize resize_en;
    hi_bool is_set_out_alpha;
    hi_u8 out_alpha;
} hi_tde_mb_opt;

typedef struct {
    hi_tde_alpha_blending alpha_blending_cmd;
    hi_tde_rop_mode rop_color;
    hi_tde_rop_mode rop_alpha;
    hi_tde_color_key_mode color_key_mode;
    hi_tde_color_key color_key_value;
    hi_tde_clip_mode clip_mode;
    hi_tde_rect clip_rect;
    hi_bool clut_reload;
    hi_u8 global_alpha;
    hi_tde_out_alpha_from out_alpha_from;
    hi_u32 color_resize;
    hi_tde_blend_opt blend_opt;
    hi_tde_csc_opt csc_opt;
} hi_tde_pattern_fill_opt;

typedef enum {
    HI_TDE_DEFLICKER_LEVEL_AUTO = 0,
    HI_TDE_DEFLICKER_LEVEL_LOW,
    HI_TDE_DEFLICKER_LEVEL_MIDDLE,
    HI_TDE_DEFLICKER_LEVEL_HIGH,
    HI_TDE_DEFLICKER_LEVEL_MAX
} hi_tde_deflicker_level;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_TDE_TYPE_H__ */
