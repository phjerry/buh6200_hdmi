/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: err msg
 * Author: data structure
 * Create: 2019-02-15
 */
#ifndef _DRV_GFX2D_STRUCT_H_
#define _DRV_GFX2D_STRUCT_H_

#include "hi_type.h"
#include "hi_module.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_GFX2D_MAX_SURFACE_NUM 3
#define DRV_GFX2d_FILTERPARAM_ADDR_CNT 2
#define DRV_GFX2D_MAX_ALPHASUM 17
#define DRV_GFX2D_MAX_HOLES_NUM 4

/* * \addtogroup      GFX TYPE */
/* * @{ */ /* * <!-- 【GFX TYPE】 */

typedef enum {
    DRV_GFX2D_DEV_ID_0 = 0x0,
    DRV_GFX2D_DEV_ID_BUTT
} drv_gfx2d_dev_id;

/* enum of the color format */
/* CNcomment:图像格式枚举 */
typedef enum {
    DRV_GFX2D_FMT_RGB444 = 0, /* < RGB444格式, Red占4bits Green占4bits, Blue占4bits, 其余格式依此类推 */
    DRV_GFX2D_FMT_BGR444,     /* < BGR444格式 */
    DRV_GFX2D_FMT_RGB555,     /* < RGB555格式 */
    DRV_GFX2D_FMT_BGR555,     /* < BGR555格式 */
    DRV_GFX2D_FMT_RGB565,     /* < RGB565格式 */
    DRV_GFX2D_FMT_BGR565,     /* < BGR565格式 */
    DRV_GFX2D_FMT_RGB888,     /* < RGB888格式 */
    DRV_GFX2D_FMT_BGR888,     /* < BGR888格式 */
    DRV_GFX2D_FMT_KRGB888,
    DRV_GFX2D_FMT_KBGR888,
    DRV_GFX2D_FMT_ARGB4444,     /* < ARGB4444格式 */
    DRV_GFX2D_FMT_ABGR4444,     /* < ABGR4444格式 */
    DRV_GFX2D_FMT_RGBA4444,     /* < RGBA4444格式 */
    DRV_GFX2D_FMT_BGRA4444,     /* < BGRA4444格式 */
    DRV_GFX2D_FMT_ARGB1555,     /* < ARGB1555格式 */
    DRV_GFX2D_FMT_ABGR1555,     /* < ABGR1555格式 */
    DRV_GFX2D_FMT_RGBA1555,     /* < RGBA1555格式 */
    DRV_GFX2D_FMT_BGRA1555,     /* < BGRA1555格式 */
    DRV_GFX2D_FMT_ARGB8565,     /* < ARGB8565格式 */
    DRV_GFX2D_FMT_ABGR8565,     /* < ABGR8565格式 */
    DRV_GFX2D_FMT_RGBA8565,     /* < RGBA8565格式 */
    DRV_GFX2D_FMT_BGRA8565,     /* < BGRA8565格式 */
    DRV_GFX2D_FMT_ARGB8888,     /* < ARGB8888格式 */
    DRV_GFX2D_FMT_ABGR8888,     /* < ABGR8888格式 */
    DRV_GFX2D_FMT_RGBA8888,     /* < RGBA8888格式 */
    DRV_GFX2D_FMT_BGRA8888,     /* < BGRA8888格式 */
    DRV_GFX2D_FMT_ARGB2101010,  /* < ARGB2101010格式 */
    DRV_GFX2D_FMT_ARGB10101010, /* < ARGB10101010格式 */
    DRV_GFX2D_FMT_FP16,         /* < FP16格式 */
    DRV_GFX2D_FMT_CLUT1,        /* < 无Alpha分量, 调色板1bit格式,每个像用1个bit表示 */
    DRV_GFX2D_FMT_CLUT2,        /* < 无Alpha分量, 调色板2bit格式,每个像用2个bit表示 */
    DRV_GFX2D_FMT_CLUT4,        /* < 无Alpha分量, 调色板4bit格式,每个像用4个bit表示 */
    DRV_GFX2D_FMT_CLUT8,        /* < 无Alpha分量, 调色板8bit格式,每个像用8个bit表示 */
    DRV_GFX2D_FMT_ACLUT44,      /* < 有Alpha分量, 调色板1bit格式,每个像用1个bit表示 */
    DRV_GFX2D_FMT_ACLUT88,      /* < 有Alpha分量, 调色板1bit格式,每个像用1个bit表示 */
    DRV_GFX2D_FMT_YUV888,       /* < YUV packet格式, 无alpha分量 */
    DRV_GFX2D_FMT_AYUV8888,     /* < YUV packet格式, 有alpha分量 */
    DRV_GFX2D_FMT_YUYV422,
    /* < YUV packet422 format */ /* <CNcomment:YUV packet422格式 */
    DRV_GFX2D_FMT_YVYU422,
    /* < YUV packet422 format */ /* <CNcomment:YUV packet422格式 */
    DRV_GFX2D_FMT_UYVY422,
    /* < YUV packet422 format */ /* <CNcomment:YUV packet422格式 */
    DRV_GFX2D_FMT_YYUV422,
    /* < YUV packet422 format */ /* <CNcomment:YUV packet422格式 */
    DRV_GFX2D_FMT_VYUY422,
    /* < YUV packet422 format */ /* <CNcomment:YUV packet422格式 */
    DRV_GFX2D_FMT_VUYY422,
    /* < YUV packet422 format */     /* <CNcomment:YUV packet422格式 */
    DRV_GFX2D_FMT_SEMIPLANAR400,     /* < Semi-planar YUV400格式, 对应于JPG解码 */
    DRV_GFX2D_FMT_SEMIPLANAR420UV,   /* < Semi-planar YUV420格式 */
    DRV_GFX2D_FMT_SEMIPLANAR420VU,   /* < Semi-planar YUV420格式 */
    DRV_GFX2D_FMT_SEMIPLANAR422UV_H, /* < Semi-planar YUV422格式, 水平方向采样, 对应于JPG解码 */
    DRV_GFX2D_FMT_SEMIPLANAR422VU_H, /* < Semi-planar YUV422格式, 水平方向采样, 对应于JPG解码 */
    DRV_GFX2D_FMT_SEMIPLANAR422UV_V, /* < Semi-planar YUV422格式, 垂直方向采样, 对应于JPG解码 */
    DRV_GFX2D_FMT_SEMIPLANAR422VU_V, /* < Semi-planar YUV422格式, 垂直方向采样, 对应于JPG解码 */
    DRV_GFX2D_FMT_SEMIPLANAR444UV,   /* < Semi-planar YUV444格式 */
    DRV_GFX2D_FMT_SEMIPLANAR444VU,
    DRV_GFX2D_FMT_PLANAR400,
    DRV_GFX2D_FMT_PLANAR420,
    DRV_GFX2D_FMT_PLANAR411,
    DRV_GFX2D_FMT_PLANAR410,
    DRV_GFX2D_FMT_PLANAR422H,
    DRV_GFX2D_FMT_PLANAR422V,
    DRV_GFX2D_FMT_PLANAR444,
    DRV_GFX2D_FMT_MAX
} drv_gfx2d_compose_fmt;

/* enum of the compress type */
/* CNcomment:图象压缩类型枚举 */
typedef enum {
    DRV_GFX2D_COMPRESS_NONE = 0x0,
    DRV_GFX2D_COMPRESS_HFBC,
    DRV_GFX2D_COMPRESS_AFBC,
    DRV_GFX2D_COMPRESS_MAX
} drv_gfx2d_compress_type;

/* structure of compress info */
/* CNcomment:压缩信息结构体 */
typedef struct {
    drv_gfx2d_compress_type compress_type;
} drv_gfx2d_compress_msg;

typedef enum {
    DRV_GFX2D_HDR_MODE_SDR,
    DRV_GFX2D_HDR_MODE_HDR10,
    DRV_GFX2D_HDR_MODE_HLG,
    DRV_GFX2D_HDR_MODE_MAX
} drv_gfx2d_xdr_mode;

typedef enum {
    DRV_GFX2D_COLOR_SPACE_BT709,
    DRV_GFX2D_COLOR_SPACE_BT2020,
    DRV_GFX2D_COLOR_SPACE_MAX
} drv_gfx2d_color_space;

/* enum of the surface type */
/* CNcomment:surface类型枚举 */
typedef enum {
    DRV_GFX2D_SURFACE_TYPE_MEM = 0x0, /* <CNcomment:内存类型surface */
    DRV_GFX2D_SURFACE_TYPE_COLOR,     /* <CNcomment:纯色类型surface, 不需要等fence之类, 无内存操作 */
} drv_gfx2d_compose_surface_type;

/* enum of the filter mode */
/* CNcomment:滤波模式枚举 */
typedef enum {
    DRV_GFX2D_FILTER_ALL = 0x0, /* <CNcomment:对颜色和alpha通道同时滤波 */
    DRV_GFX2D_FILTER_COLOR,     /* <CNcomment:对颜色进行滤波            */
    DRV_GFX2D_FILTER_ALPHA,     /* <CNcomment:对alpha通道滤波           */
    DRV_GFX2D_FILTER_NONE,      /* <CNcomment:不进行滤波, 使用复制模式, 可能有锯齿, 对哪个做滤波看效果调试 */
} drv_gfx2d_compose_filter_mode;

/* enum of the clip mode */
/* CNcomment:剪切模式枚举 */
typedef enum {
    DRV_GFX2D_CLIP_NONE = 0x0,
    DRV_GFX2D_CLIP_INSIDE,
    DRV_GFX2D_CLIP_OUTSIDE,
} drv_gfx2d_compose_clip_mode;

/* enum of the rop mode */
/* CNcomment:ROP模式枚举 */
typedef enum {
    DRV_GFX2D_ROP_BLACK = 0x0, /* < Blackness */
    DRV_GFX2D_ROP_NOTMERGEPEN, /* < ~(S2 | S1) */
    DRV_GFX2D_ROP_MASKNOTPEN,  /* < ~S2&S1 */
    DRV_GFX2D_ROP_NOTCOPYPEN,  /* < ~S2 */
    DRV_GFX2D_ROP_MASKPENNOT,  /* < S2&~S1 */
    DRV_GFX2D_ROP_NOT,         /* < ~S1 */
    DRV_GFX2D_ROP_XORPEN,      /* < S2^S1 */
    DRV_GFX2D_ROP_NOTMASKPEN,  /* < ~(S2 & S1) */
    DRV_GFX2D_ROP_MASKPEN,     /* < S2&S1 */
    DRV_GFX2D_ROP_NOTXORPEN,   /* < ~(S2^S1) */
    DRV_GFX2D_ROP_NOP,         /* < S1 */
    DRV_GFX2D_ROP_MERGENOTPEN, /* < ~S2|S1 */
    DRV_GFX2D_ROP_COPYPEN,     /* < S2 */
    DRV_GFX2D_ROP_MERGEPENNOT, /* < S2|~S1 */
    DRV_GFX2D_ROP_MERGEPEN,    /* < S2|S1 */
    DRV_GFX2D_ROP_WHITE,       /* < Whiteness */
} drv_gfx2d_compose_rop_mode;

/* structure of rect */
/* CNcomment:矩形结构体 */
typedef struct {
    hi_s32 x;      /* <CNcomment:矩形的左上点横坐标 */
    hi_s32 y;      /* <CNcomment:矩形的左上点纵坐标 */
    hi_u32 width;  /* <CNcomment:矩形的宽 */
    hi_u32 height; /* <CNcomment:矩形的高 */
} drv_gfx2d_compose_rect;

/* structure of alpha expand info */
/* CNcomment:ARGB1555格式alpha扩展信息结构体 */
typedef struct {
    hi_u8 alpha0;         /* <CNcomment:采用扩展alpha时, 0对应的alpha值. */
    hi_u8 alpha1;         /* <CNcomment:采用扩展alpha时, 1对应的alpha值. */
    hi_bool alpha_ext_en; /* <CNcomment:是否采用扩展alpha. */
} drv_gfx2d_compose_alpha_ext;

typedef struct {
    hi_u32 alpha_sum[DRV_GFX2D_MAX_ALPHASUM];
} drv_gfx2d_compose_ext_info;

/* structure of memory surface */
/* CNcomment:内存类型surface信息结构体 */
typedef struct {
    drv_gfx2d_compose_surface_type surface_type; /* <CNcomment:surface类型                     */
    drv_gfx2d_compose_fmt format;                /* <CNcomment:像素格式,只对MEM类型surface有效 */
    hi_u32 width;                                /* <CNcomment:宽度                            */
    hi_u32 height;                               /* <CNcomment:高度                            */
    hi_u32 color_data;                           /* <CNcomment:surface颜色值, 只对COLOR类型surface有效 */
    /* <CNcomment:内存地址信息,只对mem类型的surface有效
     * package格式:     phy_addr[0]表示数据地址
     * semi-planar格式: phy_addr[0]表示Y分量地址, phy_addr[1]表示CbCr分量地址
     * planar格式:      phy_addr[0]表示Y分量地址, phy_addr[1]表示Cb分量地址, phy_addr[2]表示Cr分量地址
     */
    hi_u64 phy_addr[DRV_GFX2D_MAX_SURFACE_NUM];
    hi_mem_handle mem_handle[DRV_GFX2D_MAX_SURFACE_NUM];
    /* <CNcomment:行跨度,只对mem类型的surface有效
     * package格式:     stride[0]表示数据跨度
     * semi-planar格式: stride[0]表示Y分量行跨度, stride[1]表示CbCr分量行跨度
     * planar格式:      stride[0]表示Y分量行跨度, stride[1]表示Cb分量行跨度, stride[2]表示Cr分量行跨度
     */
    hi_u32 stride[DRV_GFX2D_MAX_SURFACE_NUM];
    hi_u32 size[DRV_GFX2D_MAX_SURFACE_NUM];
    /* <CNcomment:给ARGB乘于一个alpha(像素alpha值), 是否为预乘数据, 非预乘数据做预乘, 预乘数据不做预乘, 和GPU保持一致 */
    hi_bool premulti_en;
    drv_gfx2d_compress_msg compress_msg; /* <CNcomment:压缩信息, 只对MEM类型surface有效 */
    hi_u64 histogram_phy_addr;           /* <CNcomment:直方图物理地址, 只对MEM类型surface有效, 暂时没有使用         */
    hi_mem_handle histogram_mem_handle;
    hi_u64 histogram_phy_addr_size;
    drv_gfx2d_compose_alpha_ext alpha_ext; /* <CNcomment:alpha扩展信息, 只对MEM类型surface的ARGB1555格式有效          */
    hi_u64 palette_phy_addr;               /* <CNcomment:调色板地址,只对MEM类型surface的调色板格式有效, HWC逻辑不支持 */
    hi_mem_handle palette_mem_handle;
    hi_u32 palette_phy_addr_size;
    hi_s32 acquire_fence_fd;
    hi_s32 release_fence_fd;
    drv_gfx2d_compose_ext_info ext_info;
    drv_gfx2d_xdr_mode xdr_mode;
    drv_gfx2d_color_space color_space;
    hi_bool is_secure;
} drv_gfx2d_compose_surface;

/* structure of surface */
/* CNcomment:RGB格式colorkey信息结构体 */
typedef struct {
    hi_bool alpha_en;
    hi_bool alpha_out;
    hi_u8 alpha_min;
    hi_u8 alpha_max;
    hi_u8 alpha_mask;

    hi_bool red_en;
    hi_bool red_out;
    hi_u8 red_min;
    hi_u8 red_max;
    hi_u8 red_mask;

    hi_bool green_en;
    hi_bool green_out;
    hi_u8 green_min;
    hi_u8 green_max;
    hi_u8 green_mask;

    hi_bool blue_en;
    hi_bool blue_out;
    hi_u8 blue_min;
    hi_u8 blue_max;
    hi_u8 blue_mask;
} drv_gfx2d_compose_colorkey_rgb;

/* structure of surface */
/* CNcomment:clut格式 colorkey信息结构体 */
typedef struct {
    hi_bool alpha_en;
    hi_bool alpha_out;
    hi_u8 alpha_min;
    hi_u8 alpha_max;
    hi_u8 alpha_mask;

    hi_bool clut_en;
    hi_bool clut_out;
    hi_u8 clut_min;
    hi_u8 clut_max;
    hi_u8 clut_mask;
} drv_gfx2d_compose_colorkey_clut;

typedef union {
    drv_gfx2d_compose_colorkey_rgb colorkey_rgb;
    drv_gfx2d_compose_colorkey_clut colorkey_clut;
} drv_gfx2d_compose_colorkey_type;

/* structure of color space */
/* CNcomment:颜色空间转换信息结构体 */
typedef struct {
    hi_bool user_para_en;       /* <CNcomment:是否使用用户自定义系数 */
    hi_u32 user_para_phy_addr;  /* <CNcomment:用户自定义系数物理地址 */
    hi_u32 user_para_addr_size; /* <CNcomment:用户自定义系数物理地址大小 */
} drv_gfx2d_compose_csc;

/* structure of filter info */
/* CNcomment:滤波信息结构体 */
typedef struct {
    drv_gfx2d_compose_filter_mode filter_mode;
} drv_gfx2d_compose_filter;

/* structure of clip info */
/* CNcomment:剪切域信息结构体 */
typedef struct {
    drv_gfx2d_compose_clip_mode clip_mode; /* <CNcomment:剪切模式 */
    drv_gfx2d_compose_rect clip_rect;      /* <CNcomment:剪切矩形 */
} drv_gfx2d_compose_clip;

/* structure of resize info */
/* CNcomment:缩放信息结构体 */
typedef struct {
    hi_bool resize_en;               /* <CNcomment:是否使能缩放. */
    drv_gfx2d_compose_filter filter; /* <CNcomment:滤波信息.     */
} drv_gfx2d_compose_resize;

/* structure of alpha blend info */
/* CNcomment:叠加信息结构体 */
typedef struct {
    hi_bool cov_blend;      /* CNcomment: 当前层是否和下一层做叠加, TRUE: 不做叠加, 当前层直接覆盖下一层, 6是最上层 * */
    hi_bool pixel_alpha_en; /* CNcomment: alpha参与像素点的分量运算     */
    hi_bool global_alpha_en; /* CNcomment: 全局alpha参与像素点的分量运算 */
    hi_u8 global_alpha;      /* CNcomment: 全局alpha值. */
} drv_gfx2d_compose_blend;

/* structure of rop info */
/* CNcomment:rop信息结构体 */
typedef struct {
    hi_bool enable;
    drv_gfx2d_compose_rop_mode alpha_rop_mode;
    drv_gfx2d_compose_rop_mode rgb_rop_mode;
} drv_gfx2d_compose_rop;

/* structure of key info */
/* CNcomment:colorkey信息结构体 */
typedef struct {
    hi_bool enable;
    drv_gfx2d_compose_colorkey_type colorkey_type;
} drv_gfx2d_compose_colorkey;

typedef struct {
    hi_u32 holes_cnt;
    drv_gfx2d_compose_rect holes_rect[DRV_GFX2D_MAX_HOLES_NUM];
    hi_u32 argb;
} drv_gfx2d_compose_holes;

/* structure of operation info */
/* CNcomment:操作选项信息结构体 */
typedef struct {
    drv_gfx2d_compose_blend blend;       /* <CNcomment:alpha叠加信息.                */
    drv_gfx2d_compose_resize resize;     /* <CNcomment:缩放信息.                     */
    drv_gfx2d_compose_clip clip;         /* <CNcomment:剪切域信息. HWC逻辑现在不支持 */
    drv_gfx2d_compose_rop rop;           /* <CNcomment:ROP信息, HWC逻辑现在不支持    */
    drv_gfx2d_compose_colorkey colorkey; /* <CNcomment:key信息, HWC逻辑现在不支持    */
    drv_gfx2d_compose_holes holes;
} drv_gfx2d_compose_opt;

/* structure of composed surface and operation info */
/* CNcomment:多层叠加surface及操作选项结构体 */
typedef struct {
    drv_gfx2d_compose_surface compose_surface; /* <CNcomment:surface信息.  */
    drv_gfx2d_compose_rect in_rect;            /* <CNcomment:输入操作矩形. */
    drv_gfx2d_compose_rect out_rect;           /* <CNcomment:输出矩形.     */
    drv_gfx2d_compose_opt opt;                 /* <CNcomment:操作选项.     */
} drv_gfx2d_compose_msg;

/* composed surface list */
typedef struct {
    hi_u32 background_color;        /* <CNcomment: 叠加背景色, 软件只需配置一个背景色, 虚拟层, 在最下层 */
    hi_u32 compose_cnt;             /* <CNcomment: 叠加层任务数量, 和compose大小相对应                  */
    drv_gfx2d_compose_msg *compose; /* <CNcomment: 叠加层任务, 按照由下到上的顺序排列, 0在最下面        */
    hi_u32 compose_size;            /* <CNcomment: compose指针大小                                      */
} drv_gfx2d_compose_list;

/* * @} */ /* ! <!-- Structure Definition end */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _DRV_GFX2D_STRUCT_H_ */
