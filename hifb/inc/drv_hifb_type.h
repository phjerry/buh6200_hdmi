/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hifb data struct
 * Create: 2019-06-29
 */

#ifndef __DRV_HIFB_TYPE_H__
#define __DRV_HIFB_TYPE_H__

#include <linux/fb.h>
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT 17

typedef struct {
    hi_u32 u32Width;
    hi_u32 u32Height;
} HIFB_SIZE_S;

typedef enum {
    HIFB_LAYER_HD_0 = 0x0,
    HIFB_LAYER_HD_1,
    HIFB_LAYER_HD_2,
    HIFB_LAYER_HD_3,
    HIFB_LAYER_SD_0,
    HIFB_LAYER_SD_1,
    HIFB_LAYER_ID_BUTT
} HIFB_LAYER_ID_E;

typedef struct {
    hi_bool bKeyEnable;
    hi_u32 u32Key;
} HIFB_COLORKEY_S;

typedef struct {
    hi_s32 x, y;
    hi_s32 w, h;
} HIFB_RECT;

typedef struct {
    hi_s32 l;
    hi_s32 t;
    hi_s32 r;
    hi_s32 b;
} HIFB_REGION;

typedef struct {
    hi_s32 s32XPos;
    hi_s32 s32YPos;
} HIFB_POINT_S;

typedef struct {
    hi_u32 u32HDfLevel; /**<  horizontal deflicker level */                   /**<CNcomment:水平抗闪烁级别 */
    hi_u32 u32VDfLevel; /**<  vertical deflicker level */                     /**<CNcomment:垂直抗闪烁级别 */
    hi_u8 *pu8HDfCoef; /**<  horizontal deflicker coefficient */              /**<CNcomment:水平抗闪烁系数 */
    hi_u32 u32HDfCoefLen; /**<  horizontal deflicker coefficient buff size */ /**<CNcomment:水平抗闪烁系数buffer大小 */
    hi_u8 *pu8VDfCoef; /**<  vertical deflicker coefficient */                /**<CNcomment:垂直抗闪烁系数 */
    hi_u32 u32VDfCoefLen; /**<  vertical deflicker coefficient buff size*/    /**<CNcomment:垂直抗闪烁系数buffer大小 */
} HIFB_DEFLICKER_S;

typedef struct {
    hi_bool bAlphaEnable; /**<  alpha enable flag */          /**<CNcomment:alpha使能标识*/
    hi_bool bAlphaChannel; /**<  alpha channel enable flag */ /**<CNcomment:alpha通道使能*/
    hi_u8 u8Alpha0; /**<  alpha0 value, used in ARGB1555 */   /* *CNcomment:alpha0取值，ARGB1555格式时使用 */
    hi_u8 u8Alpha1; /**<  alpha1 value, used in ARGB1555 */   /* *CNcomment:alpha1取值，ARGB1555格式时使用 */
    hi_u8 u8GlobalAlpha; /**<  global alpha value */          /**<CNcomment:全局alpha取值*/
    hi_u8 u8Reserved;
} HIFB_ALPHA_S;

typedef enum {
    DRV_HIFB_FMT_RGB565 = 0, /**<  RGB565 16bpp */
    DRV_HIFB_FMT_RGB888,     /**<  RGB888 24bpp */
    DRV_HIFB_FMT_KRGB444,    /**<  RGB444 16bpp */
    DRV_HIFB_FMT_KRGB555,    /**<  RGB555 16bpp */

    DRV_HIFB_FMT_KRGB888,  /**<  RGB888 32bpp */
    DRV_HIFB_FMT_ARGB4444, /**< ARGB4444 */
    DRV_HIFB_FMT_ARGB1555, /**< ARGB1555 */
    DRV_HIFB_FMT_ARGB8888, /**< ARGB8888 */

    DRV_HIFB_FMT_ARGB8565, /**< ARGB8565 */
    DRV_HIFB_FMT_RGBA4444, /**< ARGB4444 */
    DRV_HIFB_FMT_RGBA5551, /**< RGBA5551 */
    DRV_HIFB_FMT_RGBA5658, /**< RGBA5658 */

    DRV_HIFB_FMT_RGBA8888, /**< RGBA8888 */
    DRV_HIFB_FMT_BGR565,   /**< BGR565 */
    DRV_HIFB_FMT_BGR888,   /**< BGR888 */
    DRV_HIFB_FMT_ABGR4444, /**< ABGR4444 */

    DRV_HIFB_FMT_ABGR1555, /**< ABGR1555 */
    DRV_HIFB_FMT_ABGR8888, /**< ABGR8888 */
    DRV_HIFB_FMT_ABGR8565, /**< ABGR8565 */
    DRV_HIFB_FMT_KBGR444,  /**< BGR444 16bpp */

    DRV_HIFB_FMT_KBGR555, /**< BGR555 16bpp */
    DRV_HIFB_FMT_KBGR888, /**< BGR888 32bpp */
    DRV_HIFB_FMT_1BPP,    /**<  clut1 */
    DRV_HIFB_FMT_2BPP,    /**<  clut2 */

    DRV_HIFB_FMT_4BPP,    /**<  clut4 */
    DRV_HIFB_FMT_8BPP,    /**< clut8 */
    DRV_HIFB_FMT_ACLUT44, /**< AClUT44*/
    DRV_HIFB_FMT_ACLUT88, /**< ACLUT88 */

    DRV_HIFB_FMT_PUYVY,  /**< UYVY */
    DRV_HIFB_FMT_PYUYV,  /**< YUYV */
    DRV_HIFB_FMT_PYVYU,  /**< YVYU */
    DRV_HIFB_FMT_YUV888, /**< YUV888 */

    DRV_HIFB_FMT_AYUV8888, /**< AYUV8888 */
    DRV_HIFB_FMT_YUVA8888, /**< YUVA8888 */

    DRV_HIFB_FMT_ARGB2101010,  /**< ARGB2101010 */
    DRV_HIFB_FMT_ARGB10101010, /**< ARGB10101010 */
    DRV_HIFB_FMT_FP16,         /**< FP16 */

    DRV_HIFB_FMT_BUTT
} DRV_HIFB_COLOR_FMT_E;

typedef struct {
    hi_bool bKeyAlpha;                  /**< whether support colorkey alpha */
    hi_bool bGlobalAlpha;               /**< whether support global alpha */
    hi_bool bCmap;                      /**< whether support color map */
    hi_bool bHasCmapReg;                /**< whether has color map register*/
    hi_bool bColFmt[DRV_HIFB_FMT_BUTT]; /**< support which color format */
    hi_bool bVoScale;                   /**< support vo scale*/
    hi_bool
        bLayerSupported; /**< whether support a certain layer, for example:x5 HD support HIFB_SD_0 not support HIFB_SD_1*/
    hi_bool bCompression; /**< whether support compression */
    hi_bool bStereo;      /**< whether support 3D Stereo*/
    hi_u32 u32MaxWidth;   /**< the max pixels per line */
    hi_u32 u32MaxHeight;  /**< the max lines */
    hi_u32 u32MaxStride;
    hi_u32 u32MaxSize;
    hi_u32 u32MinWidth;  /**< the min pixels per line */
    hi_u32 u32MinHeight; /**< the min lines */
    hi_u32 u32MinStride;
    hi_u32 u32MinSize;
    hi_u32 u32VDefLevel; /**< vertical deflicker level, 0 means vertical deflicker is unsupported */
    hi_u32 u32HDefLevel; /**< horizontal deflicker level, 0 means horizontal deflicker is unsupported  */
} HIFB_CAPABILITY_S;

typedef enum {
    HIFB_LAYER_BUF_DOUBLE = 0x0,           /**< 2 display buf in fb */
    HIFB_LAYER_BUF_ONE = 0x1,              /**< 1 display buf in fb */
    HIFB_LAYER_BUF_NONE = 0x2,             /**< no display buf in fb,the buf user refreshed will be directly set to VO*/
    HIFB_LAYER_BUF_DOUBLE_IMMEDIATE = 0x3, /**< 2 display buf in fb, each refresh will be displayed*/
    HIFB_LAYER_BUF_STANDARD = 0x4,         /**< standard refresh */
    HIFB_LAYER_BUF_FENCE = 0x5,            /**< standard refresh */
    HIFB_LAYER_BUF_BUTT
} HIFB_LAYER_BUF_E;

typedef struct {
    hi_u32 u32PhyAddr;          /**<  start physical address */
    hi_u32 u32Width;            /**<  width pixels */
    hi_u32 u32Height;           /**<  height pixels */
    hi_u32 u32Pitch;            /**<  line pixels */
    DRV_HIFB_COLOR_FMT_E enFmt; /**<  color format */
} HIFB_SURFACE_S;

typedef struct {
    hi_s32 AcquireFenceFd;
    hi_s32 ReleaseFenceFd;
    hi_u32 CurDispBufferNum;
    HIFB_SURFACE_S stCanvas;
    HIFB_RECT UpdateRect; /**< refresh region*/
} HIFB_BUFFER_S;

typedef struct {
    HIFB_SURFACE_S stCursor;
    HIFB_POINT_S stHotPos;
} HIFB_CURSOR_S;

typedef enum {
    HIFB_STEREO_MONO = 0x0,      /**< Normal output to non-stereoscopic (3D) TV. No L/R content provided to TV  */
    HIFB_STEREO_SIDEBYSIDE_HALF, /**< L/R frames are downscaled horizontally by 2 andpacked side-by-side into a single frame, left on lefthalf of frame */
    HIFB_STEREO_TOPANDBOTTOM, /**< L/R frames are downscaled vertically by 2 andpacked into a single frame, left on top */
    HIFB_STEREO_FRMPACKING,   /**< one frames are copyed */
    HIFB_STEREO_BUTT
} HIFB_STEREO_MODE_E;

typedef enum {
    HIFB_STEREO_WORKMODE_HW_FULL =
        0x0, /**< 3d stereo function use hardware and transfer full frame to vo, note: hardware doesn't support this mode if encoder picture delivery method is top and bottom */
    HIFB_STEREO_WORKMODE_HW_HALF, /**< 3d stereo function use hardware and transfer half frame to vo*/
    HIFB_STEREO_WORKMODE_SW_EMUL, /**< 3d stereo function use software emulation */
    HIFB_STEREO_WORKMODE_BUTT
} HIFB_STEREO_WORKMODE_E;

/* *antiflicker level */
/* *Auto means fb will choose a appropriate antiflicker level automatically according to the color info of map */
typedef enum {
    HIFB_LAYER_ANTIFLICKER_NONE = 0x0,   /**< no antiflicker*/
    HIFB_LAYER_ANTIFLICKER_LOW = 0x1,    /**< low level*/
    HIFB_LAYER_ANTIFLICKER_MIDDLE = 0x2, /**< middle level*/
    HIFB_LAYER_ANTIFLICKER_HIGH = 0x3,   /**< high level*/
    HIFB_LAYER_ANTIFLICKER_AUTO = 0x4,   /**< auto*/
    HIFB_LAYER_ANTIFLICKER_BUTT
} HIFB_LAYER_ANTIFLICKER_LEVEL_E;

/* layer info maskbit */
typedef enum {
    HIFB_LAYERMASK_BUFMODE = 0x1,
    /**< Whether the buffer mode in HIFB_LAYER_INFO_S is masked when the graphics layer information is set.*/ /**<CNcomment:设置图层信息时，HIFB_LAYER_INFO_S中buf模式是否有效掩码 */
    HIFB_LAYERMASK_ANTIFLICKER_MODE = 0x2,
    /**< Whether the anti-flicker mode is masked.*/ /**<CNcomment:抗闪烁模式是否有效掩码 */
    HIFB_LAYERMASK_POS = 0x4,
    /**< Whether the graphics layer position is masked.*/ /**<CNcomment:图层位置是否有效掩码 */
    HIFB_LAYERMASK_CANVASSIZE = 0x8,
    /**< Whether the canvas size is masked.*/ /**<CNcomment:canvassize是否有效掩码 */
    HIFB_LAYERMASK_DISPSIZE = 0x10,
    /**< Whether the display size is masked.*/ /**<CNcomment:displaysize是否有效掩码 */
    HIFB_LAYERMASK_SCREENSIZE = 0x20,
    /**< Whether the screen size is masked.*/ /**<CNcomment:screensize是否有效掩码 */
    HIFB_LAYERMASK_BMUL = 0x40,
    /**< Whether the premultiplexed mode is masked.*/ /**<CNcomment:是否预乘是否有效掩码 */
    HIFB_LAYERMASK_BUTT
} HIFB_LAYER_INFO_MASKBIT;

/* *layer info */
typedef struct {
    HIFB_LAYER_BUF_E BufMode;
    HIFB_LAYER_ANTIFLICKER_LEVEL_E eAntiflickerLevel;
    hi_s32 s32XPos;         /**<  the x pos of origion point in screen */
    hi_s32 s32YPos;         /**<  the y pos of origion point in screen */
    hi_s32 u32CanvasWidth;  /**<  the width of canvas buffer */
    hi_s32 u32CanvasHeight; /**<  the height of canvas buffer */
    hi_u32
        u32DisplayWidth; /**<  the width of display buf in fb.for 0 buf ,there is no display buf in fb, so it's effectless*/
    hi_u32 u32DisplayHeight; /**<  the height of display buf in fb. */
    hi_u32 u32ScreenWidth;   /**<  the width of screen，现在没有用 */
    hi_u32 u32ScreenHeight;  /**<  the height of screen，现在没有用 */
    hi_bool bPreMul;         /**<  The data drawed in buf is premul data or not*/
    hi_bool bUseNewScreen;   /**<  whether use new screen*/
    hi_u32 u32Mask;          /**<  param modify mask bit*/
} HIFB_LAYER_INFO_S;

/* * scrolltext attibute */
/* * CNcomment:滚动字幕属性 */
typedef struct {
    HIFB_RECT stRect;               /**< the position you wanted to show on the screen */
    DRV_HIFB_COLOR_FMT_E ePixelFmt; /**< the color format of scrolltext content */
    hi_u16 u16CacheNum;             /**< The cached buffer number for store scrolltext content */
    hi_u16 u16RefreshFreq;          /**< The refresh frequency you wanted */
    hi_bool bDeflicker;             /**< Whether enable antificker */
} HIFB_SCROLLTEXT_ATTR_S;

typedef struct {
    HIFB_SCROLLTEXT_ATTR_S stAttr; /**< The scrolltext attribution */
    hi_u32 u32Handle;              /**< The output ID for the scrolltext，将HIGO属性中的图层移到这里变成ID */
} HIFB_SCROLLTEXT_CREATE_S;

/* * scrolltext data */
/* * CNcomment:滚动字幕数据，地址和stride */
typedef struct {
    hi_u32 u32Handle;  /**< ID of the scrolltext,这里比HIGO多了个句柄 */
    hi_u32 u32PhyAddr; /**< The physical address of the scrolltext content buffer */
    hi_u8 *pu8VirAddr; /**< The virtual address of the scrolltext content buffer */
    hi_u32 addr_len;   /**<The virtual or physical address Length of the scrolltext content buffer */
    hi_u32 u32Stride;  /**< The stride of the scrolltext content buffer */
} HIFB_SCROLLTEXT_DATA_S;

/* * scrolltext data */
/* * CNcomment:滚动字幕数据，地址和stride */
typedef struct {
    hi_u32 u32Handle;  /**< ID of the scrolltext,这里比HIGO多了个句柄 */
    hi_u32 u32PhyAddr; /**< The physical address of the scrolltext content buffer */
    hi_u64 u64VirAddr; /**< The virtual address of the scrolltext content buffer */
    hi_u32 addr_len;   /**<The virtual or physical address Length of the scrolltext content buffer */
    hi_u32 u32Stride;  /**< The stride of the scrolltext content buffer */
} HIFB_SCROLLTEXT_DATA_64BITS_S;

#define GP_LAYER_NUM 4
/* virtual window info */
typedef enum {
    HIFB_ZORDER_MOVETOP = 0,
    /**< Move to the top*/ /**<CNcomment:移到最顶部*/
    HIFB_ZORDER_MOVEUP,
    /**< Move upwards*/ /**<CNcomment:向上移*/
    HIFB_ZORDER_MOVEBOTTOM,
    /**< Move to the bottom*/ /**<CNcomment:移到最底部*/
    HIFB_ZORDER_MOVEDOWN,
    /**< Move downwards*/ /**<CNcomment:向下移*/
    HIFB_ZORDER_BUTT
} HIFB_ZORDER_E;

/* compression mode */
typedef enum {
    HIFB_CMP_REGION = 0,
    /**< fb support region compression, default mode*/ /**<CNcomment:支持局部压缩,默认模式*/
    HIFB_CMP_GLOBAL,
    /**< fb support global compression only*/ /**<CNcomment:仅支持全局压缩*/
    HIFB_CMP_BUTT
} HIFB_CMP_MODE_E;

typedef struct {
    hi_bool LowPowerEn;
    hi_u32 LpInfo[CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT];
} HI_FB_LOWPOWER_INFO_S;

typedef struct {
    hi_u32 buffer_addr;
    HIFB_RECT video_rect;
    HIFB_RECT output_rect;
    hi_bool sync_enable;
    hi_u32 handle;
} hifb_hwc_layer_private_data;

typedef enum {
    DRV_HIFB_HDR_MODE_SDR = 0,
    /* < SDR mode, default mode */ /* <CNcomment:SDR模式 */
    DRV_HIFB_HDR_MODE_HDR10,
    /* < HDR10 mode */ /* <CNcomment:HDR10模式 */
    DRV_HIFB_HDR_MODE_HLG,
    /* < HLG mode */ /* <CNcomment:HLG模式 */
    DRV_HIFB_HDR_MODE_MAX
} drv_hifb_xdr_mode;

typedef enum {
    DRV_HIFB_COLOR_SPACE_BT709,
    /* < color space is BT709 */ /* <CNcomment:BT709 */
    DRV_HIFB_COLOR_SPACE_BT2020,
    /* < color space is BT2020 */ /* <CNcomment:BT2020 */
    DRV_HIFB_COLOR_SPACE_MAX
} drv_hifb_color_space;

typedef enum {
    DRV_HIFB_CMP_MODE_NONE = 0,
    /**< no compress mode, default mode*/ /**<CNcomment:非压缩模式*/
    DRV_HIFB_CMP_MODE_HFBC,
    /**< line compress mode*/ /**<CNcomment:私有压缩模式*/
    DRV_HIFB_CMP_MODE_AFBC,
    /**< AFBC compress mode*/ /**<CNcomment:AFBC压缩模式*/
    DRV_HIFB_CMP_MODE_MAX
} drv_hifb_compress_mode;

typedef struct {
    hi_bool bPreMul;
    hi_bool bStereo;
    hi_bool bDiscardFrame;
    hi_u64 u32LayerAddr;
    hi_mem_handle mem_handle;
    hi_u32 u32Stride;
    hi_u32 u32NoCmpStride;
    hi_u32 u32Alpha;
    hi_s32 s32AcquireFenceFd;
    hi_s32 s32ReleaseFenceFd;
    HIFB_RECT stInRect;
    HIFB_POINT_S stPos;
    DRV_HIFB_COLOR_FMT_E eFmt;
    HI_FB_LOWPOWER_INFO_S stLowPowerInfo;
    hifb_hwc_layer_private_data private_data;
    drv_hifb_xdr_mode xdr_mode;
    drv_hifb_color_space color_space;
    drv_hifb_compress_mode compress_mode;
} HIFB_HWC_LAYERINFO_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_HIFB_TYPE_H__ */
