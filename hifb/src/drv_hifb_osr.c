/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb osr
 * Author: sdk
 * Create: 2016-01-01
 */

/* ********************************add include here********************************************** */
#include <linux/ctype.h>
#include <linux/hisilicon/securec.h>

#include "hi_osal.h"
#include "hi_drv_module.h"
#include "hi_drv_disp.h"

#include "drv_hifb_type.h"
#include "drv_hifb_ioctl.h"
#include "drv_hifb_osr.h"
#include "drv_hifb_mem.h"

#ifdef CONFIG_HIFB_LOGO_SUPPORT
#include "drv_hifb_logo.h"
#endif
#ifdef CONFIG_HIFB_FENCE_SUPPORT
#include "drv_hifb_fence.h"
#endif
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
#include "drv_hifb_scrolltext.h"
#endif

#include "drv_hifb_ext.h"
#include "drv_hifb_config.h"
#include "drv_hifb_debug.h"
#include "linux/ion.h"

/* **************************** Macro Definition ************************************************ */
#define CONFIG_HIFB_DISP_BUFF_MAX_SIZE (100 * 1024 * 1024)
#define HIFB_VIDEO_GFX_SYNC_LAYER (0)
#define HIFB_WAIT_VIDEO_SYNC_INT_CNT (1)
#define HIFB_MAX_WIDTH(u32LayerId) gs_pstCapacity[u32LayerId].u32MaxWidth
#define HIFB_MAX_HEIGHT(u32LayerId) gs_pstCapacity[u32LayerId].u32MaxHeight
#define HIFB_MAX_STRIDE(u32LayerId) gs_pstCapacity[u32LayerId].u32MaxStride
#define HIFB_MAX_SIZE(u32LayerId) gs_pstCapacity[u32LayerId].u32MaxSize
#define HIFB_MIN_WIDTH(u32LayerId) gs_pstCapacity[u32LayerId].u32MinWidth
#define HIFB_MIN_HEIGHT(u32LayerId) gs_pstCapacity[u32LayerId].u32MinHeight
#define HIFB_MIN_STRIDE(u32LayerId) gs_pstCapacity[u32LayerId].u32MinStride
#define HIFB_MIN_SIZE(u32LayerId) gs_pstCapacity[u32LayerId].u32MinSize

#define IS_STEREO_SBS(pstPar) ((HIFB_STEREO_SIDEBYSIDE_HALF == pstPar->st3DInfo.StereoMode))
#define IS_STEREO_TAB(pstPar) ((HIFB_STEREO_TOPANDBOTTOM == pstPar->st3DInfo.StereoMode))
#define IS_STEREO_FPK(pstPar) ((HIFB_STEREO_FRMPACKING == pstPar->st3DInfo.StereoMode))

#define IS_2BUF_MODE(pstPar)                                                 \
    (((HIFB_LAYER_BUF_DOUBLE == pstPar->stExtendInfo.enBufMode) ||           \
      (HIFB_LAYER_BUF_DOUBLE_IMMEDIATE == pstPar->stExtendInfo.enBufMode) || \
      (HIFB_LAYER_BUF_FENCE == pstPar->stExtendInfo.enBufMode)))
#define IS_1BUF_MODE(pstPar) ((HIFB_LAYER_BUF_ONE == pstPar->stExtendInfo.enBufMode))

#define HIFB_UNITE_RECT(stDstRect, stSrcRect) do { \
    HIFB_RECT stRect = {0};                                                           \
    stRect.x = (stDstRect.x < stSrcRect.x) ? stDstRect.x : stSrcRect.x;               \
    stRect.y = (stDstRect.y < stSrcRect.y) ? stDstRect.y : stSrcRect.y;               \
    stRect.w = ((stDstRect.x + stDstRect.w) > (stSrcRect.x + stSrcRect.w))            \
                    ? (stDstRect.x + stDstRect.w - stRect.x)                           \
                    : (stSrcRect.x + stSrcRect.w - stRect.x);                          \
    stRect.h = ((stDstRect.y + stDstRect.h) > (stSrcRect.y + stSrcRect.h))            \
                    ? (stDstRect.y + stDstRect.h - stRect.y)                           \
                    : (stSrcRect.y + stSrcRect.h - stRect.y);                          \
    if (memcpy_s(&stDstRect, sizeof(HIFB_RECT), &stRect, sizeof(HIFB_RECT)) != EOK) { \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");   \
    }                                                                                 \
} while (0)

#define DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp)       \
    if ((NULL == info) || (NULL == argp)) {                       \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, info);   \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, argp);   \
        return HI_FAILURE;                                        \
    }                                                             \
    pstPar = (HIFB_PAR_S *)info->par;                             \
    if (NULL == pstPar) {                                         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstPar); \
        return HI_FAILURE;                                        \
    }

#define DRV_HIFB_DEFINE_PAR_FROM_INFO(info, ret)                  \
    if (NULL == info) {                                           \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, info);   \
        return ret;                                               \
    }                                                             \
    pstPar = (HIFB_PAR_S *)info->par;                             \
    if (NULL == pstPar) {                                         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstPar); \
        return ret;                                               \
    }

#define DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_VALUE(LayerID)       \
    if (LayerID >= HIFB_LAYER_ID_BUTT) {                           \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerID); \
        return HI_FAILURE;                                         \
    }                                                              \
    info = s_stLayer[LayerID].pstInfo;                             \
    if (NULL == info) {                                            \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, info);    \
        return HI_FAILURE;                                         \
    }                                                              \
    pstPar = (HIFB_PAR_S *)info->par;                              \
    if (NULL == pstPar) {                                          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstPar);  \
        return HI_FAILURE;                                         \
    }

#define DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_NOVALUE(LayerID)     \
    if (LayerID >= HIFB_LAYER_ID_BUTT) {                           \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerID); \
        return;                                                    \
    }                                                              \
    info = s_stLayer[LayerID].pstInfo;                             \
    if (NULL == info) {                                            \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, info);    \
        return;                                                    \
    }                                                              \
    pstPar = (HIFB_PAR_S *)info->par;                              \
    if (NULL == pstPar) {                                          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstPar);  \
        return;                                                    \
    }

#define DRV_HIFB_DEFINE_INFO_FROM_LAYER(LayerID)                   \
    if (LayerID >= HIFB_LAYER_ID_BUTT) {                           \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerID); \
        return HI_FAILURE;                                         \
    }                                                              \
    info = s_stLayer[LayerID].pstInfo;                             \
    if (NULL == info) {                                            \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, info);    \
        return HI_FAILURE;                                         \
    }

// #define  CONFIG_HIFB_REALLOC_MEM /** 支持重新分配大的内存,暂不支持 **/

/* ************************** Structure Definition ********************************************** */
static char *video = NULL;
module_param(video, charp, S_IRUGO);

static char *tc_wbc = "off";
module_param(tc_wbc, charp, S_IRUGO);

HIFB_DRV_OPS_S g_stDrvAdpCallBackFunction;
HIFB_DRV_TDEOPS_S g_stGfx2dCallBackFunction;

/* to save layer id and layer size */
HIFB_LAYER_S s_stLayer[HIFB_MAX_LAYER_NUM];

const static HIFB_CAPABILITY_S *gs_pstCapacity = NULL;

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
HIFB_SCROLLTEXT_INFO_S s_stTextLayer[HIFB_LAYER_ID_BUTT];
#endif

/* ********************* Global Variable declaration ******************************************** */
static hi_u32 gs_u32MenuconfigLayerSize[HIFB_MAX_LAYER_NUM] = {
#ifdef CFG_HI_HD0_FB_VRAM_SIZE
    CFG_HI_HD0_FB_VRAM_SIZE,
#else
    0,
#endif
#ifdef CFG_HI_HD1_FB_VRAM_SIZE
    CFG_HI_HD1_FB_VRAM_SIZE,
#else
    0,
#endif
#ifdef CFG_HI_HD2_FB_VRAM_SIZE
    CFG_HI_HD2_FB_VRAM_SIZE,
#else
    0,
#endif
#ifdef CFG_HI_HD3_FB_VRAM_SIZE
    CFG_HI_HD3_FB_VRAM_SIZE,
#else
    0,
#endif
    0,
    0,
};

static struct fb_fix_screeninfo gs_stDefFix = {
    .id = "hifb",
    .type = FB_TYPE_PACKED_PIXELS,
    .visual = FB_VISUAL_TRUECOLOR,
    .xpanstep = 1,
    .ypanstep = 1,
    .ywrapstep = 0,
    .line_length = 7680,
    .accel = FB_ACCEL_NONE,
    .mmio_len = 0,
    .mmio_start = 0,
};

static struct fb_var_screeninfo s_stDefVar[HIFB_LAYER_TYPE_BUTT] = {
    {
        .xres = CONFIG_HIFB_LAYER_MAXWIDTH,
        .yres = CONFIG_HIFB_LAYER_MAXHEIGHT,
        .xres_virtual = CONFIG_HIFB_LAYER_MAXWIDTH,
        .yres_virtual = CONFIG_HIFB_LAYER_MAXHEIGHT * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
        .xoffset = 0,
        .yoffset = 0,
        .bits_per_pixel = 16,
        .red = { 10, 5, 0 },
        .green = { 5, 5, 0 },
        .blue = { 0, 5, 0 },
        .transp = { 15, 1, 0 },
        .activate = FB_ACTIVATE_NOW,
        .pixclock = -1,
        .left_margin = -1,
        .right_margin = -1,
        .upper_margin = -1,
        .lower_margin = -1,
        .hsync_len = -1,
        .vsync_len = -1,
    },
    {
        .xres = 720,
        .yres = 576,
        .xres_virtual = 720,
        .yres_virtual = 1152,
        .xoffset = 0,
        .yoffset = 0,
        .bits_per_pixel = 32,
        .red = { 16, 8, 0 },
        .green = { 8, 8, 0 },
        .blue = { 0, 8, 0 },
        .transp = { 24, 8, 0 },
        .activate = FB_ACTIVATE_NOW,
        .pixclock = -1,
        .left_margin = -1,
        .right_margin = -1,
        .upper_margin = -1,
        .lower_margin = -1,
        .hsync_len = -1,
        .vsync_len = -1,
    },
    {
        .xres = 128,
        .yres = 128,
        .xres_virtual = 128,
        .yres_virtual = 128 * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
        .xoffset = 0,
        .yoffset = 0,
        .bits_per_pixel = 32,
        .red = { 16, 8, 0 },
        .green = { 8, 8, 0 },
        .blue = { 0, 8, 0 },
        .transp = { 24, 8, 0 },
        .activate = FB_ACTIVATE_NOW,
        .pixclock = -1,
        .left_margin = -1,
        .right_margin = -1,
        .upper_margin = -1,
        .lower_margin = -1,
        .hsync_len = -1,
        .vsync_len = -1,
    },
    {
        .xres = 128,
        .yres = 128,
        .xres_virtual = 128,
        .yres_virtual = 256,
        .xoffset = 0,
        .yoffset = 0,
        .bits_per_pixel = 32,
        .red = { 16, 8, 0 },
        .green = { 8, 8, 0 },
        .blue = { 0, 8, 0 },
        .transp = { 24, 8, 0 },
        .activate = FB_ACTIVATE_NOW,
        .pixclock = -1,
        .left_margin = -1,
        .right_margin = -1,
        .upper_margin = -1,
        .lower_margin = -1,
        .hsync_len = -1,
        .vsync_len = -1,
    }
};

/* * bit filed info of color fmt, the order must be the same as DRV_HIFB_COLOR_FMT_E * */
HIFB_ARGB_BITINFO_S s_stArgbBitField[HIFB_MAX_PIXFMT_NUM] = { /* RGB565 */
                                                              {
                                                                  .stRed = { 11, 5, 0 },
                                                                  .stGreen = { 5, 6, 0 },
                                                                  .stBlue = { 0, 5, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* RGB888 */
                                                              {
                                                                  .stRed = { 16, 8, 0 },
                                                                  .stGreen = { 8, 8, 0 },
                                                                  .stBlue = { 0, 8, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* KRGB444 */
                                                              {
                                                                  .stRed = { 8, 4, 0 },
                                                                  .stGreen = { 4, 4, 0 },
                                                                  .stBlue = { 0, 4, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* KRGB555 */
                                                              {
                                                                  .stRed = { 10, 5, 0 },
                                                                  .stGreen = { 5, 5, 0 },
                                                                  .stBlue = { 0, 5, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* KRGB888 */
                                                              {
                                                                  .stRed = { 16, 8, 0 },
                                                                  .stGreen = { 8, 8, 0 },
                                                                  .stBlue = { 0, 8, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* ARGB4444 */
                                                              {
                                                                  .stRed = { 8, 4, 0 },
                                                                  .stGreen = { 4, 4, 0 },
                                                                  .stBlue = { 0, 4, 0 },
                                                                  .stTransp = { 12, 4, 0 },
                                                              },
                                                              /* ARGB1555 */
                                                              {
                                                                  .stRed = { 10, 5, 0 },
                                                                  .stGreen = { 5, 5, 0 },
                                                                  .stBlue = { 0, 5, 0 },
                                                                  .stTransp = { 15, 1, 0 },
                                                              },
                                                              /* ARGB8888 */
                                                              {
                                                                  .stRed = { 16, 8, 0 },
                                                                  .stGreen = { 8, 8, 0 },
                                                                  .stBlue = { 0, 8, 0 },
                                                                  .stTransp = { 24, 8, 0 },
                                                              },
                                                              /* ARGB8565 */
                                                              {
                                                                  .stRed = { 11, 5, 0 },
                                                                  .stGreen = { 5, 6, 0 },
                                                                  .stBlue = { 0, 5, 0 },
                                                                  .stTransp = { 16, 8, 0 },
                                                              },
                                                              /* RGBA4444 */
                                                              {
                                                                  .stRed = { 12, 4, 0 },
                                                                  .stGreen = { 8, 4, 0 },
                                                                  .stBlue = { 4, 4, 0 },
                                                                  .stTransp = { 0, 4, 0 },
                                                              },
                                                              /* RGBA5551 */
                                                              {
                                                                  .stRed = { 11, 5, 0 },
                                                                  .stGreen = { 6, 5, 0 },
                                                                  .stBlue = { 1, 5, 0 },
                                                                  .stTransp = { 0, 1, 0 },
                                                              },
                                                              /* RGBA5658 */
                                                              {
                                                                  .stRed = { 19, 5, 0 },
                                                                  .stGreen = { 13, 6, 0 },
                                                                  .stBlue = { 8, 5, 0 },
                                                                  .stTransp = { 0, 8, 0 },
                                                              },
                                                              /* RGBA8888 */
                                                              {
                                                                  .stRed = { 24, 8, 0 },
                                                                  .stGreen = { 16, 8, 0 },
                                                                  .stBlue = { 8, 8, 0 },
                                                                  .stTransp = { 0, 8, 0 },
                                                              },
                                                              /* BGR565 */
                                                              {
                                                                  .stRed = { 0, 5, 0 },
                                                                  .stGreen = { 5, 6, 0 },
                                                                  .stBlue = { 11, 5, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* BGR888 */
                                                              {
                                                                  .stRed = { 0, 8, 0 },
                                                                  .stGreen = { 8, 8, 0 },
                                                                  .stBlue = { 16, 8, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* ABGR4444 */
                                                              {
                                                                  .stRed = { 0, 4, 0 },
                                                                  .stGreen = { 4, 4, 0 },
                                                                  .stBlue = { 8, 4, 0 },
                                                                  .stTransp = { 12, 4, 0 },
                                                              },
                                                              /* ABGR1555 */
                                                              {
                                                                  .stRed = { 0, 5, 0 },
                                                                  .stGreen = { 5, 5, 0 },
                                                                  .stBlue = { 10, 5, 0 },
                                                                  .stTransp = { 15, 1, 0 },
                                                              },
                                                              /* ABGR8888 */
                                                              {
                                                                  .stRed = { 0, 8, 0 },
                                                                  .stGreen = { 8, 8, 0 },
                                                                  .stBlue = { 16, 8, 0 },
                                                                  .stTransp = { 24, 8, 0 },
                                                              },
                                                              /* ABGR8565 */
                                                              {
                                                                  .stRed = { 0, 5, 0 },
                                                                  .stGreen = { 5, 6, 0 },
                                                                  .stBlue = { 11, 5, 0 },
                                                                  .stTransp = { 16, 8, 0 },
                                                              },
                                                              /* KBGR444 16bpp */
                                                              {
                                                                  .stRed = { 0, 4, 0 },
                                                                  .stGreen = { 4, 4, 0 },
                                                                  .stBlue = { 8, 4, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* KBGR555 16bpp */
                                                              {
                                                                  .stRed = { 0, 5, 0 },
                                                                  .stGreen = { 5, 5, 0 },
                                                                  .stBlue = { 10, 5, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* KBGR888 32bpp */
                                                              {
                                                                  .stRed = { 0, 8, 0 },
                                                                  .stGreen = { 8, 8, 0 },
                                                                  .stBlue = { 16, 8, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },

                                                              /* 1bpp */
                                                              {
                                                                  .stRed = { 0, 1, 0 },
                                                                  .stGreen = { 0, 1, 0 },
                                                                  .stBlue = { 0, 1, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* 2bpp */
                                                              {
                                                                  .stRed = { 0, 2, 0 },
                                                                  .stGreen = { 0, 2, 0 },
                                                                  .stBlue = { 0, 2, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* 4bpp */
                                                              {
                                                                  .stRed = { 0, 4, 0 },
                                                                  .stGreen = { 0, 4, 0 },
                                                                  .stBlue = { 0, 4, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* 8bpp */
                                                              {
                                                                  .stRed = { 0, 8, 0 },
                                                                  .stGreen = { 0, 8, 0 },
                                                                  .stBlue = { 0, 8, 0 },
                                                                  .stTransp = { 0, 0, 0 },
                                                              },
                                                              /* ACLUT44 */
                                                              {
                                                                  .stRed = { 4, 4, 0 },
                                                                  .stGreen = { 4, 4, 0 },
                                                                  .stBlue = { 4, 4, 0 },
                                                                  .stTransp = { 0, 4, 0 },
                                                              },
                                                              /* ACLUT88 */
                                                              {
                                                                  .stRed = { 8, 8, 0 },
                                                                  .stGreen = { 8, 8, 0 },
                                                                  .stBlue = { 8, 8, 0 },
                                                                  .stTransp = { 0, 8, 0 },
                                                              }
};

static hi_s32 DRV_HIFB_Open(struct fb_info *info, hi_s32 user);
static hi_s32 DRV_HIFB_Close(struct fb_info *info, hi_s32 user);
static hi_s32 DRV_HIFB_CheckVar(struct fb_var_screeninfo *var, struct fb_info *info);
static hi_s32 DRV_HIFB_SetPar(struct fb_info *info);
static hi_s32 DRV_HIFB_SetCmap(struct fb_cmap *cmap, struct fb_info *info);
static hi_s32 DRV_HIFB_SetColorReg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp,
                                   struct fb_info *info);
#ifdef CFG_HIFB_SUPPORT_CONSOLE
static hi_void DRV_HIFB_FillRect(struct fb_info *p, const struct fb_fillrect *rect);
static hi_void DRV_HIFB_CopyArea(struct fb_info *p, const struct fb_copyarea *area);
static hi_void DRV_HIFB_ImageBlit(struct fb_info *p, const struct fb_image *image);
#endif
#ifdef CONFIG_DMA_SHARED_BUFFER
static struct dma_buf *DRV_HIFB_ExportDmaBuf(struct fb_info *info);
#endif
#ifdef CONFIG_GFX_MMU_SUPPORT
static hi_s32 DRV_HIFB_MapMemKernalToUser(struct fb_info *info, struct vm_area_struct *vma);
#endif

#ifdef CONFIG_HIFB_FENCE_SUPPORT
extern hi_s32 DRV_HIFB_REFRESH_FenceFlip(HIFB_PAR_S *pstPar, hi_void *pArgs);
#else
extern hi_s32 DRV_HIFB_REFRESH_NoFenceFlip(HIFB_PAR_S *pstPar, hi_void *pArgs);
#endif
extern hi_s32 DRV_HIFB_REFRESH_PanDisplay(struct fb_var_screeninfo *var, struct fb_info *info);

/* ********************* Global Variable declaration ******************************************** */
static struct fb_ops s_sthifbops = { .owner = THIS_MODULE,
                                     .fb_open = DRV_HIFB_Open,
                                     .fb_release = DRV_HIFB_Close,
                                     .fb_check_var = DRV_HIFB_CheckVar,
                                     .fb_set_par = DRV_HIFB_SetPar,
                                     .fb_pan_display = DRV_HIFB_REFRESH_PanDisplay,
#ifdef CONFIG_COMPAT
                                     .fb_compat_ioctl = drv_hifb_compat_ioctl,
#endif
                                     .fb_ioctl = drv_hifb_ioctl,
                                     .fb_setcolreg = DRV_HIFB_SetColorReg,
                                     .fb_setcmap = DRV_HIFB_SetCmap,
#ifdef CFG_HIFB_SUPPORT_CONSOLE
                                     .fb_fillrect = DRV_HIFB_FillRect,
                                     .fb_copyarea = DRV_HIFB_CopyArea,
                                     .fb_imageblit = DRV_HIFB_ImageBlit,
#endif
#ifdef CONFIG_DMA_SHARED_BUFFER
                                     .fb_dmabuf_export = DRV_HIFB_ExportDmaBuf,
#endif
#ifdef CONFIG_GFX_MMU_SUPPORT
                                     .fb_mmap = DRV_HIFB_MapMemKernalToUser
#endif
};

/* ****************************** API declaration *********************************************** */
static hi_void DRV_HIFB_SelectAntiflickerMode(HIFB_PAR_S *pstPar);

static hi_void DRV_HIFB_AssignDispBuf(hi_u32 u32LayerId);
static hi_s32 DRV_HIFB_InitLayerInfo(hi_u32 u32LayerID);
static hi_s32 DRV_HIFB_GetLayerBufferInfo(hi_u32 u32LayerID);
static hi_void DRV_HIFB_SetDisplayBuffer(hi_u32 u32LayerId);
#ifdef CONFIG_HIFB_STEREO_SUPPORT
static hi_s32 DRV_HIFB_StereoCallBack(hi_void *pParaml, hi_void *pParamr);
#endif
#ifdef CONFIG_DMA_SHARED_BUFFER
extern struct dma_buf *DRV_HIFB_DMABUF_MemBlockExport(phys_addr_t base, size_t size, int flags);
#endif
static inline hi_s32 DRV_HIFB_ParseInsmodParameter(hi_void);
static inline hi_ulong DRV_HIFB_GetVramSize(hi_char *pstr);
static inline hi_s32 DRV_HIFB_RegisterFrameBuffer(hi_u32 u32LayerId);
static inline hi_void DRV_HIFB_UnRegisterFrameBuffer(hi_u32 u32LayerId);
static inline hi_s32 DRV_HIFB_AllocLayerBuffer(hi_u32 u32LayerId, hi_u32 BufSize);
static inline hi_void DRV_HIFB_FreeLayerBuffer(hi_u32 u32LayerId);
static inline hi_s32 DRV_HIFB_InitAllocCmapBuffer(hi_u32 u32LayerId);
static inline hi_void DRV_HIFB_DInitAllocCmapBuffer(hi_u32 u32LayerId);
static inline hi_bool DRV_HIFB_CheckSmmuSupport(hi_u32 LayerId);
static inline hi_void DRV_HIFB_PrintVersion(hi_bool bLoad);
static inline hi_s32 DRV_HIFB_RegisterCallBackFunction(hi_u32 u32LayerId);
static inline hi_void DRV_HIFB_ReleaseScrollText(struct fb_info *info);
#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
static inline hi_void DRV_HIFB_UpLowPowerInfo(hi_u32 LayerId);
#endif
static inline hi_void DRV_HIFB_UpdataRefreshInfo(hi_u32 LayerId);
static inline hi_void DRV_HIFB_UpdataInputDataFmt(hi_u32 LayerId);
static inline hi_void DRV_HIFB_UpdataInputDataStride(hi_u32 LayerId);
static inline hi_void DRV_HIFB_UpdataInputDataDDR(hi_u32 LayerId);
static inline hi_void DRV_HIFB_SwitchDispBuffer(hi_u32 LayerId);
static inline hi_void DRV_HIFB_SetDispInfo(hi_u32 LayerId);

// static inline hi_s32   DRV_HIFB_CheckWhetherLayerSizeSupport    (struct fb_info *info, HIFB_LAYER_INFO_S
// *pstLayerInfo); static inline hi_void  DRV_HIFB_CheckWhetherDispSizeChange      (struct fb_info *info,
// HIFB_LAYER_INFO_S *pstLayerInfo); static inline hi_s32   DRV_HIFB_CheckWhetherMemSizeEnough       (struct fb_info
// *info, HIFB_LAYER_INFO_S *pstLayerInfo); static inline hi_s32   DRV_HIFB_CheckWhetherLayerPosSupport
// (HIFB_LAYER_INFO_S *pstLayerInfo);

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
static inline hi_s32 DRV_HIFB_SCROLLTEXT_Enable(hi_bool bEnable, hi_ulong arg);
#endif

/* ****************************** API realization *********************************************** */
/***************************************************************************************
 * func        : HIFB_GetFmtByArgb
 * description : CNcomment: 从argb中判断像素格式 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static DRV_HIFB_COLOR_FMT_E HIFB_GetFmtByArgb(struct fb_bitfield *red, struct fb_bitfield *green,
                                              struct fb_bitfield *blue, struct fb_bitfield *transp,
                                              hi_u32 u32ColorDepth)
{
    hi_u32 i = 0;
    hi_u32 u32Bpp = 0;

    if ((NULL == red) || (NULL == green) || (NULL == blue) || (NULL == transp)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, red);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, green);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, blue);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, transp);
        return DRV_HIFB_FMT_BUTT;
    }

    for (i = 0; i < HI_GFX_ARRAY_SIZE(s_stArgbBitField); i++) {
        if ((DRV_HIFB_MEM_BitFieldCmp(*red, s_stArgbBitField[i].stRed) == 0) &&
            (DRV_HIFB_MEM_BitFieldCmp(*green, s_stArgbBitField[i].stGreen) == 0) &&
            (DRV_HIFB_MEM_BitFieldCmp(*blue, s_stArgbBitField[i].stBlue) == 0) &&
            (DRV_HIFB_MEM_BitFieldCmp(*transp, s_stArgbBitField[i].stTransp) == 0)) {
            u32Bpp = DRV_HIFB_MEM_GetBppByFmt(i);
            if (u32Bpp == u32ColorDepth) {
                return i;
            }
        }
    }

    switch (u32ColorDepth) {
        case 1:
            i = DRV_HIFB_FMT_1BPP;
            break;
        case 2:
            i = DRV_HIFB_FMT_2BPP;
            break;
        case 4:
            i = DRV_HIFB_FMT_4BPP;
            break;
        case 8:
            i = DRV_HIFB_FMT_8BPP;
            break;
        case 16:
            i = DRV_HIFB_FMT_RGB565;
            break;
        case 24:
            i = DRV_HIFB_FMT_RGB888;
            break;
        case 32:
            i = DRV_HIFB_FMT_ARGB8888;
            break;
        default:
            i = DRV_HIFB_FMT_BUTT;
            break;
    }

    if (DRV_HIFB_FMT_BUTT != i) {
        *red = s_stArgbBitField[i].stRed;
        *green = s_stArgbBitField[i].stGreen;
        *blue = s_stArgbBitField[i].stBlue;
        *transp = s_stArgbBitField[i].stTransp;
    }

    return i;
}

#ifdef CONFIG_HIFB_REALLOC_MEM
/***************************************************************************************
* func          : HIFB_ReAllocWork
* description   : CNcomment: 重新分配内存，旧内存释放处理,放这里的原因是为了
                             保证这快内存不再被逻辑使用的时候才释放，这样就不会发生访问页表错误
                             CNend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
static hi_void HIFB_ReAllocWork(osal_workqueue *work)
{
    drv_hifb_disp_tmp_buf *disp_tmp_buf = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(work);
    disp_tmp_buf = (drv_hifb_disp_tmp_buf *)container_of(work, drv_hifb_disp_tmp_buf, dis_buff_work);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(disp_tmp_buf);

    if (disp_tmp_buf->mem_info.virtual_addr != HI_NULL) {
        drv_hifb_mem_unmap(disp_tmp_buf->mem_info.dmabuf, disp_tmp_buf->mem_info.virtual_addr);
    }

    if (disp_tmp_buf->mem_info.smmu_addr != 0) {
        drv_hifb_mem_unmap_from_smmu(disp_tmp_buf->mem_info.dmabuf, disp_tmp_buf->mem_info.smmu_addr);
        drv_hifb_mem_free(disp_tmp_buf->mem_info.dmabuf);
    } else {
        drv_hifb_mem_free(disp_tmp_buf->mem_info.dmabuf);
    }

    disp_tmp_buf->mem_info.dmabuf = HI_NULL;
    disp_tmp_buf->mem_info.phy_addr = 0;
    disp_tmp_buf->mem_info.smmu_addr = 0;
    disp_tmp_buf->mem_info.virtual_addr = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

/***************************************************************************************
 * func          : HIFB_CheckMemEnough
 * description   : CNcomment: 判断内存是否足够 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_s32 HIFB_CheckMemEnough(struct fb_info *info, hi_u32 u32Stride, hi_u32 u32Height)
{
    hi_u32 u32BufferNum = 0;
    hi_u32 u32BufSize = 0;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO(info, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_FAILURE);

    switch (pstPar->stExtendInfo.enBufMode) {
        case HIFB_LAYER_BUF_DOUBLE:
        case HIFB_LAYER_BUF_DOUBLE_IMMEDIATE:
        case HIFB_LAYER_BUF_FENCE:
            u32BufferNum = 2;
            break;
        case HIFB_LAYER_BUF_ONE:
        case HIFB_REFRESH_MODE_WITH_PANDISPLAY:
            u32BufferNum = 1;
            break;
        default:
            u32BufferNum = 0;
            break;
    }

    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32Height,
                                                 HIFB_MAX_HEIGHT(pstPar->stBaseInfo.u32LayerID) *
                                                     CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
                                                 HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_VALUE(u32Height, HIFB_MIN_HEIGHT(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32Stride, HIFB_MAX_STRIDE(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_VALUE(u32Stride, HIFB_MIN_STRIDE(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);

    u32BufSize = HI_HIFB_GetMemSize(u32Stride, u32Height);
    if (0 != u32BufferNum) {
        GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(u32BufferNum, u32BufSize, HI_FAILURE);
        u32BufSize = u32BufferNum * u32BufSize;
    }

    if (info->fix.smem_len < u32BufSize) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, u32BufSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, info->fix.smem_len);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, u32Stride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, u32Height);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, u32BufferNum);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the mem size is not enough for this refresh mode");
        return HI_FAILURE;
    }

    u32BufSize = HI_HIFB_GetMemSize(u32Stride, info->var.yres_virtual);
    if (info->fix.smem_len < u32BufSize) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, u32BufSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, info->fix.smem_len);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, u32Stride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, info->var.yres_virtual);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the mem size is not enough for this refresh mode");
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_CheckVar
 * description : CNcomment: 判断参数信息是否合法 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_CheckVar(struct fb_var_screeninfo *var, struct fb_info *info)
{
    HIFB_PAR_S *pstPar = NULL;
    DRV_HIFB_COLOR_FMT_E enFmt = DRV_HIFB_FMT_BUTT;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(var, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_FAILURE);

    enFmt = HIFB_GetFmtByArgb(&var->red, &var->green, &var->blue, &var->transp, var->bits_per_pixel);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(enFmt,
                                          HI_GFX_ARRAY_SIZE(gs_pstCapacity[pstPar->stBaseInfo.u32LayerID].bColFmt),
                                          HI_FAILURE);

    if ((!gs_pstCapacity[pstPar->stBaseInfo.u32LayerID].bColFmt[enFmt]) ||
        (!g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_SupportFmt(enFmt) &&
         pstPar->stExtendInfo.enBufMode != HIFB_REFRESH_MODE_WITH_PANDISPLAY)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enFmt);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, gs_pstCapacity[pstPar->stBaseInfo.u32LayerID].bColFmt[enFmt]);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stBaseInfo.u32LayerID);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstPar->stExtendInfo.enBufMode);
        return HI_FAILURE;
    }

    if (var->xres_virtual < HIFB_MIN_WIDTH(pstPar->stBaseInfo.u32LayerID)) {
        var->xres_virtual = HIFB_MIN_WIDTH(pstPar->stBaseInfo.u32LayerID);
    }
    if (var->yres_virtual < HIFB_MIN_HEIGHT(pstPar->stBaseInfo.u32LayerID)) {
        var->yres_virtual = HIFB_MIN_HEIGHT(pstPar->stBaseInfo.u32LayerID);
    }

    if (var->xres > var->xres_virtual) {
        var->xres = var->xres_virtual;
    } else if (var->xres < HIFB_MIN_WIDTH(pstPar->stBaseInfo.u32LayerID)) {
        var->xres = HIFB_MIN_WIDTH(pstPar->stBaseInfo.u32LayerID);
    }

    if (var->yres > var->yres_virtual) {
        var->yres = var->yres_virtual;
    } else if (var->yres < HIFB_MIN_HEIGHT(pstPar->stBaseInfo.u32LayerID)) {
        var->yres = HIFB_MIN_HEIGHT(pstPar->stBaseInfo.u32LayerID);
    }

    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(var->xres, HIFB_MAX_WIDTH(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(var->yres, HIFB_MAX_HEIGHT(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(var->xres_virtual,
                                                 HIFB_MAX_WIDTH(pstPar->stBaseInfo.u32LayerID) *
                                                     CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
                                                 HI_FAILURE);
    // GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(var->yres_virtual, HIFB_MAX_HEIGHT(pstPar->stBaseInfo.u32LayerID) *
    // CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(var->xoffset,
                                                 HIFB_MAX_WIDTH(pstPar->stBaseInfo.u32LayerID) *
                                                     CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
                                                 HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(var->yoffset,
                                                 HIFB_MAX_HEIGHT(pstPar->stBaseInfo.u32LayerID) *
                                                     CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
                                                 HI_FAILURE);

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(var->xres, var->xoffset, HI_FAILURE);
    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(var->yres, var->yoffset, HI_FAILURE);
    if ((var->xoffset > var->xres_virtual) || (var->yoffset > var->yres_virtual) ||
        (var->xoffset + var->xres > var->xres_virtual) || (var->yoffset + var->yres > var->yres_virtual)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, var->xoffset);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, var->yoffset);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, var->xres_virtual);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, var->yres_virtual);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_AssignDispBuf
 * description : CNcomment: 分配display buffer CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_void DRV_HIFB_AssignDispBuf(hi_u32 u32LayerId)
{
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    hi_u32 u32BufSize = 0;
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    if (pstPar->st3DInfo.IsStereo) {
        hi_u32 CmpStride = 0;
        hi_u32 Stride = 0;
        hi_u32 u32StartAddr = 0;

        Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT | HIFB_PARA_CHECK_BITSPERPIXEL,
                                      info->var.xres, info->var.yres, 0, 0, info->var.bits_per_pixel);
        if (HI_SUCCESS != Ret) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, info->var.xres);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, info->var.yres);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, info->var.bits_per_pixel);
            return;
        }

        Stride = CONIFG_HIFB_GetMaxStride(info->var.xres, info->var.bits_per_pixel, &CmpStride,
                                          CONFIG_HIFB_STRIDE_ALIGN);
        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(Stride, 0);

        u32BufSize = HI_HIFB_GetMemSize(Stride, info->var.yres);
        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(u32BufSize, 0);

        if (IS_2BUF_MODE(pstPar) || IS_1BUF_MODE(pstPar)) {
            u32StartAddr = info->fix.smem_start;
            if (1 == pstPar->stRunInfo.StereoBufNum) {
                pstPar->st3DInfo.u32DisplayAddr[0] = u32StartAddr;
                pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr;
            } else if (2 == pstPar->stRunInfo.StereoBufNum) {
                pstPar->st3DInfo.u32DisplayAddr[0] = u32StartAddr;
                if (u32BufSize > (info->fix.smem_len / 2)) {
                    pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr;
                } else {
                    pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr + u32BufSize;
                }
            }
            return;
        } else if ((0 == pstPar->st3DInfo.mem_info_3d.smmu_addr) || (0 == pstPar->st3DInfo.mem_info_3d.size) ||
                   (0 == pstPar->stRunInfo.StereoBufNum)) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return;
        } else { /**<-- android used pandisplay refresh **/
            u32StartAddr = pstPar->st3DInfo.mem_info_3d.smmu_addr;
        }

        if (1 == pstPar->stRunInfo.StereoBufNum) {
            pstPar->st3DInfo.u32DisplayAddr[0] = u32StartAddr;
            pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr;
        } else if (2 == pstPar->stRunInfo.StereoBufNum) { /**<-- generally is two buffer **/
            pstPar->st3DInfo.u32DisplayAddr[0] = u32StartAddr;
            if (HIFB_STEREO_SIDEBYSIDE_HALF == pstPar->st3DInfo.StereoMode) {
                pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr + pstPar->st3DInfo.st3DSurface.u32Pitch / 2;
            } else if (HIFB_STEREO_TOPANDBOTTOM == pstPar->st3DInfo.StereoMode) {
                pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr + pstPar->st3DInfo.st3DSurface.u32Pitch *
                                                                        pstPar->stExtendInfo.DisplayHeight / 2;
            } else { /**<-- frame packing user display buffer, no user 3d buffer **/
                pstPar->st3DInfo.u32DisplayAddr[1] = u32StartAddr + u32BufSize;
            }
        }
    } else
#endif
    {
        DRV_HIFB_SetDisplayBuffer(u32LayerId);
    }

    return;
}

#ifdef CFG_HIFB_CUT
/***************************************************************************************
 * func         : HIFB_GetUpdateRect
 * description  : CNcomment: 获取图层的更新区域 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 HIFB_GetUpdateRect(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf, HIFB_RECT *pstUpdateRect)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *info = NULL;
    hi_tde_rect SrcRect = {0}, DstRect = {0}, InSrcRect = {0}, InDstRect = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstUpdateRect, HI_FAILURE);

    SrcRect.width = pstCanvasBuf->stCanvas.u32Width;
    SrcRect.height = pstCanvasBuf->stCanvas.u32Height;

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, pstPar->stExtendInfo.DisplayWidth,
                                  pstPar->stExtendInfo.DisplayHeight, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayHeight);
        return HI_FAILURE;
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, SrcRect.width, SrcRect.height, 0, 0,
                                  0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, SrcRect.width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, SrcRect.height);
        return HI_FAILURE;
    }

    if (pstPar->st3DInfo.StereoMode == HIFB_STEREO_SIDEBYSIDE_HALF) {
        DstRect.width = pstPar->stExtendInfo.DisplayWidth >> 1;
        DstRect.height = pstPar->stExtendInfo.DisplayHeight;
    } else if (pstPar->st3DInfo.StereoMode == HIFB_STEREO_TOPANDBOTTOM) {
        DstRect.width = pstPar->stExtendInfo.DisplayWidth;
        DstRect.height = pstPar->stExtendInfo.DisplayHeight >> 1;
    } else {
        DstRect.width = pstPar->stExtendInfo.DisplayWidth;
        DstRect.height = pstPar->stExtendInfo.DisplayHeight;
    }

    if (SrcRect.width != DstRect.width || SrcRect.height != DstRect.height) {
        InSrcRect.pos_x = pstCanvasBuf->UpdateRect.x;
        InSrcRect.pos_y = pstCanvasBuf->UpdateRect.y;
        InSrcRect.width = pstCanvasBuf->UpdateRect.w;
        InSrcRect.height = pstCanvasBuf->UpdateRect.h;

        g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_CalScaleRect(&SrcRect, &DstRect, &InSrcRect, &InDstRect);

        pstUpdateRect->x = InDstRect.pos_x;
        pstUpdateRect->y = InDstRect.pos_y;
        pstUpdateRect->w = InDstRect.width;
        pstUpdateRect->h = InDstRect.height;
    } else {
        if (memcpy_s(pstUpdateRect, sizeof(HIFB_RECT), &pstCanvasBuf->UpdateRect, sizeof(HIFB_RECT)) != EOK) {
            GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : HIFB_BackUpForeBuf
 * description  : CNcomment: 更新前景数据CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 HIFB_BackUpForeBuf(hi_u32 u32LayerId, HIFB_BUFFER_S *pstBackBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 ForePhyAddr = 0;
    HIFB_PAR_S *pstPar = NULL;
    HIFB_RECT *pstForeUpdateRect = NULL;
    struct fb_info *info = NULL;
    HIFB_BUFFER_S stForeBuf, stBackBuf;
    HIFB_BLIT_OPT_S stBlitTmp;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstBackBuf, HI_FAILURE);
    if (memcpy_s(&stBackBuf, sizeof(HIFB_BUFFER_S), pstBackBuf, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE((1 - pstPar->stRunInfo.u32IndexForInt), CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
                                          HI_FAILURE);

    if ((pstPar->st3DInfo.StereoMode != HIFB_STEREO_MONO) && (pstPar->st3DInfo.StereoMode != HIFB_STEREO_BUTT)) {
        pstForeUpdateRect = &pstPar->st3DInfo.st3DUpdateRect;
        ForePhyAddr = pstPar->st3DInfo.u32DisplayAddr[1 - pstPar->stRunInfo.u32IndexForInt];
    } else {
        pstForeUpdateRect = &pstPar->stDispInfo.stUpdateRect;
        ForePhyAddr = pstPar->stDispInfo.u32DisplayAddr[1 - pstPar->stRunInfo.u32IndexForInt];
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, stBackBuf.stCanvas.u32Width,
                                  stBackBuf.stCanvas.u32Height, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stBackBuf.stCanvas.u32Width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stBackBuf.stCanvas.u32Height);
        return HI_FAILURE;
    }

    if (pstPar->st3DInfo.StereoMode == HIFB_STEREO_SIDEBYSIDE_HALF) {
        stBackBuf.stCanvas.u32Width = stBackBuf.stCanvas.u32Width >> 1;
    } else if (pstPar->st3DInfo.StereoMode == HIFB_STEREO_TOPANDBOTTOM) {
        stBackBuf.stCanvas.u32Height = stBackBuf.stCanvas.u32Height >> 1;
    }

    if (0 == pstForeUpdateRect->w) {
        pstForeUpdateRect->w = stBackBuf.stCanvas.u32Width;
    }

    if (0 == pstForeUpdateRect->h) {
        pstForeUpdateRect->h = stBackBuf.stCanvas.u32Height;
    }

    if (HI_TRUE == DRV_HIFB_MEM_IsConTain(&stBackBuf.UpdateRect, pstForeUpdateRect)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (memcpy_s(&stForeBuf, sizeof(HIFB_BUFFER_S), &stBackBuf, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    stForeBuf.stCanvas.u32PhyAddr = ForePhyAddr;

    if (memcpy_s(&stForeBuf.UpdateRect, sizeof(HIFB_RECT), pstForeUpdateRect, sizeof(HIFB_RECT)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    if (memcpy_s(&stBackBuf.UpdateRect, sizeof(HIFB_RECT), &stForeBuf.UpdateRect, sizeof(HIFB_RECT)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    memset_s(&stBlitTmp, sizeof(stBlitTmp), 0x0, sizeof(stBlitTmp));

    stBlitTmp.bBlock = HI_FALSE;
    pstPar->stFrameInfo.bBlitBlock = stBlitTmp.bBlock;
    pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(&stForeBuf, &stBackBuf, &stBlitTmp, HI_TRUE);
    if (Ret <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

/***************************************************************************************
 * func         : HIFB_SetDispSize
 * description  : CNcomment: set display size CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 HIFB_SetDispSize(hi_u32 u32LayerId, hi_u32 u32Width, hi_u32 u32Height)
{
    hi_s32 Ret = HI_SUCCESS;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    if ((pstPar->stExtendInfo.DisplayWidth == u32Width) && (pstPar->stExtendInfo.DisplayHeight == u32Height)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, u32Width, u32Height, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Height);
        return HI_FAILURE;
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_BITSPERPIXEL, 0, 0, 0, 0, info->var.bits_per_pixel);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.bits_per_pixel);
        return HI_FAILURE;
    }

    u32Stride = CONIFG_HIFB_GetMaxStride(u32Width, info->var.bits_per_pixel, &CmpStride, CONFIG_HIFB_STRIDE_ALIGN);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(u32Stride, 0, HI_FAILURE);

    if (HI_FAILURE == HIFB_CheckMemEnough(info, u32Stride, u32Height)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Stride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Height);
        return HI_FAILURE;
    }

    pstPar->stExtendInfo.DisplayWidth = u32Width;
    pstPar->stExtendInfo.DisplayHeight = u32Height;

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;

    DRV_HIFB_SelectAntiflickerMode(pstPar);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifdef CFG_HIFB_CUT
/***************************************************************************************
 * func        : HIFB_SetCanvasBufInfo
 * description : CNcomment: 设置canvas buffer 信息 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_s32 HIFB_SetCanvasBufInfo(struct fb_info *info, HIFB_LAYER_INFO_S *pLayerInfo)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pLayerInfo, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_FAILURE);

    if (!(pLayerInfo->u32Mask & HIFB_LAYERMASK_CANVASSIZE)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return HI_SUCCESS;
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, pLayerInfo->u32CanvasWidth,
                                  pLayerInfo->u32CanvasHeight, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pLayerInfo->u32CanvasWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pLayerInfo->u32CanvasHeight);
        return HI_FAILURE;
    }

    u32Stride = CONIFG_HIFB_GetMaxStride(pLayerInfo->u32CanvasWidth, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32Stride, HIFB_MAX_STRIDE(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_VALUE(u32Stride, HIFB_MIN_STRIDE(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);

    pstPar->stDispInfo.stCanvasSur.u32Width = pLayerInfo->u32CanvasWidth;
    pstPar->stDispInfo.stCanvasSur.u32Height = pLayerInfo->u32CanvasHeight;
    pstPar->stDispInfo.stCanvasSur.enFmt = HIFB_GetFmtByArgb(&info->var.red, &info->var.green, &info->var.blue,
                                                             &info->var.transp, info->var.bits_per_pixel);
    pstPar->stDispInfo.stCanvasSur.u32Pitch = u32Stride;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

hi_s32 drv_hifb_check_beg_up_stereo(const void *param)
{
    HIFB_PAR_S *par = (HIFB_PAR_S *)param;

    if (par == HI_NULL) {
        return 0;
    }
    if (par->st3DInfo.BegUpStereoInfo == HI_FALSE) {
        return 1;
    }
    return 0;
}

#ifdef CONFIG_HIFB_STEREO_SUPPORT
static hi_void DRV_HIFB_FreeStereoBuf(HIFB_PAR_S *par)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_ulong StereoLockFlag = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(par);
    g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_WaitAllDone(HI_TRUE);

    DRV_HIFB_Lock(&par->st3DInfo.StereoLock, &StereoLockFlag);
    par->st3DInfo.IsStereo = HI_FALSE;
    DRV_HIFB_UnLock(&par->st3DInfo.StereoLock, &StereoLockFlag);

    Ret = osal_wait_timeout_interruptible(&par->st3DInfo.WaiteFinishUpStereoInfoMutex, drv_hifb_check_beg_up_stereo,
                                          (hi_void *)par, CONFIG_HIFB_STEREO_WAITE_TIME);

    if (par->st3DInfo.mem_info_3d.smmu_addr != 0) {
        /* delay ensure for gfx finish display */
        osal_msleep_uninterruptible(25 * 4);
        drv_hifb_mem_free(par->st3DInfo.mem_info_3d.dmabuf);
    }

    DRV_HIFB_Lock(&par->st3DInfo.StereoLock, &StereoLockFlag);
    par->st3DInfo.BegUpStereoInfo = HI_FALSE;
    memset_s(&par->st3DInfo.mem_info_3d, sizeof(par->st3DInfo.mem_info_3d), 0, sizeof(par->st3DInfo.mem_info_3d));
    par->st3DInfo.st3DSurface.u32PhyAddr = 0;
    DRV_HIFB_UnLock(&par->st3DInfo.StereoLock, &StereoLockFlag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_s32 DRV_HIFB_AllocStereoBuf(struct fb_info *info, hi_u32 u32BufSize)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_char name[256] = "\0";
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_FAILURE);

    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32BufSize, HIFB_MAX_SIZE(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_VALUE(u32BufSize, HIFB_MIN_SIZE(pstPar->stBaseInfo.u32LayerID), HI_FAILURE);

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(u32BufSize, pstPar->st3DInfo.mem_info_3d.smmu_addr, HI_SUCCESS);

    if (pstPar->st3DInfo.mem_info_3d.smmu_addr != 0) {
        DRV_HIFB_FreeStereoBuf(pstPar);
    }

    snprintf(name, sizeof(name), "HIFB_STEREO_BUF_%d", pstPar->stBaseInfo.u32LayerID);
    name[sizeof(name) - 1] = '\0';

    pstPar->st3DInfo.mem_info_3d.dmabuf = osal_mem_alloc(name, u32BufSize, OSAL_NSSMMU_TYPE, HI_NULL, 0);
    osal_mem_flush(pstPar->st3DInfo.mem_info_3d.dmabuf);
    if (pstPar->st3DInfo.mem_info_3d.dmabuf == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BufSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->st3DInfo.mem_info_3d.dmabuf);
        return HI_FAILURE;
    }

    pstPar->st3DInfo.mem_info_3d.smmu_addr = drv_hifb_mem_map_to_smmu(pstPar->st3DInfo.mem_info_3d.dmabuf);
    if (pstPar->st3DInfo.mem_info_3d.smmu_addr == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BufSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->st3DInfo.mem_info_3d.smmu_addr);
        return HI_FAILURE;
    }

    pstPar->st3DInfo.mem_info_3d.size = u32BufSize;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void DRV_HIFB_ClearStereoBuf(struct fb_info *info)
{
    HIFB_PAR_S *par = NULL;
    HIFB_BLIT_OPT_S stOpt;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(par);

    if ((par->st3DInfo.mem_info_3d.smmu_addr == 0) || (par->st3DInfo.mem_info_3d.size == 0)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    memset_s(&stOpt, sizeof(stOpt), 0x0, sizeof(stOpt));
    par->st3DInfo.st3DSurface.u32PhyAddr = par->st3DInfo.mem_info_3d.smmu_addr;

    stOpt.bBlock = HI_TRUE;
    g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_ClearRect(&(par->st3DInfo.st3DSurface), &stOpt);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_s32 DRV_HIFB_ClearUnUsedStereoBuf(struct fb_info *info)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;
    HIFB_PAR_S *par = NULL;
    HIFB_BLIT_OPT_S stOpt;
    HIFB_SURFACE_S Surface;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    if (par->stExtendInfo.enBufMode == HIFB_REFRESH_MODE_WITH_PANDISPLAY ||
        par->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE) {
        DRV_HIFB_ClearStereoBuf(info);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    memset_s(&stOpt, sizeof(stOpt), 0x0, sizeof(stOpt));
    memset_s(&Surface, sizeof(Surface), 0x0, sizeof(Surface));

    Surface.enFmt = par->stExtendInfo.enColFmt;
    Surface.u32Height = par->stExtendInfo.DisplayHeight;
    Surface.u32Width = par->stExtendInfo.DisplayWidth;

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_BITSPERPIXEL, par->stExtendInfo.DisplayWidth,
                                  0, 0, 0, info->var.bits_per_pixel);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, par->stExtendInfo.DisplayWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.bits_per_pixel);
        return HI_FAILURE;
    }

    u32Stride = CONIFG_HIFB_GetMaxStride(par->stExtendInfo.DisplayWidth, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
    Surface.u32Pitch = u32Stride;

    if ((HIFB_LAYER_BUF_DOUBLE == par->stExtendInfo.enBufMode) ||
        (HIFB_LAYER_BUF_DOUBLE_IMMEDIATE == par->stExtendInfo.enBufMode) ||
        (HIFB_LAYER_BUF_FENCE == par->stExtendInfo.enBufMode)) {
        GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(par->stRunInfo.u32IndexForInt, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
                                              HI_FAILURE);
        Surface.u32PhyAddr = par->st3DInfo.u32DisplayAddr[par->stRunInfo.u32IndexForInt];
    } else {
        Surface.u32PhyAddr = par->st3DInfo.u32DisplayAddr[0];
    }

    if (0 == Surface.u32PhyAddr) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, Surface.u32PhyAddr);
        return HI_FAILURE;
    }

    stOpt.bBlock = HI_TRUE;
    g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_ClearRect(&Surface, &stOpt);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_ReAllocStereoBuf(HIFB_LAYER_ID_E enLayerId, hi_u32 u32BufStride, hi_u32 u32BufferSize)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *info = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(enLayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[enLayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_FAILURE);

    if (HIFB_STEREO_MONO == pstPar->st3DInfo.StereoMode) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((HIFB_LAYER_BUF_NONE != pstPar->stExtendInfo.enBufMode) &&
        (HIFB_REFRESH_MODE_WITH_PANDISPLAY != pstPar->stExtendInfo.enBufMode)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((HIFB_REFRESH_MODE_WITH_PANDISPLAY == pstPar->stExtendInfo.enBufMode) &&
        (HIFB_STEREO_FRMPACKING == pstPar->st3DInfo.StereoMode)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (u32BufferSize <= pstPar->st3DInfo.mem_info_3d.size) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32BufferSize, HIFB_MAX_SIZE(pstPar->stBaseInfo.u32LayerID),
                                                 HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_VALUE(u32BufferSize, HIFB_MIN_SIZE(pstPar->stBaseInfo.u32LayerID),
                                                 HI_FAILURE);
    Ret = DRV_HIFB_AllocStereoBuf(info, u32BufferSize);
    if (Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BufferSize);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_AllocStereoBuf, Ret);
        return Ret;
    }

    pstPar->st3DInfo.st3DSurface.u32Width = info->var.xres;
    pstPar->st3DInfo.st3DSurface.u32Height = info->var.yres;
    pstPar->st3DInfo.st3DSurface.u32Pitch = u32BufStride;
    pstPar->st3DInfo.st3DSurface.u32PhyAddr = pstPar->st3DInfo.mem_info_3d.smmu_addr;
    pstPar->st3DInfo.u32rightEyeAddr = pstPar->st3DInfo.st3DSurface.u32PhyAddr;
    pstPar->stRunInfo.u32IndexForInt = 0;

    DRV_HIFB_ClearStereoBuf(info);

    DRV_HIFB_AssignDispBuf(pstPar->stBaseInfo.u32LayerID);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

/***************************************************************************************
 * func         : DRV_HIFB_SetPar
 * description  : CNcomment: 配置参数 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_SetPar(struct fb_info *info)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;
    HIFB_PAR_S *pstPar = NULL;
    DRV_HIFB_COLOR_FMT_E enFmt = DRV_HIFB_FMT_ARGB8888;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_FAILURE);

    Ret = hifb_check_whether_resolution_support(pstPar->stBaseInfo.u32LayerID, info->var.xres, info->var.yres);
    if (HI_SUCCESS != Ret) {
        return HI_FAILURE;
    }

    enFmt = HIFB_GetFmtByArgb(&info->var.red, &info->var.green, &info->var.blue, &info->var.transp,
                              info->var.bits_per_pixel);

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_BITSPERPIXEL, info->var.xres_virtual, 0, 0, 0,
                                  info->var.bits_per_pixel);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.xres_virtual);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.bits_per_pixel);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_PARA_CHECK_SUPPORT, Ret);
        return HI_FAILURE;
    }
    u32Stride = CONIFG_HIFB_GetMaxStride(info->var.xres_virtual, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
#ifdef CONFIG_HIFB_REALLOC_MEM
    if (!pstPar->bPanFlag) {
        u32BufSize = HI_HIFB_GetMemSize(u32Stride, info->var.yres_virtual);
        GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32BufSize, HIFB_MAX_SIZE(pstPar->stBaseInfo.u32LayerID),
                                                     HI_FAILURE);
        GRAPHIC_CHECK_LEFT_LITTLE_RIGHT_RETURN_VALUE(u32BufSize, HIFB_MIN_SIZE(pstPar->stBaseInfo.u32LayerID),
                                                     HI_FAILURE);

        if (u32BufSize > info->fix.smem_len) {
            memset_s(&(pstPar->disp_tmp_buf), sizeof(pstPar->disp_tmp_buf), 0, sizeof(pstPar->disp_tmp_buf));

            memcpy_s(&(pstPar->disp_tmp_buf.mem_info), sizeof(drv_hifb_mem_info), &pstPar->mem_info,
                     sizeof(drv_hifb_mem_info));

            Ret = DRV_HIFB_AllocLayerBuffer(pstPar->stBaseInfo.u32LayerID, u32BufSize);
            if (HI_FAILURE == Ret) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stBaseInfo.u32LayerID);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32BufSize);
                GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_AllocLayerBuffer, Ret);
                return HI_FAILURE;
            }

            info->fix.smem_len = u32BufSize;
            pstPar->stRunInfo.u32IndexForInt = 0;

            DRV_HIFB_AssignDispBuf(pstPar->stBaseInfo.u32LayerID);

            pstPar->stRunInfo.bModifying = HI_TRUE;
            pstPar->stRunInfo.CurScreenAddr = info->fix.smem_start;
            pstPar->st3DInfo.u32rightEyeAddr = info->fix.smem_start;
            pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
            pstPar->stRunInfo.bModifying = HI_FALSE;

            g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(pstPar->stBaseInfo.u32LayerID, info->fix.smem_start);

            pstPar->disp_tmp_buf.start_release = HI_TRUE;
        }
    } else
#endif
    {
        Ret = HIFB_CheckMemEnough(info, u32Stride, info->var.yres);
        if (HI_FAILURE == Ret) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.yres);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Stride);
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_CheckMemEnough, Ret);
            return HI_FAILURE;
        }
    }

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if ((pstPar->st3DInfo.IsStereo) && (HIFB_REFRESH_MODE_WITH_PANDISPLAY == pstPar->stExtendInfo.enBufMode)) {
        u32Stride = CONIFG_HIFB_GetMaxStride(info->var.xres, info->var.bits_per_pixel, &CmpStride,
                                             CONFIG_HIFB_STRIDE_ALIGN);

        pstPar->st3DInfo.st3DSurface.enFmt = enFmt;
        pstPar->st3DInfo.st3DSurface.u32Width = info->var.xres;
        pstPar->st3DInfo.st3DSurface.u32Height = info->var.yres;

        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->st3DInfo.st3DSurface.u32Pitch = u32Stride;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
        pstPar->stRunInfo.bModifying = HI_FALSE;
        info->fix.line_length = u32Stride;
    } else
#endif
    {
        u32Stride = CONIFG_HIFB_GetMaxStride(info->var.xres_virtual, info->var.bits_per_pixel, &CmpStride,
                                             CONFIG_HIFB_STRIDE_ALIGN);
        if ((u32Stride != info->fix.line_length) || (info->var.yres != pstPar->stExtendInfo.DisplayHeight)) {
            pstPar->stRunInfo.bModifying = HI_TRUE;
            info->fix.line_length = u32Stride;
            DRV_HIFB_AssignDispBuf(pstPar->stBaseInfo.u32LayerID);
            pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
            pstPar->stRunInfo.bModifying = HI_FALSE;
        }
    }

    if ((pstPar->stExtendInfo.enColFmt != enFmt)) {
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
        GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_GFX_ARRAY_SIZE(s_stTextLayer),
                                              HI_FAILURE);
        if (s_stTextLayer[pstPar->stBaseInfo.u32LayerID].bAvailable) {
            hi_u32 i = 0;
            for (i = 0; i < SCROLLTEXT_NUM; i++) {
                if (s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[i].bAvailable) {
                    DRV_HIFB_SCROLLTEXT_FreeCacheBuf(&(s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[i]));
                    memset_s(&s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[i], sizeof(HIFB_SCROLLTEXT_S),
                             0, sizeof(HIFB_SCROLLTEXT_S));
                }
            }
            s_stTextLayer[pstPar->stBaseInfo.u32LayerID].bAvailable = HI_FALSE;
            s_stTextLayer[pstPar->stBaseInfo.u32LayerID].u32textnum = 0;
            s_stTextLayer[pstPar->stBaseInfo.u32LayerID].u32ScrollTextId = 0;
        }
#endif
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stExtendInfo.enColFmt = enFmt;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;
        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    DRV_HIFB_SetDispInfo(pstPar->stBaseInfo.u32LayerID);

    pstPar->bPanFlag = HI_TRUE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static inline hi_void DRV_HIFB_SetDispInfo(hi_u32 LayerId)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    if ((info->var.xres == pstPar->stExtendInfo.DisplayWidth) &&
        (info->var.yres == pstPar->stExtendInfo.DisplayHeight)) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, info->var.xres);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, info->var.yres);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstPar->stExtendInfo.DisplayWidth);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstPar->stExtendInfo.DisplayHeight);
        return;
    }

    if (((0 == info->var.xres) || (0 == info->var.yres)) && ((HI_TRUE == pstPar->stExtendInfo.bShow))) {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stExtendInfo.bShow = HI_FALSE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    HIFB_SetDispSize(LayerId, info->var.xres, info->var.yres);
    DRV_HIFB_AssignDispBuf(LayerId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#ifdef CFG_HIFB_CUT
static hi_void HIFB_SetDispLayerPos(hi_u32 u32LayerId, hi_s32 s32XPos, hi_s32 s32YPos)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stExtendInfo.stPos.s32XPos = s32XPos;
    pstPar->stExtendInfo.stPos.s32YPos = s32YPos;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void HIFB_SetBufMode(hi_u32 u32LayerId, HIFB_LAYER_BUF_E enLayerBufMode)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE) && (pstPar->stExtendInfo.enBufMode != enLayerBufMode)) {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;
        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    pstPar->stExtendInfo.enBufMode = enLayerBufMode;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

/***************************************************************************************
* func         : HIFB_SetDispSize
* description  : CNcomment: choose the module to do  flicker resiting,
                            TDE or VOU ? the rule is as this ,the moudle
                            should do flicker resisting who has do scaling CNend\n
* param[in]    : hi_void
* retval       : NA
* others:      : NA
***************************************************************************************/
static hi_void DRV_HIFB_SelectAntiflickerMode(HIFB_PAR_S *pstPar)
{
    HIFB_RECT stOutputRect = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    if (HIFB_LAYER_ANTIFLICKER_NONE == pstPar->stBaseInfo.enAntiflickerLevel) {
        pstPar->stBaseInfo.bNeedAntiflicker = HI_FALSE;
        pstPar->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_NONE;
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    pstPar->stBaseInfo.bNeedAntiflicker = HI_TRUE;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOutRect(pstPar->stBaseInfo.u32LayerID, &stOutputRect);

    if ((pstPar->stExtendInfo.DisplayWidth != stOutputRect.w) ||
        (pstPar->stExtendInfo.DisplayHeight != stOutputRect.h)) {
        pstPar->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_VO;
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    pstPar->stBaseInfo.enAntiflickerMode = HIFB_ANTIFLICKER_TDE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_SetAntiflickerLevel
 * description  : CNcomment: 设置抗闪级别 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_void DRV_HIFB_SetAntiflickerLevel(hi_u32 u32LayerId, HIFB_LAYER_ANTIFLICKER_LEVEL_E enAntiflickerLevel)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    /* **********************END CHECK PAR ***************************** */

    pstPar->stBaseInfo.enAntiflickerLevel = enAntiflickerLevel;
    DRV_HIFB_SelectAntiflickerMode(pstPar);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return;
}

static hi_s32 DRV_HIFB_SetDispAddressr(hi_u32 u32LayerId)
{
    hi_u32 u32Index;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    u32Index = pstPar->stRunInfo.u32IndexForInt;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (pstPar->st3DInfo.IsStereo) {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(u32LayerId, pstPar->st3DInfo.u32DisplayAddr[u32Index]);
        pstPar->stRunInfo.CurScreenAddr = pstPar->st3DInfo.u32DisplayAddr[u32Index];
        pstPar->st3DInfo.u32rightEyeAddr = pstPar->stRunInfo.CurScreenAddr;
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTriDimAddr(u32LayerId, pstPar->st3DInfo.u32rightEyeAddr);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else
#endif
    {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(u32LayerId, pstPar->stDispInfo.u32DisplayAddr[u32Index]);
        pstPar->stRunInfo.CurScreenAddr = pstPar->stDispInfo.u32DisplayAddr[u32Index];
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (HI_TRUE == pstPar->st3DInfo.IsStereo) {
        pstPar->stRunInfo.u32IndexForInt = (0 == pstPar->stRunInfo.StereoBufNum)
                                                ? (0)
                                                : (++u32Index) % pstPar->stRunInfo.StereoBufNum;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else
#endif
    {
        pstPar->stRunInfo.u32IndexForInt = (0 == pstPar->stRunInfo.u32BufNum)
                                                ? (0)
                                                : (++u32Index) % pstPar->stRunInfo.u32BufNum;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    pstPar->stRunInfo.bFliped = HI_TRUE;
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void drv_hifb_video_sync(HIFB_PAR_S *par, hi_u32 layer_id, hi_drv_disp_callback_info *disp_info)
{
    unsigned long lockflag = 0;
    hi_bool disp_gfx_sync_ready = HI_FALSE;
    hi_u8 i = 0;
    hi_u8 hwc_refresh_cnt = 0;
    hi_bool no_discard_frame = HI_FALSE;

    osal_spin_lock_irqsave(&par->stBaseInfo.lock, &lockflag);

    hwc_refresh_cnt = par->hwc_refresh_cnt;

    for (i = 0; i < hwc_refresh_cnt; i++) {
        if (layer_id == HIFB_VIDEO_GFX_SYNC_LAYER) {
            if (par->hwc_layer_info[par->rindex].private_data.buffer_addr ==
                    par->hwc_layer_info[par->rindex].u32LayerAddr &&
                par->hwc_layer_info[par->rindex].private_data.sync_enable == HI_TRUE) {
                par->sync_times[par->rindex]++;
                // disp_info->disp_info.disp_gfx_sync_info.video_rect =
                // par->hwc_layer_info[par->rindex].private_data.video_rect;
                // disp_info->disp_info.disp_gfx_sync_info.output_rect =
                // par->hwc_layer_info[par->rindex].private_data.video_rect;
                // disp_info->disp_info.disp_gfx_sync_info.handle  =
                // par->hwc_layer_info[par->rindex].private_data.handle;
                // disp_info->disp_info.disp_gfx_sync_info.sync_enable = HI_TRUE;
            } else {
                disp_gfx_sync_ready = HI_TRUE;
                // disp_info->disp_info.disp_gfx_sync_info.sync_enable  =   HI_FALSE;
            }
        } else {
            disp_gfx_sync_ready = HI_TRUE;
        }

        if (par->sync_times[par->rindex] > HIFB_WAIT_VIDEO_SYNC_INT_CNT) {
            disp_gfx_sync_ready = HI_TRUE;
            par->sync_times[par->rindex] = 0;
        }

        if (disp_gfx_sync_ready) {
            par->stRunInfo.bModifying = HI_TRUE;
            par->compress_mode = par->hwc_layer_info[par->rindex].compress_mode;
            par->stRunInfo.CurScreenAddr = par->hwc_layer_info[par->rindex].u32LayerAddr;
            par->stExtendInfo.enColFmt = par->hwc_layer_info[par->rindex].eFmt;
            par->stExtendInfo.DisplayStride = par->hwc_layer_info[par->rindex].u32Stride;
            par->stExtendInfo.stPos.s32XPos = par->hwc_layer_info[par->rindex].stInRect.x;
            par->stExtendInfo.stPos.s32YPos = par->hwc_layer_info[par->rindex].stInRect.y;
            par->stExtendInfo.DisplayWidth = par->hwc_layer_info[par->rindex].stInRect.w;
            par->stExtendInfo.DisplayHeight = par->hwc_layer_info[par->rindex].stInRect.h;
            par->stExtendInfo.private_data = par->hwc_layer_info[par->rindex].private_data;
#ifdef CONFIG_GFX_CANVASE_TO_LAYER_SUPPORT
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;
#endif
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FENCE;
            par->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_BMUL;
            par->stRunInfo.bModifying = HI_FALSE;
#ifdef CONFIG_HIFB_FENCE_SUPPORT
            par->FenceRefreshCount++;
#endif
            par->hwc_refresh_cnt--;
            par->rindex++;
            par->rindex = par->rindex % CONFIG_HIFB_LAYER_BUFFER_MAX_NUM;
            disp_gfx_sync_ready = HI_FALSE;

        } else {
            break;
        }

        if (par->hwc_refresh_cnt > 0 && par->hwc_layer_info[par->rindex].private_data.sync_enable == HI_FALSE) {
#ifndef CONFIG_HIFB_VERSION_1_0
            if ((disp_info->disp_info.fmt == HI_DRV_DISP_FMT_1080P_24) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_1080P_25) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_1080P_30) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_1080P_23_976) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_1080P_29_97) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_3840X2160_24) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_3840X2160_25) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_3840X2160_30) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_3840X2160_23_976) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_3840X2160_29_97) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_4096X2160_24) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_4096X2160_25) ||
                (disp_info->disp_info.fmt == HI_DRV_DISP_FMT_4096X2160_30)) {
                no_discard_frame = HI_FALSE;
            } else {
                no_discard_frame = HI_TRUE;
            }
#endif
            no_discard_frame = (par->hwc_layer_info[par->rindex].bDiscardFrame == HI_FALSE) ? no_discard_frame
                                                                                            : HI_FALSE;

            if (no_discard_frame == HI_FALSE) {
                par->bProcDiscardFrame = HI_TRUE;
            } else {
                break;
            }
        }
    }
    osal_spin_unlock_irqrestore(&par->stBaseInfo.lock, &lockflag);

    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_VoCallBack
 * description  : CNcomment: vo中断处理 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_VoCallBack(hi_void *pParaml, hi_void *pParamr)
{
    hi_u32 *pu32LayerId = NULL;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    HIFB_RECT stInRect = {0};
    hi_u32 NowTimeMs = 0;
    hi_u32 EndTimeMs = 0;
    hi_bool HasBeenClosedForVoCallBack = HI_FALSE;
    hi_bool bARDataDecompressErr = HI_FALSE;
    hi_bool bGBDataDecompressErr = HI_FALSE;
    hi_ulong ExpectIntLineNumsForVoCallBack = 0;
    hi_ulong ExpectIntLineNumsForEndCallBack = 0;
    hi_ulong ActualIntLineNumsForVoCallBack = 0;
    hi_ulong HardIntCntForVoCallBack = 0;
    hi_drv_disp_callback_info *disp_info = HI_NULL;
    HI_GFX_TINIT();

    /* **************************beg check para ********************************** */
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pParaml, HI_FAILURE);
    pu32LayerId = (hi_u32 *)pParaml;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(*pu32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pParamr, HI_FAILURE);
    disp_info = (hi_drv_disp_callback_info *)pParamr;
    info = s_stLayer[*pu32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **************************end check para ********************************** */

    /* **************************beg count fps *********************************** */
    HI_GFX_TSTART(NowTimeMs);
    pstPar->stFrameInfo.flip_fps_cnt++;
    if ((NowTimeMs - pstPar->stFrameInfo.StartTimeMs) >= 1000) {
        pstPar->stFrameInfo.StartTimeMs = NowTimeMs;
        pstPar->stFrameInfo.FlipFps = pstPar->stFrameInfo.flip_fps_cnt;
        pstPar->stFrameInfo.DrawFps = pstPar->stFrameInfo.RefreshFrame;
        pstPar->stFrameInfo.RefreshFrame = 0;
        pstPar->stFrameInfo.flip_fps_cnt = 0;
    }
    /* **************************end count fps *********************************** */

    /* **************************beg count times ********************************* */
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetCloseState(*pu32LayerId, &HasBeenClosedForVoCallBack, NULL);
    if (HI_TRUE == HasBeenClosedForVoCallBack) {
        pstPar->stFrameInfo.PreTimeMs = 0;
        pstPar->stFrameInfo.MaxTimeMs = 0;
    }
    if (0 == pstPar->stFrameInfo.PreTimeMs) {
        pstPar->stFrameInfo.PreTimeMs = NowTimeMs;
    }
    if ((NowTimeMs - pstPar->stFrameInfo.PreTimeMs) > pstPar->stFrameInfo.MaxTimeMs) {
        pstPar->stFrameInfo.MaxTimeMs = NowTimeMs - pstPar->stFrameInfo.PreTimeMs;
    }
    pstPar->stFrameInfo.PreTimeMs = NowTimeMs;
    /* **************************end count fps *********************************** */

    /* **************************beg count interupt information ****************** */
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDhd0Info(*pu32LayerId, &ExpectIntLineNumsForVoCallBack,
                                                        &ExpectIntLineNumsForEndCallBack,
                                                        &ActualIntLineNumsForVoCallBack, &HardIntCntForVoCallBack);
    pstPar->stFrameInfo.ExpectIntLineNumsForVoCallBack = ExpectIntLineNumsForVoCallBack;
    pstPar->stFrameInfo.ActualIntLineNumsForVoCallBack = ActualIntLineNumsForVoCallBack;
    pstPar->stFrameInfo.ExpectIntLineNumsForEndCallBack = ExpectIntLineNumsForEndCallBack;
    pstPar->stFrameInfo.HardIntCntForVoCallBack = HardIntCntForVoCallBack;

    pstPar->stFrameInfo.VoSoftCallBackCnt++;
    /* **************************end count interupt information ****************** */

    /* **************************beg count decompress times ********************** */
#ifdef CONFIG_GFX_PROC_SUPPORT
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDecompressStatus(*pu32LayerId, &bARDataDecompressErr,
                                                                &bGBDataDecompressErr,
                                                                pstPar->stProcInfo.bCloseInterrupt);
#else
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDecompressStatus(*pu32LayerId, &bARDataDecompressErr,
                                                                &bGBDataDecompressErr, HI_FALSE);
#endif
    if (HI_TRUE == bARDataDecompressErr) {
        pstPar->stFrameInfo.ARDataDecompressErrCnt++;
    }

    if (HI_TRUE == bGBDataDecompressErr) {
        pstPar->stFrameInfo.GBDataDecompressErrCnt++;
    }
    /* **************************end count decompress times ********************** */
    drv_hifb_video_sync(pstPar, *pu32LayerId, disp_info);

    /* ************************** up register information ********************* */
    if (!pstPar->stRunInfo.bModifying) {
        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_SHOW) {
            g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetEnable(*pu32LayerId, pstPar->stExtendInfo.bShow);
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_SHOW;
        }

        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_ALPHA) {
            g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAlpha(*pu32LayerId, &pstPar->stExtendInfo.stAlpha);
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_ALPHA;
        }

        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_COLORKEY) {
            g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayKeyMask(*pu32LayerId, &pstPar->stExtendInfo.stCkey);
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_COLORKEY;
        }

        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_BMUL) {
            g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerPreMult(*pu32LayerId, pstPar->stBaseInfo.bPreMul);
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_BMUL;
        }

        if (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_ANTIFLICKERLEVEL) {
            HIFB_DEFLICKER_S stDeflicker;
            stDeflicker.pu8HDfCoef = pstPar->stBaseInfo.ucHDfcoef;
            stDeflicker.pu8VDfCoef = pstPar->stBaseInfo.ucVDfcoef;
            stDeflicker.u32HDfLevel = pstPar->stBaseInfo.u32HDflevel;
            stDeflicker.u32VDfLevel = pstPar->stBaseInfo.u32VDflevel;

            g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerDeFlicker(*pu32LayerId, &stDeflicker);
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_ANTIFLICKERLEVEL;
        }

        if ((pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_INRECT) ||
            (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_OUTRECT)) {
            stInRect.x = pstPar->stExtendInfo.stPos.s32XPos;
            stInRect.y = pstPar->stExtendInfo.stPos.s32YPos;
            stInRect.w = (hi_s32)pstPar->stExtendInfo.DisplayWidth;
            stInRect.h = (hi_s32)pstPar->stExtendInfo.DisplayHeight;

            g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerRect(*pu32LayerId, &stInRect);
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_INRECT;
            pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_OUTRECT;
        }

        DRV_HIFB_UpdataInputDataFmt(*pu32LayerId);

        DRV_HIFB_UpdataInputDataStride(*pu32LayerId);

#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
        DRV_HIFB_UpLowPowerInfo(*pu32LayerId);
#endif

        DRV_HIFB_UpdataRefreshInfo(*pu32LayerId);
    }

    DRV_HIFB_SwitchDispBuffer(*pu32LayerId);

    g_stDrvAdpCallBackFunction.drv_hifb_adp_set_up_mute(*pu32LayerId, pstPar->up_mute);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_UpLayerReg(*pu32LayerId);

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    DRV_HIFB_SCROLLTEXT_Blit(*pu32LayerId);
#endif

    /* **************************beg count run times ***************************** */
    HI_GFX_TEND(EndTimeMs);
    if ((EndTimeMs - NowTimeMs) > pstPar->stFrameInfo.RunMaxTimeMs) {
        pstPar->stFrameInfo.RunMaxTimeMs = EndTimeMs - NowTimeMs;
    }
    /* **************************end count run times ***************************** */

    return HI_SUCCESS;
}

#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
static inline hi_void DRV_HIFB_UpLowPowerInfo(hi_u32 LayerId)
{
    hi_u32 IsLowPowerHasRfresh = 0x0;
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *info = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    /* **********************END CHECK PAR ***************************** */

    IsLowPowerHasRfresh = (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_LOWPOWER);
    if (!IsLowPowerHasRfresh) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLowPowerInfo(LayerId, &(pstPar->stLowPowerInfo));

    pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_LOWPOWER;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

static inline hi_void DRV_HIFB_UpdataRefreshInfo(hi_u32 LayerId)
{
    hi_u32 IsInputDataHasRfresh = 0x0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    /* **********************END CHECK PAR ***************************** */

    IsInputDataHasRfresh = (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_REFRESH);
    if (!IsInputDataHasRfresh) {
        return;
    }

    DRV_HIFB_UpdataInputDataDDR(LayerId);

#if defined(CONFIG_HIFB_FENCE_SUPPORT) || defined(CFG_HI_FB_DECOMPRESS_SUPPORT)
    DRV_HIFB_SetDecmpLayerInfo(LayerId);
#endif

    pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_REFRESH;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void DRV_HIFB_UpdataInputDataFmt(hi_u32 LayerId)
{
    hi_u32 IsInputDataFmtHasChanged = 0x0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    /* **********************END CHECK PAR ***************************** */

    IsInputDataFmtHasChanged = (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_FMT);
    if (!IsInputDataFmtHasChanged) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE) &&
        pstPar->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr) {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerDataFmt(LayerId,
                                                                pstPar->stDispInfo.stUserBuffer.stCanvas.enFmt);
    } else {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerDataFmt(LayerId, pstPar->stExtendInfo.enColFmt);
    }

    pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_FMT;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void DRV_HIFB_UpdataInputDataStride(hi_u32 LayerId)
{
    hi_u32 InputDataStride = 0;
    hi_u32 IsInputDataStrideHasChanged = 0x0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    /* **********************END CHECK PAR ***************************** */

    IsInputDataStrideHasChanged = (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_STRIDE);
    if (!IsInputDataStrideHasChanged) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (pstPar->st3DInfo.IsStereo) {
        InputDataStride = pstPar->st3DInfo.st3DSurface.u32Pitch;
    } else
#endif
    {
        if ((pstPar->stExtendInfo.enBufMode == HIFB_LAYER_BUF_NONE) &&
            (pstPar->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr)) {
            InputDataStride = pstPar->stDispInfo.stUserBuffer.stCanvas.u32Pitch;
        } else {
            InputDataStride = info->fix.line_length;
        }
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerStride(LayerId, InputDataStride);

    pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_STRIDE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void DRV_HIFB_UpdataInputDataDDR(hi_u32 LayerId)
{
    hi_u32 IsInputDataDDRHasChanged = 0x0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    /* **********************END CHECK PAR ***************************** */

    IsInputDataDDRHasChanged = (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_DISPLAYADDR);
    if (!IsInputDataDDRHasChanged) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(LayerId, pstPar->stRunInfo.CurScreenAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstPar->stRunInfo.CurScreenAddr);

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (pstPar->st3DInfo.IsStereo) {
        pstPar->st3DInfo.u32rightEyeAddr = pstPar->stRunInfo.CurScreenAddr;
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTriDimAddr(LayerId, pstPar->st3DInfo.u32rightEyeAddr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstPar->st3DInfo.u32rightEyeAddr);
    }
#endif

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_DISPLAYADDR;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_SwitchDispBuffer
 * description  : CNcomment: switch display buffer CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static inline hi_void DRV_HIFB_SwitchDispBuffer(hi_u32 LayerId)
{
    hi_bool bDispEnable = HI_TRUE;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **************************beg check par ********************************* */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(LayerId, HI_GFX_ARRAY_SIZE(s_stLayer));
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    /* **************************end check par ********************************* */

    /* **************************beg set display buffer ************************ */
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetHaltDispStatus(pstPar->stBaseInfo.u32LayerID, &bDispEnable);

    if ((HIFB_LAYER_BUF_DOUBLE == pstPar->stExtendInfo.enBufMode) && (HI_TRUE == pstPar->stRunInfo.bNeedFlip)) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        DRV_HIFB_SetDispAddressr(LayerId);
    }

    if ((HIFB_LAYER_BUF_FENCE == pstPar->stExtendInfo.enBufMode) && (HI_TRUE == pstPar->stRunInfo.bNeedFlip)) {
        DRV_HIFB_SetDispAddressr(LayerId);
#ifdef CONFIG_HIFB_FENCE_SUPPORT
        while (pstPar->FenceRefreshCount > 0) {
            drv_hifb_fence_inc_refresh_time(bDispEnable, LayerId);
            pstPar->FenceRefreshCount--;
        }
#endif
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
    /* **************************end set display buffer ************************ */

#ifdef CONFIG_HIFB_REALLOC_MEM
    if (HI_TRUE == pstPar->stDispTmpBuf.bStartRelease) {
        osal_workqueue_schedule(&(pstPar->stDispTmpBuf.bDisBufWork));
        pstPar->stDispTmpBuf.bStartRelease = HI_FALSE;
    }
#endif
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    else if ((pstPar->stExtendInfo.enBufMode == HIFB_REFRESH_MODE_WITH_PANDISPLAY) &&
             (pstPar->st3DInfo.mem_info_3d.smmu_addr != 0) && (HI_TRUE == pstPar->st3DInfo.IsStereo) &&
             (HI_TRUE == pstPar->stRunInfo.bNeedFlip)) {
        DRV_HIFB_SetDispAddressr(LayerId);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
#endif
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#ifdef CFG_HIFB_CUT
/***************************************************************************************
 * func          : DRV_HIFB_Flip_0buf
 * description   : CNcomment: no display buffer refresh CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_0buf(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_u32 u32StartAddr;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);
    u32StartAddr = pstCanvasBuf->stCanvas.u32PhyAddr;

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.CurScreenAddr = u32StartAddr;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;

    pstPar->stDispInfo.stUserBuffer.stCanvas.u32Pitch = pstCanvasBuf->stCanvas.u32Pitch;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;

    pstPar->stDispInfo.stUserBuffer.stCanvas.enFmt = pstCanvasBuf->stCanvas.enFmt;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;

    HIFB_SetDispSize(u32LayerId, pstCanvasBuf->stCanvas.u32Width, pstCanvasBuf->stCanvas.u32Height);

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;

    pstPar->stRunInfo.bModifying = HI_FALSE;

    DRV_HIFB_WaitVBlank(u32LayerId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_Flip_1buf
 * description  : CNcomment: one canvas buffer,one display buffer refresh CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_1buf(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_BUFFER_S stDisplayBuf;
    HIFB_OSD_DATA_S stOsdData;
    HIFB_BLIT_OPT_S stBlitOpt;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    memset_s(&stDisplayBuf, sizeof(stDisplayBuf), 0, sizeof(stDisplayBuf));

    stDisplayBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
    stDisplayBuf.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stDisplayBuf.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stDisplayBuf.stCanvas.u32Pitch = info->fix.line_length;
    stDisplayBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[0];

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOSDData(u32LayerId, &stOsdData);

    if (stOsdData.u32RegPhyAddr != pstPar->stDispInfo.u32DisplayAddr[0] && pstPar->stDispInfo.u32DisplayAddr[0]) {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
        pstPar->stRunInfo.CurScreenAddr = pstPar->stDispInfo.u32DisplayAddr[0];
        memset_s(info->screen_base, info->fix.smem_len, 0x0, info->fix.smem_len);
        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

#ifdef CFG_HIFB_CUT
    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }
#endif
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);

    if (pstCanvasBuf->stCanvas.u32Height != pstPar->stExtendInfo.DisplayHeight ||
        pstCanvasBuf->stCanvas.u32Width != pstPar->stExtendInfo.DisplayWidth) {
        stBlitOpt.bScale = HI_TRUE;
        stDisplayBuf.UpdateRect.x = 0;
        stDisplayBuf.UpdateRect.y = 0;
        stDisplayBuf.UpdateRect.w = stDisplayBuf.stCanvas.u32Width;
        stDisplayBuf.UpdateRect.h = stDisplayBuf.stCanvas.u32Height;
    } else {
        stDisplayBuf.UpdateRect = pstCanvasBuf->UpdateRect;
    }

    stBlitOpt.bBlock = HI_FALSE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;

    pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
    pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(pstCanvasBuf, &stDisplayBuf, &stBlitOpt, HI_TRUE);
    if (Ret <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.s32RefreshHandle = Ret;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_Flip_2buf
 * description  : CNcomment: 异步刷新 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_2buf(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Index;
    unsigned long lockflag;
    HIFB_BUFFER_S stForeBuf;
    HIFB_BUFFER_S stBackBuf;
    HIFB_BLIT_OPT_S stBlitOpt;
    HIFB_OSD_DATA_S stOsdData;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    u32Index = pstPar->stRunInfo.u32IndexForInt;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(1 - u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    memset_s(&stForeBuf, sizeof(stForeBuf), 0, sizeof(stForeBuf));
    memset_s(&stBackBuf, sizeof(stBackBuf), 0, sizeof(stBackBuf));

    stBlitOpt.bCallBack = HI_TRUE;
    stBlitOpt.pfnCallBack = (IntCallBack)DRV_HIFB_BlitFinishCallBack;
    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &lockflag);
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &lockflag);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOSDData(u32LayerId, &stOsdData);

    stBackBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
    stBackBuf.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stBackBuf.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stBackBuf.stCanvas.u32Pitch = info->fix.line_length;
    stBackBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[u32Index];

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);

    if ((pstCanvasBuf->stCanvas.u32Height != pstPar->stExtendInfo.DisplayHeight) ||
        (pstCanvasBuf->stCanvas.u32Width != pstPar->stExtendInfo.DisplayWidth)) {
        stBlitOpt.bScale = HI_TRUE;
    }

    HIFB_GetUpdateRect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

    if ((pstPar->stRunInfo.bFliped) && (stOsdData.u32RegPhyAddr == pstPar->stDispInfo.u32DisplayAddr[1 - u32Index])) {
        HIFB_BackUpForeBuf(u32LayerId, &stBackBuf);
        memset_s(&(pstPar->stDispInfo.stUpdateRect), sizeof(pstPar->stDispInfo.stUpdateRect), 0,
                 sizeof(pstPar->stDispInfo.stUpdateRect));
        pstPar->stRunInfo.bFliped = HI_FALSE;
    }

    /* update union rect */
    if ((pstPar->stDispInfo.stUpdateRect.w == 0) || (pstPar->stDispInfo.stUpdateRect.h == 0)) {
        if (memcpy_s(&pstPar->stDispInfo.stUpdateRect, sizeof(HIFB_RECT), &stBackBuf.UpdateRect, sizeof(HIFB_RECT)) !=
            EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
        }
    } else {
        HIFB_UNITE_RECT(pstPar->stDispInfo.stUpdateRect, stBackBuf.UpdateRect);
    }

    if (stBlitOpt.bScale == HI_TRUE) {
        stBackBuf.UpdateRect.x = 0;
        stBackBuf.UpdateRect.y = 0;
        stBackBuf.UpdateRect.w = stBackBuf.stCanvas.u32Width;
        stBackBuf.UpdateRect.h = stBackBuf.stCanvas.u32Height;
    } else {
        stBackBuf.UpdateRect = pstCanvasBuf->UpdateRect;
    }

    stBlitOpt.bBlock = HI_FALSE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;

    pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
    pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(pstCanvasBuf, &stBackBuf, &stBlitOpt, HI_TRUE);
    if (Ret <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
        goto RET;
    }

    pstPar->stRunInfo.s32RefreshHandle = Ret;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.bModifying = HI_FALSE;

RET:
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_Flip_2buf_immediate_display
 * description  : CNcomment: 同步刷新 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_2buf_immediate_display(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Index = 0;
    unsigned long lockflag = 0;
    HIFB_BUFFER_S stForeBuf;
    HIFB_BUFFER_S stBackBuf;
    HIFB_BLIT_OPT_S stBlitOpt;
    HIFB_OSD_DATA_S stOsdData;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    u32Index = pstPar->stRunInfo.u32IndexForInt;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(1 - u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    memset_s(&stForeBuf, sizeof(stForeBuf), 0, sizeof(stForeBuf));
    memset_s(&stBackBuf, sizeof(stBackBuf), 0, sizeof(stBackBuf));

    stBlitOpt.bCallBack = HI_FALSE;
    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &lockflag);
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &lockflag);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOSDData(u32LayerId, &stOsdData);

    stBackBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
    stBackBuf.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stBackBuf.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stBackBuf.stCanvas.u32Pitch = info->fix.line_length;
    stBackBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[u32Index];

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);

    if ((pstCanvasBuf->stCanvas.u32Height != pstPar->stExtendInfo.DisplayHeight) ||
        (pstCanvasBuf->stCanvas.u32Width != pstPar->stExtendInfo.DisplayWidth)) {
        stBlitOpt.bScale = HI_TRUE;
    }

    HIFB_GetUpdateRect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

    HIFB_BackUpForeBuf(u32LayerId, &stBackBuf);

    if (memcpy_s(&pstPar->stDispInfo.stUpdateRect, sizeof(HIFB_RECT), &stBackBuf.UpdateRect, sizeof(HIFB_RECT)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    if (stBlitOpt.bScale == HI_TRUE) {
        stBackBuf.UpdateRect.x = 0;
        stBackBuf.UpdateRect.y = 0;
        stBackBuf.UpdateRect.w = stBackBuf.stCanvas.u32Width;
        stBackBuf.UpdateRect.h = stBackBuf.stCanvas.u32Height;
    } else {
        stBackBuf.UpdateRect = pstCanvasBuf->UpdateRect;
    }

    stBlitOpt.bRegionDeflicker = HI_TRUE;
    stBlitOpt.bBlock = HI_TRUE;

    pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
    pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(pstCanvasBuf, &stBackBuf, &stBlitOpt, HI_TRUE);
    if (Ret <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
        goto RET;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.CurScreenAddr = pstPar->stDispInfo.u32DisplayAddr[u32Index];
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    pstPar->stRunInfo.u32IndexForInt = 1 - u32Index;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    DRV_HIFB_WaitVBlank(u32LayerId);
RET:
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_FlipWithFence
 * description  : CNcomment: fence异步刷新 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_FlipWithFence(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Index;
    unsigned long lockflag;
    HIFB_BUFFER_S stForeBuf;
    HIFB_BUFFER_S stBackBuf;
    HIFB_BLIT_OPT_S stBlitOpt;
    HIFB_OSD_DATA_S stOsdData;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    struct hifb_fence *pSyncfence = NULL;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **************************beg check par ********************************* */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);
    /* **************************end check par ********************************* */

    /* **************************beg wait draw finish ************************** */
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    if (pstCanvasBuf->AcquireFenceFd >= 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstCanvasBuf->AcquireFenceFd);
        pSyncfence = drv_hifb_fd_get_fence(pstCanvasBuf->AcquireFenceFd);
    }

    if (NULL != pSyncfence) {
        drv_hifb_fence_wait(pSyncfence, 3000); /* 3000 ms */
        drv_hifb_fence_put(pSyncfence);
        pSyncfence = NULL;
    }
#endif
    /* **************************end wait draw finish ************************** */

    /* **************************get cur display buffer ************************ */
    u32Index = pstPar->stRunInfo.u32IndexForInt;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(1 - u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);
    pstCanvasBuf->CurDispBufferNum = u32Index;
    /* **************************end cur display buffer ************************ */

    /* **************************beg initial struct **************************** */
    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    memset_s(&stForeBuf, sizeof(stForeBuf), 0, sizeof(stForeBuf));
    memset_s(&stBackBuf, sizeof(stBackBuf), 0, sizeof(stBackBuf));
    /* **************************end initial struct **************************** */

    stBlitOpt.bCallBack = HI_TRUE;
    stBlitOpt.pfnCallBack = (IntCallBack)DRV_HIFB_BlitFinishCallBack;
    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &lockflag);
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &lockflag);

    stBackBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
    stBackBuf.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stBackBuf.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stBackBuf.stCanvas.u32Pitch = info->fix.line_length;
    stBackBuf.stCanvas.u32PhyAddr = pstPar->stDispInfo.u32DisplayAddr[u32Index];

    if ((pstCanvasBuf->stCanvas.u32Height != pstPar->stExtendInfo.DisplayHeight) ||
        (pstCanvasBuf->stCanvas.u32Width != pstPar->stExtendInfo.DisplayWidth)) {
        stBlitOpt.bScale = HI_TRUE;
    }

    HIFB_GetUpdateRect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOSDData(u32LayerId, &stOsdData);
    if ((pstPar->stRunInfo.bFliped) && (stOsdData.u32RegPhyAddr == pstPar->stDispInfo.u32DisplayAddr[1 - u32Index])) {
        HIFB_BackUpForeBuf(u32LayerId, &stBackBuf);
        memset_s(&(pstPar->stDispInfo.stUpdateRect), sizeof(pstPar->stDispInfo.stUpdateRect), 0,
                 sizeof(pstPar->stDispInfo.stUpdateRect));
        pstPar->stRunInfo.bFliped = HI_FALSE;
    }

    if ((pstPar->stDispInfo.stUpdateRect.w == 0) || (pstPar->stDispInfo.stUpdateRect.h == 0)) {
        if (memcpy_s(&pstPar->stDispInfo.stUpdateRect, sizeof(HIFB_RECT), &stBackBuf.UpdateRect, sizeof(HIFB_RECT)) !=
            EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
        }
    } else {
        HIFB_UNITE_RECT(pstPar->stDispInfo.stUpdateRect, stBackBuf.UpdateRect);
    }

    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    if (stBlitOpt.bScale == HI_TRUE) {
        stBackBuf.UpdateRect.x = 0;
        stBackBuf.UpdateRect.y = 0;
        stBackBuf.UpdateRect.w = stBackBuf.stCanvas.u32Width;
        stBackBuf.UpdateRect.h = stBackBuf.stCanvas.u32Height;
    } else {
        stBackBuf.UpdateRect = pstCanvasBuf->UpdateRect;
    }

    stBlitOpt.bBlock = HI_FALSE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;

    pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
    pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(pstCanvasBuf, &stBackBuf, &stBlitOpt, HI_TRUE);
    if (Ret <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
        goto FINISH_EXIT;
    }

    pstPar->stRunInfo.s32RefreshHandle = Ret;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    /* **************************get create release fence fd ******************* */
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    pstCanvasBuf->ReleaseFenceFd = drv_hifb_fence_create(u32LayerId);
    if (pstCanvasBuf->ReleaseFenceFd < 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstCanvasBuf->ReleaseFenceFd);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_hifb_fence_create, pstCanvasBuf->ReleaseFenceFd);
        goto FINISH_EXIT;
    }
#endif
    /* **************************end create release fence fd ******************* */

FINISH_EXIT:
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_Flip_panbuf
 * description  : CNcomment: 给android使用 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
#ifdef CONFIG_HIFB_STEREO_SUPPORT
static hi_s32 DRV_HIFB_Flip_panbuf(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_ulong StereoLockFlag = 0;
    hi_u32 u32TmpAddr = 0;
    hi_u32 u32TmpSize = 0;
    HIFB_RECT UpdateRect;
    HIFB_BLIT_OPT_S stBlitOpt;
    HIFB_BUFFER_S stCanvasBuf;
    HIFB_BUFFER_S stDisplayBuf;
    struct fb_var_screeninfo *var = NULL;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    var = &(info->var);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(var, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);
    UpdateRect = pstCanvasBuf->UpdateRect;

    if ((UpdateRect.x >= pstPar->stExtendInfo.DisplayWidth) || (UpdateRect.y >= pstPar->stExtendInfo.DisplayHeight) ||
        (UpdateRect.w == 0) || (UpdateRect.h == 0)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, UpdateRect.x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, UpdateRect.y);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, UpdateRect.w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, UpdateRect.h);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayHeight);
        return HI_FAILURE;
    }

    DRV_HIFB_Lock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
    /* * IsStereo and BegUpStereoInfo should in one lock* */
    if (HI_FALSE == pstPar->st3DInfo.IsStereo) {
        DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (pstPar->st3DInfo.mem_info_3d.smmu_addr == 0) {
        DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
    pstPar->st3DInfo.BegUpStereoInfo = HI_TRUE;
    DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    stBlitOpt.bScale = HI_TRUE;

    if (HIFB_ANTIFLICKER_TDE == pstPar->stBaseInfo.enAntiflickerMode) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    stBlitOpt.bBlock = HI_TRUE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;

    /* * get address * */
    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT | HIFB_PARA_CHECK_STRIDE |
                                      HIFB_PARA_CHECK_BITSPERPIXEL,
                                  var->xoffset, var->yoffset, info->fix.line_length, 0, var->bits_per_pixel);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, var->xoffset);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, var->yoffset);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->fix.line_length);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, var->bits_per_pixel);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_PARA_CHECK_SUPPORT, Ret);
        return HI_FAILURE;
    }

    if (var->bits_per_pixel >= 8) {
        u32TmpSize = info->fix.line_length * var->yoffset + var->xoffset * (var->bits_per_pixel >> 3);
    } else {
        u32TmpSize = info->fix.line_length * var->yoffset + var->xoffset * var->bits_per_pixel / 8;
    }

    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32TmpSize, info->fix.smem_len, HI_FAILURE);
    GRAPHIC_CHECK_U64_ADDITION_REVERSAL_RETURN(info->fix.smem_start, u32TmpSize, HI_FAILURE);
    GRAPHIC_CHECK_ULONG_TO_UINT_REVERSAL_RETURN(info->fix.smem_start + u32TmpSize, HI_FAILURE);
    u32TmpAddr = info->fix.smem_start + u32TmpSize;

    if ((var->bits_per_pixel == 24) && ((var->xoffset != 0) || (var->yoffset != 0))) {
        u32TmpSize = info->fix.line_length * var->yoffset + var->xoffset * (var->bits_per_pixel >> 3);

        GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(u32TmpSize, info->fix.smem_len, HI_FAILURE);
        GRAPHIC_CHECK_U64_ADDITION_REVERSAL_RETURN(info->fix.smem_start, u32TmpSize, HI_FAILURE);
        GRAPHIC_CHECK_ULONG_TO_UINT_REVERSAL_RETURN(info->fix.smem_start + u32TmpSize, HI_FAILURE);

        u32TmpAddr = (info->fix.smem_start + u32TmpSize) / 16 / 3;
        u32TmpAddr = u32TmpAddr * 16 * 3;
    }

    /* *******************config pan buffer****************** */
    memset_s(&stCanvasBuf, sizeof(stCanvasBuf), 0, sizeof(stCanvasBuf));
    stCanvasBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
    stCanvasBuf.stCanvas.u32Pitch = info->fix.line_length;
    stCanvasBuf.stCanvas.u32PhyAddr = u32TmpAddr;
    stCanvasBuf.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stCanvasBuf.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stCanvasBuf.UpdateRect = UpdateRect;
    /* **********************end************************* */

    /* ******************config 3D buffer******************* */
    memset_s(&stDisplayBuf, sizeof(stDisplayBuf), 0, sizeof(stDisplayBuf));
    stDisplayBuf.stCanvas.enFmt = pstPar->st3DInfo.st3DSurface.enFmt;
    stDisplayBuf.stCanvas.u32Pitch = pstPar->st3DInfo.st3DSurface.u32Pitch;
    stDisplayBuf.stCanvas.u32PhyAddr = pstPar->stRunInfo.CurScreenAddr;
    stDisplayBuf.stCanvas.u32Width = pstPar->st3DInfo.st3DSurface.u32Width;
    stDisplayBuf.stCanvas.u32Height = pstPar->st3DInfo.st3DSurface.u32Height;
    /* **********************end************************* */

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, stDisplayBuf.stCanvas.u32Width,
                                  stDisplayBuf.stCanvas.u32Height, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stDisplayBuf.stCanvas.u32Width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stDisplayBuf.stCanvas.u32Height);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_PARA_CHECK_SUPPORT, Ret);
        return HI_FAILURE;
    }
    if (HIFB_STEREO_SIDEBYSIDE_HALF == pstPar->st3DInfo.StereoMode) {
        stDisplayBuf.stCanvas.u32Width >>= 1;
    } else if (HIFB_STEREO_TOPANDBOTTOM == pstPar->st3DInfo.StereoMode) {
        stDisplayBuf.stCanvas.u32Height >>= 1;
    }

    stDisplayBuf.UpdateRect.x = 0;
    stDisplayBuf.UpdateRect.y = 0;
    stDisplayBuf.UpdateRect.w = stDisplayBuf.stCanvas.u32Width;
    stDisplayBuf.UpdateRect.h = stDisplayBuf.stCanvas.u32Height;

    pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
    pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(&stCanvasBuf, &stDisplayBuf, &stBlitOpt, HI_TRUE);
    if (Ret <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
        return HI_FAILURE;
    }

    /* * can relese stereo memory * */
    pstPar->st3DInfo.BegUpStereoInfo = HI_FALSE;
    osal_wait_wakeup(&pstPar->st3DInfo.WaiteFinishUpStereoInfoMutex);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

int drv_hifb_beg_free_stereo_mem(const void *param)
{
    HIFB_PAR_S *pstPar = (HIFB_PAR_S *)param;

    if (pstCanvasBuf == HI_NULL) {
        return 0;
    }
    if (pstPar->st3DInfo.BegFreeStereoMem == HI_FALSE) {
        return 1;
    }
    return 0;
}

/***************************************************************************************
 * func         : DRV_HIFB_Flip_0buf_3D
 * description  : CNcomment: 单buffer刷新 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_0buf_3D(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 BufferSize = 0;
    hi_ulong StereoLockFlag = 0;
    HIFB_BLIT_OPT_S stBlitOpt;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);
    pstPar->st3DInfo.st3DSurface.enFmt = pstCanvasBuf->stCanvas.enFmt;
    pstPar->st3DInfo.st3DSurface.u32Pitch = pstCanvasBuf->stCanvas.u32Pitch;
    pstPar->st3DInfo.st3DSurface.u32Width = pstCanvasBuf->stCanvas.u32Width;
    pstPar->st3DInfo.st3DSurface.u32Height = pstCanvasBuf->stCanvas.u32Height;

    BufferSize = HI_HIFB_GetMemSize(pstCanvasBuf->stCanvas.u32Pitch, pstCanvasBuf->stCanvas.u32Height);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(BufferSize, 0, HI_FAILURE);

    osal_wait_timeout_interruptible(&pstPar->st3DInfo.WaiteFinishFreeStereoMemMutex, drv_hifb_beg_free_stereo_mem,
                                    pstPar, CONFIG_HIFB_STEREO_WAITE_TIME);

    DRV_HIFB_Lock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
    /* * IsStereo and BegUpStereoInfo should in one lock* */
    if (HI_FALSE == pstPar->st3DInfo.IsStereo) {
        DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
    pstPar->st3DInfo.BegUpStereoInfo = HI_TRUE;
    DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);

    /**<--alloc stereo buffer >**/
    Ret = DRV_HIFB_ReAllocStereoBuf(u32LayerId, pstCanvasBuf->stCanvas.u32Pitch, BufferSize);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32LayerId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, BufferSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstCanvasBuf->stCanvas.u32Pitch);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ReAllocStereoBuf, Ret);
        return Ret;
    }

    /**<--config 3D surface par, user display buffer0 >**/
    pstPar->st3DInfo.st3DSurface.u32PhyAddr = pstPar->st3DInfo.u32DisplayAddr[0];

    /**<--config 3D buffer >**/
    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    stBlitOpt.bBlock = HI_FALSE;
    stBlitOpt.bScale = HI_TRUE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;

    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    Ret = DRV_HIFB_UpStereoData(u32LayerId, pstCanvasBuf, &stBlitOpt);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_UpStereoData, Ret);
        osal_wait_wakeup(&pstPar->st3DInfo.WaiteFinishUpStereoInfoMutex);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.CurScreenAddr = pstPar->st3DInfo.mem_info_3d.smmu_addr;
    pstPar->stDispInfo.stUserBuffer.stCanvas.u32Pitch = pstCanvasBuf->stCanvas.u32Pitch;
    pstPar->stDispInfo.stUserBuffer.stCanvas.enFmt = pstCanvasBuf->stCanvas.enFmt;
    HIFB_SetDispSize(u32LayerId, pstCanvasBuf->stCanvas.u32Width, pstCanvasBuf->stCanvas.u32Height);

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    DRV_HIFB_WaitVBlank(u32LayerId);

    DRV_HIFB_Lock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
    /* * can relese stereo memory * */
    pstPar->st3DInfo.BegUpStereoInfo = HI_FALSE;
    DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);

    osal_wait_wakeup(&pstPar->st3DInfo.WaiteFinishUpStereoInfoMutex);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : DRV_HIFB_Flip_1buf_3D
 * description   : CNcomment: 双buffer刷新 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_1buf_3D(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_BLIT_OPT_S stBlitOpt;
    HIFB_OSD_DATA_S stOsdData;
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOSDData(u32LayerId, &stOsdData);

    if ((stOsdData.u32RegPhyAddr != pstPar->stDispInfo.u32DisplayAddr[0]) && (pstPar->stDispInfo.u32DisplayAddr[0])) {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
        pstPar->stRunInfo.CurScreenAddr = pstPar->stDispInfo.u32DisplayAddr[0];

        GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info->screen_base, HI_FAILURE);
        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(info->fix.smem_len, 0, HI_FAILURE);
        memset_s(info->screen_base, info->fix.smem_len, 0x0, info->fix.smem_len);

        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    u32Stride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(u32Stride, 0, HI_FAILURE);

    pstPar->st3DInfo.st3DSurface.enFmt = pstPar->stExtendInfo.enColFmt;
    pstPar->st3DInfo.st3DSurface.u32Pitch = u32Stride;
    pstPar->st3DInfo.st3DSurface.u32Width = pstPar->stExtendInfo.DisplayWidth;
    pstPar->st3DInfo.st3DSurface.u32Height = pstPar->stExtendInfo.DisplayHeight;
    pstPar->st3DInfo.st3DSurface.u32PhyAddr = pstPar->st3DInfo.u32DisplayAddr[0];

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));

    stBlitOpt.bBlock = HI_FALSE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;
    stBlitOpt.bScale = HI_TRUE;
    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);
    Ret = DRV_HIFB_UpStereoData(u32LayerId, pstCanvasBuf, &stBlitOpt);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_UpStereoData, Ret);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : DRV_HIFB_Flip_2buf_3D
 * description   : CNcomment: 3 buffer刷新 异步，刷新不等更新完，允许丢帧 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_2buf_3D(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Index;
    unsigned long lockflag;
    HIFB_BUFFER_S stBackBuf;
    HIFB_BLIT_OPT_S stBlitOpt;
    HIFB_OSD_DATA_S stOsdData;
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */
    u32Index = pstPar->stRunInfo.u32IndexForInt;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE((1 - u32Index), CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    memset_s(&stBackBuf, sizeof(stBackBuf), 0, sizeof(stBackBuf));

    stBlitOpt.bCallBack = HI_TRUE;
    stBlitOpt.pfnCallBack = (IntCallBack)DRV_HIFB_BlitFinishCallBack;
    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &lockflag);
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &lockflag);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOSDData(u32LayerId, &stOsdData);

    u32Stride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(u32Stride, 0, HI_FAILURE);

    pstPar->st3DInfo.st3DSurface.enFmt = pstPar->stExtendInfo.enColFmt;
    pstPar->st3DInfo.st3DSurface.u32Pitch = u32Stride;
    pstPar->st3DInfo.st3DSurface.u32Width = pstPar->stExtendInfo.DisplayWidth;
    pstPar->st3DInfo.st3DSurface.u32Height = pstPar->stExtendInfo.DisplayHeight;
    pstPar->st3DInfo.st3DSurface.u32PhyAddr = pstPar->st3DInfo.u32DisplayAddr[u32Index];

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstPar->st3DInfo.st3DSurface.enFmt);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstPar->st3DInfo.st3DSurface.u32Pitch);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstPar->st3DInfo.st3DSurface.u32Width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstPar->st3DInfo.st3DSurface.u32Height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstPar->st3DInfo.st3DSurface.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32Index);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstPar->stRunInfo.u32IndexForInt);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    if (memcpy_s(&stBackBuf.stCanvas, sizeof(HIFB_SURFACE_S), &pstPar->st3DInfo.st3DSurface, sizeof(HIFB_SURFACE_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    HIFB_GetUpdateRect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

    if (pstPar->stRunInfo.bFliped && (stOsdData.u32RegPhyAddr == pstPar->st3DInfo.u32DisplayAddr[1 - u32Index])) {
        HIFB_BackUpForeBuf(u32LayerId, &stBackBuf);
        memset_s(&(pstPar->st3DInfo.st3DUpdateRect), sizeof(HIFB_RECT), 0, sizeof(HIFB_RECT));
        pstPar->stRunInfo.bFliped = HI_FALSE;
    }

    if ((pstPar->st3DInfo.st3DUpdateRect.w == 0) || (pstPar->st3DInfo.st3DUpdateRect.h == 0)) {
        if (memcpy_s(&pstPar->st3DInfo.st3DUpdateRect, sizeof(HIFB_RECT), &stBackBuf.UpdateRect, sizeof(HIFB_RECT)) !=
            EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
        }
    } else {
        HIFB_UNITE_RECT(pstPar->st3DInfo.st3DUpdateRect, stBackBuf.UpdateRect);
    }

    stBlitOpt.bBlock = HI_FALSE;
    stBlitOpt.bScale = HI_TRUE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;
    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    Ret = DRV_HIFB_UpStereoData(u32LayerId, pstCanvasBuf, &stBlitOpt);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_UpStereoData, Ret);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_Flip_2buf_immediate_display_3D
 * description : CNcomment: 3 buffer 同步，刷新等待更新完 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip_2buf_immediate_display_3D(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Index = 0;
    unsigned long lockflag;
    HIFB_BUFFER_S stBackBuf;
    HIFB_BLIT_OPT_S stBlitOpt;
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    u32Index = pstPar->stRunInfo.u32IndexForInt;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE((1 - u32Index), CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, HI_FAILURE);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstCanvasBuf, HI_FAILURE);

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    memset_s(&stBackBuf, sizeof(stBackBuf), 0, sizeof(stBackBuf));

    stBlitOpt.bCallBack = HI_FALSE;
    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &lockflag);
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &lockflag);

    u32Stride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(u32Stride, 0, HI_FAILURE);

    pstPar->st3DInfo.st3DSurface.enFmt = pstPar->stExtendInfo.enColFmt;
    pstPar->st3DInfo.st3DSurface.u32Pitch = u32Stride;
    pstPar->st3DInfo.st3DSurface.u32Width = pstPar->stExtendInfo.DisplayWidth;
    pstPar->st3DInfo.st3DSurface.u32Height = pstPar->stExtendInfo.DisplayHeight;
    pstPar->st3DInfo.st3DSurface.u32PhyAddr = pstPar->st3DInfo.u32DisplayAddr[u32Index];

    if (memcpy_s(&stBackBuf.stCanvas, sizeof(HIFB_SURFACE_S), &pstPar->st3DInfo.st3DSurface, sizeof(HIFB_SURFACE_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    HIFB_GetUpdateRect(u32LayerId, pstCanvasBuf, &stBackBuf.UpdateRect);

    HIFB_BackUpForeBuf(u32LayerId, &stBackBuf);

    if (memcpy_s(&pstPar->st3DInfo.st3DUpdateRect, sizeof(HIFB_RECT), &stBackBuf.UpdateRect, sizeof(HIFB_RECT)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    stBlitOpt.bScale = HI_TRUE;
    stBlitOpt.bBlock = HI_TRUE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;
    if (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    }

    Ret = DRV_HIFB_UpStereoData(u32LayerId, pstCanvasBuf, &stBlitOpt);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_UpStereoData, Ret);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.CurScreenAddr = pstPar->st3DInfo.u32DisplayAddr[u32Index];
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    pstPar->stRunInfo.u32IndexForInt = 1 - u32Index;

    if (memcpy_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(HIFB_BUFFER_S), pstCanvasBuf, sizeof(HIFB_BUFFER_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    DRV_HIFB_WaitVBlank(u32LayerId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

/***************************************************************************************
 * func         : DRV_HIFB_Flip
 * description  : CNcomment: 刷新 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_Flip(hi_u32 u32LayerId, HIFB_BUFFER_S *pstCanvasBuf, HIFB_LAYER_BUF_E enBufMode)
{
    hi_s32 Ret = HI_FAILURE;
    struct fb_info *info = NULL;
    HIFB_PAR_S *par = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **************************beg check par ********************************* */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);
    /* **************************end check par ********************************* */

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (par->st3DInfo.IsStereo) {
        switch (enBufMode) {
            case HIFB_LAYER_BUF_DOUBLE:
            case HIFB_LAYER_BUF_FENCE:
                Ret = DRV_HIFB_Flip_2buf_3D(u32LayerId, pstCanvasBuf);
                break;
            case HIFB_LAYER_BUF_ONE:
                Ret = DRV_HIFB_Flip_1buf_3D(u32LayerId, pstCanvasBuf);
                par->up_mute = HI_FALSE;
                break;
            case HIFB_LAYER_BUF_NONE:
                Ret = DRV_HIFB_Flip_0buf_3D(u32LayerId, pstCanvasBuf);
                par->up_mute = HI_FALSE;
                break;
            case HIFB_LAYER_BUF_DOUBLE_IMMEDIATE:
                Ret = DRV_HIFB_Flip_2buf_immediate_display_3D(u32LayerId, pstCanvasBuf);
                par->up_mute = HI_FALSE;
                break;
            default:
                break;
        }
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    }
#endif
    switch (enBufMode) {
        case HIFB_LAYER_BUF_DOUBLE:
            Ret = DRV_HIFB_Flip_2buf(u32LayerId, pstCanvasBuf);
            break;
        case HIFB_LAYER_BUF_ONE:
            Ret = DRV_HIFB_Flip_1buf(u32LayerId, pstCanvasBuf);
            par->up_mute = HI_FALSE;
            break;
        case HIFB_LAYER_BUF_NONE:
            Ret = DRV_HIFB_Flip_0buf(u32LayerId, pstCanvasBuf);
            par->up_mute = HI_FALSE;
            break;
        case HIFB_LAYER_BUF_DOUBLE_IMMEDIATE:
            Ret = DRV_HIFB_Flip_2buf_immediate_display(u32LayerId, pstCanvasBuf);
            par->up_mute = HI_FALSE;
            break;
        case HIFB_LAYER_BUF_FENCE:
            Ret = DRV_HIFB_FlipWithFence(u32LayerId, pstCanvasBuf);
            par->up_mute = HI_FALSE;
            break;
        default:
            break;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

/***************************************************************************
 * func          : DRV_HIFB_Refresh
 * description   : CNcomment: 图形层扩展刷新接口 CNend\n
 * param[in]     : pFbInfo
 * param[in]     : Args
 * retval        : HI_SUCCESS 成功
 * retval        : HI_FAILURE 失败
 * others:       : NA
 ****************************************************************************/
static hi_s32 DRV_HIFB_Refresh(HIFB_PAR_S *pstPar, hi_void __user *pArgp)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_BUFFER_S stCanvasBuf;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **************************beg check par ********************************* */
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pArgp, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **************************end check par ********************************* */

    if (osal_copy_from_user(&stCanvasBuf, pArgp, sizeof(HIFB_BUFFER_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

/* **************************beg pandisplay for stereo ********************* */
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if ((HIFB_REFRESH_MODE_WITH_PANDISPLAY == pstPar->stExtendInfo.enBufMode) &&
        ((pstPar->st3DInfo.mem_info_3d.smmu_addr != 0) && (HI_TRUE == pstPar->st3DInfo.IsStereo))) {
        return DRV_HIFB_Flip_panbuf(pstPar->stBaseInfo.u32LayerID, &stCanvasBuf);
    }

    if (HIFB_REFRESH_MODE_WITH_PANDISPLAY == pstPar->stExtendInfo.enBufMode) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.enBufMode);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HIFB_REFRESH_MODE_WITH_PANDISPLAY);
        return HI_FAILURE;
    }
#endif
    /* **************************end pandisplay for stereo ********************* */

    /* **************************beg check par ********************************* */
    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, stCanvasBuf.stCanvas.u32Width,
                                  stCanvasBuf.stCanvas.u32Height, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.stCanvas.u32Width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.stCanvas.u32Height);
        return HI_FAILURE;
    }

    if (stCanvasBuf.stCanvas.enFmt >= DRV_HIFB_FMT_BUTT) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.stCanvas.enFmt);
        return HI_FAILURE;
    }

    if ((stCanvasBuf.UpdateRect.x >= stCanvasBuf.stCanvas.u32Width) ||
        (stCanvasBuf.UpdateRect.y >= stCanvasBuf.stCanvas.u32Height) || (0 == stCanvasBuf.UpdateRect.w) ||
        (0 == stCanvasBuf.UpdateRect.h)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.UpdateRect.x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.UpdateRect.y);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.UpdateRect.w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.UpdateRect.h);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.stCanvas.u32Width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.stCanvas.u32Height);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(stCanvasBuf.UpdateRect.w, stCanvasBuf.UpdateRect.x, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(stCanvasBuf.UpdateRect.x, stCanvasBuf.stCanvas.u32Width, HI_FAILURE);
    if (stCanvasBuf.UpdateRect.x + stCanvasBuf.UpdateRect.w > stCanvasBuf.stCanvas.u32Width) {
        stCanvasBuf.UpdateRect.w = stCanvasBuf.stCanvas.u32Width - stCanvasBuf.UpdateRect.x;
    }

    GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(stCanvasBuf.UpdateRect.h, stCanvasBuf.UpdateRect.y, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(stCanvasBuf.UpdateRect.y, stCanvasBuf.stCanvas.u32Height, HI_FAILURE);
    if (stCanvasBuf.UpdateRect.y + stCanvasBuf.UpdateRect.h > stCanvasBuf.stCanvas.u32Height) {
        stCanvasBuf.UpdateRect.h = stCanvasBuf.stCanvas.u32Height - stCanvasBuf.UpdateRect.y;
    }

    if (HIFB_LAYER_BUF_NONE == pstPar->stExtendInfo.enBufMode) {
        if ((stCanvasBuf.stCanvas.u32PhyAddr & 0xf) || (stCanvasBuf.stCanvas.u32Pitch & 0xf)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.stCanvas.u32PhyAddr);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stCanvasBuf.stCanvas.u32Pitch);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "address and stride are not 16 bytes align");
            return HI_FAILURE;
        }
    }
    /* **************************end check par ********************************* */

    Ret = DRV_HIFB_Flip(pstPar->stBaseInfo.u32LayerID, &stCanvasBuf, pstPar->stExtendInfo.enBufMode);

    /* **************************beg get fence fd and buffer num *************** */
    if (HIFB_LAYER_BUF_FENCE == pstPar->stExtendInfo.enBufMode) {
        if (osal_copy_to_user(pArgp, &stCanvasBuf, sizeof(HIFB_BUFFER_S))) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
            return HI_FAILURE;
        }
    }
    /* **************************end get fence fd and buffer num *************** */

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

static inline hi_s32 DRV_HIFB_CheckWhetherMemSizeEnough(struct fb_info *info, HIFB_LAYER_INFO_S *pstLayerInfo)
{
    hi_u32 stride;
    hi_u32 cmp_stride;
    hi_u32 is_input_data_size_changed;
    hi_s32 ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstLayerInfo, HI_FAILURE);
    is_input_data_size_changed = (pstLayerInfo->u32Mask & HIFB_LAYERMASK_DISPSIZE);

    if (is_input_data_size_changed != 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    stride = CONIFG_HIFB_GetMaxStride(pstLayerInfo->u32DisplayWidth, info->var.bits_per_pixel, &cmp_stride,
                                      CONFIG_HIFB_STRIDE_ALIGN);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(stride, 0, HI_FAILURE);

    if (pstLayerInfo->u32DisplayWidth == 0 || pstLayerInfo->u32DisplayHeight == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstLayerInfo->u32DisplayWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstLayerInfo->u32DisplayHeight);
        return HI_FAILURE;
    }

    ret = HIFB_CheckMemEnough(info, stride, pstLayerInfo->u32DisplayHeight);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_CheckMemEnough, FAILURE_TAG);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static inline hi_s32 DRV_HIFB_CheckWhetherLayerSizeSupport(struct fb_info *info, HIFB_LAYER_INFO_S *pstLayerInfo)
{
    hi_u32 LayerSize = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstLayerInfo, HI_FAILURE);
    if (HIFB_LAYERMASK_BUFMODE == (pstLayerInfo->u32Mask & HIFB_LAYERMASK_BUFMODE)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    LayerSize = HI_HIFB_GetMemSize(info->fix.line_length, info->var.yres);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(LayerSize, 0, HI_FAILURE);

    if (pstLayerInfo->BufMode == HIFB_LAYER_BUF_ONE) {
        LayerSize = 1 * LayerSize;
    } else if ((HIFB_LAYER_BUF_DOUBLE == pstLayerInfo->BufMode) ||
               (HIFB_LAYER_BUF_DOUBLE_IMMEDIATE == pstLayerInfo->BufMode) ||
               (HIFB_LAYER_BUF_FENCE == pstLayerInfo->BufMode)) {
        GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(LayerSize, 2, HI_FAILURE);
        LayerSize = 2 * LayerSize;
    } else {
        LayerSize = 0 * LayerSize;
    }

    if (LayerSize > info->fix.smem_len) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->fix.smem_len);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static inline hi_s32 DRV_HIFB_CheckWhetherLayerPosSupport(HIFB_LAYER_INFO_S *pstLayerInfo)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstLayerInfo, HI_FAILURE);

    if ((pstLayerInfo->u32Mask & HIFB_LAYERMASK_POS) && ((pstLayerInfo->s32XPos < 0) || (pstLayerInfo->s32YPos < 0))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstLayerInfo->u32Mask);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstLayerInfo->s32XPos);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstLayerInfo->s32YPos);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void DRV_HIFB_CheckWhetherDispSizeChange(struct fb_info *info, HIFB_LAYER_INFO_S *pstLayerInfo)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 IsInputDataSizeHasChanged = 0x0;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstLayerInfo);
    IsInputDataSizeHasChanged = (pstLayerInfo->u32Mask & HIFB_LAYERMASK_DISPSIZE);
    if (!IsInputDataSizeHasChanged) {
        return;
    }

    if ((pstLayerInfo->u32DisplayWidth > info->var.xres_virtual) ||
        (pstLayerInfo->u32DisplayHeight > info->var.yres_virtual)) {
        return;
    }

    Ret = HIFB_SetDispSize(pstPar->stBaseInfo.u32LayerID, pstLayerInfo->u32DisplayWidth,
                           pstLayerInfo->u32DisplayHeight);
    if (Ret == HI_SUCCESS) {
        info->var.xres = pstLayerInfo->u32DisplayWidth;
        info->var.yres = pstLayerInfo->u32DisplayHeight;
        DRV_HIFB_AssignDispBuf(pstPar->stBaseInfo.u32LayerID);
    }

    DRV_HIFB_FlipAll(info);

    return;
}

static hi_s32 DRV_HIFB_RefreshUserBuffer(hi_u32 u32LayerId)
{
    HIFB_BUFFER_S stCanvas;
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *info = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **********************BEG CHECK PAR ***************************** */
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32LayerId, HI_GFX_ARRAY_SIZE(s_stLayer), HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    /* **********************END CHECK PAR ***************************** */

    if (pstPar->stDispInfo.stUserBuffer.stCanvas.u32PhyAddr) { /**<-- has user buffer >**/
        memset_s(&stCanvas, sizeof(stCanvas), 0x0, sizeof(stCanvas));
        stCanvas = pstPar->stDispInfo.stUserBuffer;
        stCanvas.UpdateRect.x = 0;
        stCanvas.UpdateRect.y = 0;
        stCanvas.UpdateRect.w = stCanvas.stCanvas.u32Width;
        stCanvas.UpdateRect.h = stCanvas.stCanvas.u32Height;

        DRV_HIFB_Flip(pstPar->stBaseInfo.u32LayerID, &stCanvas, pstPar->stExtendInfo.enBufMode);
    } else { /**<-- after open, change un stereo to stereo, now has not user buffer >**/
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stRunInfo.CurScreenAddr = info->fix.smem_start;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_FMT;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_FlipAll(struct fb_info *info)
{
    HIFB_PAR_S *par = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (par->st3DInfo.IsStereo) {
        if (HIFB_REFRESH_MODE_WITH_PANDISPLAY == par->stExtendInfo.enBufMode) {
            DRV_HIFB_REFRESH_PanDisplay(&info->var, info);
        }

        if (HIFB_LAYER_BUF_NONE == par->stExtendInfo.enBufMode) {
            DRV_HIFB_RefreshUserBuffer(par->stBaseInfo.u32LayerID);
        }
    }
#endif

    if (HIFB_REFRESH_MODE_WITH_PANDISPLAY != par->stExtendInfo.enBufMode &&
        HIFB_LAYER_BUF_NONE != par->stExtendInfo.enBufMode) {
        DRV_HIFB_RefreshUserBuffer(par->stBaseInfo.u32LayerID);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetColorKey(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_COLORKEY_S ck;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    memset_s(&ck, sizeof(ck), 0x0, sizeof(ck));
    ck.bKeyEnable = pstPar->stExtendInfo.stCkey.bKeyEnable;
    ck.u32Key = pstPar->stExtendInfo.stCkey.u32Key;
    if (osal_copy_to_user(argp, &ck, sizeof(HIFB_COLORKEY_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetColorKey(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_COLORKEY_S ckey;
    hi_u32 RLen = 0, GLen = 0, BLen = 0;
    hi_u32 ROffset = 0, GOffset = 0, BOffset = 0;
    hi_u8 RMask = 0, GMask = 0, BMask = 0;
    hi_u8 RKey = 0, GKey = 0, BKey = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&ckey, argp, sizeof(HIFB_COLORKEY_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if (ckey.bKeyEnable && pstPar->stBaseInfo.bPreMul) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ckey.bKeyEnable);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstPar->stBaseInfo.bPreMul);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stExtendInfo.stCkey.u32Key = ckey.u32Key;
    pstPar->stExtendInfo.stCkey.bKeyEnable = ckey.bKeyEnable;

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(info->var.bits_per_pixel, 0, HI_FAILURE);
    if (info->var.bits_per_pixel <= 8) {
        if (ckey.u32Key >= (1 << info->var.bits_per_pixel)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ckey.u32Key);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, info->var.bits_per_pixel);
            return HI_FAILURE;
        }
        pstPar->stExtendInfo.stCkey.u8BlueMax = pstPar->stExtendInfo.stCkey.u8BlueMin = info->cmap.blue[ckey.u32Key];
        pstPar->stExtendInfo.stCkey.u8GreenMax = pstPar->stExtendInfo.stCkey.u8GreenMin = info->cmap.green[ckey.u32Key];
        pstPar->stExtendInfo.stCkey.u8RedMax = pstPar->stExtendInfo.stCkey.u8RedMin = info->cmap.red[ckey.u32Key];
    } else {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_ColorConvert(&info->var, &pstPar->stExtendInfo.stCkey);

        GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(pstPar->stExtendInfo.enColFmt, HIFB_MAX_PIXFMT_NUM, HI_FAILURE);
        RLen = s_stArgbBitField[pstPar->stExtendInfo.enColFmt].stRed.length;
        GLen = s_stArgbBitField[pstPar->stExtendInfo.enColFmt].stGreen.length;
        BLen = s_stArgbBitField[pstPar->stExtendInfo.enColFmt].stBlue.length;

        RMask = (0xff >> (8 - RLen));
        GMask = (0xff >> (8 - GLen));
        BMask = (0xff >> (8 - BLen));

        ROffset = s_stArgbBitField[pstPar->stExtendInfo.enColFmt].stRed.offset;
        GOffset = s_stArgbBitField[pstPar->stExtendInfo.enColFmt].stGreen.offset;
        BOffset = s_stArgbBitField[pstPar->stExtendInfo.enColFmt].stBlue.offset;

        if (DRV_HIFB_FMT_ABGR8888 != pstPar->stExtendInfo.enColFmt) {
            RKey = (pstPar->stExtendInfo.stCkey.u32Key >> (BLen + GLen)) & (RMask);
            GKey = (pstPar->stExtendInfo.stCkey.u32Key >> (BLen)) & (GMask);
            BKey = (pstPar->stExtendInfo.stCkey.u32Key) & (BMask);
        } else {
            RKey = (pstPar->stExtendInfo.stCkey.u32Key >> ROffset) & (RMask);
            GKey = (pstPar->stExtendInfo.stCkey.u32Key >> GOffset) & (GMask);
            BKey = (pstPar->stExtendInfo.stCkey.u32Key >> BOffset) & (BMask);
        }

        /* * add low bit with 0 value * */
        pstPar->stExtendInfo.stCkey.u8RedMin = RKey << (8 - RLen);
        pstPar->stExtendInfo.stCkey.u8GreenMin = GKey << (8 - GLen);
        pstPar->stExtendInfo.stCkey.u8BlueMin = BKey << (8 - BLen);

        /* * add low bit with 1 value * */
        pstPar->stExtendInfo.stCkey.u8RedMax = pstPar->stExtendInfo.stCkey.u8RedMin | (0xff >> RLen);
        pstPar->stExtendInfo.stCkey.u8GreenMax = pstPar->stExtendInfo.stCkey.u8GreenMin | (0xff >> GLen);
        pstPar->stExtendInfo.stCkey.u8BlueMax = pstPar->stExtendInfo.stCkey.u8BlueMin | (0xff >> BLen);
    }

    pstPar->stExtendInfo.stCkey.u8RedMask = 0xff;
    pstPar->stExtendInfo.stCkey.u8BlueMask = 0xff;
    pstPar->stExtendInfo.stCkey.u8GreenMask = 0xff;

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_COLORKEY;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetLayerAlpha(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_to_user(argp, &pstPar->stExtendInfo.stAlpha, sizeof(HIFB_ALPHA_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetLayerAlpha(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_ALPHA_S stAlpha = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&(pstPar->stExtendInfo.stAlpha), argp, sizeof(HIFB_ALPHA_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstPar->stExtendInfo.stAlpha.u8Alpha0);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstPar->stExtendInfo.stAlpha.u8Alpha1);

    stAlpha = pstPar->stExtendInfo.stAlpha;
    if (!pstPar->stExtendInfo.stAlpha.bAlphaChannel) {
        stAlpha.u8GlobalAlpha |= 0xff;
        pstPar->stExtendInfo.stAlpha.u8GlobalAlpha |= 0xff;
    }

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstPar->stExtendInfo.stAlpha.u8Alpha0);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstPar->stExtendInfo.stAlpha.u8Alpha1);

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_ALPHA;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetScreenOriginPos(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_to_user(argp, &pstPar->stExtendInfo.stPos, sizeof(HIFB_POINT_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetScreenOriginPos(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_POINT_S origin;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&origin, argp, sizeof(HIFB_POINT_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if (pstPar->stBaseInfo.u32LayerID != HIFB_LAYER_HD_3) {
        if (origin.s32XPos < 0 || origin.s32YPos < 0) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, origin.s32XPos);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, origin.s32YPos);
            return HI_FAILURE;
        }
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stExtendInfo.stPos.s32XPos = origin.s32XPos;
    pstPar->stExtendInfo.stPos.s32YPos = origin.s32YPos;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "=========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstPar->stExtendInfo.stPos.s32XPos);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstPar->stExtendInfo.stPos.s32YPos);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "=========================================================");

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetDeFlicker(struct fb_info *info, hi_ulong arg)
{
    return HI_FAILURE;
}

static hi_s32 DRV_HIFB_SetDeFlicker(struct fb_info *info, hi_ulong arg)
{
    return HI_FAILURE;
}

static hi_s32 DRV_HIFB_GetVblank(struct fb_info *info, hi_ulong arg)
{
    hi_s32 Ret = HI_FAILURE;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (NULL == info) {
        return HI_FAILURE;
    }
    pstPar = (HIFB_PAR_S *)info->par;
    if (NULL == pstPar) {
        return HI_FAILURE;
    }

    Ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_WaitVBlank(pstPar->stBaseInfo.u32LayerID);
    if (Ret < 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_WaitVBlank, Ret);
        return -EPERM;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_ShowLayer(struct fb_info *info, hi_ulong arg)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hi_bool bShow = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    Ret = osal_copy_from_user(&bShow, argp, sizeof(hi_bool));
    if (Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, Ret);
        return -EFAULT;
    }

    /* reset the same status */
    if (bShow == pstPar->stExtendInfo.bShow) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return 0;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stExtendInfo.bShow = bShow;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetLayerShowState(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_to_user(argp, &pstPar->stExtendInfo.bShow, sizeof(hi_bool))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetCapablity(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (pstPar->stBaseInfo.u32LayerID >= HIFB_LAYER_ID_BUTT) {
        return HI_FAILURE;
    }

    if (osal_copy_to_user(argp, (hi_void *)&gs_pstCapacity[pstPar->stBaseInfo.u32LayerID], sizeof(HIFB_CAPABILITY_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetDecompress(struct fb_info *info, hi_ulong arg)
{
#ifdef CFG_HI_FB_DECOMPRESS_SUPPORT
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hi_bool bDeComp = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&bDeComp, argp, sizeof(hi_bool))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetDeCmpSwitch(pstPar->stBaseInfo.u32LayerID, bDeComp);

    pstPar->compress_mode = (bDeComp == HI_TRUE) ? DRV_HIFB_CMP_MODE_HFBC : DRV_HIFB_CMP_MODE_NONE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
#else
#endif

    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetLayerInfo(struct fb_info *info, hi_ulong arg)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_LAYER_INFO_S stLayerInfo;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&stLayerInfo, argp, sizeof(HIFB_LAYER_INFO_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = HIFB_SetCanvasBufInfo(info, &stLayerInfo);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_SetCanvasBufInfo, Ret);
        return HI_FAILURE;
    }

    Ret = DRV_HIFB_CheckWhetherMemSizeEnough(info, &stLayerInfo);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_CheckWhetherMemSizeEnough, Ret);
        return HI_FAILURE;
    }

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if ((stLayerInfo.u32Mask & HIFB_LAYERMASK_DISPSIZE) && pstPar->st3DInfo.IsStereo) {
        DRV_HIFB_ClearUnUsedStereoBuf(info);
    }
#endif

    Ret = DRV_HIFB_CheckWhetherLayerSizeSupport(info, &stLayerInfo);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_CheckWhetherLayerSizeSupport, Ret);
        return HI_FAILURE;
    }

    Ret = DRV_HIFB_CheckWhetherLayerPosSupport(&stLayerInfo);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_CheckWhetherLayerPosSupport, Ret);
        return HI_FAILURE;
    }

    if ((stLayerInfo.u32Mask & HIFB_LAYERMASK_BMUL) && pstPar->stExtendInfo.stCkey.bKeyEnable) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stLayerInfo.u32Mask);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstPar->stExtendInfo.stCkey.bKeyEnable);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.bModifying = HI_TRUE;

    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_BMUL) {
        pstPar->stBaseInfo.bPreMul = stLayerInfo.bPreMul;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_BMUL;
    }

    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_BUFMODE) {
        HIFB_SetBufMode(pstPar->stBaseInfo.u32LayerID, stLayerInfo.BufMode);
    }

    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_POS) {
        HIFB_SetDispLayerPos(pstPar->stBaseInfo.u32LayerID, stLayerInfo.s32XPos, stLayerInfo.s32YPos);
    }

    if (stLayerInfo.u32Mask & HIFB_LAYERMASK_ANTIFLICKER_MODE) {
        DRV_HIFB_SetAntiflickerLevel(pstPar->stBaseInfo.u32LayerID, stLayerInfo.eAntiflickerLevel);
    }

    DRV_HIFB_CheckWhetherDispSizeChange(info, &stLayerInfo);

    pstPar->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

static hi_s32 DRV_HIFB_GetLayerInfo(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_LAYER_INFO_S stLayerInfo = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    DRV_HIFB_WaitVBlank(pstPar->stBaseInfo.u32LayerID);

    stLayerInfo.bPreMul = pstPar->stBaseInfo.bPreMul;
    stLayerInfo.BufMode = pstPar->stExtendInfo.enBufMode;
    stLayerInfo.eAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
    stLayerInfo.s32XPos = pstPar->stExtendInfo.stPos.s32XPos;
    stLayerInfo.s32YPos = pstPar->stExtendInfo.stPos.s32YPos;
    stLayerInfo.u32DisplayWidth = pstPar->stExtendInfo.DisplayWidth;
    stLayerInfo.u32DisplayHeight = pstPar->stExtendInfo.DisplayHeight;
    stLayerInfo.u32ScreenWidth = pstPar->stExtendInfo.DisplayWidth;
    stLayerInfo.u32ScreenHeight = pstPar->stExtendInfo.DisplayHeight;

    if (osal_copy_to_user(argp, &stLayerInfo, sizeof(HIFB_LAYER_INFO_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetCanvasBuffer(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_to_user(argp, &(pstPar->stDispInfo.stCanvasSur), sizeof(HIFB_SURFACE_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************
 * func          : DRV_HIFB_RefreshLayer
 * description   : CNcomment: 图形层扩展刷新接口 CNend\n
 * param[in]     : pFbInfo
 * param[in]     : Args
 * retval        : HI_SUCCESS 成功
 * retval        : HI_FAILURE 失败
 * others:       : NA
 ****************************************************************************/
static hi_s32 DRV_HIFB_RefreshLayer(struct fb_info *pFbInfo, hi_ulong Args)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 NowTimeMs = 0;
    hi_u32 EndTimeMs = 0;
    HIFB_PAR_S *pstPar = NULL;
    HI_GFX_TINIT();
    hi_void __user *pArgp = (hi_void __user *)(uintptr_t)Args;

    /* **************************beg check par ********************************* */
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(pFbInfo, pArgp);
    /* **************************end check par ********************************* */

    /* **************************beg count times ******************************* */
    HI_GFX_TSTART(NowTimeMs);
    pstPar->stFrameInfo.TwiceRefreshTimeMs = NowTimeMs - pstPar->stFrameInfo.PreRefreshTimeMs;
    pstPar->stFrameInfo.PreRefreshTimeMs = NowTimeMs;
    /* **************************end count times ******************************* */

    Ret = DRV_HIFB_Refresh(pstPar, pArgp);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_Refresh, Ret);
        return HI_FAILURE;
    }

    /* **************************beg count draw fps **************************** */
    pstPar->stFrameInfo.RefreshFrame++;
    /* **************************end count draw fps **************************** */

    /* **************************beg count run refresh times ******************* */
    HI_GFX_TEND(EndTimeMs);
    pstPar->stFrameInfo.RunRefreshTimeMs = EndTimeMs - NowTimeMs;
    /* **************************end count run refresh times ******************* */

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_WaiteRefreshFinish(struct fb_info *info, hi_ulong arg)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (NULL == info) {
        return HI_FAILURE;
    }
    pstPar = (HIFB_PAR_S *)info->par;
    if (NULL == pstPar) {
        return HI_FAILURE;
    }

    if (pstPar->stRunInfo.s32RefreshHandle) {
        Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_WaitForDone(pstPar->stRunInfo.s32RefreshHandle, 1000);
    }

    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_WaitForDone, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetSteroFraming(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_FAILURE;
}

static hi_s32 DRV_HIFB_GetSteroFraming(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_FAILURE;
}

static hi_s32 DRV_HIFB_SetSteroMode(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetSteroMode(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetScreenSize(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetScreenSize(struct fb_info *info, hi_ulong arg)
{
    HIFB_SIZE_S stScreenSize = {0};
    HIFB_RECT stOutputRect = {0};
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOutRect(pstPar->stBaseInfo.u32LayerID, &stOutputRect);

    stScreenSize.u32Width = stOutputRect.w;
    stScreenSize.u32Height = stOutputRect.h;
    if (osal_copy_to_user(argp, &stScreenSize, sizeof(HIFB_SIZE_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetCompress(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetCompress(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_ScrolltextCreate(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_SCROLLTEXT_CREATE_S stScrollText;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    memset_s(&stScrollText, sizeof(stScrollText), 0x0, sizeof(stScrollText));
    if (osal_copy_from_user(&stScrollText, argp, sizeof(HIFB_SCROLLTEXT_CREATE_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if (stScrollText.stAttr.ePixelFmt >= DRV_HIFB_FMT_BUTT) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stScrollText.stAttr.ePixelFmt);
        return HI_FAILURE;
    }

    if (stScrollText.stAttr.stRect.w < 0 || stScrollText.stAttr.stRect.h < 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stScrollText.stAttr.stRect.w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stScrollText.stAttr.stRect.h);
        return HI_FAILURE;
    }

    Ret = DRV_HIFB_SCROLLTEXT_Create(pstPar->stBaseInfo.u32LayerID, &stScrollText);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_Create, Ret);
        return -EFAULT;
    }

    if (osal_copy_to_user(argp, &stScrollText, sizeof(HIFB_SCROLLTEXT_CREATE_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
#else
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support scrolltext");
    return HI_FAILURE;
#endif
}

static hi_s32 DRV_HIFB_ScrolltextFill(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    hi_s32 Ret = HI_SUCCESS;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_SCROLLTEXT_DATA_S stScrollTextData;
    HIFB_SCROLLTEXT_DATA_64BITS_S stScrollText64BitsData;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (NULL == argp) {
        return HI_FAILURE;
    }

    memset_s(&stScrollTextData, sizeof(stScrollTextData), 0x0, sizeof(stScrollTextData));
    memset_s(&stScrollText64BitsData, sizeof(stScrollText64BitsData), 0x0, sizeof(stScrollText64BitsData));
    if (osal_copy_from_user(&stScrollText64BitsData, argp, sizeof(HIFB_SCROLLTEXT_DATA_64BITS_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    stScrollTextData.u32Handle = stScrollText64BitsData.u32Handle;
    stScrollTextData.u32PhyAddr = stScrollText64BitsData.u32PhyAddr;
    stScrollTextData.pu8VirAddr = (hi_u8 *)(uintptr_t)stScrollText64BitsData.u64VirAddr;
    stScrollTextData.addr_len = stScrollText64BitsData.addr_len;
    stScrollTextData.u32Stride = stScrollText64BitsData.u32Stride;

    if ((0 == stScrollTextData.u32PhyAddr) && (NULL == stScrollTextData.pu8VirAddr)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextData.u32PhyAddr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, stScrollTextData.pu8VirAddr);
        return -EFAULT;
    }

    if (stScrollTextData.addr_len == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextData.addr_len);
        return -EFAULT;
    }

    Ret = DRV_HIFB_SCROLLTEXT_FillText(&stScrollTextData);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_FillText, Ret);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
#else
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support scrolltext");
    return HI_FAILURE;
#endif
}

static hi_s32 DRV_HIFB_ScrolltextPause(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    Ret = DRV_HIFB_SCROLLTEXT_Enable(HI_TRUE, arg);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_Enable, Ret);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
#else
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support scrolltext");
    return HI_FAILURE;
#endif
}

static hi_s32 DRV_HIFB_ScrolltextResume(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    Ret = DRV_HIFB_SCROLLTEXT_Enable(HI_FALSE, arg);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_Enable, Ret);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
#else
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support scrolltext");
    return HI_FAILURE;
#endif
}

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
static inline hi_s32 DRV_HIFB_SCROLLTEXT_Enable(hi_bool bEnable, hi_ulong arg)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32LayerId = HIFB_LAYER_ID_BUTT;
    hi_u32 u32ScrollTextID = SCROLLTEXT_NUM;
    hi_u32 u32Handle;
    HIFB_SCROLLTEXT_S *pstScrollText = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (NULL == argp) {
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&u32Handle, argp, sizeof(hi_u32))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return HI_FAILURE;
    }

    Ret = DRV_HIFB_SCROLLTEXT_GetHandle(u32Handle, &u32LayerId, &u32ScrollTextID);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_GetHandle, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32ScrollTextID, SCROLLTEXT_NUM, HI_FAILURE);
    pstScrollText = &(s_stTextLayer[u32LayerId].stScrollText[u32ScrollTextID]);

    pstScrollText->bPause = bEnable;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

static hi_s32 DRV_HIFB_ScrolltextDestory(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    hi_s32 Ret = HI_SUCCESS;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hi_u32 u32LayerId = HIFB_LAYER_ID_BUTT;
    hi_u32 u32ScrollTextID = SCROLLTEXT_NUM;
    hi_u32 u32Handle;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (NULL == argp) {
        return HI_FAILURE;
    }

    if (osal_copy_from_user(&u32Handle, argp, sizeof(hi_u32))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = DRV_HIFB_SCROLLTEXT_GetHandle(u32Handle, &u32LayerId, &u32ScrollTextID);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_GetHandle, Ret);
        return -EFAULT;
    }

    Ret = DRV_HIFB_SCROLLTEXT_Destory(u32LayerId, u32ScrollTextID);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_Destory, Ret);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
#else
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support scrolltext");
    return HI_FAILURE;
#endif
}

static hi_s32 DRV_HIFB_SetSteroDepth(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hi_s32 s32StereoDepth = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&s32StereoDepth, argp, sizeof(hi_s32))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if (!pstPar->st3DInfo.IsStereo) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstPar->st3DInfo.IsStereo);
        return HI_FAILURE;
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetStereoDepth(pstPar->stBaseInfo.u32LayerID, s32StereoDepth);

    pstPar->st3DInfo.s32StereoDepth = s32StereoDepth;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
#else
#endif

    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetSteroDepth(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (!pstPar->st3DInfo.IsStereo) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "u need to set disp stereo mode first");
        return HI_FAILURE;
    }

    if (osal_copy_to_user(argp, &(pstPar->st3DInfo.s32StereoDepth), sizeof(hi_s32))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
#else
#endif

    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetLayerZorder(struct fb_info *info, hi_ulong arg)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_ZORDER_E enZorder;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&enZorder, argp, sizeof(HIFB_ZORDER_E))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    Ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerPriority(pstPar->stBaseInfo.u32LayerID, enZorder);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetLayerPriority, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetLayerZorder(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    hi_u32 u32Zorder = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerPriority(pstPar->stBaseInfo.u32LayerID, &u32Zorder);
    if (osal_copy_to_user(argp, &(u32Zorder), sizeof(hi_u32))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_ReleaseLogo(struct fb_info *info, hi_ulong arg)
{
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetCompressionMode(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetCompressionMode(struct fb_info *info, hi_ulong arg)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_FenceRefresh(struct fb_info *info, hi_ulong arg)
{
    hi_s32 ret;
    hi_u32 NowTimeMs = 0;
    hi_u32 EndTimeMs = 0;
#ifndef CONFIG_GFX_CANVASE_TO_LAYER_SUPPORT
    hi_u32 UnCmpStride = 0, CmpStride = 0;
#endif
    HIFB_PAR_S *pstPar = NULL;
    HIFB_HWC_LAYERINFO_S stLayerInfo;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    HI_GFX_TINIT();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    /* **************************beg count times ********************************* */
    HI_GFX_TSTART(NowTimeMs);
    pstPar->stFrameInfo.TwiceRefreshTimeMs = NowTimeMs - pstPar->stFrameInfo.PreRefreshTimeMs;
    pstPar->stFrameInfo.PreRefreshTimeMs = NowTimeMs;
    /* **************************end count times ********************************* */

    /* **************************beg discard times ******************************* */
    if ((0 != pstPar->DiscardFrameTimes) && (pstPar->DiscardFrameTimes < 10)) { /* * if suspend should not refresh black
                                                                                   ui, and can soon resume * */
        pstPar->DiscardFrameTimes--;
        return HI_SUCCESS;
    }
    /* **************************end discard times ******************************* */

    /* **************************beg check whether support *********************** */

    if (osal_copy_from_user(&stLayerInfo, argp, sizeof(HIFB_HWC_LAYERINFO_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return HI_FAILURE;
    }

    stLayerInfo.u32LayerAddr = drv_hifb_mem_get_smmu_from_fd(stLayerInfo.mem_handle);

#ifndef CONFIG_GFX_CANVASE_TO_LAYER_SUPPORT
    if ((stLayerInfo.u32LayerAddr != info->fix.smem_start) &&
        (stLayerInfo.u32LayerAddr !=
         info->fix.smem_start + info->fix.line_length * pstPar->stExtendInfo.DisplayHeight) &&
        (stLayerInfo.u32LayerAddr !=
         info->fix.smem_start + info->fix.line_length * pstPar->stExtendInfo.DisplayHeight * 2)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, stLayerInfo.u32LayerAddr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, info->fix.smem_start);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT,
                              info->fix.smem_start + info->fix.line_length * pstPar->stExtendInfo.DisplayHeight);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT,
                              info->fix.smem_start + info->fix.line_length * pstPar->stExtendInfo.DisplayHeight * 2);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "input disp ddr is not alloc by fb");
        return HI_FAILURE;
    }

    UnCmpStride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, 32, &CmpStride, /* 32 bits */
                                           CONFIG_HIFB_STRIDE_ALIGN);                         /* 32 bits */
#ifndef CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE
    if ((stLayerInfo.compress_mode == DRV_HIFB_CMP_MODE_HFBC) && (CmpStride != stLayerInfo.u32Stride))
#else
    if ((stLayerInfo.compress_mode == DRV_HIFB_CMP_MODE_HFBC) && (CmpStride != stLayerInfo.u32Stride))
#endif
    {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CmpStride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stLayerInfo.u32Stride);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "input stride is not frome fb, input error");
        return HI_FAILURE;
    }

    if ((stLayerInfo.compress_mode == DRV_HIFB_CMP_MODE_NONE) && (UnCmpStride != stLayerInfo.u32Stride)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, UnCmpStride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stLayerInfo.compress_mode);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stLayerInfo.u32Stride);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "input stride is not frome fb, input error");
        return HI_FAILURE;
    }

    if (DRV_HIFB_FMT_ARGB8888 != stLayerInfo.eFmt) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stLayerInfo.eFmt);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, DRV_HIFB_FMT_ARGB8888);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "input fmt is not frome fb, input error");
        return HI_FAILURE;
    }
#endif
    /* **************************end check whether support *********************** */

    /* ************************** beg check whether has repeat frame ************* */
    if (stLayerInfo.u32LayerAddr == pstPar->stRunInfo.PreRefreshAddr) {
        pstPar->stFrameInfo.RepeatFrameCnt++;
    }
    pstPar->stRunInfo.PreRefreshAddr = stLayerInfo.u32LayerAddr;
    /* ************************** end check whether has repeat frame ************* */

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    ret = DRV_HIFB_REFRESH_FenceFlip(pstPar, argp);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_REFRESH_FenceFlip, ret);
        return HI_FAILURE;
    }
#else
    ret = DRV_HIFB_REFRESH_NoFenceFlip(pstPar, argp);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_REFRESH_NoFenceFlip, ret);
        return HI_FAILURE;
    }
#endif

    pstPar->stFrameInfo.RefreshFrame++;
    pstPar->hifb_sync = HI_TRUE;

    /* **************************beg count times ********************************* */
    HI_GFX_TEND(EndTimeMs);
    pstPar->stFrameInfo.RunRefreshTimeMs = EndTimeMs - NowTimeMs;
    /* **************************end count times ********************************* */

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetSmemStartPhy(struct fb_info *info, hi_ulong arg)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_to_user(argp, &(pstPar->u32SmemStartPhy), sizeof(hi_u32))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_FixScreenInfo(struct fb_info *info, hi_ulong arg)
{
    struct fb_fix_screeninfo fix;
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (memcpy_s(&fix, sizeof(fix), &(info->fix), sizeof(fix)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
#ifdef CONFIG_GFX_MMU_SUPPORT
    fix.smem_start = pstPar->u32SmemStartPhy;
#endif
    if (osal_copy_to_user(argp, &fix, sizeof(fix))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_HwcGetLayerInfo(struct fb_info *info, hi_ulong arg)
{
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    HIFB_PAR_S *pstPar = NULL;
    hi_u32 Stride = 0;
    hi_u32 CmpStride = 0;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    HIFB_HWC_LAYERINFO_S stLayerInfo = {0};
    hi_u32 BitsPerPixel = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    if (osal_copy_from_user(&stLayerInfo, argp, sizeof(HIFB_HWC_LAYERINFO_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
        return -EFAULT;
    }

    if ((DRV_HIFB_FMT_ABGR8888 == stLayerInfo.eFmt) || (HIFB_FMT_ARGB8888 == stLayerInfo.eFmt) ||
        (HIFB_FMT_RGBA8888 == stLayerInfo.eFmt)) {
        BitsPerPixel = 32;
    } else if ((DRV_HIFB_FMT_RGB888 == stLayerInfo.eFmt) || (HIFB_FMT_BGR888 == stLayerInfo.eFmt)) {
        BitsPerPixel = 24;
    } else if ((DRV_HIFB_FMT_ABGR1555 == stLayerInfo.eFmt) || (HIFB_FMT_ARGB1555 == stLayerInfo.eFmt) ||
               (HIFB_FMT_RGBA5551 == stLayerInfo.eFmt)) {
        BitsPerPixel = 16;
    } else {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stLayerInfo.eFmt);
        return -EFAULT;
    }

    Stride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, BitsPerPixel, &CmpStride,
                                      CONFIG_HIFB_STRIDE_ALIGN);
    stLayerInfo.u32NoCmpStride = Stride;
    stLayerInfo.u32Stride = CmpStride;

    if (osal_copy_to_user(argp, &stLayerInfo, sizeof(stLayerInfo))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
#else
#endif

    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetCurDispBuffer(struct fb_info *info, hi_ulong arg)
{
    hi_u32 CurDispPhy = 0;
    HIFB_PAR_S *pstPar = NULL;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO_WITH_ARGP(info, argp);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerAddr(pstPar->stBaseInfo.u32LayerID, &CurDispPhy);
    if (osal_copy_to_user(argp, &CurDispPhy, sizeof(hi_u32))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return -EFAULT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

#ifdef CONFIG_HIFB_STEREO_SUPPORT
/***************************************************************************
* func          : DRV_HIFB_StereoCallBack
* description   : NA
                  CNcomment: 使用工作队列方式 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static hi_s32 DRV_HIFB_StereoCallBack(hi_void *pParaml, hi_void *pParamr)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 LayerId = 0, BufferSize = 0, CompressStride = 0, UnCompressStride = 0;
    HIFB_PAR_S *pstPar = NULL;
    hi_ulong StereoLockFlag = 0;
    struct fb_info *info = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pParamr, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pParaml, HI_FAILURE);

    LayerId = *((hi_u32 *)pParaml);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(LayerId, HI_FAILURE);
    info = s_stLayer[LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);

    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    pstPar->st3DInfo.StereoMode = *((HIFB_STEREO_MODE_E *)pParamr);

    if (HIFB_STEREO_MONO == pstPar->st3DInfo.StereoMode) {
        pstPar->st3DInfo.BegFreeStereoMem = HI_TRUE;
        DRV_HIFB_FreeStereoBuf(pstPar);
        pstPar->st3DInfo.BegFreeStereoMem = HI_FALSE;
        osal_wait_wakeup(&pstPar->st3DInfo.WaiteFinishFreeStereoMemMutex);
    } else {
        if (HIFB_REFRESH_MODE_WITH_PANDISPLAY == pstPar->stExtendInfo.enBufMode) {
            UnCompressStride = CONIFG_HIFB_GetMaxStride(info->var.xres, info->var.bits_per_pixel, &CompressStride,
                                                        CONFIG_HIFB_STRIDE_ALIGN);
            GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(UnCompressStride, 0, HI_FAILURE);

            /**<-- one buffer need size >**/
            BufferSize = HI_HIFB_GetMemSize(UnCompressStride, info->var.yres) / 2;
            GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(BufferSize, 0, HI_FAILURE);

            /**<-- generally need two buffer >**/
            GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(BufferSize, pstPar->stRunInfo.StereoBufNum, HI_FAILURE);
            BufferSize *= pstPar->stRunInfo.StereoBufNum;
            Ret = DRV_HIFB_ReAllocStereoBuf(pstPar->stBaseInfo.u32LayerID, UnCompressStride, BufferSize);
            if (HI_SUCCESS != Ret) {
                return HI_FAILURE;
            }

            pstPar->st3DInfo.st3DSurface.u32Pitch = UnCompressStride;
        }
        DRV_HIFB_Lock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
        pstPar->st3DInfo.IsStereo = HI_TRUE;
        DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
    }

    if ((pstPar->st3DInfo.mem_info_3d.smmu_addr == 0) &&
        (pstPar->st3DInfo.StereoMode == HIFB_STEREO_SIDEBYSIDE_HALF ||
         pstPar->st3DInfo.StereoMode == HIFB_STEREO_TOPANDBOTTOM) &&
        (HIFB_REFRESH_MODE_WITH_PANDISPLAY == pstPar->stExtendInfo.enBufMode)) {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTriDimMode(pstPar->stBaseInfo.u32LayerID,
                                                              pstPar->st3DInfo.StereoMode, HIFB_STEREO_MONO);
    } else {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTriDimMode(pstPar->stBaseInfo.u32LayerID,
                                                              pstPar->st3DInfo.StereoMode, pstPar->st3DInfo.StereoMode);
    }

    DRV_HIFB_AssignDispBuf(pstPar->stBaseInfo.u32LayerID);
    DRV_HIFB_ClearUnUsedStereoBuf(info);

    if (HIFB_REFRESH_MODE_WITH_PANDISPLAY == pstPar->stExtendInfo.enBufMode) {
        DRV_HIFB_REFRESH_PanDisplay(&info->var, info);
    } else {
        // DRV_HIFB_RefreshUserBuffer(pstPar->stBaseInfo.u32LayerID);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);

    return HI_SUCCESS;
}
#endif

#ifdef CONFIG_GFX_MMU_SUPPORT
extern phys_addr_t hisi_iommu_domain_iova_to_phys(unsigned long iova);
extern struct sg_table *get_meminfo(u32 addr, u32 iommu, u32 *size, u32 *base);

static inline int hifb_valid_mmap_phys_addr_range(unsigned long pfn, size_t size)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return (pfn + (size >> PAGE_SHIFT)) <= (1 + (PHYS_MASK >> PAGE_SHIFT));
}

/***************************************************************************
* func          : HI_GFX_MapToUser
* description   :
                  CNcomment: 将内核态的内存映射到用户态使用 CNend\n
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
****************************************************************************/
static inline hi_s32 hifb_smmu_map_to_user(hi_void *dma_buf, struct vm_area_struct *vma)
{
    hi_u32 u32Cnt = 0;
    hi_ulong len = 0;
    struct sg_table *table = NULL;
    struct scatterlist *sg = NULL;
    struct page *page = NULL;
    int ret;
    hi_ulong offset;
    unsigned long addr = 0;
    hi_ulong remainder;
    struct dma_buf *dmabuf = dma_buf;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (dmabuf == HI_NULL || vma == HI_NULL || dmabuf->priv == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dmabuf);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, vma);
        return HI_FAILURE;
    }

    table = ((struct ion_buffer *)(dmabuf->priv))->sg_table;
    if (table == NULL) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, get_meminfo, FAILURE_TAG);
        return HI_FAILURE;
    }

    addr = vma->vm_start;
    offset = vma->vm_pgoff * PAGE_SIZE;

    for_each_sg(table->sgl, sg, table->nents, u32Cnt)
    {
        page = sg_page(sg);
        remainder = vma->vm_end - addr;
        len = sg->length;

        if (offset >= sg->length) {
            offset -= sg->length;
            continue;
        } else if (offset) {
            page += offset / PAGE_SIZE;
            len = sg->length - offset;
            offset = 0;
        }
        len = osal_min(len, remainder);
        ret = remap_pfn_range(vma, addr, page_to_pfn(page), len, vma->vm_page_prot);
        if (ret)
            return ret;
        addr += len;
        if (addr >= vma->vm_end)
            return 0;
    }
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_MapMemKernalToUser(struct fb_info *info, struct vm_area_struct *vma)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 Size = 0;
    unsigned long iova = 0;
    unsigned long addr = 0;
    unsigned long phy_addr;
    HIFB_PAR_S *par = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(vma, HI_FAILURE);

    par = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    iova = (unsigned long)info->fix.smem_start;
    addr = vma->vm_start;

    if (0 == info->fix.smem_start) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->fix.smem_start);
        return HI_FAILURE;
    }

    Size = vma->vm_end - vma->vm_start;
    if (info->fix.smem_len < Size) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->fix.smem_len);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, Size);
        return HI_FAILURE;
    }

    vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

    if (info->fix.smem_len == Size) {
        if (!hifb_valid_mmap_phys_addr_range(vma->vm_pgoff, info->fix.smem_len)) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hifb_valid_mmap_phys_addr_range, FAILURE_TAG);
            return HI_FAILURE;
        }

        Ret = hifb_smmu_map_to_user(par->mem_info.dmabuf, vma);
        if (HI_SUCCESS != Ret) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, Size);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->fix.smem_len);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, info->fix.smem_start);
        }
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    }

    while (addr < vma->vm_end) {
        phy_addr = (unsigned long)hisi_iommu_domain_iova_to_phys(iova);
        if (!phy_addr) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hisi_iommu_domain_iova_to_phys, FAILURE_TAG);
            return HI_FAILURE;
        }

        if (!hifb_valid_mmap_phys_addr_range(vma->vm_pgoff, SZ_4K)) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hifb_valid_mmap_phys_addr_range, FAILURE_TAG);
            return HI_FAILURE;
        }

        if (remap_pfn_range(vma, addr, __phys_to_pfn(phy_addr), SZ_4K, vma->vm_page_prot)) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, remap_pfn_range, FAILURE_TAG);
            return HI_FAILURE;
        }
        addr = addr + SZ_4K;
        iova = iova + SZ_4K;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

static hi_s32 drv_hifb_init_par_lock(HIFB_PAR_S *par)
{
    if (osal_spin_lock_init(&par->stBaseInfo.lock) != 0) {
        par->stBaseInfo.lock.lock = HI_NULL;
        return HI_FAILURE;
    }
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (osal_spin_lock_init(&par->st3DInfo.StereoLock) != 0) {
        par->st3DInfo.StereoLock.lock = HI_NULL;
        return HI_FAILURE;
    }
    if (osal_wait_init(&par->st3DInfo.WaiteFinishUpStereoInfoMutex) != 0) {
        par->st3DInfo.WaiteFinishUpStereoInfoMutex.wait = HI_NULL;
        return HI_FAILURE;
    }
    if (osal_wait_init(&par->st3DInfo.WaiteFinishFreeStereoMemMutex) != 0) {
        par->st3DInfo.WaiteFinishFreeStereoMemMutex.wait = HI_NULL;
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

static hi_void drv_hifb_deinit_par_lock(HIFB_PAR_S *par)
{
    if (par->stBaseInfo.lock.lock != HI_NULL) {
        osal_spin_lock_destory(&par->stBaseInfo.lock);
    }
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (&par->st3DInfo.StereoLock.lock != HI_NULL) {
        osal_spin_lock_destory(&par->st3DInfo.StereoLock);
    }
    if (par->st3DInfo.WaiteFinishUpStereoInfoMutex.wait != HI_NULL) {
        osal_wait_destroy(&par->st3DInfo.WaiteFinishUpStereoInfoMutex);
    }
    if (par->st3DInfo.WaiteFinishFreeStereoMemMutex.wait != HI_NULL) {
        osal_wait_destroy(&par->st3DInfo.WaiteFinishFreeStereoMemMutex);
    }
#endif
}

static hi_s32 DRV_HIFB_Close(struct fb_info *info, hi_s32 user)
{
    HIFB_PAR_S *par = NULL;
    hi_u32 s32Cnt = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    s32Cnt = osal_atomic_read(&par->stBaseInfo.ref_count);
    if (s32Cnt <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_atomic_read, s32Cnt);
        return -EINVAL;
    }

    if (s32Cnt > 1) {
        osal_atomic_dec_return(&par->stBaseInfo.ref_count);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return HI_SUCCESS;
    }

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(par->stBaseInfo.u32LayerID, HI_FAILURE);

    g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_WaitAllDone(HI_TRUE);

    par->stExtendInfo.bShow = HI_FALSE;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetEnable(par->stBaseInfo.u32LayerID, HI_FALSE);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_UpLayerReg(par->stBaseInfo.u32LayerID);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetCallback(HIFB_CALLBACK_TYPE_VO, NULL, par->stBaseInfo.u32LayerID);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetCallback(HIFB_CALLBACK_TYPE_3DMode_CHG, NULL,
                                                        par->stBaseInfo.u32LayerID);

    DRV_HIFB_ReleaseScrollText(info);

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    DRV_HIFB_FreeStereoBuf(par);
    par->st3DInfo.StereoMode = HIFB_STEREO_MONO;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTriDimMode(par->stBaseInfo.u32LayerID, HIFB_STEREO_MONO,
                                                          HIFB_STEREO_MONO);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTriDimAddr(par->stBaseInfo.u32LayerID, 0);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(par->stBaseInfo.u32LayerID, 0);
#endif

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_CloseLayer(par->stBaseInfo.u32LayerID);

#ifdef CONFIG_GFX_PROC_SUPPORT
    DRV_HIFB_PROC_Destory(par->stBaseInfo.u32LayerID);
#endif

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    drv_hifb_fence_deinit(par);
#endif

    if ((NULL != info->screen_base) && (0 != info->fix.smem_len)) { /* * this should cost some times * */
        memset_s(info->screen_base, info->fix.smem_len, 0x0, info->fix.smem_len);
    }

    osal_atomic_dec_return(&par->stBaseInfo.ref_count);

#ifdef CONFIG_HIFB_LOGO_SUPPORT
    drv_hifb_logo_deinit(par->stBaseInfo.u32LayerID);
#endif

    drv_hifb_deinit_par_lock(par);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static inline hi_void DRV_HIFB_ReleaseScrollText(struct fb_info *info)
{
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    hi_u32 ScrollTextCnt = 0;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    if (NULL == info) {
        return;
    }
    pstPar = (HIFB_PAR_S *)info->par;
    if (NULL == pstPar) {
        return;
    }
    if (pstPar->stBaseInfo.u32LayerID >= HIFB_LAYER_ID_BUTT) {
        return;
    }

    if (HI_TRUE != s_stTextLayer[pstPar->stBaseInfo.u32LayerID].bAvailable) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return;
    }

    for (ScrollTextCnt = 0; ScrollTextCnt < SCROLLTEXT_NUM; ScrollTextCnt++) {
        if (HI_TRUE != s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[ScrollTextCnt].bAvailable) {
            continue;
        }
        DRV_HIFB_SCROLLTEXT_FreeCacheBuf(&(s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[ScrollTextCnt]));
        memset_s(&s_stTextLayer[pstPar->stBaseInfo.u32LayerID].stScrollText[ScrollTextCnt], sizeof(HIFB_SCROLLTEXT_S),
                 0, sizeof(HIFB_SCROLLTEXT_S));
    }

    s_stTextLayer[pstPar->stBaseInfo.u32LayerID].bAvailable = HI_FALSE;
    s_stTextLayer[pstPar->stBaseInfo.u32LayerID].u32textnum = 0;
    s_stTextLayer[pstPar->stBaseInfo.u32LayerID].u32ScrollTextId = 0;

    osal_workqueue_flush(&s_stTextLayer[pstPar->stBaseInfo.u32LayerID].blitScrollTextWork);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
#else
#endif

    return;
}

static hi_s32 DRV_HIFB_SetCmpReg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp,
                                 struct fb_info *info, hi_bool bUpdateReg)
{
    hi_u32 argb = 0x0;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO(info, HI_FAILURE);

    argb = ((transp & 0xff) << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);

    if (regno > 255) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, regno);
        return HI_FAILURE;
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetColorReg(pstPar->stBaseInfo.u32LayerID, regno, argb, bUpdateReg);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_SetCmapColor(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp,
                                    struct fb_info *info)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO(info, HI_FAILURE);
    if (regno >= 256) {
        return HI_FAILURE;
    }

    if ((1 == info->var.bits_per_pixel) || (2 == info->var.bits_per_pixel) || (4 == info->var.bits_per_pixel) ||
        (8 == info->var.bits_per_pixel)) {
        Ret = DRV_HIFB_SetCmpReg(regno, red, green, blue, transp, info, HI_TRUE);
        return Ret;
    }

    if ((16 == info->var.bits_per_pixel) && (8 == info->var.red.offset)) { /* * ACLUT88 * */
        Ret = DRV_HIFB_SetCmpReg(regno, red, green, blue, transp, info, HI_TRUE);
        return Ret;
    }

    switch (info->var.bits_per_pixel) {
        case 16:
            if (regno >= 16) {
                break;
            }
            if (info->var.red.offset == 10) {
                /* 1:5:5:5 */
                ((u32 *)(info->pseudo_palette))[regno] = ((red & 0xf800) >> 1) | ((green & 0xf800) >> 6) |
                                                         ((blue & 0xf800) >> 11);
            } else {
                /* 0:5:6:5 */
                ((u32 *)(info->pseudo_palette))[regno] = ((red & 0xf800)) | ((green & 0xfc00) >> 5) |
                                                         ((blue & 0xf800) >> 11);
            }
            break;
        case 24:
        case 32:
            red >>= 8;
            green >>= 8;
            blue >>= 8;
            transp >>= 8;
            ((u32 *)(info->pseudo_palette))[regno] = (red << info->var.red.offset) | (green << info->var.green.offset) |
                                                     (blue << info->var.blue.offset) |
                                                     (transp << info->var.transp.offset);
            break;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return Ret;
}

static hi_s32 DRV_HIFB_SetColorReg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp,
                                   struct fb_info *info)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return DRV_HIFB_SetCmapColor(regno, red, green, blue, transp, info);
}

/***************************************************************************************
 * func          : DRV_HIFB_SetCmap
 * description   : CNcomment: 设置调色板 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_SetCmap(struct fb_cmap *cmap, struct fb_info *info)
{
    hi_s32 Index = 0, Start = 0;
    unsigned short *Red = NULL, *Green = NULL, *Blue = NULL, *Transp = NULL;
    unsigned short hRed = 0, hGreen = 0, hBlue = 0, hTransp = 0xffff;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO(info, HI_FAILURE);
    if (pstPar->stBaseInfo.u32LayerID >= HIFB_LAYER_ID_BUTT) {
        return HI_FAILURE;
    }

    if (!gs_pstCapacity[pstPar->stBaseInfo.u32LayerID].bCmap) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "bcmap no support");
        return HI_FAILURE;
    }

    if (cmap->len > 256) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, cmap->len);
        return HI_FAILURE;
    }

    Red = cmap->red;
    Green = cmap->green;
    Blue = cmap->blue;
    Transp = cmap->transp;
    Start = cmap->start;

    if ((NULL == Red) || (NULL == Green) || (NULL == Blue)) {
        return HI_FAILURE;
    }

    for (Index = 0; Index < cmap->len; Index++) {
        hRed = *Red++;
        hGreen = *Green++;
        hBlue = *Blue++;
        hTransp = (Transp != NULL) ? (*Transp++) : (0xffff);
        DRV_HIFB_SetCmapColor(Start++, hRed, hGreen, hBlue, hTransp, info);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifdef CFG_HIFB_SUPPORT_CONSOLE
static hi_void DRV_HIFB_FillRect(struct fb_info *p, const struct fb_fillrect *rect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(p);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(rect);
    cfb_fillrect(p, rect);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
}

static hi_void DRV_HIFB_CopyArea(struct fb_info *p, const struct fb_copyarea *area)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(p);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(area);
    cfb_copyarea(p, area);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
}
static hi_void DRV_HIFB_ImageBlit(struct fb_info *p, const struct fb_image *image)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(p);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(image);
    cfb_imageblit(p, image);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
}
#endif

#ifdef CONFIG_DMA_SHARED_BUFFER
static struct dma_buf *DRV_HIFB_ExportDmaBuf(struct fb_info *info)
{
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_INFO(info, NULL);

    osal_mem_ref_get(pstPar->mem_info.dmabuf, ConvertID(HIGFX_FB_ID));

    return pstPar->mem_info.dmabuf;
}
#endif

static hi_s32 DRV_HIFB_Open(struct fb_info *info, hi_s32 user)
{
    hi_s32 Ret;
    hi_s32 HifbDevOpenCnt;
    hi_u32 layer_id;
    HIFB_PAR_S *pstPar = NULL;
    DRV_HIFB_DEFINE_PAR_FROM_INFO(info, HI_FAILURE);

    HifbDevOpenCnt = osal_atomic_read(&pstPar->stBaseInfo.ref_count);
    osal_atomic_inc_return(&pstPar->stBaseInfo.ref_count);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(pstPar->stBaseInfo.u32LayerID, HI_FAILURE);
    if (HI_TRUE != gs_pstCapacity[pstPar->stBaseInfo.u32LayerID].bLayerSupported) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stBaseInfo.u32LayerID);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this layer not support");
        goto ERROR_EXIT;
    }

    layer_id = pstPar->stBaseInfo.u32LayerID;

    if (0 != HifbDevOpenCnt) {
        return HI_SUCCESS;
    }

    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Open();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_DRV_TdeOpen, Ret);
        /* wait tde done */
        /* goto ERROR_EXIT; */
    }

#ifdef CONFIG_HIFB_LOGO_SUPPORT
    drv_hifb_logo_init(info->fix.smem_start, info->screen_base, info->fix.line_length, info->fix.smem_len, layer_id);
#endif

    Ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_OpenLayer(pstPar->stBaseInfo.u32LayerID);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_OpenLayer, Ret);
        goto ERROR_EXIT;
    }

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    Ret = drv_hifb_fence_init(pstPar);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_hifb_fence_init, Ret);
        goto ERROR_EXIT;
    }
#endif

    Ret = DRV_HIFB_InitLayerInfo(pstPar->stBaseInfo.u32LayerID);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_InitLayerInfo, Ret);
        goto ERROR_EXIT;
    }

#ifdef CONFIG_HIFB_LOGO_SUPPORT
    if ((pstPar->stBaseInfo.u32LayerID == HIFB_LAYER_HD_0) || (pstPar->stBaseInfo.u32LayerID == HIFB_LAYER_HD_3)) {
        drv_hifb_logo_get_base_info(layer_id);
    }
#endif

    Ret = DRV_HIFB_GetLayerBufferInfo(pstPar->stBaseInfo.u32LayerID);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GetLayerBufferInfo, Ret);
        goto ERROR_EXIT;
    }

    pstPar->up_mute = HI_TRUE;
    pstPar->is_first_pandisplay = HI_TRUE;
    pstPar->stRunInfo.LastScreenAddr = 0;
    pstPar->stRunInfo.CurScreenAddr = info->fix.smem_start;
    pstPar->st3DInfo.u32rightEyeAddr = pstPar->stRunInfo.CurScreenAddr;
    pstPar->stRunInfo.u32IndexForInt = 0;
    DRV_HIFB_SetDisplayBuffer(pstPar->stBaseInfo.u32LayerID);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerStride(pstPar->stBaseInfo.u32LayerID, info->fix.line_length);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(pstPar->stBaseInfo.u32LayerID, info->fix.smem_start);

    Ret = DRV_HIFB_RegisterCallBackFunction(pstPar->stBaseInfo.u32LayerID);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_RegisterCallBackFunction, Ret);
        goto ERROR_EXIT;
    }

#ifdef CONFIG_GFX_PROC_SUPPORT
    DRV_HIFB_PROC_Create(pstPar->stBaseInfo.u32LayerID);
#endif

    pstPar->stExtendInfo.bShow = HI_TRUE;
    pstPar->bVblank = HI_TRUE;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetEnable(pstPar->stBaseInfo.u32LayerID, HI_TRUE);

    return HI_SUCCESS;
ERROR_EXIT:

    DRV_HIFB_Close(info, user);

    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "hifb open failure");
    return HI_FAILURE;
}

static hi_s32 DRV_HIFB_InitLayerInfo(hi_u32 u32LayerID)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    DRV_HIFB_COLOR_FMT_E enColorFmt = DRV_HIFB_FMT_BUTT;
    HIFB_RECT stInRect = {0};
    hi_u32 u32Stride = 0;
    hi_u32 CmpStride = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_VALUE(u32LayerID);

    if (IS_HD_LAYER(u32LayerID)) {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_HD];
    } else if (IS_SD_LAYER(u32LayerID)) {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_SD];
    } else if (IS_MINOR_HD_LAYER(u32LayerID)) {
        info->var = s_stDefVar[HIFB_LAYER_TYPE_AD];
    } else {
        return HI_FAILURE;
    }

    memset_s(&(pstPar->stDispInfo.stUserBuffer), sizeof(pstPar->stDispInfo.stUserBuffer), 0,
             sizeof(pstPar->stDispInfo.stUserBuffer));
    memset_s(&(pstPar->stDispInfo.stCanvasSur), sizeof(pstPar->stDispInfo.stCanvasSur), 0,
             sizeof(pstPar->stDispInfo.stCanvasSur));
    memset_s(&(pstPar->stExtendInfo.stCkey), sizeof(pstPar->stExtendInfo.stCkey), 0,
             sizeof(pstPar->stExtendInfo.stCkey));
    memset_s(&(pstPar->stExtendInfo.stPos), sizeof(pstPar->stExtendInfo.stPos), 0, sizeof(pstPar->stExtendInfo.stPos));

    pstPar->stBaseInfo.bNeedAntiflicker = HI_FALSE;
    DRV_HIFB_SetAntiflickerLevel(pstPar->stBaseInfo.u32LayerID, HIFB_LAYER_ANTIFLICKER_AUTO);

    pstPar->stRunInfo.bModifying = HI_FALSE;
    pstPar->stRunInfo.u32ParamModifyMask = 0;
    pstPar->stExtendInfo.stAlpha.bAlphaEnable = HI_TRUE;
    pstPar->stExtendInfo.stAlpha.bAlphaChannel = HI_FALSE;
    pstPar->stExtendInfo.stAlpha.u8Alpha0 = 0x0;
    pstPar->stExtendInfo.stAlpha.u8Alpha1 = 0xff;
    pstPar->stExtendInfo.stAlpha.u8GlobalAlpha = 0xff;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAlpha(pstPar->stBaseInfo.u32LayerID, &pstPar->stExtendInfo.stAlpha);

    pstPar->stExtendInfo.stCkey.u8RedMask = 0xff;
    pstPar->stExtendInfo.stCkey.u8GreenMask = 0xff;
    pstPar->stExtendInfo.stCkey.u8BlueMask = 0xff;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayKeyMask(pstPar->stBaseInfo.u32LayerID, &pstPar->stExtendInfo.stCkey);

    enColorFmt = HIFB_GetFmtByArgb(&info->var.red, &info->var.green, &info->var.blue, &info->var.transp,
                                   info->var.bits_per_pixel);
    pstPar->stExtendInfo.enColFmt = enColorFmt;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerDataFmt(pstPar->stBaseInfo.u32LayerID,
                                                            pstPar->stExtendInfo.enColFmt);

    u32Stride = CONIFG_HIFB_GetMaxStride(info->var.xres_virtual, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
    info->fix.line_length = u32Stride;

    pstPar->stExtendInfo.DisplayWidth = info->var.xres;
    pstPar->stExtendInfo.DisplayHeight = info->var.yres;

    pstPar->st3DInfo.st3DSurface.u32Pitch = info->fix.line_length;
    pstPar->st3DInfo.st3DSurface.enFmt = pstPar->stExtendInfo.enColFmt;
    pstPar->st3DInfo.st3DSurface.u32Width = info->var.xres;
    pstPar->st3DInfo.st3DSurface.u32Height = info->var.yres;
    pstPar->st3DInfo.mem_info_3d.size = 0;
    pstPar->st3DInfo.mem_info_3d.smmu_addr = 0;

    stInRect.x = 0;
    stInRect.y = 0;
    stInRect.w = info->var.xres;
    stInRect.h = info->var.yres;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerRect(pstPar->stBaseInfo.u32LayerID, &stInRect);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerStride(pstPar->stBaseInfo.u32LayerID, info->fix.line_length);

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTriDimMode(pstPar->stBaseInfo.u32LayerID, HIFB_STEREO_MONO,
                                                          HIFB_STEREO_MONO);
#endif

    pstPar->stExtendInfo.enBufMode = HIFB_REFRESH_MODE_WITH_PANDISPLAY;
    pstPar->stRunInfo.u32BufNum = CONFIG_HIFB_LAYER_BUFFER_MAX_NUM;
    pstPar->stRunInfo.StereoBufNum = CONFIG_HIFB_STEREO_BUFFER_MAX_NUM;

    pstPar->bPanFlag = HI_FALSE;
    if (drv_hifb_init_par_lock(pstPar) != HI_SUCCESS) {
        drv_hifb_deinit_par_lock(pstPar);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GetLayerBufferInfo(hi_u32 u32LayerID)
{
#ifdef CONFIG_HIFB_REALLOC_MEM
    hi_s32 Ret = HI_SUCCESS;
#endif
    hi_u32 MemStride = 0, CmpStride = 0, LayerBufferNewSize = 0, layer_one_buffer_size = 0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_VALUE(u32LayerID);

    MemStride = CONIFG_HIFB_GetMaxStride(info->var.xres_virtual, info->var.bits_per_pixel, &CmpStride,
                                         CONFIG_HIFB_STRIDE_ALIGN);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(MemStride, 0, HI_FAILURE);

    LayerBufferNewSize = HI_HIFB_GetMemSize(MemStride, info->var.yres_virtual);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(LayerBufferNewSize, 0, HI_FAILURE);

    if (info->fix.smem_len < LayerBufferNewSize) {
#ifdef CONFIG_HIFB_REALLOC_MEM
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return HI_SUCCESS;
#else
        layer_one_buffer_size = HI_HIFB_GetMemSize(MemStride, info->var.yres);
        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(layer_one_buffer_size, 0, HI_FAILURE);

        if (info->fix.smem_len < layer_one_buffer_size) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32LayerID);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.xres_virtual);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.yres_virtual);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.bits_per_pixel);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->fix.smem_len);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerBufferNewSize);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, layer_one_buffer_size);
            return HI_FAILURE;
        }
        info->var.yres_virtual = info->var.yres;
        pstPar->stRunInfo.u32BufNum = info->fix.smem_len / layer_one_buffer_size;
        return HI_SUCCESS;
#endif
    }

#ifdef CONFIG_HIFB_REALLOC_MEM
    Ret = DRV_HIFB_ReAllocLayerBuffer(u32LayerID, LayerBufferNewSize);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ReAllocLayerBuffer, Ret);
        return HI_FAILURE;
    }
    info->fix.smem_len = LayerBufferNewSize;
#endif
    info->fix.line_length = MemStride;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void DRV_HIFB_SetDisplayBuffer(hi_u32 u32LayerId)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    hi_u32 OneBufSize = 0;
    hi_u32 BufferNum = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_NOVALUE(u32LayerId);

    OneBufSize = HI_HIFB_GetMemSize(info->fix.line_length, info->var.yres);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, OneBufSize);
    BufferNum = info->fix.smem_len / OneBufSize;

    if (CONFIG_HIFB_LAYER_0BUFFER == BufferNum) {
        return;
    } else if (CONFIG_HIFB_LAYER_1BUFFER == BufferNum) {
        pstPar->stDispInfo.u32DisplayAddr[0] = info->fix.smem_start;
        pstPar->stDispInfo.u32DisplayAddr[1] = info->fix.smem_start;
    } else {
        GRAPHIC_CHECK_U64_ADDITION_REVERSAL_UNRETURN(info->fix.smem_start, OneBufSize);
        GRAPHIC_CHECK_ULONG_TO_UINT_REVERSAL_UNRETURN((info->fix.smem_start + OneBufSize));
        pstPar->stDispInfo.u32DisplayAddr[0] = info->fix.smem_start;
        if (OneBufSize <= info->fix.smem_len / 2) {
            pstPar->stDispInfo.u32DisplayAddr[1] = info->fix.smem_start + OneBufSize;
        } else {
            pstPar->stDispInfo.u32DisplayAddr[1] = info->fix.smem_start;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return;
}

static inline hi_s32 DRV_HIFB_RegisterCallBackFunction(hi_u32 u32LayerId)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    Ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetCallback(HIFB_CALLBACK_TYPE_VO, (IntCallBack)DRV_HIFB_VoCallBack,
                                                              u32LayerId);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetCallback, Ret);
        return HI_FAILURE;
    }

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    Ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetCallback(HIFB_CALLBACK_TYPE_3DMode_CHG,
                                                              (IntCallBack)DRV_HIFB_StereoCallBack, u32LayerId);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetCallback, Ret);
        return HI_FAILURE;
    }
#endif

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    Ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetCallback(HIFB_CALLBACK_TYPE_FRAME_END,
                                                              (IntCallBack)drv_hifb_fence_frame_end_callback,
                                                              u32LayerId);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetCallback, Ret);
        return HI_FAILURE;
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void drv_hifb_get_layer(hi_u32 layer_id, HIFB_LAYER_S **layer)
{
    if ((layer != NULL) && (layer_id < HIFB_LAYER_ID_BUTT)) {
        *layer = &s_stLayer[layer_id];
    };
}

hi_u32 DRV_HIFB_GetPixDepth(hi_u32 BitsPerPixel)
{
    hi_u32 PerPixDepth = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (BitsPerPixel <= 32) {
        PerPixDepth = (BitsPerPixel >= 8) ? (BitsPerPixel >> 3) : (BitsPerPixel / 8);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return PerPixDepth;
}

hi_void DRV_HIFB_WaitVBlank(HIFB_LAYER_ID_E enLayerId)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_WaitVBlank(enLayerId);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 DRV_HIFB_UpStereoData(HIFB_LAYER_ID_E enLayerId, HIFB_BUFFER_S *pstSrcBuffer, HIFB_BLIT_OPT_S *pstBlitOpt)
{
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    hi_s32 ret;
    hi_ulong lock_flag = 0;
    HIFB_BUFFER_S stDstBuf;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);
    DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_VALUE(enLayerId);

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &lock_flag);
    pstPar->stRunInfo.bNeedFlip = HI_FALSE;
    pstPar->stRunInfo.s32RefreshHandle = 0;
    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &lock_flag);

    if (memcpy_s(&stDstBuf.stCanvas, sizeof(HIFB_SURFACE_S), &pstPar->st3DInfo.st3DSurface, sizeof(HIFB_SURFACE_S)) !=
        EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, stDstBuf.stCanvas.u32Width,
                                  stDstBuf.stCanvas.u32Height, 0, 0, 0);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_PARA_CHECK_SUPPORT, ret);
        return HI_FAILURE;
    }
    stDstBuf.stCanvas.u32Width = (HIFB_STEREO_SIDEBYSIDE_HALF == pstPar->st3DInfo.StereoMode)
                                        ? (stDstBuf.stCanvas.u32Width >> 1)
                                        : (stDstBuf.stCanvas.u32Width);
    stDstBuf.stCanvas.u32Height = (HIFB_STEREO_TOPANDBOTTOM == pstPar->st3DInfo.StereoMode)
                                        ? (stDstBuf.stCanvas.u32Height >> 1)
                                        : (stDstBuf.stCanvas.u32Height);

    stDstBuf.UpdateRect.x = 0;
    stDstBuf.UpdateRect.y = 0;
    stDstBuf.UpdateRect.w = stDstBuf.stCanvas.u32Width;
    stDstBuf.UpdateRect.h = stDstBuf.stCanvas.u32Height;

    pstPar->stFrameInfo.bBlitBlock = pstBlitOpt->bBlock;
    pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
    ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(pstSrcBuffer, &stDstBuf, pstBlitOpt, HI_TRUE);
    if (ret <= 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, ret);
        return HI_FAILURE;
    }

    pstPar->stRunInfo.s32RefreshHandle = ret;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
#else
#endif

    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_BlitFinishCallBack
 * description  : CNcomment: tde 任务完成 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_BlitFinishCallBack(hi_void *pParaml, hi_void *pParamr)
{
    hi_u32 LayerId = 0;
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *info = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_ENTER_FUNC);

    /* **************************beg check par ********************************* */
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pParaml, HI_FAILURE);
    LayerId = *(hi_u32 *)pParaml;
    DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_VALUE(LayerId);
    /* **************************end check par ********************************* */

    pstPar->up_mute = HI_FALSE;

    /* **************************finsh blit, can flip ************************** */
    pstPar->stRunInfo.bNeedFlip = HI_TRUE;
    /* **************************finsh blit, can flip ************************** */

    /* *******************beg check whether should switch buffer **************** */
    if (HI_FALSE == pstPar->st3DInfo.IsStereo) {
        pstPar->FenceRefreshCount++;
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        return HI_SUCCESS;
    }

#if defined(CONFIG_HIFB_FENCE_SUPPORT) && defined(CONFIG_HIFB_STEREO_SUPPORT)
    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;
#endif
    /* *******************end check whether should switch buffer *************** */

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void HIFB_DRV_GetSettingInfo(hi_u32 LayerId, drv_hifb_settings_info *settings_info)
{
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    DRV_HIFB_DEFINE_PAR_FROM_LAYER_RETURN_NOVALUE(LayerId);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(settings_info);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId);

    settings_info->fence_refresh_cnt = pstPar->FenceRefreshCount;
    settings_info->buffer_size = info->fix.line_length * pstPar->stExtendInfo.DisplayHeight;

    drv_hifb_fence_get_fence_value(&settings_info->fence_value, LayerId);
    drv_hifb_fence_get_timeline_value(&settings_info->timeline_value, LayerId);

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerWillWorkAddr(LayerId, &settings_info->will_work_addr);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerWorkAddr(LayerId, &settings_info->work_addr);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerStride(LayerId, &settings_info->layer_stride);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDeCmpSwitch(LayerId, &settings_info->decompress_state);
#else
#endif
    return;
}

static hi_void DRV_HIFB_ShowCmd(hi_void)
{
#ifdef CFG_HIFB_CUT
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_COLORKEY_HIFB                 >**/\n", FBIOGET_COLORKEY_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_COLORKEY_HIFB                 >**/\n", FBIOPUT_COLORKEY_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_ALPHA_HIFB                    >**/\n", FBIOGET_ALPHA_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_ALPHA_HIFB                    >**/\n", FBIOPUT_ALPHA_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_SCREEN_ORIGIN_HIFB            >**/\n", FBIOGET_SCREEN_ORIGIN_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_SCREEN_ORIGIN_HIFB            >**/\n", FBIOPUT_SCREEN_ORIGIN_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_DEFLICKER_HIFB                >**/\n", FBIOGET_DEFLICKER_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_DEFLICKER_HIFB                >**/\n", FBIOPUT_DEFLICKER_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_VBLANK_HIFB                   >**/\n", FBIOGET_VBLANK_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_SHOW_HIFB                     >**/\n", FBIOPUT_SHOW_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_SHOW_HIFB                     >**/\n", FBIOGET_SHOW_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_CAPABILITY_HIFB               >**/\n", FBIOGET_CAPABILITY_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_DECOMPRESS_HIFB               >**/\n", FBIOPUT_DECOMPRESS_HIFB);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_LAYER_INFO                    >**/\n", FBIOPUT_LAYER_INFO);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_LAYER_INFO                    >**/\n", FBIOGET_LAYER_INFO);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_CANVAS_BUFFER                 >**/\n", FBIOGET_CANVAS_BUFFER);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_REFRESH                          >**/\n", FBIO_REFRESH);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_WAITFOR_FREFRESH_DONE            >**/\n", FBIO_WAITFOR_FREFRESH_DONE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_ENCODER_PICTURE_FRAMING       >**/\n", FBIOPUT_ENCODER_PICTURE_FRAMING);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_ENCODER_PICTURE_FRAMING       >**/\n", FBIOGET_ENCODER_PICTURE_FRAMING);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_STEREO_MODE                   >**/\n", FBIOPUT_STEREO_MODE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_STEREO_MODE                   >**/\n", FBIOGET_STEREO_MODE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_SCREENSIZE                    >**/\n", FBIOPUT_SCREENSIZE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_SCREENSIZE                    >**/\n", FBIOGET_SCREENSIZE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_COMPRESSION                   >**/\n", FBIOPUT_COMPRESSION);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_COMPRESSION                   >**/\n", FBIOGET_COMPRESSION);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_SCROLLTEXT_CREATE                >**/\n", FBIO_SCROLLTEXT_CREATE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_SCROLLTEXT_FILL                  >**/\n", FBIO_SCROLLTEXT_FILL);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_SCROLLTEXT_PAUSE                 >**/\n", FBIO_SCROLLTEXT_PAUSE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_SCROLLTEXT_RESUME                >**/\n", FBIO_SCROLLTEXT_RESUME);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_SCROLLTEXT_DESTORY               >**/\n", FBIO_SCROLLTEXT_DESTORY);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_STEREO_DEPTH                  >**/\n", FBIOPUT_STEREO_DEPTH);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_STEREO_DEPTH                  >**/\n", FBIOGET_STEREO_DEPTH);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_ZORDER                        >**/\n", FBIOPUT_ZORDER);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_ZORDER                        >**/\n", FBIOGET_ZORDER);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_FREE_LOGO                        >**/\n", FBIO_FREE_LOGO);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOPUT_COMPRESSIONMODE               >**/\n", FBIOPUT_COMPRESSIONMODE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_COMPRESSIONMODE               >**/\n", FBIOGET_COMPRESSIONMODE);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_HWC_REFRESH                      >**/\n", FBIO_HWC_REFRESH);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIOGET_SMEMSTART_PHY                 >**/\n", FBIOGET_SMEMSTART_PHY);
    GRAPHIC_COMM_PRINT("0x%X   /**<-- FBIO_HWC_GETLAYERINFO                 >**/\n", FBIO_HWC_GETLAYERINFO);
#endif
    return;
}

static hi_s32 drv_hifb_init_osal(hi_void)
{
    hi_s32 ret;

    ret = drv_init_logo_wait_queue();
    if (ret != HI_SUCCESS) {
        return ret;
    }

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    ret = drv_init_scrolltext_lock();
    if (ret != HI_SUCCESS) {
        return ret;
    }
#endif
    return HI_SUCCESS;
}

static hi_void drv_hifb_deinit_osal(hi_void)
{
    drv_deinit_logo_wait_queue();
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    drv_deinit_scrolltext_lock();
#endif
}

/***************************************************************************************
 * func        : HIFB_DRV_ModInit
 * description : CNcomment: insmod ko initialCNend\n
 * param[in]   : hi_s32
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_s32 HIFB_DRV_ModInit(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 u32LayerId = HIFB_LAYER_HD_0;
    hi_u32 buf_size;

    memset_s(&s_stLayer, sizeof(s_stLayer), 0x0, sizeof(s_stLayer));

    ret = drv_hifb_init_osal();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_hifb_init_osal, ret);
        return ret;
    }

    DRV_HIFB_ADP_GetCallBackFunction(&g_stDrvAdpCallBackFunction);
    DRV_HIFB_GFX2D_GetCallBackFunction(&g_stGfx2dCallBackFunction);

#ifndef HI_MCE_SUPPORT
    ret = DRV_HIFB_ModInit_K();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ModInit_K, ret);
        drv_hifb_deinit_osal();
        return ret;
    }
#endif

    ret = g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_Init();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_Init, ret);
        goto ERR_EXIT;
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDevCap(&gs_pstCapacity);
    if (NULL == gs_pstCapacity) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_GetDevCap, ret);
        goto ERR_EXIT;
    }

    if (!osal_strncmp("on", 2, tc_wbc, 2)) {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetTCFlag(HI_TRUE);
    }

    ret = DRV_HIFB_ParseInsmodParameter();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ParseInsmodParameter, ret);
        goto ERR_EXIT;
    }

    if ((0 == s_stLayer[u32LayerId].u32LayerSize) ||
        (s_stLayer[u32LayerId].u32LayerSize > CONFIG_HIFB_DISP_BUFF_MAX_SIZE)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, s_stLayer[u32LayerId].u32LayerSize);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, CONFIG_HIFB_DISP_BUFF_MAX_SIZE);
        goto ERR_EXIT;
    }

    for (u32LayerId = HIFB_LAYER_HD_0; u32LayerId < HIFB_LAYER_ID_BUTT; u32LayerId++) {
        if (s_stLayer[u32LayerId].u32LayerSize == 0) {
            continue;
        }

        buf_size = hifb_adp_get_layer_max_size(u32LayerId);
        if (buf_size == 0) {
            continue;
        }

        if ((s_stLayer[u32LayerId].u32LayerSize < buf_size) &&
            ((u32LayerId == HIFB_LAYER_HD_0) || (u32LayerId == HIFB_LAYER_HD_3))) {
            s_stLayer[u32LayerId].u32LayerSize = buf_size;
        }

        ret = DRV_HIFB_RegisterFrameBuffer(u32LayerId);
        if (ret != HI_SUCCESS) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_RegisterFrameBuffer, ret);
            goto ERR_EXIT;
        }

        ret = DRV_HIFB_AllocLayerBuffer(u32LayerId, s_stLayer[u32LayerId].u32LayerSize);
        if (ret != HI_SUCCESS) {
            goto ERR_EXIT;
        }

        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32LayerId);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, s_stLayer[u32LayerId].u32LayerSize);

        ret = DRV_HIFB_InitAllocCmapBuffer(u32LayerId);
        if (ret != HI_SUCCESS) {
            continue;
        }

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
        memset_s(&s_stTextLayer[u32LayerId], sizeof(s_stTextLayer[u32LayerId]), 0, sizeof(s_stTextLayer[u32LayerId]));
#endif
    }

    hifb_adp_set_vdp_layer_enable(0, 0);

    DRV_HIFB_PrintVersion(HI_TRUE);

    DRV_HIFB_ShowCmd();

    return HI_SUCCESS;
ERR_EXIT:

    HIFB_DRV_ModExit();

    return HI_FAILURE;
}

/***************************************************************************************
 * func        : HIFB_DRV_ModExit
 * description : CNcomment: rmmod ko dinitialCNend\n
 * param[in]   : hi_s32
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void HIFB_DRV_ModExit(hi_void)
{
    hi_u32 u32LayerId = HIFB_LAYER_HD_0;
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
    hi_u32 ScrollTextNum = 0;
#endif

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_DeInit();
    drv_hifb_deinit_osal();
    for (u32LayerId = HIFB_LAYER_HD_0; u32LayerId <= HIFB_LAYER_SD_1; u32LayerId++) {
        DRV_HIFB_DInitAllocCmapBuffer(u32LayerId);
        DRV_HIFB_FreeLayerBuffer(u32LayerId);
        DRV_HIFB_UnRegisterFrameBuffer(u32LayerId);
#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
        if (s_stTextLayer[u32LayerId].bAvailable) {
            continue;
        }

        for (ScrollTextNum = 0; ScrollTextNum < SCROLLTEXT_NUM; ScrollTextNum++) {
            if (s_stTextLayer[u32LayerId].stScrollText[ScrollTextNum].bAvailable) {
                DRV_HIFB_SCROLLTEXT_FreeCacheBuf(&(s_stTextLayer[u32LayerId].stScrollText[ScrollTextNum]));
                memset_s(&s_stTextLayer[u32LayerId].stScrollText[ScrollTextNum], sizeof(HIFB_SCROLLTEXT_S), 0,
                         sizeof(HIFB_SCROLLTEXT_S));
            }
        }
        s_stTextLayer[u32LayerId].bAvailable = HI_FALSE;
        s_stTextLayer[u32LayerId].u32textnum = 0;
        s_stTextLayer[u32LayerId].u32ScrollTextId = 0;
        osal_workqueue_flush(&s_stTextLayer[u32LayerId].blitScrollTextWork);
#endif
    }

    g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Close();

#ifndef HI_MCE_SUPPORT
    DRV_HIFB_ModExit_K();
#endif

    DRV_HIFB_PrintVersion(HI_FALSE);

    return;
}

static hi_s32 DRV_HIFB_ParseInsmodParameter(hi_void)
{
    hi_char *pInputStr = NULL;
    hi_char number[4] = {0};
    hi_u32 LayerCnt = 0;
    hi_u32 u32LayerId = 0;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(gs_pstCapacity, HI_FAILURE);

    if (video == HI_NULL) {
        for (u32LayerId = HIFB_LAYER_HD_0;
             (u32LayerId <= HIFB_LAYER_SD_1) && (gs_pstCapacity[u32LayerId].bLayerSupported == HI_TRUE); u32LayerId++) {
            s_stLayer[u32LayerId].u32LayerSize = (gs_u32MenuconfigLayerSize[u32LayerId] == 0)
                                                        ? 4096
                                                        : HI_HIFB_GetMemSize(gs_u32MenuconfigLayerSize[u32LayerId] * 1024,
                                                                          1); /* 4096: 4K, 1024: 1K */
        }
        return HI_SUCCESS;
    }

    pInputStr = strstr(video, "vram");
    while (NULL != pInputStr) {
        LayerCnt = 0;
        pInputStr += 4;
        while ('_' != *pInputStr) {
            if (LayerCnt > 1) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerCnt);
                return HI_FAILURE;
            }
            number[LayerCnt] = *pInputStr;
            LayerCnt++;
            pInputStr++;
        }
        number[LayerCnt] = '\0';

        u32LayerId = osal_strtoul(number, (char **)NULL, OSAL_BASE_DEC);
        GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);

        if (HI_FALSE == gs_pstCapacity[u32LayerId].bLayerSupported) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32LayerId);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this layer is not support");
            return HI_FAILURE;
        }

        pInputStr += sizeof("size") + LayerCnt;
        s_stLayer[u32LayerId].u32LayerSize = DRV_HIFB_GetVramSize(pInputStr);

        pInputStr = strstr(pInputStr, "vram");
    }

    return HI_SUCCESS;
}

static inline hi_ulong DRV_HIFB_GetVramSize(hi_char *pstr)
{
    hi_bool str_is_valid = HI_TRUE;
    unsigned long vram_size = 0;
    hi_char *ptr = pstr;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((ptr == NULL) || (*ptr == '\0')) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }

    while (*ptr != '\0') {
        if (*ptr == ',') {
            break;
        } else if ((!isdigit(*ptr)) && ('X' != *ptr) && ('x' != *ptr) &&
                   ((*ptr > 'f' && *ptr <= 'z') || (*ptr > 'F' && *ptr <= 'Z'))) {
            str_is_valid = HI_FALSE;
            break;
        }
        ptr++;
    }

    if (HI_TRUE == str_is_valid) {
        vram_size = osal_strtoul(pstr, (char **)NULL, 0);
        vram_size = ((vram_size * 1024 + PAGE_SIZE - 1) & PAGE_MASK) / 1024;
    }

    if (0 != vram_size) {
        vram_size = HI_HIFB_GetMemSize(vram_size * 1024, 1);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return vram_size;
}

static inline hi_s32 DRV_HIFB_RegisterFrameBuffer(hi_u32 u32LayerId)
{
    hi_s32 Ret = 0;
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *FbInfo = NULL;

    FbInfo = framebuffer_alloc(((sizeof(HIFB_PAR_S)) + (sizeof(hi_u32) * 256)), NULL);
    if (NULL == FbInfo) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, framebuffer_alloc, FAILURE_TAG);
        return -ENOMEM;
    }

    pstPar = (HIFB_PAR_S *)(FbInfo->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);
    s_stLayer[u32LayerId].pstInfo = FbInfo;

    FbInfo->fix = gs_stDefFix;
    FbInfo->fbops = &s_sthifbops;
    FbInfo->flags = FBINFO_FLAG_DEFAULT | FBINFO_HWACCEL_YPAN | FBINFO_HWACCEL_XPAN;
    FbInfo->pseudo_palette = (hi_u8 *)(FbInfo->par) + sizeof(HIFB_PAR_S);
    FbInfo->fix.smem_len = s_stLayer[u32LayerId].u32LayerSize;

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    if (osal_wait_init(&pstPar->WaiteEndFenceRefresh)) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, register_framebuffer, Ret);
        framebuffer_release(FbInfo);
        s_stLayer[u32LayerId].pstInfo = NULL;
        pstPar->WaiteEndFenceRefresh.wait = HI_NULL;
        return HI_FAILURE;
    }
#endif

    Ret = osal_atomic_init(&pstPar->stBaseInfo.ref_count);
    if (Ret != 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, register_framebuffer, Ret);
        framebuffer_release(FbInfo);
        s_stLayer[u32LayerId].pstInfo = NULL;
#ifdef CONFIG_HIFB_FENCE_SUPPORT
        osal_wait_destroy(&pstPar->WaiteEndFenceRefresh);
#endif
        return HI_FAILURE;
    }

    Ret = register_framebuffer(FbInfo);
    if (Ret < 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, register_framebuffer, Ret);
        framebuffer_release(FbInfo);
        s_stLayer[u32LayerId].pstInfo = NULL;
        osal_atomic_destory(&pstPar->stBaseInfo.ref_count);
#ifdef CONFIG_HIFB_FENCE_SUPPORT
        osal_wait_destroy(&pstPar->WaiteEndFenceRefresh);
#endif
        return HI_FAILURE;
    }

    pstPar->bFrameBufferRegister = HI_TRUE;

    return HI_SUCCESS;
}

static inline hi_void DRV_HIFB_UnRegisterFrameBuffer(hi_u32 u32LayerId)
{
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *FbInfo = NULL;
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(u32LayerId);

    FbInfo = s_stLayer[u32LayerId].pstInfo;
    if (NULL == FbInfo) {
        return;
    }

    pstPar = (HIFB_PAR_S *)FbInfo->par;
    if (NULL == pstPar) {
        return;
    }

    if (HI_TRUE == pstPar->bFrameBufferRegister) {
        unregister_framebuffer(FbInfo);
    }

    framebuffer_release(FbInfo);

    s_stLayer[u32LayerId].pstInfo = NULL;
    osal_atomic_destory(&pstPar->stBaseInfo.ref_count);
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    if (pstPar->WaiteEndFenceRefresh.wait != HI_NULL) {
        osal_wait_destroy(&pstPar->WaiteEndFenceRefresh);
    }
#endif
    return;
}

#ifdef CONFIG_HIFB_REALLOC_MEM
static inline hi_s32 DRV_HIFB_ReAllocLayerBuffer(hi_u32 u32LayerId, hi_u32 BufSize)
{
    hi_s32 ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    DRV_HIFB_FreeLayerBuffer(u32LayerId);
    ret = DRV_HIFB_AllocLayerBuffer(u32LayerId, BufSize);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return ret;
}
#endif

static inline hi_s32 DRV_HIFB_AllocLayerBuffer(hi_u32 u32LayerId, hi_u32 BufSize)
{
    hi_char LayerBufferName[256] = {'\0'};
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *FbInfo = NULL;
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_bool bSmmu = HI_FALSE;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);
    FbInfo = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(FbInfo, HI_FAILURE);

    pstPar = (HIFB_PAR_S *)FbInfo->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    pstPar->u32SmemStartPhy = 0;
    pstPar->stBaseInfo.u32LayerID = u32LayerId;
    pstPar->stDispInfo.stCanvasSur.u32PhyAddr = 0;

    snprintf(LayerBufferName, sizeof(LayerBufferName), "HIFB_FB%d_DisBuf", u32LayerId);
    LayerBufferName[sizeof(LayerBufferName) - 1] = '\0';

#ifdef CONFIG_GFX_MMU_SUPPORT
    bSmmu = DRV_HIFB_CheckSmmuSupport(u32LayerId);
    if (HI_FALSE == bSmmu) {
        pstPar->mem_info.dmabuf = osal_mem_alloc(LayerBufferName, BufSize, OSAL_MMZ_TYPE, HI_NULL, 0);
        osal_mem_flush(pstPar->mem_info.dmabuf);
        if (pstPar->mem_info.dmabuf == HI_NULL) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, BufSize);
            return HI_FAILURE;
        }
        pstPar->mem_info.mmz_addr = drv_hifb_mem_get_phy_addr(pstPar->mem_info.dmabuf);
        if (pstPar->mem_info.mmz_addr == 0) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, BufSize);
            goto ERR_EXIT;
        }
        pstPar->u32SmemStartPhy = pstPar->mem_info.mmz_addr;
        pstPar->mem_info.smmu_addr = drv_hifb_mem_map_to_smmu(pstPar->mem_info.dmabuf);
        FbInfo->fix.smem_start = pstPar->mem_info.smmu_addr;
    } else
#endif
    {
        pstPar->mem_info.dmabuf = osal_mem_alloc(LayerBufferName, BufSize, OSAL_NSSMMU_TYPE, HI_NULL, 0);
        osal_mem_flush(pstPar->mem_info.dmabuf);
        if (pstPar->mem_info.dmabuf == HI_NULL) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, BufSize);
            return HI_FAILURE;
        }
        pstPar->mem_info.smmu_addr = drv_hifb_mem_map_to_smmu(pstPar->mem_info.dmabuf);
        if (pstPar->mem_info.smmu_addr == 0) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, BufSize);
            goto ERR_EXIT;
        }
        pstPar->u32SmemStartPhy = pstPar->mem_info.smmu_addr;
        FbInfo->fix.smem_start = pstPar->mem_info.smmu_addr;
    }

    if (FbInfo->fix.smem_start == 0) {
        goto ERR_EXIT;
    }

    pstPar->mem_info.virtual_addr = drv_hifb_mem_map(pstPar->mem_info.dmabuf);
    FbInfo->screen_base = pstPar->mem_info.virtual_addr;
    if (FbInfo->screen_base == HI_NULL) {
        goto ERR_EXIT;
    }

    memset_s(FbInfo->screen_base, BufSize, 0x0, BufSize);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, BufSize);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, FbInfo->fix.smem_len);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, FbInfo->fix.smem_start);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;

ERR_EXIT:

    if (pstPar->mem_info.smmu_addr != 0) {
        drv_hifb_mem_unmap_from_smmu(pstPar->mem_info.dmabuf, pstPar->mem_info.smmu_addr);
        pstPar->mem_info.smmu_addr = 0;
    }

    if (pstPar->mem_info.dmabuf != HI_NULL) {
        drv_hifb_mem_free(pstPar->mem_info.dmabuf);
        pstPar->mem_info.dmabuf = HI_NULL;
    }

    return HI_FAILURE;
}

static inline hi_void DRV_HIFB_FreeLayerBuffer(hi_u32 u32LayerId)
{
    struct fb_info *FbInfo = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(u32LayerId);
    FbInfo = s_stLayer[u32LayerId].pstInfo;
    if (NULL == FbInfo) {
        return;
    }

    pstPar = (HIFB_PAR_S *)FbInfo->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    if (pstPar->mem_info.virtual_addr != HI_NULL) {
        drv_hifb_mem_unmap(pstPar->mem_info.dmabuf, pstPar->mem_info.virtual_addr);
        pstPar->mem_info.virtual_addr = HI_NULL;
        FbInfo->screen_base = HI_NULL;
    }

    if (FbInfo->fix.smem_start == 0) {
        return;
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    if (pstPar->mem_info.smmu_addr) {
        drv_hifb_mem_unmap_from_smmu(pstPar->mem_info.dmabuf, pstPar->mem_info.smmu_addr);
        drv_hifb_mem_free(pstPar->mem_info.dmabuf);
    } else {
        drv_hifb_mem_free(pstPar->mem_info.dmabuf);
    }
#else
    drv_hifb_mem_free(pstPar->mem_info.dmabuf);
#endif

    pstPar->mem_info.dmabuf = HI_NULL;
    pstPar->mem_info.mmz_addr = 0;
    pstPar->mem_info.smmu_addr = 0;
    pstPar->mem_info.virtual_addr = 0;
    pstPar->mem_info.size = 0;

    pstPar->u32SmemStartPhy = 0;
    FbInfo->fix.smem_start = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_s32 DRV_HIFB_InitAllocCmapBuffer(hi_u32 u32LayerId)
{
    hi_u32 CmapLen = 256;
    struct fb_info *FbInfo = NULL;

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);
    FbInfo = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(FbInfo, HI_FAILURE);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);
    if (HI_TRUE != gs_pstCapacity[u32LayerId].bCmap) {
        return HI_SUCCESS;
    }

    if (fb_alloc_cmap(&FbInfo->cmap, CmapLen, 1) < 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, fb_alloc_cmap, FAILURE_TAG);
        FbInfo->cmap.len = 0;
        return HI_FAILURE;
    }

    FbInfo->cmap.len = CmapLen;

    return HI_SUCCESS;
}

static inline hi_void DRV_HIFB_DInitAllocCmapBuffer(hi_u32 u32LayerId)
{
    struct fb_cmap *cmap = NULL;
    struct fb_info *FbInfo = NULL;
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(u32LayerId);

    FbInfo = s_stLayer[u32LayerId].pstInfo;
    if (NULL == FbInfo) {
        return;
    }

    cmap = &FbInfo->cmap;
    if (0 != cmap->len) {
        fb_dealloc_cmap(cmap);
    }

    return;
}

static inline hi_bool DRV_HIFB_CheckSmmuSupport(hi_u32 LayerId)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    switch (LayerId) {
        case HIFB_LAYER_HD_0:
#ifdef CONFIG_GFX_HI_FB0_SMMU_SUPPORT
            return HI_TRUE;
#else
            return HI_FALSE;
#endif
        case HIFB_LAYER_HD_1:
#ifdef CONFIG_GFX_HI_FB1_SMMU_SUPPORT
            return HI_TRUE;
#else
            return HI_FALSE;
#endif
        case HIFB_LAYER_HD_2:
#ifdef CONFIG_GFX_HI_FB2_SMMU_SUPPORT
            return HI_TRUE;
#else
            return HI_FALSE;
#endif
        case HIFB_LAYER_HD_3:
#ifdef CONFIG_GFX_HI_FB3_SMMU_SUPPORT
            return HI_TRUE;
#else
            return HI_FALSE;
#endif
        default:
            return HI_FALSE;
    }
#else
    return HI_FALSE;
#endif
}

static inline hi_void DRV_HIFB_PrintVersion(hi_bool bLoad)
{
#if !defined(CONFIG_GFX_COMM_VERSION_DISABLE) && !defined(CONFIG_GFX_COMM_DEBUG_DISABLE) && defined(MODULE)
    if (HI_TRUE == bLoad) {
        GRAPHIC_COMM_PRINT("Load hi_fb.ko success.\t\t(%s)\n", VERSION_STRING);
    } else {
        GRAPHIC_COMM_PRINT("UnLoad hi_fb.ko success.\t(%s)\n", VERSION_STRING);
    }
#endif
    return;
}

hi_void DRV_HIFB_SetDecmpLayerInfo(hi_u32 LayerId)
{
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    hi_bool bDispEnable = HI_FALSE;
#endif
    hi_u32 IsFenceRefresh = 0x0;
    hi_ulong LockParFlag = 0;
    hi_u32 CmpStride = 0;
    hi_u32 UnCmpStride = 0;
    HIFB_PAR_S *pstPar = NULL;
    hi_s32 Ret = HI_SUCCESS;
#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT) && !defined(CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE)
    hi_u32 head_size = 0;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId);
    if ((NULL == s_stLayer[LayerId].pstInfo) || (NULL == s_stLayer[LayerId].pstInfo->par)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "pstInfo or par is null");
        return;
    }
    pstPar = (HIFB_PAR_S *)s_stLayer[LayerId].pstInfo->par;

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &LockParFlag);

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetHaltDispStatus(pstPar->stBaseInfo.u32LayerID, &bDispEnable);
    while (pstPar->FenceRefreshCount > 0) {
        drv_hifb_fence_inc_refresh_time(bDispEnable, LayerId);
        pstPar->FenceRefreshCount--;
    }
#endif

    IsFenceRefresh = (pstPar->stRunInfo.u32ParamModifyMask & HIFB_LAYER_PARAMODIFY_FENCE);
    if (!IsFenceRefresh) {
        pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_FENCE;
        DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);
        return;
    }

    if (HI_TRUE == pstPar->st3DInfo.IsStereo) {
        pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_FENCE;
        DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);
        return;
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, pstPar->stExtendInfo.DisplayWidth,
                                  pstPar->stExtendInfo.DisplayHeight, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayHeight);
        DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);
        return;
    }

    if (pstPar->stExtendInfo.enColFmt == DRV_HIFB_FMT_ARGB10101010) {
        UnCmpStride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, 40, &CmpStride, /* 40 bits */
                                               CONFIG_HIFB_STRIDE_ALIGN);
    } else {
        UnCmpStride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, 32, &CmpStride, /* 32 bits */
                                               CONFIG_HIFB_STRIDE_ALIGN);
    }

#ifdef CONFIG_GFX_CANVASE_TO_LAYER_SUPPORT
    UnCmpStride = pstPar->stExtendInfo.DisplayStride;
#endif

#ifdef CFG_HI_FB_DECOMPRESS_SUPPORT
#ifndef CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE
    if (pstPar->compress_mode == DRV_HIFB_CMP_MODE_HFBC) {
        hifb_decmp_info dec_info = {0};
        dec_info.fmt = pstPar->stExtendInfo.enColFmt;
        dec_info.stride = CmpStride;
        DRV_HIFB_MEM_GetCmpHeadInfo(pstPar->stExtendInfo.DisplayHeight, &head_size, NULL);
        dec_info.ar_header = pstPar->stRunInfo.CurScreenAddr;

        if (((dec_info.ar_header) == 0) || ((dec_info.ar_header) > (UINT_MAX - (head_size)))) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dec_info.ar_header);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, head_size);
            DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);
            return;
        }
        dec_info.ar_data = dec_info.ar_header + head_size;

        if (((dec_info.ar_data) == 0) ||
            ((dec_info.ar_data) > (UINT_MAX - (CmpStride * pstPar->stExtendInfo.DisplayHeight)))) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dec_info.ar_data);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CmpStride * pstPar->stExtendInfo.DisplayHeight);
            DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);
            return;
        }
        dec_info.gb_header = dec_info.ar_data + CmpStride * pstPar->stExtendInfo.DisplayHeight;

        if (((dec_info.gb_header) == 0) || ((dec_info.gb_header) > (UINT_MAX - (head_size)))) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dec_info.gb_header);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, head_size);
            DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);
            return;
        }
        dec_info.gb_data = dec_info.gb_header + head_size;

        g_stDrvAdpCallBackFunction.drv_hifb_adp_set_decmp_info(LayerId, &dec_info);
    } else
#endif
    {
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerStride(LayerId, UnCmpStride);
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(LayerId, pstPar->stRunInfo.CurScreenAddr);
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetDeCmpSwitch(LayerId, pstPar->compress_mode == DRV_HIFB_CMP_MODE_HFBC);
#else
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerStride(LayerId, UnCmpStride);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerAddr(LayerId, pstPar->stRunInfo.CurScreenAddr);
#endif

    pstPar->stRunInfo.u32ParamModifyMask &= ~HIFB_LAYER_PARAMODIFY_FENCE;

    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void DRV_HIFB_Lock(osal_spinlock *pLock, hi_ulong *pLockParFlag)
{
    osal_spin_lock_irqsave(pLock, pLockParFlag);
    return;
}

hi_void DRV_HIFB_UnLock(osal_spinlock *pLock, hi_ulong *pLockParFlag)
{
    osal_spin_unlock_irqrestore(pLock, pLockParFlag);
    return;
}

#ifdef MODULE
module_init(HIFB_DRV_ModInit);
module_exit(HIFB_DRV_ModExit);
MODULE_LICENSE("GPL");
#endif
