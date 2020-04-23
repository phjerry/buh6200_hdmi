/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb adp layer
 * Author: sdk
 * Create: 2016-01-01
 */

/* **************************** add include here*************************************************** */
#ifdef HI_BUILD_IN_BOOT
#include "hifb_debug.h"
#include "hi_drv_pq.h"
#include "hi_gfx_mem.h"
#endif

#include "drv_disp_ext.h"
#include "drv_hifb_hal.h"
#include "drv_hifb_adp.h"
#include "hi_drv_disp.h"
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
#include "drv_hifb_wbc.h"
#endif

#ifndef HI_BUILD_IN_BOOT
// #include "drv_pq_ext.h"
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
#include "drv_hifb_proc.h"
#endif

#include "drv_hifb_config.h"
#include "drv_hifb_common.h"

#ifdef CONFIG_HIFB_VERSION_1_0
#include "optm_alg_csc.h"
#include "optm_alg_gzme.h"
#include "optm_alg_gsharp.h"
#endif

#ifdef HI_BUILD_IN_BOOT
#include "hi_gfx_debug.h"
#include "hi_gfx_mem.h"
#else
#include "drv_hifb_debug.h"
#include "drv_hifb_ut.h"
#endif

#ifdef CONFIG_HIFB_DECOMPRESS_SEGMENT
#include "hifb_alg_decmp.h"
#endif

/* **************************** Macro Definition ************************************************** */
#ifndef HI_BUILD_IN_BOOT
static disp_export_func *ps_DispExportFuncs = NULL;
#endif

#define GRAPHIC_LAYER_REALLY_4K_WIDTH_DISPLAY 4096
#define GRAPHIC_LAYER_4K_WIDTH_DISPLAY 3840
#define GRAPHIC_LAYER_4K_HEIGHT_DISPLAY 2160
#define OPTM_EXTRACTLINE_RATIO 4

#define GRAPHIC_DISPLAY_MAX_WIDTH 7680
#define GRAPHIC_DISPLAY_MAX_HIGHT 4320

#ifdef CONFIG_HIFB_VERSION_1_0
#define OPTM_CURSOR_LAYERID HIFB_LAYER_SD_1
#else
#define OPTM_CURSOR_LAYERID HIFB_LAYER_HD_2
#endif

#define OPTM_GP_SUPPORT_MAXLAYERNUM 5
#define OPTM_GP0_GFX_COUNT CONFIG_HIFB_GP0_SUPPORT_GFX_COUNT
#define OPTM_GP1_GFX_COUNT CONFIG_HIFB_GP1_SUPPORT_GFX_COUNT

#define HIFB_GP0_ZME_SPLIT_NUM 5

const static hi_u32 g_hifb_layer_support[HIFB_LAYER_ID_BUTT] = { 1, 1, 1, 1, 0, 0 };
#define config_hifb_layer_support(layer) (g_hifb_layer_support[(layer) % HIFB_LAYER_ID_BUTT])

const static hi_u32 g_hifb_layer_max_width[HIFB_LAYER_ID_BUTT] = {
    3840, /* G0 support max width is 3840 */
    3840, /* G1 support max width is 3840 */
    1920, /* G2 support max width is 1920 */
    1920, /* G3 support max width is 1920 */
    0
};
const static hi_u32 g_hifb_layer_max_height[HIFB_LAYER_ID_BUTT] = {
    2160, /* G0 support max height is 2160 */
    2160, /* G1 support max height is 2160 */
    1080, /* G2 support max height is 1080 */
    1080, /* G3 support max height is 1080 */
    0
};
const static hi_u32 g_hifb_layer_max_stride[HIFB_LAYER_ID_BUTT] = {
    30720, /* G0 support max stride is 30720 */
    30720, /* G1 support max stride is 30720 */
    15360, /* G2 support max stride is 15360 */
    15360, /* G3 support max stride is 15360 */
    0
};
const static hi_u32 g_hifb_layer_max_size[HIFB_LAYER_ID_BUTT] = {
    30720 * 2160 * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, /* G0 support max stride is 30720, max height is 2160 */
    30720 * 2160 * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, /* G1 support max stride is 30720, max height is 2160 */
    15360 * 1080 * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, /* G2 support max stride is 15360, max height is 1080 */
    15360 * 1080 * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM, /* G3 support max stride is 15360, max height is 1080 */
    0
};

#define config_hifb_layer_max_width(layer) (g_hifb_layer_max_width[(layer) % HIFB_LAYER_ID_BUTT])
#define config_hifb_layer_max_height(layer) (g_hifb_layer_max_height[(layer) % HIFB_LAYER_ID_BUTT])
#define config_hifb_layer_max_stride(layer) (g_hifb_layer_max_stride[(layer) % HIFB_LAYER_ID_BUTT])
#define config_hifb_layer_max_size(layer) (g_hifb_layer_max_size[(layer) % HIFB_LAYER_ID_BUTT])

#define HIFB_CHECK_LAYER_OPEN(enLayerId) do { \
    if (enLayerId >= HIFB_LAYER_ID_BUTT) {                                                \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enLayerId);                           \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support layer id");         \
        return HI_FAILURE;                                                                \
    }                                                                                     \
    if (HI_TRUE != g_pstGfxDevice[enLayerId]->bOpened) {                                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enLayerId);                           \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this layer has not been open"); \
        return HI_FAILURE;                                                                \
    }                                                                                     \
} while (0)

#define HIFB_CHECK_GP_OPEN(enGpId) do { \
    if (enGpId >= HIFB_ADP_GP_BUTT) {                                                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enGpId);                           \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support this gp id");    \
        return HI_FAILURE;                                                             \
    }                                                                                  \
    if (HI_TRUE != g_pstGfxGPDevice[enGpId]->bOpen) {                                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enGpId);                           \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this gp has not been open"); \
        return HI_FAILURE;                                                             \
    }                                                                                  \
} while (0)

#ifndef HI_BUILD_IN_BOOT
#define STATIC static
#else
#define STATIC
#endif
/* **************************** Structure Definition ********************************************** */
typedef struct tagOPTM_GFX_IRQ_S {
    hi_void *param0; /* * layer id      * */
    hi_void *param1;
    IntCallBack pFunc; /* * irq function * */
#ifndef HI_BUILD_IN_BOOT
    osal_spinlock FuncLock;
#endif
} OPTM_GFX_IRQ_S;

typedef struct tagOPTM_GFX_CALLBACK_S {
    hi_u32 u32CTypeFlag;
    OPTM_GFX_IRQ_S stGfxIrq[HIFB_CALLBACK_TYPE_BUTT];
} OPTM_GFX_CALLBACK_S;

typedef struct tagOPTM_GP_IRQ_S {
    hi_bool bRegistered[HI_DRV_DISP_C_TYPE_BUTT];
    OPTM_GFX_CALLBACK_S stGfxCallBack[OPTM_GP_SUPPORT_MAXLAYERNUM];
} OPTM_GP_IRQ_S;

typedef struct {
    hi_u32 zme_iw;
    hi_u32 zme_ow;
    hi_u32 hor_loffset;
    hi_u32 hor_coffset;
    hi_u32 hratio;
} hifb_gp0_zme_split_cfg;

typedef struct {
    hi_u32 iw;
    hi_u32 ow;
    hi_s32 hor_loffset;
    hi_s32 hor_coffset;

    hi_u64 overlap;
    hi_u64 hratio;
    hi_u64 ow_per;
    hi_u64 ow_align;
    hi_u64 xst_pos_cord;
    hi_u64 xed_pos_cord;
    hi_u64 xst_pos_cord_in;
    hi_u64 c_xst_pos_cord_in;

    hi_u32 hcratio;
    hi_u32 xst_pos_cord_in_int;
    hi_u32 xst_pos_cord_in_int_align2[4]; /* split number 4 */
    hi_u32 pre_xst_pos_cord_in_int_align2;
    hi_u64 xst_pos_cord_in_overlap;
    hi_u64 c_xst_pos_cord_in_overlap;
    hi_s64 loffset;
    hi_s64 coffset;
    hi_u32 node_cfg_zme_iw[4];      /* split number 4 */
    hi_u32 node_cfg_zme_ow[4];      /* split number 4 */
    hi_s32 node_cfg_hor_loffset[4]; /* split number 4 */
    hi_s32 node_cfg_hor_coffset[4]; /* split number 4 */
} hifb_gp0_zme_split_node_cfg;

/* **************************** Global Variable declaration *************************************** */

#ifndef CONFIG_GFX_PQ
static OPTM_ALG_GZME_MEM_S gs_stGPZme;
#endif

HIFB_GFX_MODE_EN g_enOptmGfxWorkMode = HIFB_GFX_MODE_NORMAL;

OPTM_GFX_GP_S *g_pstGfxGPDevice[HIFB_ADP_GP_BUTT] = {NULL};
OPTM_GFX_LAYER_S *g_pstGfxDevice[HIFB_LAYER_ID_BUTT] = {NULL};
HIFB_CAPABILITY_S *g_pstGfxCap[HIFB_LAYER_ID_BUTT] = {NULL};

static OPTM_GP_IRQ_S *g_pstGfxGPIrq[HIFB_ADP_GP_BUTT] = {NULL};

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
extern DRV_HIFB_WBC_GFX_S g_stGfxWbc2;
#endif

#ifndef HI_BUILD_IN_BOOT
static pq_export_func *gs_pstPqFuncs = NULL;
static HIFB_LAYER_ID_E gs_CallBackLayerId[HIFB_LAYER_ID_BUTT];
#endif

/* **************************** API forward declarations ****************************************** */
#ifndef HI_BUILD_IN_BOOT
static hi_void OPTM_GfxWVBCallBack(hi_u32 enLayerId);
#endif

#ifndef HI_BUILD_IN_BOOT
static hi_s32 HIFB_ADP_DispInfoUpdate(HIFB_GP_ID_E enGPId);
static hi_s32 HIFB_ADP_DistributeCallback(hi_void *u32Param0, hi_void *u32Param1);
static hi_void DRV_HIFB_ADP_FrameEndCallBack(hi_void *u32Param0, hi_void *u32Param1);
#endif

#if !defined(HI_BUILD_IN_BOOT) && defined(CONFIG_HIFB_STEREO_SUPPORT)
static hi_s32 DRV_HIFB_ADP_SetStereoDepth(HIFB_LAYER_ID_E enLayerId, hi_s32 s32Depth);
#endif

static hi_s32 HIFB_ADP_GP_Recovery(HIFB_GP_ID_E enGPId);
static hi_s32 HIFB_ADP_SetGpCsc(HIFB_GP_ID_E enGfxGpId, hi_bool bIsBGRIn);

#ifndef CONFIG_GFX_PQ
static OPTM_COLOR_SPACE_E DRV_HIFB_ADP_GetCscTypeFromDisp(hi_drv_color_space enHiDrvCsc);
#else
static hi_drv_color_space DRV_HIFB_ADP_GetCscTypeFromDisp(hi_drv_color_space enHiDrvCsc);
#endif

static inline OPTM_VDP_GFX_IFMT_E OPTM_ClutPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);
static inline OPTM_VDP_GFX_IFMT_E OPTM_RGBPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);
static inline OPTM_VDP_GFX_IFMT_E OPTM_XRGBPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);
static inline OPTM_VDP_GFX_IFMT_E OPTM_XYCbCrPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);

#ifndef HI_BUILD_IN_BOOT
static inline DRV_HIFB_COLOR_FMT_E OPTM_ClutHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt);
static inline DRV_HIFB_COLOR_FMT_E OPTM_RGBHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt);
static inline DRV_HIFB_COLOR_FMT_E OPTM_XRGBHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt);
static inline DRV_HIFB_COLOR_FMT_E OPTM_XYCbCrHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt);
static inline hi_void HIFB_ADP_ReOpen(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
static inline hi_void HIFB_ADP_ReClose(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
static inline hi_void HIFB_ADP_DispSetting(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
static inline hi_void HIFB_ADP_UpdataLayerInfo(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
#endif

#ifndef CONFIG_GFX_PQ
static inline hi_void HIFB_ADP_SetZme(HIFB_GP_ID_E enGpId, hi_bool bGfxSharpen, OPTM_ALG_GZME_RTL_PARA_S *pstZmeRtlPara,
                                      OPTM_ALG_GDTI_RTL_PARA_S *pstDtiRtlPara);
#endif

static inline hi_s32 HIFB_ADP_SetLayerDataBigEndianFmt(HIFB_LAYER_ID_E enLayerId);
static inline hi_s32 HIFB_ADP_SetLayerDataLittleEndianFmt(HIFB_LAYER_ID_E enLayerId);

/* **************************** API realization *************************************************** */
/***************************************************************************************
 * func        : HIFB_PARA_CHECK_SUPPORT
 * description : CNcomment: 判断输入参数是否支持 CNend\n
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_s32 HIFB_PARA_CHECK_SUPPORT(hi_u32 Mask, hi_u32 Width, hi_u32 Height, hi_u32 Stride, hi_u32 Fmt, hi_u32 BitsPerPixel)
{
    if ((Mask & HIFB_PARA_CHECK_WIDTH) &&
        ((Width > CONFIG_HIFB_LAYER_MAXWIDTH) || (Width < CONFIG_HIFB_LAYER_MINWIDTH))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, Width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_HIFB_LAYER_MAXWIDTH);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_HIFB_LAYER_MINWIDTH);
        return HI_FAILURE;
    }

    if ((Mask & HIFB_PARA_CHECK_HEIGHT) &&
        ((Height > CONFIG_HIFB_LAYER_MAXHEIGHT) || (Height < CONFIG_HIFB_LAYER_MINHEIGHT))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, Height);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_HIFB_LAYER_MAXHEIGHT);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_HIFB_LAYER_MINHEIGHT);
        return HI_FAILURE;
    }

    if ((Mask & HIFB_PARA_CHECK_STRIDE) &&
        ((Stride > CONFIG_HIFB_LAYER_MAXSTRIDE) || (Stride <= CONFIG_HIFB_LAYER_MINSTRIDE))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, Stride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_HIFB_LAYER_MAXSTRIDE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_HIFB_LAYER_MINSTRIDE);
        return HI_FAILURE;
    }

    if ((Mask & HIFB_PARA_CHECK_BITSPERPIXEL) && (BitsPerPixel > CONFIG_HIFB_LAYER_BITSPERPIX)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, BitsPerPixel);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_HIFB_LAYER_BITSPERPIX);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_u32 HIFB_ADP_Align(const hi_u32 x, const hi_u32 a)
{
    return (0 == a) ? (x) : (((x + (a - 1)) / a) * a);
}

#ifdef HI_BUILD_IN_BOOT
extern hi_s32 GfxWinReviseOutRect(const hi_drv_rect *tmp_virtscreen, const HI_DRV_DISP_OFFSET_S *stOffsetInfo,
                                  const hi_drv_rect *fmt_resolution, const hi_drv_rect *pixel_fmt_resolution,
                                  hi_drv_rect *stToBeRevisedRect, hi_drv_rect *stRevisedRect);

/***************************************************************************************
 * func         : DRV_HIFB_ADP_LOGO_InitGpFromDisp
 * description  : CNcomment: 初始化参数，参数来源于display  CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_LOGO_InitGpFromDisp(HIFB_GP_ID_E enGPId, hi_bool Support4KLogo)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_drv_display enDisp = HI_DRV_DISPLAY_0;
    hi_disp_display_info stInfo;
#ifndef CONFIG_GFX_PQ
    OPTM_COLOR_SPACE_E enGpCsc = OPTM_CS_BT709_YUV_LIMITED;
#else
    HI_PQ_GFX_COLOR_SPACE_E enGpCsc = PQ_GFX_CS_BUTT;
#endif

    HIFB_CHECK_GP_OPEN(enGPId);
    enDisp = (enGPId == HIFB_ADP_GP0) ? (HI_DRV_DISPLAY_0) : (HI_DRV_DISPLAY_1);

    memset(&stInfo, 0, sizeof(stInfo));
    Ret = HI_DISP_GetDisplayInfo(enDisp, &stInfo);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_DISP_GetDisplayInfo, Ret);
        return HI_FAILURE;
    }

    g_pstGfxGPDevice[enGPId]->bInterface = stInfo.bInterlace;

    GfxWinReviseOutRect(&stInfo.virtaul_screen, &stInfo.stOffsetInfo, &stInfo.fmt_resolution,
                        &stInfo.pixel_fmt_resolution, &stInfo.virtaul_screen, &g_pstGfxGPDevice[enGPId]->stOutRect);

    if ((HIFB_ADP_GP0 == enGPId) && (HI_TRUE == Support4KLogo)) {
        g_pstGfxGPDevice[enGPId]->stOutRect.rect_x = 0;
        g_pstGfxGPDevice[enGPId]->stOutRect.rect_y = 0;
        g_pstGfxGPDevice[enGPId]->stOutRect.rect_w = stInfo.fmt_resolution.rect_w;
        g_pstGfxGPDevice[enGPId]->stOutRect.rect_h = stInfo.fmt_resolution.rect_h;
    }

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HI_TRUE == stInfo.bIsMaster) && (HIFB_ADP_GP0 == enGPId)) {
        g_enOptmGfxWorkMode = HIFB_GFX_MODE_HD_WBC;
    }
#endif

    HIFB_ADP_GP_Recovery(enGPId);

#ifndef CONFIG_GFX_PQ
    enGpCsc = DRV_HIFB_ADP_GetCscTypeFromDisp(stInfo.eColorSpace);
#else
    enGpCsc = DRV_HIFB_ADP_GetCscTypeFromDisp(stInfo.eColorSpace);
#endif

    g_pstGfxGPDevice[enGPId]->enOutputCsc = enGpCsc;
    HIFB_ADP_SetGpCsc(enGPId, g_pstGfxGPDevice[enGPId]->bBGRState);

    return HI_SUCCESS;
}
#endif

/***************************************************************************************
 * func         : DRV_HIFB_ADP_GetDevCap
 * description  : CNcomment: 获取图层能力级 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static inline hi_void DRV_HIFB_ADP_GetDevCap(const HIFB_CAPABILITY_S **pstCap)
{
    if ((NULL != pstCap) && (NULL != g_pstGfxCap[0])) {
        *pstCap = g_pstGfxCap[0];
    }

    return;
}

static hi_void hifb_init_capability(hi_u32 layer)
{
    HIFB_CAPABILITY_S gfx_cap = {
        .bKeyAlpha = 1,
        .bGlobalAlpha = 1,
#ifdef CONFIG_HIFB_CMP_SUPPORT
        .bCmap = 1,
        .bHasCmapReg = 1,
#else
        .bCmap = 0,
        .bHasCmapReg = 0,
#endif
        .bColFmt = { 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, /* 0 - 9 */
                     1, 0, 1, 1, 1, 1, 1, 1, 0, 0, /* 10 - 19 */
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 20 - 29 */
                     0, 0, 0, 0, 1, 1, 0 },        /* 20 - 36 */
        .bVoScale = HI_TRUE,
        .bLayerSupported = config_hifb_layer_support(layer),
        .bCompression = HI_TRUE,
        .bStereo = 1,
        .u32MaxWidth = config_hifb_layer_max_width(layer),
        .u32MaxHeight = config_hifb_layer_max_height(layer),
        .u32MaxStride = config_hifb_layer_max_stride(layer),
        .u32MaxSize = config_hifb_layer_max_size(layer),
        .u32MinWidth = CONFIG_HIFB_LAYER_MINWIDTH,
        .u32MinHeight = CONFIG_HIFB_LAYER_MINHEIGHT,
        .u32MinStride = CONFIG_HIFB_LAYER_MINSTRIDE,
        .u32MinSize = CONFIG_HIFB_LAYER_MINSIZE,
        .u32VDefLevel = 0,
        .u32HDefLevel = 0,
    };
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(g_pstGfxCap);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(g_pstGfxCap[layer]);
    memcpy(g_pstGfxCap[layer], &gfx_cap, sizeof(HIFB_CAPABILITY_S));
}

/***************************************************************************************
 * func         : GFXCAPABILITY_Init
 * description  : CNcomment: 图锟斤拷锟斤拷锟斤拷锟斤拷 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static inline hi_s32 GFXCAPABILITY_Init(hi_void)
{
    HIFB_LAYER_ID_E layer = 0;

    for (layer = HIFB_LAYER_HD_0; layer < HIFB_LAYER_ID_BUTT; layer++) {
        hifb_init_capability(layer);
    }

    return HI_SUCCESS;
}

hi_u32 hifb_adp_get_layer_max_size(HIFB_LAYER_ID_E layer)
{
    hi_u32 stride;
    hi_u32 cmp_stride = 0;
    hi_u32 buf_size;

    stride = CONIFG_HIFB_GetMaxStride(config_hifb_layer_max_width(layer), CONFIG_HIFB_DEFAULT_DEPTH, &cmp_stride,
                                      CONFIG_HIFB_STRIDE_ALIGN);
    buf_size = HI_HIFB_GetMemSize(stride, config_hifb_layer_max_height(layer) * CONFIG_HIFB_LAYER_BUFFER_MAX_NUM);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, buf_size);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, config_hifb_layer_max_width(layer));
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, config_hifb_layer_max_height(layer));
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stride);
    return buf_size;
}

hi_s32 hifb_check_whether_resolution_support(hi_u32 layer, hi_u32 width, hi_u32 height)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (layer) {
        case HIFB_LAYER_HD_0:
        case HIFB_LAYER_HD_1:
        case HIFB_LAYER_HD_2:
        case HIFB_LAYER_HD_3:
            if ((width < CONFIG_HIFB_LAYER_MINWIDTH) || (width > config_hifb_layer_max_width(layer)) ||
                (height < CONFIG_HIFB_LAYER_MINHEIGHT) || (height > config_hifb_layer_max_height(layer))) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, width);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, height);
                return HI_FAILURE;
            }
            break;
        default:
            return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : HIFB_ADP_GetGfxHalId
 * description  : CNcomment: hifb ID 转换成 图层ID，配寄存器使用 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static OPTM_VDP_LAYER_GFX_E HIFB_ADP_GetGfxHalId(HIFB_LAYER_ID_E LayerId)
{
#if defined(CONFIG_HIFB_VERSION_1_0) || defined(CONFIG_HIFB_VERSION_2_0)
    switch (LayerId) {
        case HIFB_LAYER_HD_0:
            return OPTM_VDP_LAYER_GFX0;
        case HIFB_LAYER_HD_1:
            return OPTM_VDP_LAYER_GFX1;
        case HIFB_LAYER_HD_2:
            return OPTM_VDP_LAYER_GFX2;
        case HIFB_LAYER_HD_3:
            return OPTM_VDP_LAYER_GFX3;
        case HIFB_LAYER_SD_0:
            return OPTM_VDP_LAYER_GFX4;
        case HIFB_LAYER_SD_1:
            return OPTM_VDP_LAYER_GFX5;
        default:
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerId);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, OPTM_VDP_LAYER_GFX_BUTT);
            return OPTM_VDP_LAYER_GFX_BUTT;
    }
#elif defined(CONFIG_HIFB_VERSION_3_0)
    switch (LayerId) {
        case HIFB_LAYER_HD_0:
            return OPTM_VDP_LAYER_GFX0; /* *match to g0 register * */
        case HIFB_LAYER_HD_1:
            return OPTM_VDP_LAYER_GFX2; /* *match to g3 register * */
        case HIFB_LAYER_SD_0:
            return OPTM_VDP_LAYER_GFX1; /* *match to g1 register * */
        default:
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerId);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, OPTM_VDP_LAYER_GFX_BUTT);
            return OPTM_VDP_LAYER_GFX_BUTT;
    }
#else
    switch (LayerId) {
        case HIFB_LAYER_HD_0:
            return OPTM_VDP_LAYER_GFX0;
        case HIFB_LAYER_HD_1:
            return OPTM_VDP_LAYER_GFX1;
        case HIFB_LAYER_HD_2:
            return OPTM_VDP_LAYER_GFX2;
        case HIFB_LAYER_HD_3:
            return OPTM_VDP_LAYER_GFX3;
        case HIFB_LAYER_SD_0:
            return OPTM_VDP_LAYER_GFX4;
        case HIFB_LAYER_SD_1:
            return OPTM_VDP_LAYER_GFX5;
        default:
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerId);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, OPTM_VDP_LAYER_GFX_BUTT);
            return OPTM_VDP_LAYER_GFX_BUTT;
    }
#endif
}

/***************************************************************************************
 * func          : HIFB_ADP_GetGpId
 * description   : CNcomment: 将layer id 转换成 GP ID CNend\n
 * param[in]     : LayerId
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static HIFB_GP_ID_E HIFB_ADP_GetGpId(HIFB_LAYER_ID_E LayerId)
{
    if (LayerId <= HIFB_LAYER_HD_2) {
        return HIFB_ADP_GP0;
    }

    if ((LayerId >= HIFB_LAYER_HD_3) && (LayerId <= HIFB_LAYER_SD_1)) {
        return HIFB_ADP_GP1;
    }

    return HIFB_ADP_GP_BUTT;
}

/***************************************************************************************
 * func          : HIFB_ADP_GetGpHalId
 * description   : CNcomment: 将Gp id 转换成 GP 硬件 ID CNend\n
 * param[in]     : LayerId
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static OPTM_VDP_LAYER_GP_E HIFB_ADP_GetGpHalId(HIFB_GP_ID_E GpId)
{
    if (HIFB_ADP_GP0 == GpId) {
        return OPTM_VDP_LAYER_GP0;
    }

    if (HIFB_ADP_GP1 == GpId) {
        return OPTM_VDP_LAYER_GP1;
    }

    return OPTM_VDP_LAYER_GP_BUTT;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_GetCscTypeFromDisp
 * description  : CNcomment: 从disp中获取CSC类型 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
#ifndef CONFIG_GFX_PQ
static OPTM_COLOR_SPACE_E DRV_HIFB_ADP_GetCscTypeFromDisp(hi_drv_color_space enHiDrvCsc)
{
    return enHiDrvCsc;
}
#else
static hi_drv_color_space DRV_HIFB_ADP_GetCscTypeFromDisp(hi_drv_color_space enHiDrvCsc)
{
    return enHiDrvCsc;
}
#endif

#ifdef CONFIG_HIFB_STEREO_SUPPORT
/***************************************************************************************
 * func        : HIFB_ADP_GetStereoModeFromDisp
 * description : CNcomment: 从disp中获取3D模式 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static HIFB_STEREO_MODE_E HIFB_ADP_GetStereoModeFromDisp(OPTM_VDP_DISP_MODE_E enDispStereo)
{
    switch (enDispStereo) {
        case VDP_DISP_MODE_2D:
            return HIFB_STEREO_MONO;
        case VDP_DISP_MODE_SBS:
            return HIFB_STEREO_SIDEBYSIDE_HALF;
        case VDP_DISP_MODE_TAB:
            return HIFB_STEREO_TOPANDBOTTOM;
        case VDP_DISP_MODE_FP:
            return HIFB_STEREO_FRMPACKING;
        default:
            return HIFB_STEREO_BUTT;
    }
}
#endif

/***************************************************************************************
 * func        : HIFB_ADP_InitVdpReg
 * description : CNcomment: vdp register inital  CNend\n
 * param[in]   : hi_s32
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_s32 HIFB_ADP_InitVdpReg(hi_void)
{
    return drv_hifb_hal_vdp_initial();
}

/***************************************************************************************
 * func        : HIFB_ADP_DeInitVdpReg
 * description : CNcomment: vdp register dinital  CNend\n
 * param[in]   : hi_s32
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_void HIFB_ADP_DeInitVdpReg(hi_void)
{
    return drv_hifb_hal_vdp_deinitial();
}

/***************************************************************************************
 * func        : HIFB_ADP_InitGpIrq
 * description : CNcomment: GP IRQ inital  CNend\n
 * param[in]   : hi_s32
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_s32 HIFB_ADP_InitGpIrq(hi_void)
{
    hi_s32 Index = 0;

#ifdef HI_BUILD_IN_BOOT
    g_pstGfxGPIrq[0] = (OPTM_GP_IRQ_S *)GraphicSysMemMalloc(sizeof(OPTM_GP_IRQ_S) * HIFB_ADP_GP_BUTT, "gp_irq");
    if (NULL == g_pstGfxGPIrq[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GraphicSysMemMalloc, FAILURE_TAG);
        return HI_FAILURE;
    }
#else
    g_pstGfxGPIrq[0] = (OPTM_GP_IRQ_S *)HI_GFX_VMALLOC(HIGFX_FB_ID, sizeof(OPTM_GP_IRQ_S) * HIFB_ADP_GP_BUTT);
    if (NULL == g_pstGfxGPIrq[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_VMALLOC, FAILURE_TAG);
        return HI_FAILURE;
    }
#endif

    for (Index = 0; Index < HIFB_ADP_GP_BUTT; Index++) {
        g_pstGfxGPIrq[Index] = (OPTM_GP_IRQ_S *)((hi_void *)g_pstGfxGPIrq[0] + Index * sizeof(OPTM_GP_IRQ_S));
        memset(g_pstGfxGPIrq[Index], 0, sizeof(OPTM_GP_IRQ_S));
    }

    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : HIFB_ADP_DeInitGpIrq
 * description : CNcomment: GP IRQ dinital  CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_void HIFB_ADP_DeInitGpIrq(hi_void)
{
    hi_s32 i;
#ifdef HI_BUILD_IN_BOOT
    if (NULL != g_pstGfxGPIrq[0]) {
        GraphicSysMemFree((hi_void *)g_pstGfxGPIrq[0]);
    }
#else
    if (NULL != g_pstGfxGPIrq[0]) {
        HI_GFX_VFREE(HIGFX_FB_ID, (hi_void *)g_pstGfxGPIrq[0]);
    }
#endif

    for (i = 0; i < HIFB_ADP_GP_BUTT; i++) {
        g_pstGfxGPIrq[i] = NULL;
    }

    return;
}

/***************************************************************************************
 * func        : HIFB_ADP_InitGpDev
 * description : CNcomment: GP Device inital  CNend\n
 * param[in]   : hi_s32
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_s32 HIFB_ADP_InitGpDev(hi_void)
{
    hi_s32 Index = 0;

#ifdef HI_BUILD_IN_BOOT
    g_pstGfxGPDevice[0] = (OPTM_GFX_GP_S *)GraphicSysMemMalloc(sizeof(OPTM_GFX_GP_S) * HIFB_ADP_GP_BUTT, "gp_dev");
    if (NULL == g_pstGfxGPDevice[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GraphicSysMemMalloc, FAILURE_TAG);
        return HI_FAILURE;
    }
#else
    g_pstGfxGPDevice[0] = (OPTM_GFX_GP_S *)HI_GFX_VMALLOC(HIGFX_FB_ID, sizeof(OPTM_GFX_GP_S) * HIFB_ADP_GP_BUTT);
    if (NULL == g_pstGfxGPDevice[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_VMALLOC, FAILURE_TAG);
        return HI_FAILURE;
    }
#endif

    for (Index = 0; Index < HIFB_ADP_GP_BUTT; Index++) {
        g_pstGfxGPDevice[Index] = (OPTM_GFX_GP_S *)((hi_void *)g_pstGfxGPDevice[0] + Index * sizeof(OPTM_GFX_GP_S));
        memset(g_pstGfxGPDevice[Index], 0, sizeof(OPTM_GFX_GP_S));
        g_pstGfxGPDevice[Index]->stInRect.rect_w = CONFIG_HIFB_LAYER_MINWIDTH;
        g_pstGfxGPDevice[Index]->stInRect.rect_h = CONFIG_HIFB_LAYER_MINHEIGHT;
        g_pstGfxGPDevice[Index]->stOutRect.rect_w = CONFIG_HIFB_LAYER_MINWIDTH;
        g_pstGfxGPDevice[Index]->stOutRect.rect_h = CONFIG_HIFB_LAYER_MINHEIGHT;
        if (osal_spin_lock_init(&g_pstGfxGPDevice[Index]->gp_lock) != 0) {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : HIFB_ADP_DeInitGpDev
 * description : CNcomment: GP Device dinital  CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_void HIFB_ADP_DeInitGpDev(hi_void)
{
    hi_s32 i;

    for (i = 0; i < HIFB_ADP_GP_BUTT; i++) {
        if (g_pstGfxGPDevice[i]->gp_lock.lock != HI_NULL) {
            osal_spin_lock_destory(&g_pstGfxGPDevice[i]->gp_lock);
        }
    }
#ifdef HI_BUILD_IN_BOOT
    if (NULL != g_pstGfxGPDevice[0]) {
        GraphicSysMemFree((hi_void *)g_pstGfxGPDevice[0]);
    }
#else
    if (NULL != g_pstGfxGPDevice[0]) {
        HI_GFX_VFREE(HIGFX_FB_ID, (hi_void *)g_pstGfxGPDevice[0]);
    }
#endif

    for (i = 0; i < HIFB_ADP_GP_BUTT; i++) {
        g_pstGfxGPDevice[i] = NULL;
    }

    return;
}

/***************************************************************************************
 * func        : HIFB_ADP_InitGfxDev
 * description : CNcomment: GFX Device inital  CNend\n
 * param[in]   : hi_s32
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_s32 HIFB_ADP_InitGfxDev(hi_void)
{
    hi_s32 Index = 0;

#ifdef HI_BUILD_IN_BOOT
    g_pstGfxDevice[0] = (OPTM_GFX_LAYER_S *)GraphicSysMemMalloc(sizeof(OPTM_GFX_LAYER_S) * HIFB_LAYER_ID_BUTT,
                                                                "gfx_dev");
    if (NULL == g_pstGfxDevice[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GraphicSysMemMalloc, FAILURE_TAG);
        return HI_FAILURE;
    }
#else
    g_pstGfxDevice[0] = (OPTM_GFX_LAYER_S *)HI_GFX_VMALLOC(HIGFX_FB_ID, sizeof(OPTM_GFX_LAYER_S) * HIFB_LAYER_ID_BUTT);
    if (NULL == g_pstGfxDevice[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_VMALLOC, FAILURE_TAG);
        return HI_FAILURE;
    }
#endif

    for (Index = 0; Index < HIFB_LAYER_ID_BUTT; Index++) {
        g_pstGfxDevice[Index] = (OPTM_GFX_LAYER_S *)((hi_void *)g_pstGfxDevice[0] + Index * sizeof(OPTM_GFX_LAYER_S));
        memset(g_pstGfxDevice[Index], 0, sizeof(OPTM_GFX_LAYER_S));
        g_pstGfxDevice[Index]->stInRect.w = CONFIG_HIFB_LAYER_MINWIDTH;
        g_pstGfxDevice[Index]->stInRect.h = CONFIG_HIFB_LAYER_MINHEIGHT;
    }

    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : HIFB_ADP_DeInitGfxDev
 * description : CNcomment: GFX Device dinital  CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_void HIFB_ADP_DeInitGfxDev(hi_void)
{
    hi_s32 i;
#ifdef HI_BUILD_IN_BOOT
    if (NULL != g_pstGfxDevice[0]) {
        GraphicSysMemFree((hi_void *)g_pstGfxDevice[0]);
    }
#else
    if (NULL != g_pstGfxDevice[0]) {
        HI_GFX_VFREE(HIGFX_FB_ID, (hi_void *)g_pstGfxDevice[0]);
    }
#endif

    for (i = 0; i < HIFB_LAYER_ID_BUTT; i++) {
        g_pstGfxDevice[i] = NULL;
    }
    return;
}

/***************************************************************************************
 * func         : HIFB_ADP_InitGfxCapability
 * description  : CNcomment: 初始化图层能力级 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static inline hi_s32 HIFB_ADP_InitGfxCapability(hi_void)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_s32 Index = 0;
#ifdef HI_BUILD_IN_BOOT
    g_pstGfxCap[0] = (HIFB_CAPABILITY_S *)GraphicSysMemMalloc(sizeof(HIFB_CAPABILITY_S) * HIFB_LAYER_ID_BUTT,
                                                              "gfx_capability");
    if (NULL == g_pstGfxCap[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GraphicSysMemMalloc, FAILURE_TAG);
        return HI_FAILURE;
    }
#else
    g_pstGfxCap[0] = (HIFB_CAPABILITY_S *)HI_GFX_VMALLOC(HIGFX_FB_ID, sizeof(HIFB_CAPABILITY_S) * HIFB_LAYER_ID_BUTT);
    if (NULL == g_pstGfxCap[0]) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_VMALLOC, FAILURE_TAG);
        return HI_FAILURE;
    }
#endif

    for (Index = 0; Index < HIFB_LAYER_ID_BUTT; Index++) {
        g_pstGfxCap[Index] = (HIFB_CAPABILITY_S *)((hi_void *)g_pstGfxCap[0] + Index * sizeof(HIFB_CAPABILITY_S));
        memset(g_pstGfxCap[Index], 0, sizeof(HIFB_CAPABILITY_S));
    }

    Ret = GFXCAPABILITY_Init();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFXCAPABILITY_Init, FAILURE_TAG);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : HIFB_ADP_DeInitGfxCapability
 * description  : CNcomment: 去初始化图层能力级 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static inline hi_void HIFB_ADP_DeInitGfxCapability(hi_void)
{
    hi_s32 i;
#ifdef HI_BUILD_IN_BOOT
    if (NULL != g_pstGfxCap[0]) {
        GraphicSysMemFree((hi_void *)g_pstGfxCap[0]);
    }
#else
    if (NULL != g_pstGfxCap[0]) {
        HI_GFX_VFREE(HIGFX_FB_ID, (hi_void *)g_pstGfxCap[0]);
    }
#endif

    for (i = 0; i < HIFB_LAYER_ID_BUTT; i++) {
        g_pstGfxCap[i] = NULL;
    }
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_Init
 * description  : CNcomment: 驱动初始化 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_Init(hi_void)
{
    hi_s32 Ret = HI_SUCCESS;

    Ret = HIFB_ADP_InitVdpReg();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_ADP_InitVdpReg, Ret);
        return HI_FAILURE;
    }

    Ret = HIFB_ADP_InitGfxCapability();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_ADP_InitGfxCapability, Ret);
        return HI_FAILURE;
    }

    Ret = HIFB_ADP_InitGfxDev();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_ADP_InitGfxDev, Ret);
        return HI_FAILURE;
    }

    Ret = HIFB_ADP_InitGpDev();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_ADP_InitGpDev, Ret);
        return HI_FAILURE;
    }

    Ret = HIFB_ADP_InitGpIrq();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_ADP_InitGpIrq, Ret);
        return HI_FAILURE;
    }

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    Ret = DRV_HIFB_WBC_Init();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_WBC_Init, Ret);
        return HI_FAILURE;
    }
#endif

#ifndef CONFIG_GFX_PQ
    Ret = OPTM_ALG_GZmeVdpComnInit(&gs_stGPZme);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, OPTM_ALG_GZmeVdpComnInit, Ret);
        return HI_FAILURE;
    }
#else
#ifdef HI_BUILD_IN_BOOT
    Ret = DRV_PQ_InitGfxZme();
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_PQ_InitGfxZme, Ret);
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_DeInit
 * description : CNcomment: 图形设备去初始化 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_DeInit(hi_void)
{
#ifndef HI_BUILD_IN_BOOT
    hi_u32 u32LayerId = HIFB_LAYER_HD_0;

    for (u32LayerId = HIFB_LAYER_HD_0; u32LayerId < HIFB_LAYER_ID_BUTT; u32LayerId++) {
        if (g_pstGfxDevice[u32LayerId] == HI_NULL) {
            continue;
        }
        if (HI_FALSE != g_pstGfxDevice[u32LayerId]->bOpened) {
            DRV_HIFB_ADP_CloseLayer(u32LayerId);
        }
    }

#ifndef CONFIG_GFX_PQ
    OPTM_ALG_GZmeVdpComnDeInit(&gs_stGPZme);
#endif

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    DRV_HIFB_WBC_DeInit();
    DRV_HIFB_WBC_SetTcFlag(HI_FALSE);
#endif

    ps_DispExportFuncs = NULL;
#endif

    HIFB_ADP_DeInitGpIrq();
    HIFB_ADP_DeInitGpDev();
    HIFB_ADP_DeInitGfxDev();
    HIFB_ADP_DeInitGfxCapability();
    HIFB_ADP_DeInitVdpReg();

    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func         : DRV_HIFB_ADP_StereoCallBack
 * description  : CNcomment: 执行3D  工作对列函数 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_void DRV_HIFB_ADP_StereoCallBack(struct work_struct *data)
{
    hi_u32 i = 0;
    hi_u32 u32CTypeFlag = 0;
    hi_u32 u32LayerCount = 0;
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    OPTM_GFX_WORK_S *pst3DModeWork = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(data);
    pst3DModeWork = container_of(data, OPTM_GFX_WORK_S, work);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pst3DModeWork);
    enGpId = (HIFB_GP_ID_E)(pst3DModeWork->u32Data);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    u32LayerCount = (HIFB_ADP_GP0 == enGpId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(u32LayerCount, OPTM_GP_SUPPORT_MAXLAYERNUM);

    for (i = 0; i < u32LayerCount; i++) {
        u32CTypeFlag = g_pstGfxGPIrq[enGpId]->stGfxCallBack[i].u32CTypeFlag;
        if (0 == u32CTypeFlag) {
            continue;
        }

        if (HIFB_CALLBACK_TYPE_3DMode_CHG != (u32CTypeFlag & HIFB_CALLBACK_TYPE_3DMode_CHG)) {
            continue;
        }

        if (NULL != g_pstGfxGPIrq[enGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_3DMode_CHG].pFunc) {
            g_pstGfxGPIrq[enGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_3DMode_CHG].pFunc(
                g_pstGfxGPIrq[enGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_3DMode_CHG].param0,
                (hi_void *)&g_pstGfxGPDevice[enGpId]->enTriDimMode);
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

/***************************************************************************************
 * func        : HIFB_ADP_InitAlg
 * description : CNcomment: 算法初始化 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_void HIFB_ADP_InitAlg(HIFB_GP_ID_E enGPId)
{
#ifndef CONFIG_GFX_PQ
    OPTM_ALG_GDTI_RTL_PARA_S stDtiRtlPara;
    memset(&stDtiRtlPara, 0, sizeof(stDtiRtlPara));

    OPTM_ALG_GDtiInit(&stDtiRtlPara);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGPId);
    DRV_HIFB_HAL_GP_SetTiHpCoef(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_CHM,
                                (hi_s32 *)stDtiRtlPara.s32CTIHPTmp);
    DRV_HIFB_HAL_GP_SetTiHpCoef(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_LUM,
                                (hi_s32 *)stDtiRtlPara.s32LTIHPTmp);
    DRV_HIFB_HAL_GP_SetTiGainRatio(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_CHM,
                                   (hi_s32)stDtiRtlPara.s16CTICompsatRatio);
    DRV_HIFB_HAL_GP_SetTiGainRatio(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_LUM,
                                   (hi_s32)stDtiRtlPara.s16LTICompsatRatio);
    DRV_HIFB_HAL_GP_SetTiCoringThd(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_CHM,
                                   (hi_u32)stDtiRtlPara.u16CTICoringThrsh);
    DRV_HIFB_HAL_GP_SetTiCoringThd(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_LUM,
                                   (hi_u32)stDtiRtlPara.u16LTICoringThrsh);
    DRV_HIFB_HAL_GP_SetTiSwingThd(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_CHM,
                                  (hi_u32)stDtiRtlPara.u16CTIOverSwingThrsh,
                                  (hi_u32)stDtiRtlPara.u16CTIUnderSwingThrsh);
    DRV_HIFB_HAL_GP_SetTiSwingThd(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_LUM,
                                  (hi_u32)stDtiRtlPara.u16LTIOverSwingThrsh,
                                  (hi_u32)stDtiRtlPara.u16LTIUnderSwingThrsh);
    DRV_HIFB_HAL_GP_SetTiMixRatio(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_CHM,
                                  (hi_u32)stDtiRtlPara.u8CTIMixingRatio);
    DRV_HIFB_HAL_GP_SetTiMixRatio(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_LUM,
                                  (hi_u32)stDtiRtlPara.u8LTIMixingRatio);
    DRV_HIFB_HAL_GP_SetTiHfThd(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_LUM,
                               (hi_u32 *)stDtiRtlPara.u32LTIHFreqThrsh);
    DRV_HIFB_HAL_GP_SetTiGainCoef(g_pstGfxGPDevice[enGPId]->enGpHalId, VDP_TI_MODE_LUM,
                                  (hi_u32 *)stDtiRtlPara.u32LTICompsatMuti);
#else
#endif
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_GP_Open
 * description  : CNcomment: 打开GP设备并设置ZME系数 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_GP_Open(HIFB_GP_ID_E enGPId)
{
#ifndef HI_BUILD_IN_BOOT
    hi_s32 Ret = HI_SUCCESS;
#endif
    hi_u32 PrioValue = 0;
    hi_u32 InitLayerID = 0;
    hi_u32 LayerIdIndex = 0;
    hi_u32 MaxLayerCount = 0;
    hi_ulong lock_flag = 0;
    OPTM_VDP_BKG_S stBkg = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_pstGfxGPDevice[0], HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_pstGfxDevice[0], HI_FAILURE);

    osal_spin_lock_irqsave(&g_pstGfxGPDevice[enGPId]->gp_lock, &lock_flag);
    if (HI_TRUE == g_pstGfxGPDevice[enGPId]->bOpen) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        osal_spin_unlock_irqrestore(&g_pstGfxGPDevice[enGPId]->gp_lock, &lock_flag);
        return HI_SUCCESS;
    }
    g_pstGfxGPDevice[enGPId]->bOpen = HI_TRUE;
    osal_spin_unlock_irqrestore(&g_pstGfxGPDevice[enGPId]->gp_lock, &lock_flag);
#ifndef HI_BUILD_IN_BOOT
    Ret = osal_exportfunc_get(HI_ID_DISP, (hi_void **)&ps_DispExportFuncs);
    if ((HI_SUCCESS != Ret) || (NULL == ps_DispExportFuncs)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "get disp func failure");
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_drv_module_get_function, Ret);
        return HI_FAILURE;
    }

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if (HIFB_ADP_GP0 == enGPId) {
        g_pstGfxGPDevice[enGPId]->queue = create_workqueue(HIFB_WORK_QUEUE);
        if (NULL == g_pstGfxGPDevice[enGPId]->queue) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, create_workqueue, FAILURE_TAG);
            return HI_FAILURE;
        }

        INIT_WORK(&g_pstGfxGPDevice[enGPId]->stStartWbcWork.work, DRV_HIFB_WBC_StartWorkQueue);
        INIT_WORK(&g_pstGfxGPDevice[enGPId]->stStartStereoWork.work, DRV_HIFB_ADP_StereoCallBack);
    }
#endif
#endif

    g_pstGfxGPDevice[enGPId]->u32ZmeDeflicker = (HIFB_ADP_GP0 == enGPId) ? (0) : (1);
    g_pstGfxGPDevice[enGPId]->u32Alpha = 0XFF;
    g_pstGfxGPDevice[enGPId]->enReadMode = VDP_RMODE_PROGRESSIVE;
    g_pstGfxGPDevice[enGPId]->stBkg = stBkg;
#ifndef CONFIG_GFX_PQ
    g_pstGfxGPDevice[enGPId]->enInputCsc = OPTM_CS_BT709_RGB_FULL;
    g_pstGfxGPDevice[enGPId]->enOutputCsc = OPTM_CS_UNKNOWN;
#else
    g_pstGfxGPDevice[enGPId]->in_csc.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    g_pstGfxGPDevice[enGPId]->in_csc.color_space = HI_DRV_COLOR_CS_RGB;
    g_pstGfxGPDevice[enGPId]->in_csc.quantify_range = HI_DRV_COLOR_FULL_RANGE;
    g_pstGfxGPDevice[enGPId]->out_csc.color_space = HI_DRV_COLOR_CS_MAX;
#endif
    g_pstGfxGPDevice[enGPId]->bBGRState = HI_FALSE;
    g_pstGfxGPDevice[enGPId]->bGpClose = HI_FALSE;
    g_pstGfxGPDevice[enGPId]->bRecoveryInNextVT = HI_TRUE;
    g_pstGfxGPDevice[enGPId]->bDispInitial = HI_FALSE;

    if (HIFB_ADP_GP0 == enGPId) {
        InitLayerID = (hi_u32)HIFB_LAYER_HD_0;
        MaxLayerCount = (hi_u32)(OPTM_GP0_GFX_COUNT - 1);
        g_pstGfxGPDevice[enGPId]->enMixg = VDP_CBM_MIXG0;
        g_pstGfxGPDevice[enGPId]->enDispCh = OPTM_DISPCHANNEL_1;
    } else {
        InitLayerID = (hi_u32)HIFB_LAYER_SD_0;
        MaxLayerCount = (hi_u32)(OPTM_GP1_GFX_COUNT + InitLayerID - 1);
        g_pstGfxGPDevice[enGPId]->enMixg = VDP_CBM_MIXG1;
        g_pstGfxGPDevice[enGPId]->enDispCh = OPTM_DISPCHANNEL_0;
    }

    g_pstGfxGPDevice[enGPId]->enGpHalId = HIFB_ADP_GetGpHalId(enGPId);
    g_pstGfxGPDevice[enGPId]->u32Prior = 0x0;

    drv_hifb_hal_gp_set_layer_galpha(g_pstGfxGPDevice[enGPId]->enGpHalId, g_pstGfxGPDevice[enGPId]->u32Alpha);
    drv_hifb_hal_gp_set_read_mode(g_pstGfxGPDevice[enGPId]->enGpHalId, g_pstGfxGPDevice[enGPId]->enReadMode);
    drv_hifb_hal_gp_set_up_mode(g_pstGfxGPDevice[enGPId]->enGpHalId, g_pstGfxGPDevice[enGPId]->enReadMode);
    drv_hifb_hal_cbm_set_mixer_bkg(g_pstGfxGPDevice[enGPId]->enMixg, &(g_pstGfxGPDevice[enGPId]->stBkg));

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(MaxLayerCount, HIFB_LAYER_ID_BUTT, HI_FAILURE);
    for (LayerIdIndex = InitLayerID; LayerIdIndex <= MaxLayerCount; LayerIdIndex++) {
        PrioValue = LayerIdIndex;
#ifdef CONFIG_HIFB_VERSION_3_0
        if (HIFB_LAYER_HD_1 == LayerIdIndex) { /* *match to g3 mix * */
            PrioValue = 0x2;
        }
#endif
        g_pstGfxDevice[LayerIdIndex]->enGfxHalId = HIFB_ADP_GetGfxHalId(LayerIdIndex);
        g_pstGfxDevice[LayerIdIndex]->u32ZOrder = LayerIdIndex;
        drv_hifb_hal_cbm_set_mixer_prio(g_pstGfxGPDevice[enGPId]->enMixg, PrioValue,
                                        g_pstGfxDevice[LayerIdIndex]->u32ZOrder);
    }
    drv_hifb_hal_cbm_get_mixer_prio(g_pstGfxGPDevice[enGPId]->enMixg, &g_pstGfxGPDevice[enGPId]->u32Prior);

    HIFB_ADP_InitAlg(enGPId);

#ifndef CONFIG_HIFB_GP1_SUPPORT
    if (HIFB_ADP_GP1 == enGPId) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
#endif

#ifndef HI_BUILD_IN_BOOT
    Ret = DRV_HIFB_ADP_SetCallbackToDisp(enGPId, (IntCallBack)DRV_HIFB_ADP_UpDataCallBack,
                                         HI_DRV_DISP_C_INTPOS_90_PERCENT, HI_TRUE);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetCallbackToDisp, Ret);
        return HI_FAILURE;
    }

    Ret = DRV_HIFB_ADP_SetCallbackToDisp(enGPId, (IntCallBack)DRV_HIFB_ADP_FrameEndCallBack,
                                         HI_DRV_DISP_C_INTPOS_100_PERCENT, HI_TRUE);

    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetCallbackToDisp, Ret);
        return HI_FAILURE;
    }
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_GP_Close
 * description  : CNcomment: 关闭GP设备 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_GP_Close(HIFB_GP_ID_E enGPId)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_pstGfxGPDevice[0], HI_FAILURE);

#ifndef HI_BUILD_IN_BOOT
    if (g_pstGfxGPDevice[enGPId]->bOpen != HI_FALSE) {
        g_pstGfxGPDevice[enGPId]->bOpen = HI_FALSE;
        DRV_HIFB_ADP_SetCallbackToDisp(enGPId, (IntCallBack)DRV_HIFB_ADP_UpDataCallBack,
                                       HI_DRV_DISP_C_INTPOS_90_PERCENT, HI_FALSE);
        DRV_HIFB_ADP_SetCallbackToDisp(enGPId, (IntCallBack)DRV_HIFB_ADP_FrameEndCallBack,
                                       HI_DRV_DISP_C_INTPOS_100_PERCENT, HI_FALSE);
    }
    if (NULL != g_pstGfxGPDevice[enGPId]->queue) {
        destroy_workqueue(g_pstGfxGPDevice[enGPId]->queue);
        g_pstGfxGPDevice[enGPId]->queue = NULL;
    }
#endif

    g_pstGfxGPDevice[enGPId]->bOpen = HI_FALSE;
    g_pstGfxGPDevice[enGPId]->bDispInitial = HI_FALSE;
    g_pstGfxGPDevice[enGPId]->bNeedExtractLine = HI_FALSE;
    g_pstGfxGPDevice[enGPId]->bMaskFlag = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetReadMode
 * description  : CNcomment: 设置读取数据模式 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetReadMode(HIFB_LAYER_ID_E enLayerId, OPTM_VDP_DATA_RMODE_E enReadMode)
{
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    drv_hifb_hal_gfx_set_read_mode(g_pstGfxDevice[enLayerId]->enGfxHalId, enReadMode);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->enGPId, HI_FAILURE);
    drv_hifb_hal_gp_set_read_mode(g_pstGfxGPDevice[g_pstGfxDevice[enLayerId]->enGPId]->enGpHalId, enReadMode);

    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_GFX_InitLayer
 * description  : CNcomment: 初始化图层信息 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_GFX_InitLayer(HIFB_LAYER_ID_E enLayerId)
{
    OPTM_VDP_BKG_S stBkg = {0};
#ifndef HI_BUILD_IN_BOOT
    hi_s32 ret = HI_SUCCESS;
    hi_char MemName[256] = {'\0'};
    HIFB_CALLBACK_TPYE_E CallBackType = 0;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_pstGfxGPIrq[0], HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_pstGfxDevice[0], HI_FAILURE);

#ifndef HI_BUILD_IN_BOOT
    /* * maybe check whether cmap if support use g_pstGfxCap[enLayerId]->bHasCmapReg* */
    if (g_pstGfxCap[enLayerId]->bHasCmapReg != HI_FALSE) {
        snprintf(MemName, sizeof(MemName), "HIFB_FB%d_Clut", enLayerId);
        MemName[sizeof(MemName) - 1] = '\0';
        ret = drv_gfx_mem_alloc_map(&g_pstGfxDevice[enLayerId]->clut_table, MemName, HI_FALSE, HI_FALSE,
                                    OPTM_CMAP_SIZE);
        if (ret != HI_SUCCESS) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, OPTM_CMAP_SIZE);
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_gfx_mem_alloc_map, FAILURE_TAG);
            return HI_FAILURE;
        }
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enLayerId);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_STR, MemName);
    }
#endif

    if (osal_wait_init(&(g_pstGfxDevice[enLayerId]->vblEvent)) != 0) {
        return HI_FAILURE;
    }

    g_pstGfxDevice[enLayerId]->stBkg = stBkg;
    g_pstGfxDevice[enLayerId]->CscState = OPTM_CSC_SET_PARA_RGB;
    g_pstGfxDevice[enLayerId]->enBitExtend = VDP_GFX_BITEXTEND_3RD;
    g_pstGfxDevice[enLayerId]->enReadMode = VDP_RMODE_PROGRESSIVE;
    g_pstGfxDevice[enLayerId]->enUpDateMode = VDP_RMODE_PROGRESSIVE;
    g_pstGfxDevice[enLayerId]->enGfxHalId = HIFB_ADP_GetGfxHalId(enLayerId);
    g_pstGfxDevice[enLayerId]->enGPId = HIFB_ADP_GetGpId(enLayerId);
    g_pstGfxDevice[enLayerId]->stAlpha.bAlphaEnable = HI_TRUE;
    g_pstGfxDevice[enLayerId]->stAlpha.bAlphaChannel = HI_TRUE;
    g_pstGfxDevice[enLayerId]->stAlpha.u8Alpha0 = 0XFF;
    g_pstGfxDevice[enLayerId]->stAlpha.u8Alpha1 = 0XFF;
    g_pstGfxDevice[enLayerId]->stAlpha.u8GlobalAlpha = 0XFF;

    g_pstGfxDevice[enLayerId]->stInRect.x = 0;
    g_pstGfxDevice[enLayerId]->stInRect.y = 0;
    g_pstGfxDevice[enLayerId]->stInRect.w = config_hifb_layer_max_width(enLayerId);
    g_pstGfxDevice[enLayerId]->stInRect.h = config_hifb_layer_max_height(enLayerId);

    g_pstGfxDevice[enLayerId]->old_fmt = DRV_HIFB_FMT_BUTT;

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enLayerId);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_pstGfxDevice[enLayerId]->enGfxHalId);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_pstGfxDevice[enLayerId]->enGPId);

    drv_hifb_hal_gfx_set_no_sec_flag(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_TRUE);
    drv_hifb_hal_gfx_set_dcmp_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_FALSE);
    drv_hifb_hal_gfx_set_layer_bkg(g_pstGfxDevice[enLayerId]->enGfxHalId, &(g_pstGfxDevice[enLayerId]->stBkg));
    drv_hifb_hal_gfx_set_bit_extend(g_pstGfxDevice[enLayerId]->enGfxHalId, g_pstGfxDevice[enLayerId]->enBitExtend);
    drv_hifb_hal_gfx_set_read_mode(g_pstGfxDevice[enLayerId]->enGfxHalId, g_pstGfxDevice[enLayerId]->enReadMode);
    drv_hifb_hal_gfx_set_upd_mode(g_pstGfxDevice[enLayerId]->enGfxHalId, g_pstGfxDevice[enLayerId]->enUpDateMode);
    drv_hifb_hal_gfx_set_pre_mult_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_TRUE, HI_FALSE);
    drv_hifb_hal_gfx_set_palpha(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_TRUE, HI_TRUE, 0XFF, 0XFF);
    drv_hifb_hal_gfx_set_layer_galpha(g_pstGfxDevice[enLayerId]->enGfxHalId, 0XFF);
    drv_hifb_hal_gfx_set_lut_addr(g_pstGfxDevice[enLayerId]->enGfxHalId,
                                  g_pstGfxDevice[enLayerId]->clut_table.phy_addr);

#ifndef HI_BUILD_IN_BOOT
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->enGPId, HI_FAILURE);
    for (CallBackType = HIFB_CALLBACK_TYPE_VO; CallBackType < HIFB_CALLBACK_TYPE_BUTT; CallBackType++) {
        ret = osal_spin_lock_init(&(
            g_pstGfxGPIrq[g_pstGfxDevice[enLayerId]->enGPId]->stGfxCallBack[enLayerId].stGfxIrq[CallBackType].FuncLock));
        if (ret != 0) {
            return HI_FAILURE;
        }
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : DRV_HIFB_ADP_ReleaseClutBuf
 * description   : CNcomment: 释放调色板内存 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void DRV_HIFB_ADP_ReleaseClutBuf(HIFB_LAYER_ID_E enLayerId)
{
#ifndef HI_BUILD_IN_BOOT
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);

    if (NULL != g_pstGfxDevice[enLayerId]->clut_table.virtual_addr) {
        drv_gfx_mem_free(&(g_pstGfxDevice[enLayerId]->clut_table));
    }
    memset(&(g_pstGfxDevice[enLayerId]->clut_table), 0, sizeof(drv_gfx_mem_info));
#else
#endif
    return;
}

static hi_void hifb_adp_hdr_disable(HIFB_GP_ID_E gp_id)
{
    hifb_hal_gp_hdr_set(g_pstGfxGPDevice[gp_id]->enGpHalId, HI_FALSE);
}

static hi_s32 hifb_adp_set_hdr_info(HIFB_GP_ID_E gp_id, hi_disp_display_info *disp_info)
{
    hi_s32 ret;
    hi_drv_pq_gfx_layer gfx_layer;
    hi_drv_pq_gfx_hdr_info gfx_hdr_info = {0};

    if ((gs_pstPqFuncs == NULL) || (gs_pstPqFuncs->pq_set_gfx_hdr_cfg == NULL)) {
        return HI_FAILURE;
    }

    if (gp_id == HIFB_ADP_GP0) {
        gfx_layer = HI_DRV_PQ_GFX_LAYER_GP0;
    } else {
        gfx_layer = HI_DRV_PQ_GFX_LAYER_GP1;
    }

    gfx_hdr_info.src_hdr_type = HI_DRV_HDR_TYPE_SDR;
    gfx_hdr_info.color_space_in.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    gfx_hdr_info.color_space_in.color_space = HI_DRV_COLOR_CS_RGB;
    gfx_hdr_info.color_space_in.quantify_range = HI_DRV_COLOR_FULL_RANGE;

    if (disp_info->disp_type == HI_DRV_DISP_TYPE_HDR10) {
        gfx_hdr_info.disp_hdr_type = HI_DRV_HDR_TYPE_HDR10;
    } else if (disp_info->disp_type == HI_DRV_DISP_TYPE_HLG) {
        gfx_hdr_info.disp_hdr_type = HI_DRV_HDR_TYPE_HLG;
    }
    memcpy(&gfx_hdr_info.color_space_out, &disp_info->color_space, sizeof(hi_drv_color_descript));

    ret = gs_pstPqFuncs->pq_set_gfx_hdr_cfg(gfx_layer, &gfx_hdr_info);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void hifb_adp_set_gp_csc_enable(HIFB_GP_ID_E gp, hi_bool enable)
{
    hifb_hal_set_gp_csc_im_en(g_pstGfxGPDevice[gp]->enGpHalId, enable == HI_TRUE ? 1 : 0);
    hifb_hal_set_gp_csc_im_yuv2rgb_en(g_pstGfxGPDevice[gp]->enGpHalId, enable == HI_TRUE ? 1 : 0);
}

/***************************************************************************************
 * func        : HIFB_ADP_SetGpCsc
 * description : CNcomment: 设置GP CSC系数 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_void hifb_adp_set_gp_csc_coef(HIFB_GP_ID_E gp, hi_drv_pq_gfx_csc_coef *pq_csc)
{
    hi_u32 clip_max;
    hi_u32 scale;
    hi_u32 thr_r = 0;
    hi_u32 thr_b = 0;
    hifb_hal_set_gp_csc_coef00(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef00 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef01(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef01 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef02(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef02 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef10(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef10 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef11(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef11 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef12(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef12 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef20(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef20 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef21(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef21 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_coef22(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_ac_coef.csc_coef22 << 5); /* alg num: 5 */
    hifb_hal_set_gp_csc_im_yuv2rgb_mode(g_pstGfxGPDevice[gp]->enGpHalId, 3); /* rgb2yuv mode is 3 */
    hifb_hal_set_gp_csc_thr_r(g_pstGfxGPDevice[gp]->enGpHalId, thr_r);
    hifb_hal_set_gp_csc_thr_b(g_pstGfxGPDevice[gp]->enGpHalId, thr_b);

    if (gp == HIFB_ADP_GP0) {
        scale = 13;                /* precision is 13 */
        clip_max = 65535 << scale; /* algorithm num: 65535 */
        hifb_hal_set_gp_csc_scale2p(g_pstGfxGPDevice[gp]->enGpHalId, scale);
        hifb_hal_set_gp_csc_dc_in0(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_in_dc0 << 19); /* alg num:
                                                                                                              19 */
        hifb_hal_set_gp_csc_dc_in1(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_in_dc1 << 19); /* alg num:
                                                                                                              19 */
        hifb_hal_set_gp_csc_dc_in2(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_in_dc2 << 19); /* alg num:
                                                                                                              19 */
        /* alg num: -19 */
        hifb_hal_set_gp_csc_dc_out0(g_pstGfxGPDevice[gp]->enGpHalId, (pq_csc->gfx_dc_coef.csc_out_dc0 << 19) * (-1));
        /* alg num: -19 */
        hifb_hal_set_gp_csc_dc_out1(g_pstGfxGPDevice[gp]->enGpHalId, (pq_csc->gfx_dc_coef.csc_out_dc1 << 19) * (-1));
        /* alg num: -19 */
        hifb_hal_set_gp_csc_dc_out2(g_pstGfxGPDevice[gp]->enGpHalId, (pq_csc->gfx_dc_coef.csc_out_dc2 << 19) * (-1));
        hifb_hal_set_gp_csc_clip_min(g_pstGfxGPDevice[gp]->enGpHalId, 0);
        hifb_hal_set_gp_csc_clip_max(g_pstGfxGPDevice[gp]->enGpHalId, clip_max);
        hifb_hal_set_gp_csc_thr_r(g_pstGfxGPDevice[gp]->enGpHalId, thr_r);
        hifb_hal_set_gp_csc_thr_b(g_pstGfxGPDevice[gp]->enGpHalId, thr_b);
    } else {
        scale = 15; /* precision is 15 */
        hifb_hal_set_gp1_csc_scale2p(g_pstGfxGPDevice[gp]->enGpHalId, scale);
        hifb_hal_set_gp_csc_dc_in0(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_in_dc0 << 2); /* alg num: 2 */
        hifb_hal_set_gp_csc_dc_in1(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_in_dc1 << 2); /* alg num: 2 */
        hifb_hal_set_gp_csc_dc_in2(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_in_dc2 << 2); /* alg num: 2 */
        hifb_hal_set_gp1_csc_dc_out0(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_out_dc0);
        hifb_hal_set_gp1_csc_dc_out1(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_out_dc1);
        hifb_hal_set_gp1_csc_dc_out2(g_pstGfxGPDevice[gp]->enGpHalId, pq_csc->gfx_dc_coef.csc_out_dc2);
        hifb_hal_set_gp1_csc_clip_max_y(g_pstGfxGPDevice[gp]->enGpHalId, 0x3ff);
        hifb_hal_set_gp1_csc_clip_max_c(g_pstGfxGPDevice[gp]->enGpHalId, 0x3ff);
    }
    hifb_adp_set_gp_csc_enable(gp, HI_TRUE);
}

static hi_s32 HIFB_ADP_SetGpCsc(HIFB_GP_ID_E enGfxGpId, hi_bool is_bgr_in)
{
    hi_drv_pq_gfx_csc_info gfx_csc_info;
    hi_drv_pq_gfx_csc_coef gfx_csc_coef = {{0}};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    memset(&gfx_csc_info, 0x0, sizeof(gfx_csc_info));

    OPTM_CheckGPMask_BYGPID(enGfxGpId);

    g_pstGfxGPDevice[enGfxGpId]->in_csc.color_primary = HI_DRV_COLOR_PRIMARY_BT709;
    g_pstGfxGPDevice[enGfxGpId]->in_csc.color_space = HI_DRV_COLOR_CS_RGB;
    g_pstGfxGPDevice[enGfxGpId]->in_csc.quantify_range = HI_DRV_COLOR_FULL_RANGE;

    gfx_csc_info.is_bgr_in = is_bgr_in;
    memcpy(&gfx_csc_info.gfx_csc_mode.csc_in, &g_pstGfxGPDevice[enGfxGpId]->in_csc, sizeof(hi_drv_color_descript));
    memcpy(&gfx_csc_info.gfx_csc_mode.csc_out, &g_pstGfxGPDevice[enGfxGpId]->out_csc, sizeof(hi_drv_color_descript));
    gfx_csc_info.gfx_csc_mode.csc_type = HI_PQ_CSC_NORMAL_SETTING;

#ifndef HI_BUILD_IN_BOOT

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(gs_pstPqFuncs, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(gs_pstPqFuncs->pq_get_gfx_csc_coef, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enGfxGpId);
    if (enGfxGpId == HIFB_ADP_GP0) {
        gs_pstPqFuncs->pq_get_gfx_csc_coef(HI_DRV_PQ_GFX_LAYER_GP0, &gfx_csc_info, &gfx_csc_coef);
    } else {
        gs_pstPqFuncs->pq_get_gfx_csc_coef(HI_DRV_PQ_GFX_LAYER_GP1, &gfx_csc_info, &gfx_csc_coef);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enGfxGpId);
    }

    if ((g_pstGfxGPDevice[enGfxGpId]->in_csc.color_primary != g_pstGfxGPDevice[enGfxGpId]->out_csc.color_primary) ||
        (g_pstGfxGPDevice[enGfxGpId]->in_csc.color_space != g_pstGfxGPDevice[enGfxGpId]->out_csc.color_space) ||
        (g_pstGfxGPDevice[enGfxGpId]->in_csc.quantify_range != g_pstGfxGPDevice[enGfxGpId]->out_csc.quantify_range)) {
        hifb_adp_set_gp_csc_coef(enGfxGpId, &gfx_csc_coef);
    } else {
        hifb_adp_set_gp_csc_enable(enGfxGpId, HI_FALSE);
    }
#else
    if (enGfxGpId == HIFB_ADP_GP0) {
        DRV_PQ_SetGfxCscCoef(PQ_GFX_GP0, &stGfxCscPara);
    } else {
        DRV_PQ_SetGfxCscCoef(PQ_GFX_GP1, &stGfxCscPara);
    }
#endif

    drv_hifb_hal_gp_set_reg_up(g_pstGfxGPDevice[enGfxGpId]->enGpHalId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
* func         : DRV_HIFB_ADP_SetCallback
* description  : CNcomment: set call back function
                             设置中断响应函数,在对应的中断中调用该对应的赋值函数 CNend\n
* param[in]    : hi_void
* retval       : NA
* others:      : NA
***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_SetCallback(HIFB_CALLBACK_TPYE_E enCType, IntCallBack pCallBack, HIFB_LAYER_ID_E enLayerId)
{
    hi_ulong LockFlag = 0;
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (enCType >= HIFB_CALLBACK_TYPE_BUTT) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HIFB_CALLBACK_TYPE_BUTT);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enCType);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGPId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    if (NULL != pCallBack) {
        gs_CallBackLayerId[enLayerId] = enLayerId;
        g_pstGfxGPIrq[enGPId]->stGfxCallBack[enLayerId].stGfxIrq[enCType].pFunc = pCallBack;
        g_pstGfxGPIrq[enGPId]->stGfxCallBack[enLayerId].stGfxIrq[enCType].param0 =
            (hi_void *)&gs_CallBackLayerId[enLayerId];
        g_pstGfxGPIrq[enGPId]->stGfxCallBack[enLayerId].u32CTypeFlag |= (hi_u32)enCType;
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    osal_spin_lock_irqsave(&(g_pstGfxGPIrq[enGPId]->stGfxCallBack[enLayerId].stGfxIrq[enCType].FuncLock), &LockFlag);
    {
        g_pstGfxGPIrq[enGPId]->stGfxCallBack[enLayerId].u32CTypeFlag &= ~((hi_u32)enCType);
        g_pstGfxGPIrq[enGPId]->stGfxCallBack[enLayerId].stGfxIrq[enCType].pFunc = NULL;
    }
    osal_spin_unlock_irqrestore(&(g_pstGfxGPIrq[enGPId]->stGfxCallBack[enLayerId].stGfxIrq[enCType].FuncLock),
                                &LockFlag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

#if !defined(HI_BUILD_IN_BOOT) && defined(CONFIG_HIFB_STEREO_SUPPORT)
/***************************************************************************************
* func         : HIFB_ADP_CheckGfxCallbackReg
* description  : CNcomment: check whether call back function have register
                       判断相应的回调函数是否被注册 CNend\n
* param[in]    : hi_void
* retval       : NA
* others:      : NA
***************************************************************************************/
static hi_s32 HIFB_ADP_CheckGfxCallbackReg(HIFB_GP_ID_E enGPId, HIFB_CALLBACK_TPYE_E eCallbackType)
{
    hi_u32 LayerId = 0;

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(eCallbackType, HIFB_CALLBACK_TYPE_BUTT, HI_FAILURE);

    for (LayerId = 0; LayerId < OPTM_GP_SUPPORT_MAXLAYERNUM; LayerId++) {
        if (g_pstGfxGPIrq[enGPId]->stGfxCallBack[LayerId].u32CTypeFlag & (hi_u32)eCallbackType) {
            if (g_pstGfxGPIrq[enGPId]->stGfxCallBack[LayerId].stGfxIrq[eCallbackType].pFunc) {
                return HI_SUCCESS;
            }
        }
    }

    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerId);
    return HI_FAILURE;
}
#endif

/***************************************************************************************
 * func         : HIFB_ADP_GetScreenRectFromDispInfo
 * description  : CNcomment: 从disp中获取screen rect CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 HIFB_ADP_GetScreenRectFromDispInfo(const hi_rect *tmp_virtscreen, const OPTM_GFX_OFFSET_S *stOffsetInfo,
                                                 const hi_rect *fmt_resolution, const hi_rect *pixel_fmt_resolution,
                                                 hi_drv_rect *stScreenRect)
{
    OPTM_GFX_OFFSET_S tmp_offsetInfo;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(tmp_virtscreen, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(stOffsetInfo, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fmt_resolution, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pixel_fmt_resolution, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(stScreenRect, HI_FAILURE);

    memcpy(&tmp_offsetInfo, stOffsetInfo, sizeof(tmp_offsetInfo));

    if ((tmp_virtscreen->width == 0) || (tmp_virtscreen->height == 0)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, tmp_virtscreen->width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, tmp_virtscreen->height);
        return HI_FAILURE;
    }

    if ((pixel_fmt_resolution->width == 0) || (pixel_fmt_resolution->width > GRAPHIC_DISPLAY_MAX_WIDTH)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pixel_fmt_resolution->width);
        return HI_FAILURE;
    }

    if ((pixel_fmt_resolution->height == 0) || (pixel_fmt_resolution->height > GRAPHIC_DISPLAY_MAX_HIGHT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pixel_fmt_resolution->height);
        return HI_FAILURE;
    }

    if ((fmt_resolution->width * 2) == pixel_fmt_resolution->width) {
        tmp_offsetInfo.u32Left *= 2;
        tmp_offsetInfo.u32Right *= 2;
    }

    if ((tmp_offsetInfo.u32Left > CONFIG_HIFB_LAYER_MAXWIDTH) ||
        (tmp_offsetInfo.u32Top > CONFIG_HIFB_LAYER_MAXHEIGHT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Left);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Top);
        return HI_FAILURE;
    }

    if ((tmp_offsetInfo.u32Right > CONFIG_HIFB_LAYER_MAXWIDTH) ||
        (tmp_offsetInfo.u32Bottom > CONFIG_HIFB_LAYER_MAXHEIGHT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Right);
        // GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Bottom);
        return HI_FAILURE;
    }

    if ((pixel_fmt_resolution->width <= (tmp_offsetInfo.u32Left + tmp_offsetInfo.u32Right)) ||
        (pixel_fmt_resolution->height <= (tmp_offsetInfo.u32Top + tmp_offsetInfo.u32Bottom))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pixel_fmt_resolution->width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pixel_fmt_resolution->height);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Right);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Bottom);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Left);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, tmp_offsetInfo.u32Top);
        return HI_FAILURE;
    }

    stScreenRect->rect_x = tmp_offsetInfo.u32Left;
    stScreenRect->rect_y = tmp_offsetInfo.u32Top;
    stScreenRect->rect_w = (pixel_fmt_resolution->width - tmp_offsetInfo.u32Left - tmp_offsetInfo.u32Right);
    stScreenRect->rect_h = (pixel_fmt_resolution->height - tmp_offsetInfo.u32Top - tmp_offsetInfo.u32Bottom);
    stScreenRect->rect_x = HIFB_ADP_Align(stScreenRect->rect_x, 2);
    stScreenRect->rect_y = HIFB_ADP_Align(stScreenRect->rect_y, 2);
    stScreenRect->rect_w = HIFB_ADP_Align(stScreenRect->rect_w, 2);
    stScreenRect->rect_h = HIFB_ADP_Align(stScreenRect->rect_h, 2);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
static hi_void DRV_HIFB_ADP_FrameEndCallBack(hi_void *u32Param0, hi_void *u32Param1)
{
    hi_bool bIsBottomField = 0;
    hi_u32 u32CTypeFlag = 0;
    hi_void *pLayerId = NULL;
#ifndef CONFIG_HIFB_VERSION_1_0
    hi_u32 WorkedLayerDdr = 0;
    hi_u32 WillWorkLayerDdr = 0;
#endif
    HIFB_LAYER_ID_E LayerIdIndex = HIFB_LAYER_ID_BUTT;
    HIFB_GP_ID_E *pEnGpId = NULL;
    hi_drv_disp_callback_info *pstDispInfo = NULL;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(u32Param0);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(u32Param1);
    pstDispInfo = (hi_drv_disp_callback_info *)u32Param1;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDispInfo);
    pEnGpId = (HIFB_GP_ID_E *)u32Param0;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pEnGpId);

    if (g_pstGfxGPDevice[*pEnGpId]->bOpen != HI_TRUE) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, *pEnGpId);
        return;
    }

    drv_hifb_hal_disp_get_dhd1_field(&bIsBottomField);
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    DRV_HIFB_WBC_FrameEndProcess(*pEnGpId, pstDispInfo->disp_info.interlace,
                                 bIsBottomField);  // pstDispInfo->disp_info.bIsBottomField);
#endif

    for (LayerIdIndex = HIFB_LAYER_HD_0; LayerIdIndex <= HIFB_LAYER_HD_3; LayerIdIndex++) {
        OPTM_GfxWVBCallBack(LayerIdIndex);

        pLayerId = g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[LayerIdIndex].stGfxIrq[HIFB_CALLBACK_TYPE_FRAME_END].param0;
        if (NULL == pLayerId) {
            continue;
        }

#ifndef CONFIG_HIFB_VERSION_1_0
        drv_hifb_hal_gfx_get_work_layer_addr(g_pstGfxDevice[LayerIdIndex]->enGfxHalId, &WorkedLayerDdr);
        drv_hifb_hal_gfx_get_will_work_layer_addr(g_pstGfxDevice[LayerIdIndex]->enGfxHalId, &WillWorkLayerDdr);
        if (WorkedLayerDdr != WillWorkLayerDdr) {
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, WorkedLayerDdr);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, WillWorkLayerDdr);
            return;
        }
#endif
        u32CTypeFlag = g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[LayerIdIndex].u32CTypeFlag;
        /* *TODO: here spin lock should test by peach-driver * */
        // spin_lock_irqsave(&(g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[LayerIdIndex].stGfxIrq[HIFB_CALLBACK_TYPE_FRAME_END].FuncLock),LockFlag);
        if ((u32CTypeFlag & HIFB_CALLBACK_TYPE_FRAME_END) &&
            (NULL !=
             g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[LayerIdIndex].stGfxIrq[HIFB_CALLBACK_TYPE_FRAME_END].pFunc)) {
            g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[LayerIdIndex].stGfxIrq[HIFB_CALLBACK_TYPE_FRAME_END].pFunc(pLayerId,
                                                                                                              NULL);
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        }
        // spin_unlock_irqrestore(&(g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[LayerIdIndex].stGfxIrq[HIFB_CALLBACK_TYPE_FRAME_END].FuncLock),LockFlag);
    }

    return;
}

/***************************************************************************************
 * func        : HIFB_ADP_DispInfoProcess
 * description : CNcomment: display信息处理 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_bool HIFB_ADP_DispInfoProcess(HIFB_GP_ID_E eGpId, hi_disp_display_info *pstDispInfo)
{
    hi_bool bDispInfoChange = HI_FALSE;
    hi_drv_rect stOutRect;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(eGpId, HI_FALSE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDispInfo, HI_FALSE);

    if ((g_pstGfxGPDevice[eGpId]->stInRect.rect_w != pstDispInfo->virtaul_screen.width) ||
        (g_pstGfxGPDevice[eGpId]->stInRect.rect_h != pstDispInfo->virtaul_screen.height) ||
        (g_pstGfxGPDevice[eGpId]->bInterface != pstDispInfo->interlace)) {
        bDispInfoChange = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stInRect.rect_w);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stInRect.rect_h);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstDispInfo->virtaul_screen.width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstDispInfo->virtaul_screen.height);

    g_pstGfxGPDevice[eGpId]->bInterface = pstDispInfo->interlace;
    g_pstGfxGPDevice[eGpId]->stInRect.rect_x = pstDispInfo->virtaul_screen.x;
    g_pstGfxGPDevice[eGpId]->stInRect.rect_y = pstDispInfo->virtaul_screen.y;
    g_pstGfxGPDevice[eGpId]->stInRect.rect_w = pstDispInfo->virtaul_screen.width;
    g_pstGfxGPDevice[eGpId]->stInRect.rect_h = pstDispInfo->virtaul_screen.height;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stInRect.rect_x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stInRect.rect_y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stInRect.rect_w);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stInRect.rect_h);

    HIFB_ADP_GetScreenRectFromDispInfo(&pstDispInfo->virtaul_screen, (OPTM_GFX_OFFSET_S *)&pstDispInfo->offset_info,
                                       &pstDispInfo->fmt_resolution, &pstDispInfo->pixel_fmt_resolution, &stOutRect);

    if ((g_pstGfxGPDevice[eGpId]->stOutRect.rect_w != stOutRect.rect_w) ||
        (g_pstGfxGPDevice[eGpId]->stOutRect.rect_h != stOutRect.rect_h) ||
        (g_pstGfxGPDevice[eGpId]->stOutRect.rect_x != stOutRect.rect_x) ||
        (g_pstGfxGPDevice[eGpId]->stOutRect.rect_y != stOutRect.rect_y)) {
        bDispInfoChange = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stOutRect.rect_w);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stOutRect.rect_h);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stOutRect.rect_x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[eGpId]->stOutRect.rect_y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_w);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_h);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_y);

    memcpy(&g_pstGfxGPDevice[eGpId]->stOutRect, &stOutRect, sizeof(stOutRect));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return bDispInfoChange;
}

/***************************************************************************************
 * func          : HIFB_ADP_GP_SetMask
 * description   : CNcomment: 在90%中断处理中调用,设置GP掩码 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 HIFB_ADP_GP_SetMask(HIFB_GP_ID_E enGPId, hi_bool bFlag)
{
    hi_bool bEnable = HI_FALSE;
    hi_u32 LayerIndex = 0;
    hi_u32 u32LayerCount = 0;
    HIFB_LAYER_ID_E enLayerId = HIFB_LAYER_HD_0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);
    if (HI_TRUE != g_pstGfxGPDevice[enGPId]->bOpen) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enGPId);
        return HI_SUCCESS;
    }

    g_pstGfxGPDevice[enGPId]->bMaskFlag = bFlag;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, bFlag);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP0 == enGPId)) {
        g_pstGfxGPDevice[HIFB_ADP_GP1]->bMaskFlag = bFlag;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP1 == enGPId)) {
        bEnable = (HI_TRUE == bFlag) ? (HI_FALSE) : (g_pstGfxDevice[HIFB_LAYER_SD_0]->bEnable);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, bEnable);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, bFlag);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[HIFB_LAYER_SD_0]->bEnable);
        drv_hifb_hal_gfx_set_layer_enable(g_pstGfxDevice[HIFB_LAYER_SD_0]->enGfxHalId, bEnable);
        drv_hifb_hal_gfx_set_reg_up(g_pstGfxDevice[HIFB_LAYER_SD_0]->enGfxHalId);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
#endif

    enLayerId = (HIFB_ADP_GP0 == enGPId) ? (HIFB_LAYER_HD_0) : (HIFB_LAYER_SD_0);
    u32LayerCount = (HIFB_ADP_GP0 == enGPId) ? (OPTM_GP0_GFX_COUNT) : (OPTM_GP1_GFX_COUNT);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enLayerId);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, u32LayerCount);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enGPId);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE((enLayerId + u32LayerCount), HI_FAILURE);

    for (LayerIndex = 0; LayerIndex < u32LayerCount; LayerIndex++) {
        if (HI_FALSE == g_pstGfxDevice[enLayerId + LayerIndex]->bOpened) {
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, LayerIndex);
            continue;
        }
        g_pstGfxDevice[enLayerId + LayerIndex]->old_fmt = DRV_HIFB_FMT_BUTT;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, LayerIndex);

        bEnable = (HI_TRUE == bFlag) ? (HI_FALSE) : (g_pstGfxDevice[enLayerId + LayerIndex]->bEnable);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, bEnable);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, bFlag);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[enLayerId + LayerIndex]->bEnable);

        drv_hifb_hal_gfx_set_layer_enable(g_pstGfxDevice[enLayerId + LayerIndex]->enGfxHalId, bEnable);
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
        g_pstGfxDevice[HIFB_LAYER_SD_0]->bEnable = bEnable;
#endif
    }

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP0 == enGPId)) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[HIFB_LAYER_SD_0]->bEnable);
        g_pstGfxDevice[HIFB_LAYER_SD_0]->bEnable = bFlag ? (HI_FALSE) : (g_pstGfxDevice[HIFB_LAYER_SD_0]->bEnable);
        g_pstGfxGPDevice[HIFB_ADP_GP1]->unUpFlag.bits.IsEnable = 1;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, bFlag);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[HIFB_LAYER_SD_0]->bEnable);
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_void DRV_HIFB_ADP_SetLowPowerInfo(HIFB_LAYER_ID_E enLayerId, HIFB_ADP_LOWPOWER_INFO_S *pstLowPowerInfo)
{
#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    if (NULL != pstLowPowerInfo) {
        drv_hifb_hal_gfx_enable_layer_low_power(g_pstGfxDevice[enLayerId]->enGfxHalId, pstLowPowerInfo->LowPowerEn);
        drv_hifb_hal_gfx_enable_gp_low_power(HIFB_ADP_GetGpId(enLayerId), pstLowPowerInfo->LowPowerEn);
        drv_hifb_hal_gfx_set_low_power_info(g_pstGfxDevice[enLayerId]->enGfxHalId, pstLowPowerInfo->LpInfo);
    }
#else
#endif
}
#endif

#ifdef CONFIG_HIFB_DECOMPRESS_SEGMENT
STATIC hi_void drv_hifb_adp_set_decmp_info(HIFB_LAYER_ID_E layer_id, hifb_decmp_info *dec_info)
{
#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT)
    ice_v3r2_line_osd_input input = {0};
    ice_v3r2_line_osd_cfg cfg = {0};

    if ((dec_info == HI_NULL) || (layer_id >= HIFB_LAYER_ID_BUTT)) {
        return;
    }

    if (dec_info->fmt != g_pstGfxDevice[layer_id]->old_fmt) {
        g_pstGfxDevice[layer_id]->old_fmt = dec_info->fmt;
        input.frame_width = 512; /* 512 for SEG decompress width */
        input.frame_height = 1;
        input.pix_format = 1;
        input.bit_depth = (dec_info->fmt == DRV_HIFB_FMT_ARGB10101010 || dec_info->fmt == DRV_HIFB_FMT_ARGB2101010) ? 1
                                                                                                                    : 0;
        input.conv_en = 0;
        input.cmp_mode = 0;
        input.is_lossless = 0;
        input.comp_ratio_int = 2000; /* 2000 for compress ratio */
        input.rc_type_mode = 0;

        ice_v3r2_line_osd_cfg_init(&cfg, &input);

        drv_hifb_hal_set_max_mbqp(layer_id, cfg.max_mb_qp);
        drv_hifb_hal_set_conv_en(layer_id, cfg.conv_en);
        drv_hifb_hal_set_pixel_fmt_od(layer_id, cfg.pix_format);
        drv_hifb_hal_set_is_lossless(layer_id, cfg.is_lossless);
        drv_hifb_hal_set_cmp_mode(layer_id, cfg.cmp_mode);
        drv_hifb_hal_set_bit_depth(layer_id, cfg.bit_depth);
    }
    drv_hifb_hal_set_typ(layer_id, 0);
    drv_hifb_hal_set_dcmp_type(layer_id, 1);
    drv_hifb_hal_set_ice_en(layer_id, 1);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(layer_id);
    g_pstGfxDevice[layer_id]->ARHeadDdr = dec_info->ar_header;
    g_pstGfxDevice[layer_id]->ARDataDdr = dec_info->ar_data;
    g_pstGfxDevice[layer_id]->CmpStride = (hi_u16)dec_info->stride;
    drv_hifb_hal_gfx_set_de_cmp_ddr_info(g_pstGfxDevice[layer_id]->enGfxHalId, dec_info->ar_header, dec_info->ar_data,
                                         0, 0, dec_info->stride);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[layer_id]->ARHeadDdr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[layer_id]->ARDataDdr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[layer_id]->CmpStride);

#else
#endif
    return;
}
#else
STATIC hi_void drv_hifb_adp_set_decmp_info(HIFB_LAYER_ID_E layer_id, hifb_decmp_info *dec_info)
{
#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT) && !defined(CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE)
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);

    if (dec_info == HI_NULL) {
        return;
    }
    g_pstGfxDevice[enLayerId]->ARHeadDdr = dec_info->ar_header;
    g_pstGfxDevice[enLayerId]->ARDataDdr = dec_info->ar_data;
    g_pstGfxDevice[enLayerId]->GBHeadDdr = dec_info->gb_header;
    g_pstGfxDevice[enLayerId]->GBDataDdr = dec_info->gb_data;
    g_pstGfxDevice[enLayerId]->CmpStride = (hi_u16)dec_info->stride;
    drv_hifb_hal_gfx_set_de_cmp_ddr_info(g_pstGfxDevice[enLayerId]->enGfxHalId, dec_info->ar_header, dec_info->ar_data,
                                         dec_info->gb_header, dec_info->gb_data, dec_info->stride);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[enLayerId]->ARHeadDdr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[enLayerId]->ARDataDdr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[enLayerId]->GBHeadDdr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[enLayerId]->GBDataDdr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[enLayerId]->CmpStride);
#else
#endif
    return;
}
#endif

STATIC hi_void DRV_HIFB_ADP_GetDecompressStatus(HIFB_LAYER_ID_E enLayerId, hi_bool *pbARDataDecompressErr,
                                                hi_bool *pbGBDataDecompressErr, hi_bool bCloseInterrupt)
{
#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT) && !defined(CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE)
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    drv_hifb_hal_gfx_get_de_cmp_status(g_pstGfxDevice[enLayerId]->enGfxHalId, pbARDataDecompressErr,
                                       pbGBDataDecompressErr, bCloseInterrupt);
#else
#endif
    return;
}

#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT)
STATIC hi_void OPTM_GFX_DECMP_Open(HIFB_LAYER_ID_E enLayerId)
{
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);

    g_pstGfxDevice[enLayerId]->bDmpOpened = HI_TRUE;
    drv_hifb_hal_gfx_set_dcmp_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_TRUE);

    return;
}

STATIC hi_void OPTM_GFX_DECMP_Close(HIFB_LAYER_ID_E enLayerId)
{
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);

    g_pstGfxDevice[enLayerId]->bDmpOpened = HI_FALSE;
    drv_hifb_hal_gfx_set_dcmp_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_FALSE);

    return;
}
#endif

#ifndef HI_BUILD_IN_BOOT
hi_void DRV_HIFB_ADP_UpDataCallBack(hi_void *u32Param0, hi_void *u32Param1)
{
    HIFB_GP_ID_E *pEnGpId = NULL;
    hi_drv_disp_callback_info *pstDispInfo = NULL;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(u32Param0);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(u32Param1);
    pstDispInfo = (hi_drv_disp_callback_info *)u32Param1;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDispInfo);
    pEnGpId = (HIFB_GP_ID_E *)u32Param0;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(*pEnGpId);

    if (HI_TRUE != g_pstGfxGPDevice[*pEnGpId]->bOpen) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, *pEnGpId);
        return;
    }

#ifdef CHIP_TYPE_hi3798cv200
    { /* * support hdmi cts * */
        g_pstGfxGPDevice[*pEnGpId]->enDispFmt = pstDispInfo->disp_info.eFmt;
    }
#endif
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HIFB_GFX_MODE_NORMAL == g_enOptmGfxWorkMode) && (HI_TRUE == pstDispInfo->disp_info.bIsMaster) &&
        (HIFB_ADP_GP0 == *pEnGpId) && (NULL != g_pstGfxGPDevice[*pEnGpId]->queue)) {
        g_enOptmGfxWorkMode = HIFB_GFX_MODE_HD_WBC;
        g_pstGfxGPDevice[*pEnGpId]->bRecoveryInNextVT = HI_TRUE;
        g_pstGfxGPDevice[*pEnGpId]->stStartWbcWork.bOpenLayer = HI_TRUE;
        g_pstGfxGPDevice[*pEnGpId]->stStartWbcWork.u32Data = HIFB_LAYER_SD_0;
        queue_work(g_pstGfxGPDevice[*pEnGpId]->queue, &g_pstGfxGPDevice[*pEnGpId]->stStartWbcWork.work);
    }
#endif

    if (pstDispInfo->disp_info.enable == HI_FALSE) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call HIFB_ADP_ReClose");
        HIFB_ADP_ReClose(*pEnGpId, &pstDispInfo->disp_info);
#ifndef CONFIG_HIFB_GP1_SUPPORT
        HIFB_ADP_ReClose(HIFB_ADP_GP1, &pstDispInfo->disp_info);
#endif
        return;
    }

    if (pstDispInfo->disp_info.enable == HI_TRUE && g_pstGfxGPDevice[*pEnGpId]->bGpClose == HI_TRUE) {
        g_pstGfxGPDevice[*pEnGpId]->bHasBeenReOpen[0] = HI_TRUE;
        g_pstGfxGPDevice[*pEnGpId]->bHasBeenReOpen[1] = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call HIFB_ADP_ReOpen");
        HIFB_ADP_ReOpen(*pEnGpId, &pstDispInfo->disp_info);
#ifndef CONFIG_HIFB_GP1_SUPPORT
        HIFB_ADP_ReOpen(HIFB_ADP_GP1, &pstDispInfo->disp_info);
#endif
        return;
    }

    {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call HIFB_ADP_DispSetting");
        HIFB_ADP_DispSetting(*pEnGpId, &pstDispInfo->disp_info);
#ifndef CONFIG_HIFB_GP1_SUPPORT
        HIFB_ADP_DispSetting(HIFB_ADP_GP1, &pstDispInfo->disp_info);
#endif
    }

    if (HI_FALSE == g_pstGfxGPDevice[*pEnGpId]->bGpClose) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call HIFB_ADP_UpdataLayerInfo");
        HIFB_ADP_UpdataLayerInfo(*pEnGpId, &pstDispInfo->disp_info);
        HIFB_ADP_DistributeCallback(u32Param0, u32Param1);
#ifndef CONFIG_HIFB_GP1_SUPPORT
        HIFB_ADP_UpdataLayerInfo(HIFB_ADP_GP1, &pstDispInfo->disp_info);
#endif
    }

    return;
}

static inline hi_void HIFB_ADP_ReOpen(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDispInfo);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    if (HI_FALSE == g_pstGfxGPDevice[enGpId]->bDispInitial) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enGpId);
        return;
    }

    if (HI_TRUE == g_pstGfxGPDevice[enGpId]->bGpClose) {
        g_pstGfxGPDevice[enGpId]->bGpClose = HI_FALSE;
        g_pstGfxGPDevice[enGpId]->bRecoveryInNextVT = HI_TRUE;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enGpId);
    }

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    DRV_HIFB_WBC_Reset(HI_FALSE);
#endif

    HIFB_ADP_GP_SetMask(enGpId, HI_FALSE);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void HIFB_ADP_ReClose(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDispInfo);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    if (HI_FALSE == g_pstGfxGPDevice[enGpId]->bDispInitial) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enGpId);
        return;
    }

    g_pstGfxGPDevice[enGpId]->bGpClose = HI_TRUE;
    HIFB_ADP_GP_SetMask(enGpId, HI_TRUE);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void HIFB_ADP_DispSetting(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo)
{
    hi_bool bDispInfoChange = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDispInfo);

    bDispInfoChange = HIFB_ADP_DispInfoProcess(enGpId, pstDispInfo);
    if (HI_FALSE == bDispInfoChange) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    HIFB_ADP_DispInfoUpdate(enGpId);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP0 == enGpId)) {
        DRV_HIFB_WBC_Reset(HI_TRUE);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void HIFB_ADP_UpdataLayerInfo(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDispInfo);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    if (HI_TRUE == g_pstGfxGPDevice[enGpId]->bRecoveryInNextVT) {
        g_pstGfxGPDevice[enGpId]->bInterface = pstDispInfo->interlace;
        g_pstGfxGPDevice[enGpId]->stInRect.rect_x = pstDispInfo->virtaul_screen.x;
        g_pstGfxGPDevice[enGpId]->stInRect.rect_y = pstDispInfo->virtaul_screen.y;
        g_pstGfxGPDevice[enGpId]->stInRect.rect_w = pstDispInfo->virtaul_screen.width;
        g_pstGfxGPDevice[enGpId]->stInRect.rect_h = pstDispInfo->virtaul_screen.height;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enGpId);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->bInterface);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_w);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_h);

        HIFB_ADP_GetScreenRectFromDispInfo(&pstDispInfo->virtaul_screen, (OPTM_GFX_OFFSET_S *)&pstDispInfo->offset_info,
                                           &pstDispInfo->fmt_resolution, &pstDispInfo->pixel_fmt_resolution,
                                           &g_pstGfxGPDevice[enGpId]->stOutRect);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stOutRect.rect_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stOutRect.rect_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stOutRect.rect_w);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stOutRect.rect_h);

        HIFB_ADP_GP_Recovery(enGpId);

        g_pstGfxGPDevice[enGpId]->bRecoveryInNextVT = HI_FALSE;
        g_pstGfxGPDevice[enGpId]->bDispInitial = HI_TRUE;
    }

    memcpy(&g_pstGfxGPDevice[enGpId]->out_csc, &pstDispInfo->color_space, sizeof(hi_drv_color_descript));

    g_pstGfxGPDevice[enGpId]->bHdr = ((pstDispInfo->disp_type == HI_DRV_DISP_TYPE_HDR10) ||
        (pstDispInfo->disp_type == HI_DRV_DISP_TYPE_HLG)) ? HI_TRUE : HI_FALSE;
    if (enGpId == HIFB_ADP_GP0) {
        if (g_pstGfxGPDevice[enGpId]->bHdr == HI_TRUE) {
            hifb_adp_set_hdr_info(enGpId, pstDispInfo);
        } else {
            hifb_adp_hdr_disable(enGpId);
            HIFB_ADP_SetGpCsc(enGpId, g_pstGfxGPDevice[enGpId]->bBGRState);
        }
    } else {
        HIFB_ADP_SetGpCsc(enGpId, g_pstGfxGPDevice[enGpId]->bBGRState);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/***************************************************************************************
 * func        : OPTM_GfxChn2DispChn
 * description : CNcomment: 获取display通道 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_drv_display OPTM_GfxChn2DispChn(OPTM_DISPCHANNEL_E enDispCh)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (OPTM_DISPCHANNEL_0 == enDispCh) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_DRV_DISPLAY_1;
    }

    if (OPTM_DISPCHANNEL_1 == enDispCh) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_DRV_DISPLAY_0;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_DRV_DISPLAY_BUTT;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetCallbackToDisp
 * description  : CNcomment: 向Display注册中断 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetCallbackToDisp(HIFB_GP_ID_E enGPId, IntCallBack pCallBack, hi_drv_disp_callback_type eType,
                                      hi_bool bFlag)
{
    hi_s32 s32Ret = HI_FAILURE;
    hi_drv_display enDisp;
    hi_drv_disp_callback stCallback;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(eType, HI_DRV_DISP_C_TYPE_BUTT, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pCallBack, HI_FAILURE);

    if (bFlag == g_pstGfxGPIrq[enGPId]->bRegistered[eType]) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    enDisp = OPTM_GfxChn2DispChn(g_pstGfxGPDevice[enGPId]->enDispCh);
    stCallback.hdst = (hi_void *)(&g_pstGfxGPDevice[enGPId]->enGpHalId);
    stCallback.pf_disp_callback = (hi_void *)pCallBack;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(ps_DispExportFuncs, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(ps_DispExportFuncs->disp_register_call_back, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(ps_DispExportFuncs->disp_unregister_call_back, HI_FAILURE);
    if (bFlag) {
        s32Ret = ps_DispExportFuncs->disp_register_call_back(enDisp, eType, &stCallback);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        s32Ret = ps_DispExportFuncs->disp_unregister_call_back(enDisp, eType, &stCallback);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if (HI_SUCCESS == s32Ret) {
        g_pstGfxGPIrq[enGPId]->bRegistered[eType] = bFlag;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return s32Ret;
}
#endif

/***************************************************************************************
 * func         : DRV_HIFB_ADP_OpenLayer
 * description  : CNcomment: 打开图层 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_OpenLayer(HIFB_LAYER_ID_E enLayerId)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);

    if (HI_TRUE != g_pstGfxCap[enLayerId]->bLayerSupported) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxCap[enLayerId]->bLayerSupported);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this layer can not support");
        return HI_FAILURE;
    }

    if (HI_TRUE == g_pstGfxDevice[enLayerId]->bOpened) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

#ifndef HI_BUILD_IN_BOOT
    Ret = osal_exportfunc_get(HI_ID_PQ, (hi_void **)&gs_pstPqFuncs);
    if ((HI_SUCCESS != Ret) || (NULL == gs_pstPqFuncs)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "get pq func failure");
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_drv_module_get_function, Ret);
        /* wait pq done */
        /* return HI_FAILURE; */
    }
#endif

    Ret = DRV_HIFB_ADP_GFX_InitLayer(enLayerId);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_GFX_InitLayer, Ret);
        return HI_FAILURE;
    }

    Ret = DRV_HIFB_ADP_GP_Open(g_pstGfxDevice[enLayerId]->enGPId);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_GP_Open, Ret);
        return HI_FAILURE;
    }

    g_pstGfxDevice[enLayerId]->bOpened = HI_TRUE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_CloseLayer
 * description : CNcomment: 关闭打开的图层 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void DRV_HIFB_ADP_CloseLayer(HIFB_LAYER_ID_E enLayerId)
{
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    enGPId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGPId);

    /* * close master ui * */
    DRV_HIFB_ADP_SetEnable(enLayerId, HI_FALSE);
    drv_hifb_hal_gfx_set_no_sec_flag(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_TRUE);
    drv_hifb_hal_gfx_set_reg_up(g_pstGfxDevice[enLayerId]->enGfxHalId);
    DRV_HIFB_ADP_ReleaseClutBuf(enLayerId);

    DRV_HIFB_ADP_GP_Close(enGPId);

    g_pstGfxDevice[enLayerId]->bExtractLine = HI_FALSE;
    g_pstGfxDevice[enLayerId]->bOpened = HI_FALSE;

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    /* * close wbc * */
    if (HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) {
        g_enOptmGfxWorkMode = HIFB_GFX_MODE_NORMAL;

        DRV_HIFB_WBC_CloseLayer(OPTM_SLAVER_LAYERID);
        DRV_HIFB_ADP_GP_Close(OPTM_SLAVER_GPID);

        g_pstGfxDevice[HIFB_LAYER_SD_0]->bExtractLine = HI_FALSE;
        g_pstGfxDevice[HIFB_LAYER_SD_0]->bOpened = HI_FALSE;

        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "run close wbc");
    }
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_SetEnable
 * description : CNcomment: 图层使能 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetEnable(HIFB_LAYER_ID_E enLayerId, hi_bool bEnable)
{
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP_BUTT;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGPId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    g_pstGfxDevice[enLayerId]->bEnable = bEnable;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enLayerId);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enGPId);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, bEnable);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_pstGfxGPDevice[enGPId]->bMaskFlag);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_enOptmGfxWorkMode);

    if (HI_TRUE == g_pstGfxGPDevice[enGPId]->bMaskFlag) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    drv_hifb_hal_gfx_set_layer_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, bEnable);
    drv_hifb_hal_gfx_set_reg_up(g_pstGfxDevice[enLayerId]->enGfxHalId);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP0 == enGPId)) {
        DRV_HIFB_WBC_SetEnable(bEnable);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call DRV_HIFB_WBC_SetEnable");
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetEnable
 * description : CNcomment: 图层使能 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void DRV_HIFB_ADP_GetEnable(HIFB_LAYER_ID_E enLayerId, hi_bool *pbEnable)
{
#ifndef HI_BUILD_IN_BOOT
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    drv_hifb_hal_gfx_get_layer_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, pbEnable);
#else
#endif
    return;
}

hi_void hifb_adp_set_vdp_layer_enable(hi_u32 layer, hi_bool enable)
{
    hifb_hal_set_video_enable(layer, enable);
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetLayerAddr
 * description  : CNcomment: 设置显示地址 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerAddr(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Addr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->enGPId, HI_FAILURE);

    g_pstGfxDevice[enLayerId]->NoCmpBufAddr = u32Addr;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enLayerId);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, g_pstGfxDevice[enLayerId]->NoCmpBufAddr);

    if (HI_TRUE == g_pstGfxGPDevice[g_pstGfxDevice[enLayerId]->enGPId]->bMaskFlag) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    drv_hifb_hal_gfx_set_layer_addr_ex(g_pstGfxDevice[enLayerId]->enGfxHalId, u32Addr);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : DRV_HIFB_ADP_GetLayerAddr
 * description   : CNcomment: 获取显示地址 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_GetLayerAddr(HIFB_LAYER_ID_E enLayerId, hi_u32 *pu32Addr)
{
#ifndef HI_BUILD_IN_BOOT
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    drv_hifb_hal_gfx_get_layer_addr(g_pstGfxDevice[enLayerId]->enGfxHalId, pu32Addr);
#else
#endif
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : DRV_HIFB_ADP_GetLayerWorkAddr
 * description   : CNcomment: 获取已经工作的显存地址 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_void DRV_HIFB_ADP_GetLayerWorkAddr(HIFB_LAYER_ID_E enLayerId, hi_u32 *pAddress)
{
#if !defined(HI_BUILD_IN_BOOT) && !defined(CONFIG_HIFB_VERSION_1_0)
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pAddress);
    drv_hifb_hal_gfx_get_work_layer_addr(g_pstGfxDevice[enLayerId]->enGfxHalId, pAddress);
#else
#endif
    return;
}

/***************************************************************************************
 * func          : DRV_HIFB_ADP_GetLayerWillWorkAddr
 * description   : CNcomment: 获取将要工作的显存地址 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_void DRV_HIFB_ADP_GetLayerWillWorkAddr(HIFB_LAYER_ID_E enLayerId, hi_u32 *pAddress)
{
#if !defined(HI_BUILD_IN_BOOT) && !defined(CONFIG_HIFB_VERSION_1_0)
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pAddress);
    drv_hifb_hal_gfx_get_will_work_layer_addr(g_pstGfxDevice[enLayerId]->enGfxHalId, pAddress);
#else
#endif
    return;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_SetLayerStride
 * description : CNcomment: 设置图层stride CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerStride(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Stride)
{
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGPId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    g_pstGfxDevice[enLayerId]->Stride = (hi_u16)u32Stride;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enLayerId);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[enLayerId]->Stride);

    if (HI_TRUE == g_pstGfxGPDevice[enGPId]->bMaskFlag) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (g_pstGfxGPDevice[enGPId]->bNeedExtractLine && (OPTM_CURSOR_LAYERID != enLayerId)) {
        drv_hifb_hal_gfx_set_layer_stride(g_pstGfxDevice[enLayerId]->enGfxHalId, (u32Stride * 2));
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        drv_hifb_hal_gfx_set_layer_stride(g_pstGfxDevice[enLayerId]->enGfxHalId, u32Stride);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetLayerStride
 * description : CNcomment: 设置图层stride CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void DRV_HIFB_ADP_GetLayerStride(HIFB_LAYER_ID_E enLayerId, hi_u32 *pStride)
{
#ifndef HI_BUILD_IN_BOOT
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pStride);
    drv_hifb_hal_gfx_get_layer_stride(g_pstGfxDevice[enLayerId]->enGfxHalId, pStride);
#else
#endif
    return;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func        : DRV_HIFB_ADP_HalFmtTransferToPixerFmt
 * description : CNcomment: 由VDP像素格式转换成hifb像素格式 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static DRV_HIFB_COLOR_FMT_E DRV_HIFB_ADP_HalFmtTransferToPixerFmt(OPTM_VDP_GFX_IFMT_E enDataFmt)
{
    DRV_HIFB_COLOR_FMT_E ePixFmt = DRV_HIFB_FMT_BUTT;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    ePixFmt = OPTM_ClutHalFmtTransferToPixFmt(enDataFmt);
    if (DRV_HIFB_FMT_BUTT != ePixFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return ePixFmt;
    }

    ePixFmt = OPTM_XYCbCrHalFmtTransferToPixFmt(enDataFmt);
    if (DRV_HIFB_FMT_BUTT != ePixFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return ePixFmt;
    }

    ePixFmt = OPTM_RGBHalFmtTransferToPixFmt(enDataFmt);
    if (DRV_HIFB_FMT_BUTT != ePixFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return ePixFmt;
    }

    ePixFmt = OPTM_XRGBHalFmtTransferToPixFmt(enDataFmt);
    if (DRV_HIFB_FMT_BUTT != ePixFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return ePixFmt;
    }

    return DRV_HIFB_FMT_BUTT;
}

static inline DRV_HIFB_COLOR_FMT_E OPTM_ClutHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case VDP_GFX_IFMT_CLUT_1BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_1BPP;
        case VDP_GFX_IFMT_CLUT_2BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_2BPP;
        case VDP_GFX_IFMT_CLUT_4BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_4BPP;
        case VDP_GFX_IFMT_CLUT_8BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_8BPP;
        case VDP_GFX_IFMT_ACLUT_44:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_ACLUT44;
        case VDP_GFX_IFMT_ACLUT_88:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_ACLUT88;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_BUTT;
    }
}

static inline DRV_HIFB_COLOR_FMT_E OPTM_RGBHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case VDP_GFX_IFMT_RGB_565:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_RGB565;
        case VDP_GFX_IFMT_RGB_888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_RGB888;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_BUTT;
    }
}

static inline DRV_HIFB_COLOR_FMT_E OPTM_XRGBHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case VDP_GFX_IFMT_RGB_444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_KRGB444;
        case VDP_GFX_IFMT_RGB_555:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_KRGB555;
        case VDP_GFX_IFMT_ARGB_8565:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_ARGB8565;
        case VDP_GFX_IFMT_KRGB_888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_KRGB888;
        case VDP_GFX_IFMT_ARGB_8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_ARGB8888;
        case VDP_GFX_IFMT_ARGB_4444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_ARGB4444;
        case VDP_GFX_IFMT_ARGB_1555:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_ARGB1555;
        case VDP_GFX_IFMT_RGBA_4444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_RGBA4444;
        case VDP_GFX_IFMT_RGBA_5551:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_RGBA5551;
        case VDP_GFX_IFMT_RGBA_5658:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_RGBA5658;
        case VDP_GFX_IFMT_RGBA_8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_RGBA8888;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_BUTT;
    }
}

static inline DRV_HIFB_COLOR_FMT_E OPTM_XYCbCrHalFmtTransferToPixFmt(OPTM_VDP_GFX_IFMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case VDP_GFX_IFMT_PKG_UYVY:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_PUYVY;
        case VDP_GFX_IFMT_PKG_YUYV:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_PYUYV;
        case VDP_GFX_IFMT_PKG_YVYU:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_PYVYU;
        case VDP_GFX_IFMT_YCBCR_888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_YUV888;
        case VDP_GFX_IFMT_AYCBCR_8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_AYUV8888;
        case VDP_GFX_IFMT_YCBCRA_8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_YUVA8888;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return DRV_HIFB_FMT_BUTT;
    }
}
#endif

/***************************************************************************************
 * func          : DRV_HIFB_ADP_PixerFmtTransferToHalFmt
 * description   : CNcomment: 由hifb像素格式转换成图形层像素格式 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
OPTM_VDP_GFX_IFMT_E DRV_HIFB_ADP_PixerFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    OPTM_VDP_GFX_IFMT_E eHalFmt = VDP_GFX_IFMT_BUTT;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    eHalFmt = OPTM_ClutPixFmtTransferToHalFmt(enDataFmt);
    if (VDP_GFX_IFMT_BUTT != eHalFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return eHalFmt;
    }

    eHalFmt = OPTM_XRGBPixFmtTransferToHalFmt(enDataFmt);
    if (VDP_GFX_IFMT_BUTT != eHalFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return eHalFmt;
    }

    eHalFmt = OPTM_RGBPixFmtTransferToHalFmt(enDataFmt);
    if (VDP_GFX_IFMT_BUTT != eHalFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return eHalFmt;
    }

    eHalFmt = OPTM_XYCbCrPixFmtTransferToHalFmt(enDataFmt);
    if (VDP_GFX_IFMT_BUTT != eHalFmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return eHalFmt;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return VDP_GFX_IFMT_BUTT;
}

static inline OPTM_VDP_GFX_IFMT_E OPTM_ClutPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case DRV_HIFB_FMT_1BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_CLUT_1BPP;
        case DRV_HIFB_FMT_2BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_CLUT_2BPP;
        case DRV_HIFB_FMT_4BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_CLUT_4BPP;
        case DRV_HIFB_FMT_8BPP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_CLUT_8BPP;
        case DRV_HIFB_FMT_ACLUT44:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ACLUT_44;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_BUTT;
    }
}

static inline OPTM_VDP_GFX_IFMT_E OPTM_RGBPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case DRV_HIFB_FMT_KRGB444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_444;
        case DRV_HIFB_FMT_KRGB555:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_555;
        case DRV_HIFB_FMT_RGB565:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_565;
        case DRV_HIFB_FMT_RGB888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_888;
        case DRV_HIFB_FMT_YUV888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_YCBCR_888;
        case DRV_HIFB_FMT_KRGB888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_KRGB_888;
        case DRV_HIFB_FMT_BGR565:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_565;
        case DRV_HIFB_FMT_BGR888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_888;
        case DRV_HIFB_FMT_KBGR444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_444;
        case DRV_HIFB_FMT_KBGR555:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGB_555;
        case DRV_HIFB_FMT_KBGR888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_KRGB_888;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_BUTT;
    }
}

static inline OPTM_VDP_GFX_IFMT_E OPTM_XRGBPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    switch (enDataFmt) {
        case DRV_HIFB_FMT_ARGB8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_8888;
        case DRV_HIFB_FMT_ARGB4444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_4444;
        case DRV_HIFB_FMT_ARGB1555:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_1555;
        case DRV_HIFB_FMT_RGBA4444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGBA_4444;
        case DRV_HIFB_FMT_RGBA5551:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGBA_5551;
        case DRV_HIFB_FMT_RGBA5658:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGBA_5658;
        case DRV_HIFB_FMT_RGBA8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_RGBA_8888;
        case DRV_HIFB_FMT_ABGR4444:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_4444;
        case DRV_HIFB_FMT_ABGR1555:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_1555;
        case DRV_HIFB_FMT_ABGR8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ABGR_8888;
        case DRV_HIFB_FMT_ABGR8565:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_8565;
        case DRV_HIFB_FMT_ARGB8565:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_8565;
        case DRV_HIFB_FMT_ARGB2101010:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_2101010;
        case DRV_HIFB_FMT_ARGB10101010:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ARGB_10101010;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_BUTT;
    }
}

static inline OPTM_VDP_GFX_IFMT_E OPTM_XYCbCrPixFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enDataFmt) {
        case DRV_HIFB_FMT_PUYVY:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_PKG_UYVY;
        case DRV_HIFB_FMT_PYUYV:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_PKG_YUYV;
        case DRV_HIFB_FMT_PYVYU:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_PKG_YVYU;
        case DRV_HIFB_FMT_ACLUT88:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_ACLUT_88;
        case DRV_HIFB_FMT_YUVA8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_YCBCRA_8888;
        case DRV_HIFB_FMT_AYUV8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_AYCBCR_8888;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return VDP_GFX_IFMT_BUTT;
    }
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_SetLayerDataFmt
 * description : CNcomment: 设置图层格式 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerDataFmt(HIFB_LAYER_ID_E enLayerId, DRV_HIFB_COLOR_FMT_E enDataFmt)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGPId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(enDataFmt, DRV_HIFB_FMT_BUTT, HI_FAILURE);
    if (HI_FALSE == g_pstGfxCap[enLayerId]->bColFmt[enDataFmt]) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDataFmt);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxCap[enLayerId]->bColFmt[enDataFmt]);
        return HI_FAILURE;
    }

    g_pstGfxDevice[enLayerId]->enDataFmt = enDataFmt;

    OPTM_CheckGPMask_BYGPID(enGPId);

    if ((DRV_HIFB_FMT_ABGR8888 != enDataFmt) &&
        ((enDataFmt >= DRV_HIFB_FMT_BGR565) && (DRV_HIFB_FMT_KBGR888 >= enDataFmt)) &&
        (g_pstGfxDevice[enLayerId]->CscState == OPTM_CSC_SET_PARA_RGB)) {
        Ret = HIFB_ADP_SetLayerDataBigEndianFmt(enLayerId);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call HIFB_ADP_SetLayerDataBigEndianFmt");
    } else if (((DRV_HIFB_FMT_ABGR8888 == enDataFmt) ||
                (DRV_HIFB_FMT_BGR565 > enDataFmt || enDataFmt > DRV_HIFB_FMT_KBGR888)) &&
               (g_pstGfxDevice[enLayerId]->CscState == OPTM_CSC_SET_PARA_BGR)) {
        Ret = HIFB_ADP_SetLayerDataLittleEndianFmt(enLayerId);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call HIFB_ADP_SetLayerDataLittleEndianFmt");
    }

    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDataFmt);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[enLayerId]->CscState);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_ADP_SetLayerDataBigEndianFmt, Ret);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_ADP_SetLayerDataLittleEndianFmt, Ret);
        return HI_FAILURE;
    }

    drv_hifb_hal_gfx_set_in_data_fmt(g_pstGfxDevice[enLayerId]->enGfxHalId,
                                     DRV_HIFB_ADP_PixerFmtTransferToHalFmt(enDataFmt));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static inline hi_s32 HIFB_ADP_SetLayerDataBigEndianFmt(HIFB_LAYER_ID_E enLayerId)
{
    hi_s32 s32Cnt = 0;
    hi_u32 u32LayerCount = 0;
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGPId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    u32LayerCount = (HIFB_LAYER_HD_3 >= enLayerId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;

    for (s32Cnt = 0; s32Cnt < u32LayerCount; s32Cnt++) {
        if ((s32Cnt != enLayerId) && (g_pstGfxDevice[s32Cnt]->bEnable) &&
            (g_pstGfxDevice[s32Cnt]->CscState != OPTM_CSC_SET_PARA_BGR)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Cnt);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[s32Cnt]->bEnable);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[s32Cnt]->CscState);
            return HI_FAILURE;
        }
    }

    g_pstGfxDevice[enLayerId]->CscState = OPTM_CSC_SET_PARA_BGR;
    g_pstGfxGPDevice[enGPId]->bBGRState = HI_TRUE;

    HIFB_ADP_SetGpCsc(enGPId, HI_TRUE);

    if (IS_MASTER_GP(enGPId)) {
        HIFB_ADP_SetGpCsc(OPTM_SLAVER_GPID, HI_TRUE);
        g_pstGfxGPDevice[OPTM_SLAVER_GPID]->bBGRState = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call HIFB_ADP_SetGpCsc for gp1");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static inline hi_s32 HIFB_ADP_SetLayerDataLittleEndianFmt(HIFB_LAYER_ID_E enLayerId)
{
    hi_s32 s32Cnt = 0;
    hi_u32 u32LayerCount = 0;
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGPId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    u32LayerCount = (HIFB_LAYER_HD_3 >= enLayerId) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;

    for (s32Cnt = 0; s32Cnt < u32LayerCount; s32Cnt++) {
        if ((s32Cnt != enLayerId) && (g_pstGfxDevice[s32Cnt]->bEnable) &&
            (g_pstGfxDevice[s32Cnt]->CscState != OPTM_CSC_SET_PARA_RGB)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Cnt);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[s32Cnt]->bEnable);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[s32Cnt]->CscState);
            return HI_FAILURE;
        }
    }

    g_pstGfxDevice[enLayerId]->CscState = OPTM_CSC_SET_PARA_RGB;
    g_pstGfxGPDevice[enGPId]->bBGRState = HI_FALSE;

    HIFB_ADP_SetGpCsc(enGPId, HI_FALSE);

    if (IS_MASTER_GP(enGPId)) {
        HIFB_ADP_SetGpCsc(OPTM_SLAVER_GPID, HI_FALSE);
        g_pstGfxGPDevice[OPTM_SLAVER_GPID]->bBGRState = HI_FALSE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetColorReg
 * description  : CNcomment: 设置调色板颜色寄存器 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_SetColorReg(HIFB_LAYER_ID_E enLayerId, hi_u32 u32OffSet, hi_u32 u32Color, hi_s32 UpFlag)
{
    hi_u32 *pCTab = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    pCTab = (hi_u32 *)(g_pstGfxDevice[enLayerId]->clut_table.virtual_addr);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pCTab, HI_FAILURE);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32OffSet, OPTM_CMAP_SIZE, HI_FAILURE);
    pCTab[u32OffSet] = u32Color;

    if (UpFlag != 0) {
        drv_hifb_hal_gfx_set_para_upd(g_pstGfxDevice[enLayerId]->enGfxHalId, VDP_DISP_COEFMODE_LUT);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func        : OPTM_GfxWVBCallBack
 * description : CNcomment: 等中断完成 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_void OPTM_GfxWVBCallBack(hi_u32 enLayerId)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);

    if (HI_FALSE == g_pstGfxDevice[enLayerId]->bOpened) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    g_pstGfxDevice[enLayerId]->vblflag = 1;
    osal_wait_wakeup(&(g_pstGfxDevice[enLayerId]->vblEvent));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 drv_hifb_get_vblflag(const void *param)
{
    HIFB_LAYER_ID_E *enLayerId = (HIFB_LAYER_ID_E *)param;
    if (enLayerId == HI_NULL) {
        return 0;
    }
    return g_pstGfxDevice[*enLayerId]->vblflag;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_WaitVBlank
 * description  : CNcomment: 等中断 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_WaitVBlank(HIFB_LAYER_ID_E enLayerId)
{
    hi_u32 u32TimeOutMs = (1000 * HZ) / 1000;
    hi_s32 s32Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    HIFB_CHECK_LAYER_OPEN(enLayerId);

    g_pstGfxDevice[enLayerId]->vblflag = 0;
    s32Ret = osal_wait_timeout_interruptible(&g_pstGfxDevice[enLayerId]->vblEvent, drv_hifb_get_vblflag,
                                             (hi_void *)&enLayerId, u32TimeOutMs);
    if (s32Ret <= 0) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, u32TimeOutMs);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enLayerId);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[enLayerId]->vblflag);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "call wait_event_interruptible_timeout failure");
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#else
hi_s32 DRV_HIFB_ADP_WaitVBlank(HIFB_LAYER_ID_E enLayerId)
{
    return HI_FAILURE;
}

#endif

/***************************************************************************************
 * func        : DRV_HIFB_ADP_SetLayerDeFlicker
 * description : CNcomment: 设置图层抗闪，不支持 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_SetLayerDeFlicker(HIFB_LAYER_ID_E enLayerId, HIFB_DEFLICKER_S *pstDeFlicker)
{
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetLayerAlpha
 * description  : CNcomment: 设置图层alpha CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerAlpha(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstAlpha, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->enGPId, HI_FAILURE);

    memcpy(&g_pstGfxDevice[enLayerId]->stAlpha, pstAlpha, sizeof(HIFB_ALPHA_S));

    if (HI_TRUE == g_pstGfxGPDevice[g_pstGfxDevice[enLayerId]->enGPId]->bMaskFlag) { /* * whether finish logo to app * */
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enLayerId);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_pstGfxDevice[enLayerId]->enGPId);
        return HI_SUCCESS;
    }

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstAlpha->u8Alpha0);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstAlpha->u8Alpha1);

    drv_hifb_hal_gfx_set_palpha(g_pstGfxDevice[enLayerId]->enGfxHalId, pstAlpha->bAlphaEnable, HI_TRUE,
                                pstAlpha->u8Alpha0, pstAlpha->u8Alpha1);
    drv_hifb_hal_gfx_set_layer_galpha(g_pstGfxDevice[enLayerId]->enGfxHalId, pstAlpha->u8GlobalAlpha);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetLayerRect
 * description : CNcomment: 获取图层输入分辨率 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_GetLayerRect(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstRect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRect, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);

    memcpy(pstRect, &g_pstGfxDevice[enLayerId]->stInRect, sizeof(HIFB_RECT));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void DRV_HIFB_PrintLayerRect(HIFB_LAYER_ID_E enLayerId, HIFB_GP_ID_E enGpId,
                                       const OPTM_VDP_DISP_RECT_S *pstGfxRect, hi_bool RectChange, hi_u32 Line)
{
    if ((HI_TRUE == RectChange) && (NULL != pstGfxRect)) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION,
                               "=========================================================");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, Line);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enLayerId);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[enLayerId]->stInRect.x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[enLayerId]->stInRect.y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[enLayerId]->stInRect.w);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxDevice[enLayerId]->stInRect.h);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enGpId);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_w);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, g_pstGfxGPDevice[enGpId]->stInRect.rect_h);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "disp position");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32DXS);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32DYS);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32DXL);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32DYL);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "video position");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32VX);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32VY);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32VX + pstGfxRect->u32OWth);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32VY + pstGfxRect->u32OHgt);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "input size");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32IWth);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32IHgt);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "output size");
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32OWth);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstGfxRect->u32OHgt);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION,
                               "=========================================================");
    }
}

/***************************************************************************************
 * func          : DRV_HIFB_ADP_SetLayerRect
 * description   : CNcomment: 设置图层输入矩形 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerRect(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect)
{
    hi_bool RectChange = HI_FALSE;
    hi_s32 Ret = HI_SUCCESS;
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    OPTM_VDP_DISP_RECT_S stGfxRect;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRect, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGpId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    if ((g_pstGfxDevice[enLayerId]->stInRect.x != pstRect->x) ||
        (g_pstGfxDevice[enLayerId]->stInRect.y != pstRect->y) ||
        (g_pstGfxDevice[enLayerId]->stInRect.w != pstRect->w) ||
        (g_pstGfxDevice[enLayerId]->stInRect.h != pstRect->h)) {
        RectChange = HI_TRUE;
    }

    g_pstGfxDevice[enLayerId]->stInRect.x = pstRect->x;
    g_pstGfxDevice[enLayerId]->stInRect.y = pstRect->y;
    g_pstGfxDevice[enLayerId]->stInRect.w = pstRect->w;
    g_pstGfxDevice[enLayerId]->stInRect.h = pstRect->h;

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT, g_pstGfxDevice[enLayerId]->stInRect.w,
                                  g_pstGfxDevice[enLayerId]->stInRect.h, 0, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[enLayerId]->stInRect.w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[enLayerId]->stInRect.h);
        return HI_FAILURE;
    }

    if ((g_pstGfxDevice[enLayerId]->stInRect.x > config_hifb_layer_max_width(enLayerId)) ||
        (g_pstGfxDevice[enLayerId]->stInRect.y > config_hifb_layer_max_height(enLayerId))) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[enLayerId]->stInRect.x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxDevice[enLayerId]->stInRect.y);
        return HI_FAILURE;
    }

    if (HI_TRUE == g_pstGfxGPDevice[enGpId]->bMaskFlag) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    memset(&stGfxRect, 0, sizeof(stGfxRect));

    stGfxRect.u32DXL = 0;
    stGfxRect.u32DYL = 0;
    stGfxRect.u32OWth = 0;
    stGfxRect.u32VX = g_pstGfxDevice[enLayerId]->stInRect.x;
    stGfxRect.u32VY = g_pstGfxDevice[enLayerId]->stInRect.y;
    stGfxRect.u32IWth = g_pstGfxDevice[enLayerId]->stInRect.w;
    stGfxRect.u32IHgt = g_pstGfxDevice[enLayerId]->stInRect.h;
    stGfxRect.u32DXS = g_pstGfxDevice[enLayerId]->stInRect.x;
    stGfxRect.u32DYS = g_pstGfxDevice[enLayerId]->stInRect.y;
    DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);

    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(g_pstGfxCap[enLayerId]->u32MinWidth,
                                                 g_pstGfxGPDevice[enGpId]->stInRect.rect_w, HI_FAILURE);
    if (g_pstGfxDevice[enLayerId]->stInRect.x >
        g_pstGfxGPDevice[enGpId]->stInRect.rect_w - g_pstGfxCap[enLayerId]->u32MinWidth) {
        g_pstGfxDevice[enLayerId]->stInRect.x = g_pstGfxGPDevice[enGpId]->stInRect.rect_w -
                                                g_pstGfxCap[enLayerId]->u32MinWidth;
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    }

    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(g_pstGfxCap[enLayerId]->u32MinHeight,
                                                 g_pstGfxGPDevice[enGpId]->stInRect.rect_h, HI_FAILURE);
    if (g_pstGfxDevice[enLayerId]->stInRect.y >
        g_pstGfxGPDevice[enGpId]->stInRect.rect_h - g_pstGfxCap[enLayerId]->u32MinHeight) {
        g_pstGfxDevice[enLayerId]->stInRect.y = g_pstGfxGPDevice[enGpId]->stInRect.rect_h -
                                                g_pstGfxCap[enLayerId]->u32MinHeight;
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    }

    if (!IS_SLAVER_GP(enGpId)) {
        GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->stInRect.x,
                                                     g_pstGfxGPDevice[enGpId]->stInRect.rect_w, HI_FAILURE);
        if ((g_pstGfxDevice[enLayerId]->stInRect.x + g_pstGfxDevice[enLayerId]->stInRect.w) >
            g_pstGfxGPDevice[enGpId]->stInRect.rect_w) {
            stGfxRect.u32IWth = g_pstGfxGPDevice[enGpId]->stInRect.rect_w - g_pstGfxDevice[enLayerId]->stInRect.x;
            DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
        }

        GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->stInRect.y,
                                                     g_pstGfxGPDevice[enGpId]->stInRect.rect_h, HI_FAILURE);
        if ((g_pstGfxDevice[enLayerId]->stInRect.y + g_pstGfxDevice[enLayerId]->stInRect.h) >
            g_pstGfxGPDevice[enGpId]->stInRect.rect_h) {
            stGfxRect.u32IHgt = g_pstGfxGPDevice[enGpId]->stInRect.rect_h - g_pstGfxDevice[enLayerId]->stInRect.y;
            DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
        }
    }

    if (g_pstGfxGPDevice[enGpId]->bNeedExtractLine) { /* * 是否抽行处理 * */
        stGfxRect.u32IHgt /= 2;
        stGfxRect.u32VY /= 2;
        stGfxRect.u32DYS /= 2;
        g_pstGfxDevice[enLayerId]->bExtractLine = HI_TRUE;
        drv_hifb_hal_gfx_set_layer_stride(g_pstGfxDevice[enLayerId]->enGfxHalId, g_pstGfxDevice[enLayerId]->Stride * 2);
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    } else if (HI_TRUE == g_pstGfxDevice[enLayerId]->bExtractLine) {
        DRV_HIFB_ADP_SetLayerStride(enLayerId, g_pstGfxDevice[enLayerId]->Stride);
        g_pstGfxDevice[enLayerId]->bExtractLine = HI_FALSE;
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    }

    if (enGpId == HIFB_ADP_GP0) {
        stGfxRect.u32OWth = stGfxRect.u32IWth;
        stGfxRect.u32OHgt = stGfxRect.u32IHgt;
    } else {
        stGfxRect.u32OWth = g_pstGfxGPDevice[enGpId]->stOutRect.rect_w;
        stGfxRect.u32OHgt = g_pstGfxGPDevice[enGpId]->stOutRect.rect_h;
    }
    stGfxRect.u32DXL = g_pstGfxDevice[enLayerId]->stInRect.x + stGfxRect.u32OWth;
    stGfxRect.u32DYL = g_pstGfxDevice[enLayerId]->stInRect.y + stGfxRect.u32OHgt;
    DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);

    if ((g_enOptmGfxWorkMode == HIFB_GFX_MODE_NORMAL) && (enGpId == HIFB_ADP_GP1)) {
        stGfxRect.u32DXS = 0;
        stGfxRect.u32DYS = 0;
        if (g_pstGfxGPDevice[enGpId]->bNeedExtractLine) {
            stGfxRect.u32DYL /= 2;
            DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
        }
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    }

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if (HIFB_STEREO_SIDEBYSIDE_HALF == g_pstGfxDevice[enLayerId]->enTriDimMode) {
        stGfxRect.u32IWth = stGfxRect.u32IWth / 2;
        stGfxRect.u32IWth &= 0xfffffffe;
        stGfxRect.u32OWth = (stGfxRect.u32IWth) * 2;
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    } else if (HIFB_STEREO_TOPANDBOTTOM == g_pstGfxDevice[enLayerId]->enTriDimMode) {
        stGfxRect.u32IHgt = stGfxRect.u32IHgt / 2;
        stGfxRect.u32IHgt &= 0xfffffffe;
        stGfxRect.u32OHgt = (stGfxRect.u32IHgt) * 2;
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    }
#endif

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if ((OPTM_CURSOR_LAYERID != enLayerId) ||
        (HIFB_STEREO_SIDEBYSIDE_HALF == g_pstGfxDevice[enLayerId]->enTriDimMode) ||
        (HIFB_STEREO_TOPANDBOTTOM == g_pstGfxDevice[enLayerId]->enTriDimMode))
#else
    if (OPTM_CURSOR_LAYERID != enLayerId)
#endif
    { /* * cursor support odd size * */
        stGfxRect.u32VX &= 0xfffffffe;
        stGfxRect.u32VY &= 0xfffffffe;
        stGfxRect.u32DXS &= 0xfffffffe;
        stGfxRect.u32DYS &= 0xfffffffe;
        stGfxRect.u32DXL &= 0xfffffffe;
        stGfxRect.u32DYL &= 0xfffffffe;
        stGfxRect.u32IWth &= 0xfffffffe;
        stGfxRect.u32IHgt &= 0xfffffffe;
        stGfxRect.u32OWth &= 0xfffffffe;
        stGfxRect.u32OHgt &= 0xfffffffe;
        DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);
    }

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    if ((OPTM_CURSOR_LAYERID != enLayerId) ||
        (HIFB_STEREO_SIDEBYSIDE_HALF == g_pstGfxDevice[enLayerId]->enTriDimMode) ||
        (HIFB_STEREO_TOPANDBOTTOM == g_pstGfxDevice[enLayerId]->enTriDimMode))
#else
    if (OPTM_CURSOR_LAYERID != enLayerId)
#endif
    { /* * not support odd size * */
        if ((0 != stGfxRect.u32VX % 2) || (0 != stGfxRect.u32VY % 2) || (0 != stGfxRect.u32DXS % 2) ||
            (0 != stGfxRect.u32DYS % 2)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stGfxRect.u32VX);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stGfxRect.u32VY);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stGfxRect.u32DXS);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stGfxRect.u32DYS);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "not support odd size");
        }
    }

    drv_hifb_hal_gfx_set_layer_reso(g_pstGfxDevice[enLayerId]->enGfxHalId, &stGfxRect);

    DRV_HIFB_PrintLayerRect(enLayerId, enGpId, &stGfxRect, RectChange, __LINE__);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : DRV_HIFB_ADP_GetDispFMTSize
 * description   : CNcomment: 获取disp大小 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
STATIC hi_s32 DRV_HIFB_ADP_GetDispFMTSize(HIFB_LAYER_ID_E LayerId, HIFB_RECT *pstOutRect)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP_BUTT;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOutRect, HI_FAILURE);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(LayerId, HI_FAILURE);
    enGpId = HIFB_ADP_GetGpId(LayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    pstOutRect->x = g_pstGfxGPDevice[enGpId]->stOutRect.rect_x;
    pstOutRect->y = g_pstGfxGPDevice[enGpId]->stOutRect.rect_y;
    pstOutRect->w = g_pstGfxGPDevice[enGpId]->stOutRect.rect_w;
    pstOutRect->h = g_pstGfxGPDevice[enGpId]->stOutRect.rect_h;

    if (pstOutRect->w == 0 || pstOutRect->h == 0) {
        pstOutRect->x = 0;
        pstOutRect->y = 0;
        pstOutRect->w = 1280;
        pstOutRect->h = 720;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if (pstOutRect->w == 1440 && (pstOutRect->h == 576 || pstOutRect->h == 480)) {
        pstOutRect->w /= 2;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : OPTM_GfxSetDispFMTSize
 * description  : CNcomment: 设置display像素格式和大小 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 OPTM_GfxSetDispFMTSize(HIFB_GP_ID_E enGpId, const hi_drv_rect *pstOutRect)
{
    hi_u32 u32Ratio = 0;
    HIFB_RECT stInputRect = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOutRect, HI_FAILURE);

    if (pstOutRect->rect_w <= 0 || pstOutRect->rect_h <= 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstOutRect->rect_w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstOutRect->rect_h);
        return HI_FAILURE;
    }

    g_pstGfxGPDevice[enGpId]->stOutRect.rect_x = pstOutRect->rect_x;
    g_pstGfxGPDevice[enGpId]->stOutRect.rect_y = pstOutRect->rect_y;
    if (pstOutRect->rect_x < 0) {
        g_pstGfxGPDevice[enGpId]->stOutRect.rect_x = 0;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstOutRect->rect_x);
    }
    if (pstOutRect->rect_y < 0) {
        g_pstGfxGPDevice[enGpId]->stOutRect.rect_y = 0;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstOutRect->rect_y);
    }

    g_pstGfxGPDevice[enGpId]->stOutRect.rect_w = pstOutRect->rect_w;
    g_pstGfxGPDevice[enGpId]->stOutRect.rect_h = pstOutRect->rect_h;

    OPTM_CheckGPMask_BYGPID(enGpId);

    stInputRect.x = g_pstGfxGPDevice[enGpId]->stInRect.rect_x;
    stInputRect.y = g_pstGfxGPDevice[enGpId]->stInRect.rect_y;
    stInputRect.w = g_pstGfxGPDevice[enGpId]->stInRect.rect_w;
    stInputRect.h = g_pstGfxGPDevice[enGpId]->stInRect.rect_h;

    /* judge wether need to extract line for layer or not */
    u32Ratio = g_pstGfxGPDevice[enGpId]->stInRect.rect_h * 2;
    u32Ratio /= g_pstGfxGPDevice[enGpId]->stOutRect.rect_h;

    if (g_pstGfxGPDevice[enGpId]->bInterface) {
        // u32Ratio *= 2;
    }

    if (u32Ratio >= OPTM_EXTRACTLINE_RATIO && !IS_SLAVER_GP(enGpId)) {
        /* g_pstGfxGPDevice[enGpId]->bNeedExtractLine = HI_TRUE; */
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        g_pstGfxGPDevice[enGpId]->bNeedExtractLine = HI_FALSE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if (stInputRect.w && stInputRect.h) {
        DRV_HIFB_ADP_SetGpRect(enGpId, &stInputRect);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
* func        : DRV_HIFB_ADP_SetGpRect
* description : CNcomment: 设置GP RECT CNend\n
*                    (1)回写更新需要设置GP寄存器
                      (2)显示制式发生变化的时候
                      (3)输入分辨率发生变化的时候
                      (4)待机打开图层需要重新配置GP
* param[in]   : hi_void
* retval      : NA
* others:     : NA
***************************************************************************************/
#ifndef CONFIG_GFX_PQ
#define SHARPEN_RATIO 3
#define SHARPEN_MAX_WIDTH 1920
hi_s32 DRV_HIFB_ADP_SetGpRect(HIFB_GP_ID_E enGpId, const HIFB_RECT *pstInputRect)
{
    hi_bool bGfxSharpen = HI_FALSE;
    OPTM_VDP_DISP_RECT_S stGfxRect = {0};
    OPTM_ALG_GZME_DRV_PARA_S stZmeDrvPara = {0};
    OPTM_ALG_GZME_RTL_PARA_S stZmeRtlPara = {0};
    OPTM_ALG_GDTI_DRV_PARA_S stDtiDrvPara = {0};
    OPTM_ALG_GDTI_RTL_PARA_S stDtiRtlPara = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstInputRect, HI_FAILURE);

    g_pstGfxGPDevice[enGpId]->stInRect.rect_w = pstInputRect->w;
    g_pstGfxGPDevice[enGpId]->stInRect.rect_h = pstInputRect->h;

    OPTM_CheckGPMask_BYGPID(enGpId);

    stGfxRect.u32IWth = (hi_u32)g_pstGfxGPDevice[enGpId]->stInRect.rect_w & 0xfffffffe;
    stGfxRect.u32IHgt = (hi_u32)g_pstGfxGPDevice[enGpId]->stInRect.rect_h & 0xfffffffe;
    stGfxRect.u32OWth = (hi_u32)g_pstGfxGPDevice[enGpId]->stOutRect.rect_w & 0xfffffffe;
    stGfxRect.u32OHgt = (hi_u32)g_pstGfxGPDevice[enGpId]->stOutRect.rect_h & 0xfffffffe;

    stGfxRect.u32DXS = (hi_u32)g_pstGfxGPDevice[enGpId]->stOutRect.rect_x & 0xfffffffe;
    stGfxRect.u32DYS = (hi_u32)g_pstGfxGPDevice[enGpId]->stOutRect.rect_y & 0xfffffffe;

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OWth, stGfxRect.u32DXS, HI_FAILURE);
    stGfxRect.u32DXL = stGfxRect.u32OWth + stGfxRect.u32DXS;
    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OHgt, stGfxRect.u32DYS, HI_FAILURE);
    stGfxRect.u32DYL = stGfxRect.u32OHgt + stGfxRect.u32DYS;

    stGfxRect.u32VX = stGfxRect.u32DXS;
    stGfxRect.u32VY = stGfxRect.u32DYS;

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OWth, stGfxRect.u32VX, HI_FAILURE);
    stGfxRect.u32VXL = stGfxRect.u32OWth + stGfxRect.u32VX;
    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OHgt, stGfxRect.u32VY, HI_FAILURE);
    stGfxRect.u32VYL = stGfxRect.u32OHgt + stGfxRect.u32VY;

    if ((stGfxRect.u32IWth == 0) || (stGfxRect.u32IHgt == 0) || (stGfxRect.u32OWth == 0) || (stGfxRect.u32OHgt == 0)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (g_pstGfxGPDevice[enGpId]->bNeedExtractLine == HI_TRUE) {
        stGfxRect.u32IHgt /= 2;
    }

    drv_hifb_hal_gp_set_layer_reso(g_pstGfxGPDevice[enGpId]->enGpHalId, &stGfxRect);

    stZmeDrvPara.bZmeFrmFmtIn = HI_TRUE;
    stZmeDrvPara.bZmeFrmFmtOut = g_pstGfxGPDevice[enGpId]->bInterface ? HI_FALSE : HI_TRUE;

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if (IS_SLAVER_GP(enGpId)) {
        stGfxRect.u32IWth = g_stGfxWbc2.stInRect.w & 0xfffffffe;
        stGfxRect.u32IHgt = g_stGfxWbc2.stInRect.h & 0xfffffffe;

        if (stGfxRect.u32IWth == 0 || stGfxRect.u32IHgt == 0) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_SUCCESS;
        }

        if (HIFB_STEREO_SIDEBYSIDE_HALF == g_pstGfxGPDevice[OPTM_SLAVER_GPID]->enTriDimMode) {
            stGfxRect.u32IWth /= 2;
        } else if (HIFB_STEREO_TOPANDBOTTOM == g_pstGfxGPDevice[OPTM_SLAVER_GPID]->enTriDimMode) {
            stGfxRect.u32IHgt /= 2;
        }

        if ((g_pstGfxGPDevice[OPTM_MASTER_GPID]->bNeedExtractLine == HI_TRUE)) {
            stGfxRect.u32IHgt /= 2;
        }

        DRV_HIFB_WBC_SetCropReso(stGfxRect);
    }
#endif
    stZmeDrvPara.u32ZmeFrmWIn = stGfxRect.u32IWth;
    stZmeDrvPara.u32ZmeFrmHIn = stGfxRect.u32IHgt;
    stZmeDrvPara.u32ZmeFrmWOut = stGfxRect.u32OWth;
    stZmeDrvPara.u32ZmeFrmHOut = stGfxRect.u32OHgt;

    if (OPTM_DISPCHANNEL_1 == g_pstGfxGPDevice[enGpId]->enDispCh) {
        stZmeDrvPara.u32ZmeHdDeflicker = g_pstGfxGPDevice[enGpId]->u32ZmeDeflicker;
        OPTM_ALG_GZmeHDSet(&gs_stGPZme, &stZmeDrvPara, &stZmeRtlPara);
    } else if (OPTM_DISPCHANNEL_0 == g_pstGfxGPDevice[enGpId]->enDispCh) {
        stZmeDrvPara.u32ZmeSdDeflicker = g_pstGfxGPDevice[enGpId]->u32ZmeDeflicker;
        OPTM_ALG_GZmeSDSet(&gs_stGPZme, &stZmeDrvPara, &stZmeRtlPara);
    } else {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enGpId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, g_pstGfxGPDevice[enGpId]->enDispCh);
        return HI_FAILURE;
    }

    stDtiDrvPara.u32ZmeFrmWIn = stGfxRect.u32IWth;
    stDtiDrvPara.u32ZmeFrmHIn = stGfxRect.u32IHgt;
    stDtiDrvPara.u32ZmeFrmWOut = stGfxRect.u32OWth;
    stDtiDrvPara.u32ZmeFrmHOut = stGfxRect.u32OHgt;

    OPTM_ALG_GDtiSet(&stDtiDrvPara, &stDtiRtlPara);

    if (stZmeDrvPara.u32ZmeFrmWIn > SHARPEN_MAX_WIDTH) {
        stZmeRtlPara.bZmeEnH = HI_FALSE;
        stZmeRtlPara.bZmeEnV = HI_FALSE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    /* zme enable horizontal */
    DRV_HIFB_HAL_GP_SetZmeEnable(enGpId, VDP_ZME_MODE_HOR, stZmeRtlPara.bZmeEnH);
    DRV_HIFB_HAL_GP_SetZmeEnable(enGpId, VDP_ZME_MODE_VER, stZmeRtlPara.bZmeEnV);

    if (stZmeRtlPara.bZmeEnH && stZmeRtlPara.bZmeEnV) {
        bGfxSharpen = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, stDtiDrvPara.u32ZmeFrmWOut, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, stDtiDrvPara.u32ZmeFrmHOut, HI_FAILURE);
    if ((stDtiDrvPara.u32ZmeFrmWIn * 2 / stDtiDrvPara.u32ZmeFrmWOut > SHARPEN_RATIO) ||
        (stDtiDrvPara.u32ZmeFrmHIn * 2 / stDtiDrvPara.u32ZmeFrmHOut > SHARPEN_RATIO)) {
        bGfxSharpen = HI_FALSE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    HIFB_ADP_SetZme(enGpId, bGfxSharpen, &stZmeRtlPara, &stDtiRtlPara);

    drv_hifb_hal_gp_set_para_upd(g_pstGfxGPDevice[enGpId]->enGpHalId, VDP_ZME_MODE_HOR);
    drv_hifb_hal_gp_set_para_upd(g_pstGfxGPDevice[enGpId]->enGpHalId, VDP_ZME_MODE_VER);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static inline hi_void HIFB_ADP_SetZme(HIFB_GP_ID_E enGpId, hi_bool bGfxSharpen, OPTM_ALG_GZME_RTL_PARA_S *pstZmeRtlPara,
                                      OPTM_ALG_GDTI_RTL_PARA_S *pstDtiRtlPara)
{
    hi_bool bSlvGp = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (IS_SLAVER_GP(enGpId)) {
        bSlvGp = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstZmeRtlPara);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDtiRtlPara);

    if ((HI_FALSE == pstZmeRtlPara->bZmeEnH) && (HI_FALSE == pstZmeRtlPara->bZmeEnV)) {
        DRV_HIFB_HAL_GP_SetTiEnable(enGpId, VDP_TI_MODE_CHM, HI_FALSE);
        DRV_HIFB_HAL_GP_SetTiEnable(enGpId, VDP_TI_MODE_LUM, HI_FALSE);
        DRV_HIFB_HAL_GP_SetIpOrder(enGpId, bSlvGp, VDP_GP_ORDER_CSC);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    DRV_HIFB_HAL_GP_SetIpOrder(enGpId, bSlvGp, VDP_GP_ORDER_ZME_CSC);
    DRV_HIFB_HAL_GP_SetZmeHfirOrder(enGpId, VDP_ZME_ORDER_HV);
    DRV_HIFB_HAL_GP_SetZmeCoefAddr(enGpId, VDP_GP_PARA_ZME_HOR, pstZmeRtlPara->u32ZmeCoefAddrHL);
    DRV_HIFB_HAL_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_HOR, pstZmeRtlPara->bZmeMdHLC);
    DRV_HIFB_HAL_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_ALPHA, pstZmeRtlPara->bZmeMdHA);
    DRV_HIFB_HAL_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_ALPHA, pstZmeRtlPara->bZmeMedHA);
    DRV_HIFB_HAL_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_HORL, pstZmeRtlPara->bZmeMedHL);
    DRV_HIFB_HAL_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_HORC, pstZmeRtlPara->bZmeMedHC);
    DRV_HIFB_HAL_GP_SetZmePhase(enGpId, VDP_ZME_MODE_HORL, pstZmeRtlPara->s32ZmeOffsetHL);
    DRV_HIFB_HAL_GP_SetZmePhase(enGpId, VDP_ZME_MODE_HORC, pstZmeRtlPara->s32ZmeOffsetHC);
    DRV_HIFB_HAL_GP_SetZmeHorRatio(enGpId, pstZmeRtlPara->u32ZmeRatioHL);
    DRV_HIFB_HAL_GP_SetZmeCoefAddr(enGpId, VDP_GP_PARA_ZME_VER, pstZmeRtlPara->u32ZmeCoefAddrVL);
    DRV_HIFB_HAL_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_VER, pstZmeRtlPara->bZmeMdVLC);
    DRV_HIFB_HAL_GP_SetZmeFirEnable(enGpId, VDP_ZME_MODE_ALPHAV, pstZmeRtlPara->bZmeMdVA);
    DRV_HIFB_HAL_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_ALPHAV, pstZmeRtlPara->bZmeMedVA);
    DRV_HIFB_HAL_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_VERL, pstZmeRtlPara->bZmeMedVL);
    DRV_HIFB_HAL_GP_SetZmeMidEnable(enGpId, VDP_ZME_MODE_VERC, pstZmeRtlPara->bZmeMedVC);
    DRV_HIFB_HAL_GP_SetZmePhase(enGpId, VDP_ZME_MODE_VERL, pstZmeRtlPara->s32ZmeOffsetVBtm);
    DRV_HIFB_HAL_GP_SetZmePhase(enGpId, VDP_ZME_MODE_VERC, pstZmeRtlPara->s32ZmeOffsetVTop);
    DRV_HIFB_HAL_GP_SetZmeVerRatio(enGpId, pstZmeRtlPara->u32ZmeRatioVL);

    if (OPTM_DISPCHANNEL_1 == g_pstGfxGPDevice[enGpId]->enDispCh && bGfxSharpen) {
        DRV_HIFB_HAL_GP_SetTiEnable(enGpId, VDP_TI_MODE_CHM, pstDtiRtlPara->bEnCTI);
        DRV_HIFB_HAL_GP_SetTiEnable(enGpId, VDP_TI_MODE_LUM, pstDtiRtlPara->bEnLTI);
        DRV_HIFB_HAL_GP_SetZmeHfirOrder(enGpId, VDP_ZME_ORDER_VH);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        DRV_HIFB_HAL_GP_SetTiEnable(enGpId, VDP_TI_MODE_CHM, HI_FALSE);
        DRV_HIFB_HAL_GP_SetTiEnable(enGpId, VDP_TI_MODE_LUM, HI_FALSE);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    DRV_HIFB_HAL_GP_SetTiHpCoef(enGpId, VDP_TI_MODE_CHM, (hi_s32 *)pstDtiRtlPara->s32CTIHPTmp);
    DRV_HIFB_HAL_GP_SetTiHpCoef(enGpId, VDP_TI_MODE_LUM, (hi_s32 *)pstDtiRtlPara->s32LTIHPTmp);
    DRV_HIFB_HAL_GP_SetTiGainRatio(enGpId, VDP_TI_MODE_CHM, (hi_s32)pstDtiRtlPara->s16CTICompsatRatio);
    DRV_HIFB_HAL_GP_SetTiGainRatio(enGpId, VDP_TI_MODE_LUM, (hi_s32)pstDtiRtlPara->s16LTICompsatRatio);
    DRV_HIFB_HAL_GP_SetTiCoringThd(enGpId, VDP_TI_MODE_CHM, (hi_u32)pstDtiRtlPara->u16CTICoringThrsh);
    DRV_HIFB_HAL_GP_SetTiCoringThd(enGpId, VDP_TI_MODE_LUM, (hi_u32)pstDtiRtlPara->u16LTICoringThrsh);
    DRV_HIFB_HAL_GP_SetTiSwingThd(enGpId, VDP_TI_MODE_CHM, (hi_u32)pstDtiRtlPara->u16CTIOverSwingThrsh,
                                  (hi_u32)pstDtiRtlPara->u16CTIUnderSwingThrsh);
    DRV_HIFB_HAL_GP_SetTiSwingThd(enGpId, VDP_TI_MODE_LUM, (hi_u32)pstDtiRtlPara->u16LTIOverSwingThrsh,
                                  (hi_u32)pstDtiRtlPara->u16LTIUnderSwingThrsh);
    DRV_HIFB_HAL_GP_SetTiMixRatio(enGpId, VDP_TI_MODE_CHM, (hi_u32)pstDtiRtlPara->u8CTIMixingRatio);
    DRV_HIFB_HAL_GP_SetTiMixRatio(enGpId, VDP_TI_MODE_LUM, (hi_u32)pstDtiRtlPara->u8LTIMixingRatio);
    DRV_HIFB_HAL_GP_SetTiHfThd(enGpId, VDP_TI_MODE_LUM, (hi_u32 *)pstDtiRtlPara->u32LTIHFreqThrsh);
    DRV_HIFB_HAL_GP_SetTiGainCoef(enGpId, VDP_TI_MODE_LUM, (hi_u32 *)pstDtiRtlPara->u32LTICompsatMuti);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 hifb_set_wbc_gp_zme(HIFB_GP_ID_E gp_id)
{
    hi_bool sharp_en = HI_FALSE;
    OPTM_VDP_DISP_RECT_S rect = {0};
    OPTM_ALG_GZME_DRV_PARA_S zme_drv_para = {0};
    OPTM_ALG_GZME_RTL_PARA_S me_rtl_para = {0};
    OPTM_ALG_GDTI_DRV_PARA_S dti_drv_para = {0};
    OPTM_ALG_GDTI_RTL_PARA_S dti_rtl_para = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(gp_id, HI_FAILURE);

    rect.u32IWth = g_stGfxWbc2.stInRect.w & 0xfffffffe;
    rect.u32IHgt = g_stGfxWbc2.stInRect.h & 0xfffffffe;
    rect.u32OWth = g_pstGfxGPDevice[gp_id]->stOutRect.rect_w & 0xfffffffe;
    rect.u32OHgt = g_pstGfxGPDevice[gp_id]->stOutRect.rect_h & 0xfffffffe;

    rect.u32DXS = g_pstGfxGPDevice[gp_id]->stOutRect.rect_x & 0xfffffffe;
    rect.u32DYS = g_pstGfxGPDevice[gp_id]->stOutRect.rect_y & 0xfffffffe;
    rect.u32DXL = rect.u32OWth + rect.u32DXS;
    rect.u32DYL = rect.u32OHgt + rect.u32DYS;
    rect.u32VX = rect.u32DXS;
    rect.u32VY = rect.u32DYS;
    rect.u32VXL = rect.u32OWth + rect.u32VX;
    rect.u32VYL = rect.u32OHgt + rect.u32VY;

    if ((rect.u32IWth == 0) || (rect.u32IHgt == 0) || (rect.u32OWth == 0) || (rect.u32OHgt == 0)) {
        return HI_SUCCESS;
    }

    if (g_pstGfxGPDevice[gp_id]->bNeedExtractLine == HI_TRUE) {
        rect.u32IHgt /= 2;
    }

    /*  Frame format for zme : 0-field; 1-frame */
    zme_drv_para.bZmeFrmFmtIn = HI_TRUE;
    zme_drv_para.bZmeFrmFmtOut = g_pstGfxGPDevice[gp_id]->bInterface ? HI_FALSE : HI_TRUE;
    zme_drv_para.u32ZmeFrmWIn = rect.u32IWth;
    zme_drv_para.u32ZmeFrmHIn = rect.u32IHgt;
    zme_drv_para.u32ZmeFrmWOut = rect.u32OWth;
    zme_drv_para.u32ZmeFrmHOut = rect.u32OHgt;

    if (g_pstGfxGPDevice[gp_id]->enDispCh == OPTM_DISPCHANNEL_1) {
        zme_drv_para.u32ZmeHdDeflicker = g_pstGfxGPDevice[gp_id]->u32ZmeDeflicker;
        OPTM_ALG_GZmeHDSet(&gs_stGPZme, &zme_drv_para, &me_rtl_para);
    } else if (g_pstGfxGPDevice[gp_id]->enDispCh == OPTM_DISPCHANNEL_0) {
        zme_drv_para.u32ZmeSdDeflicker = g_pstGfxGPDevice[gp_id]->u32ZmeDeflicker;
        OPTM_ALG_GZmeSDSet(&gs_stGPZme, &zme_drv_para, &me_rtl_para);
    } else {
        return HI_FAILURE;
    }

    dti_drv_para.u32ZmeFrmWIn = rect.u32IWth;
    dti_drv_para.u32ZmeFrmHIn = rect.u32IHgt;
    dti_drv_para.u32ZmeFrmWOut = rect.u32OWth;
    dti_drv_para.u32ZmeFrmHOut = rect.u32OHgt;

    OPTM_ALG_GDtiSet(&dti_drv_para, &dti_rtl_para);

    if (zme_drv_para.u32ZmeFrmWIn > SHARPEN_MAX_WIDTH) {
        me_rtl_para.bZmeEnH = HI_FALSE;
        me_rtl_para.bZmeEnV = HI_FALSE;
    }

    /* zme enable horizontal */
    DRV_HIFB_HAL_GP_SetZmeEnable(gp_id, VDP_ZME_MODE_HOR, me_rtl_para.bZmeEnH);
    DRV_HIFB_HAL_GP_SetZmeEnable(gp_id, VDP_ZME_MODE_VER, me_rtl_para.bZmeEnV);

    if (me_rtl_para.bZmeEnH && me_rtl_para.bZmeEnV) {
        sharp_en = HI_TRUE;
    }

    if ((((dti_drv_para.u32ZmeFrmWIn * 2) / dti_drv_para.u32ZmeFrmWOut) > SHARPEN_RATIO) ||
        (((dti_drv_para.u32ZmeFrmHIn * 2) / dti_drv_para.u32ZmeFrmHOut) > SHARPEN_RATIO)) {
        sharp_en = HI_FALSE;
    }

    HIFB_ADP_SetZme(gp_id, sharp_en, &me_rtl_para, &dti_rtl_para);

    drv_hifb_hal_gp_set_para_upd(g_pstGfxGPDevice[gp_id]->enGpHalId, VDP_ZME_MODE_HOR);
    drv_hifb_hal_gp_set_para_upd(g_pstGfxGPDevice[gp_id]->enGpHalId, VDP_ZME_MODE_VER);

    return HI_SUCCESS;
}
#else
static hi_void OPTM_GfxGetPqGpFromDispChannel(const OPTM_DISPCHANNEL_E enChannel, hi_drv_pq_gfx_layer *pPqGp)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pPqGp);

    if (OPTM_DISPCHANNEL_1 == enChannel) {
        *pPqGp = HI_DRV_PQ_GFX_LAYER_GP0;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else if (OPTM_DISPCHANNEL_0 == enChannel) {
        *pPqGp = HI_DRV_PQ_GFX_LAYER_GP1;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enChannel);
        *pPqGp = HI_DRV_PQ_GFX_LAYER_MAX;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void hifb_gp_split_zme_set_node(hi_drv_pq_gfx_zme_out *zme_out, hifb_gp0_zme_split_node_cfg *node)
{
    node->iw = zme_out->gfx_zme_common_out.zme_width_in;
    node->ow = zme_out->gfx_zme_common_out.zme_width_out;
    node->hor_loffset = zme_out->gfx_zme_common_out.zme_offset.zme_offset_hl;
    node->hor_coffset = zme_out->gfx_zme_common_out.zme_offset.zme_offset_hc;
    node->hratio = zme_out->gfx_zme_common_out.zme_ratio.zme_ratio_h;
    node->ow_per = node->ow / (HIFB_GP0_ZME_SPLIT_NUM - 1);
    node->ow_align = node->ow_per / 8 * 8; /* align 8 */
    node->overlap = 8;                     /* overlay is 8 */

    node->pre_xst_pos_cord_in_int_align2 = 0;
}

hi_void hifb_gp_split_zme(hi_drv_pq_gfx_zme_out *zme_out, hifb_gp0_zme_split_cfg *out_cfg, hi_u32 size)
{
    hi_u32 i = 0;
    hifb_gp0_zme_split_node_cfg node = {0};

    if (size > HIFB_GP0_ZME_SPLIT_NUM) {
        return;
    }
    hifb_gp_split_zme_set_node(zme_out, &node);

    for (i = 0; i < HIFB_GP0_ZME_SPLIT_NUM - 1; i++) {
        node.xst_pos_cord = i * node.ow_align;
        node.xed_pos_cord = (i == 3) ? (node.ow - 1) : ((i + 1) * node.ow_align - 1); /* node 3 */
        node.node_cfg_zme_ow[i] = (node.xed_pos_cord - node.xst_pos_cord + 1);
        node.xst_pos_cord_in = node.hor_loffset + ((hi_u64)node.xst_pos_cord * node.hratio);
        node.xst_pos_cord_in_int = node.xst_pos_cord_in >> 20;                   /* 20 is precision */
        node.xst_pos_cord_in_int_align2[i] = (node.xst_pos_cord_in_int) / 2 * 2; /* align 2 */
        node.xst_pos_cord_in_overlap = (i == 0) ? 0
                                                : (node.xst_pos_cord_in_int_align2[i] - ((i == 0) ? 0 : node.overlap));
        node.loffset = node.xst_pos_cord_in - (node.xst_pos_cord_in_overlap << 20); /* 20 is precision */

        node.hcratio = node.hratio / 2; /* div 2 */
        node.c_xst_pos_cord_in = node.hor_coffset + ((hi_u64)node.xst_pos_cord * node.hcratio);
        node.c_xst_pos_cord_in_overlap = node.xst_pos_cord_in_overlap / 2;              /* div 2 */
        node.coffset = node.c_xst_pos_cord_in - (node.c_xst_pos_cord_in_overlap << 20); /* 20 is precision */

        node.node_cfg_hor_loffset[i] = node.loffset;
        node.node_cfg_hor_coffset[i] = node.coffset;
    }

    for (i = 0; i < HIFB_GP0_ZME_SPLIT_NUM - 1; i++) {
        if (i == 3) {                                                                              /* node 3 */
            node.node_cfg_zme_iw[i] = node.iw - node.xst_pos_cord_in_int_align2[3] + node.overlap; /* node 3 */
        } else if (i != 0) {
            node.node_cfg_zme_iw[i] = node.xst_pos_cord_in_int_align2[i + 1] - node.xst_pos_cord_in_int_align2[i] +
                                      node.overlap * 2; /* multiply 2 */
        } else {
            node.node_cfg_zme_iw[i] = node.xst_pos_cord_in_int_align2[i + 1] - 0 + node.overlap;
        }
    }

    out_cfg[0].zme_iw = node.iw - 1;
    out_cfg[0].zme_ow = node.ow - 1;
    out_cfg[0].hor_loffset = node.hor_loffset;
    out_cfg[0].hor_coffset = node.hor_coffset;

    for (i = 0; i < HIFB_GP0_ZME_SPLIT_NUM - 1; i++) {
        out_cfg[i + 1].zme_iw = node.node_cfg_zme_iw[i] - 1;
        out_cfg[i + 1].zme_ow = node.node_cfg_zme_ow[i] - 1;
        out_cfg[i + 1].hor_loffset = node.node_cfg_hor_loffset[i];
        out_cfg[i + 1].hor_coffset = node.node_cfg_hor_coffset[i];
    }
}

hi_void hifb_set_gp_zme(HIFB_GP_ID_E gpid, hi_drv_pq_gfx_zme_out *zme_out)
{
    hifb_gp0_zme_split_cfg out_cfg[HIFB_GP0_ZME_SPLIT_NUM] = {{0}};
    hi_u32 i;

    hifb_gp_split_zme(zme_out, out_cfg, sizeof(out_cfg) / sizeof(hifb_gp0_zme_split_cfg));

    hifb_hal_gp_set_zme_ckgt_en(gpid, 1);
    hifb_hal_gp_set_zme_hsc_en(gpid, zme_out->gfx_zme_common_out.zme_fir_en.zme_h_en);
    hifb_hal_gp_set_zme_hamid_en(gpid, zme_out->gfx_zme_common_out.zme_med_fir_en.zme_med_ha_en);
    hifb_hal_gp_set_zme_hlmid_en(gpid, zme_out->gfx_zme_common_out.zme_med_fir_en.zme_med_hl_en);
    hifb_hal_gp_set_zme_hchmid_en(gpid, zme_out->gfx_zme_common_out.zme_med_fir_en.zme_med_hc_en);
    hifb_hal_gp_set_zme_hfir_en(gpid, zme_out->gfx_zme_common_out.zme_fir_mode.zme_fir_h == HI_DRV_PQ_ZME_COPY ? 0 : 1);
    hifb_hal_gp_set_zme_hafir_en(gpid,
                                 zme_out->gfx_zme_common_out.zme_fir_mode.zme_fir_ha == HI_DRV_PQ_ZME_COPY ? 0 : 1);
    hifb_hal_gp_set_zme_hfir_order(gpid, zme_out->gfx_zme_common_out.zme_order == HI_DRV_PQ_ZME_VER_HOR ? 1 : 0);
    hifb_hal_gp_set_zme_hratio(gpid, zme_out->gfx_zme_common_out.zme_ratio.zme_ratio_h);
    hifb_hal_gp_set_zme_out_height(gpid, zme_out->gfx_zme_common_out.zme_height_out - 1);
    hifb_hal_gp_set_zme_vsc_en(gpid, zme_out->gfx_zme_common_out.zme_fir_en.zme_v_en);
    hifb_hal_gp_set_zme_vamid_en(gpid, zme_out->gfx_zme_common_out.zme_med_fir_en.zme_med_va_en);
    hifb_hal_gp_set_zme_vlmid_en(gpid, zme_out->gfx_zme_common_out.zme_med_fir_en.zme_med_vl_en);
    hifb_hal_gp_set_zme_vchmid_en(gpid, zme_out->gfx_zme_common_out.zme_med_fir_en.zme_med_vc_en);
    hifb_hal_gp_set_zme_vfir_en(gpid, zme_out->gfx_zme_common_out.zme_fir_mode.zme_fir_v == HI_DRV_PQ_ZME_COPY ? 0 : 1);
    hifb_hal_gp_set_zme_vafir_en(gpid,
                                 zme_out->gfx_zme_common_out.zme_fir_mode.zme_fir_va == HI_DRV_PQ_ZME_COPY ? 0 : 1);
    hifb_hal_gp_set_zme_vratio(gpid, zme_out->gfx_zme_common_out.zme_ratio.zme_ratio_v);
    hifb_hal_gp_set_zme_vtp_offset(gpid, zme_out->gfx_zme_common_out.zme_offset.zme_offset_v_tp);
    hifb_hal_gp_set_zme_vbtm_offset(gpid, zme_out->gfx_zme_common_out.zme_offset.zme_offset_v_btm);

    for (i = 0; i < HIFB_GP0_ZME_SPLIT_NUM; i++) {
        if (gpid == HIFB_ADP_GP1 && i != 0) {
            break;
        }
        hifb_hal_gp_set_zme_in_width(gpid, out_cfg[i].zme_iw, i);
        hifb_hal_gp_set_zme_out_width(gpid, out_cfg[i].zme_ow, i);
        hifb_hal_gp_set_zme_hor_loffset(gpid, out_cfg[i].hor_loffset, i);
        hifb_hal_gp_set_zme_hor_coffset(gpid, out_cfg[i].hor_coffset, i);
    }

    hifb_hal_gp_set_zme_coef_zme(gpid, zme_out->zme_coef_addr.zme_coef_hl_addr);

    hifb_hal_gp_set_para_up_zme(gpid);
}

/* for gp0 is in_rect, but for wbc is out_rect */
hi_s32 DRV_HIFB_ADP_SetGpRect(HIFB_GP_ID_E enGpId, const HIFB_RECT *pstInputRect)
{
    OPTM_VDP_DISP_RECT_S stGfxRect = {0};
    hi_drv_pq_gfx_zme_in gfx_zme_in = {0};
    hi_drv_pq_gfx_zme_out gfx_zme_out = {{0}};
    hi_drv_rect stOutRect = {0};
    hi_drv_pq_gfx_layer enPqGfxGp = HI_DRV_PQ_GFX_LAYER_MAX;
#ifdef CHIP_TYPE_hi3798cv200
    HI_DRV_DISP_FMT_E enDispFmt = HI_DRV_DISP_FMT_1080P_50;
#ifdef HI_BUILD_IN_BOOT
    hi_disp_display_info stDispInfo;
    hi_s32 ret = 0;
#endif
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstInputRect, HI_FAILURE);

#ifdef CHIP_TYPE_hi3798cv200
#ifdef HI_BUILD_IN_BOOT
    memset(&stDispInfo, 0, sizeof(stDispInfo));
    if (enGpId == HIFB_ADP_GP0) {
        ret = HI_DISP_GetDisplayInfo(HI_DRV_DISPLAY_0, &stDispInfo);
    } else {
        ret = HI_DISP_GetDisplayInfo(HI_DRV_DISPLAY_1, &stDispInfo);
    }
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_DISP_GetDisplayInfo, ret);
        return HI_FAILURE;
    }
    g_pstGfxGPDevice[enGpId]->enDispFmt = stDispInfo.fmt;
#endif
    enDispFmt = g_pstGfxGPDevice[enGpId]->enDispFmt;
#endif

    g_pstGfxGPDevice[enGpId]->stInRect.rect_w = pstInputRect->w;
    g_pstGfxGPDevice[enGpId]->stInRect.rect_h = pstInputRect->h;

    if ((0 == g_pstGfxGPDevice[enGpId]->stOutRect.rect_w) || (0 == g_pstGfxGPDevice[enGpId]->stOutRect.rect_h)) {
        return HI_FAILURE;
    }
    stOutRect.rect_x = g_pstGfxGPDevice[enGpId]->stOutRect.rect_x;
    stOutRect.rect_y = g_pstGfxGPDevice[enGpId]->stOutRect.rect_y;
    stOutRect.rect_w = g_pstGfxGPDevice[enGpId]->stOutRect.rect_w;
    stOutRect.rect_h = g_pstGfxGPDevice[enGpId]->stOutRect.rect_h;

    OPTM_CheckGPMask_BYGPID(enGpId);

    if ((stOutRect.rect_x > CONFIG_HIFB_LAYER_MAXWIDTH) || (stOutRect.rect_y > CONFIG_HIFB_LAYER_MAXHEIGHT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stOutRect.rect_x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stOutRect.rect_y);
        return HI_FAILURE;
    }

    if ((stOutRect.rect_w == 0) || (stOutRect.rect_w > GRAPHIC_DISPLAY_MAX_WIDTH)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stOutRect.rect_w);
        return HI_FAILURE;
    }

    if ((stOutRect.rect_h == 0) || (stOutRect.rect_h > GRAPHIC_DISPLAY_MAX_HIGHT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stOutRect.rect_w);
        return HI_FAILURE;
    }

#ifdef CHIP_TYPE_hi3798cv200
    if ((HIFB_ADP_GP0 == enGpId) &&
        ((HI_DRV_DISP_FMT_PAL <= enDispFmt) && (HI_DRV_DISP_FMT_1440x480i_60 >= enDispFmt))) {
        stOutRect.rect_x = g_pstGfxGPDevice[enGpId]->stOutRect.rect_x * 2;
        stOutRect.rect_w = g_pstGfxGPDevice[enGpId]->stOutRect.rect_w * 2;
        stOutRect.rect_y = g_pstGfxGPDevice[enGpId]->stOutRect.rect_y;
        stOutRect.rect_h = g_pstGfxGPDevice[enGpId]->stOutRect.rect_h;
    }
#endif

    stGfxRect.u32IWth = (hi_u32)g_pstGfxGPDevice[enGpId]->stInRect.rect_w & 0xfffffffe;
    stGfxRect.u32IHgt = (hi_u32)g_pstGfxGPDevice[enGpId]->stInRect.rect_h & 0xfffffffe;
    stGfxRect.u32OWth = (hi_u32)stOutRect.rect_w & 0xfffffffe;
    stGfxRect.u32OHgt = (hi_u32)stOutRect.rect_h & 0xfffffffe;

    stGfxRect.u32DXS = (hi_u32)stOutRect.rect_x & 0xfffffffe;
    stGfxRect.u32DYS = (hi_u32)stOutRect.rect_y & 0xfffffffe;

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OWth, stGfxRect.u32DXS, HI_FAILURE);
    stGfxRect.u32DXL = stGfxRect.u32OWth + stGfxRect.u32DXS;
    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OHgt, stGfxRect.u32DYS, HI_FAILURE);
    stGfxRect.u32DYL = stGfxRect.u32OHgt + stGfxRect.u32DYS;

    stGfxRect.u32VX = stGfxRect.u32DXS;
    stGfxRect.u32VY = stGfxRect.u32DYS;

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OWth, stGfxRect.u32VX, HI_FAILURE);
    stGfxRect.u32VXL = stGfxRect.u32OWth + stGfxRect.u32VX;
    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(stGfxRect.u32OHgt, stGfxRect.u32VY, HI_FAILURE);
    stGfxRect.u32VYL = stGfxRect.u32OHgt + stGfxRect.u32VY;

    if (g_pstGfxGPDevice[enGpId]->bNeedExtractLine == HI_TRUE) {
        stGfxRect.u32IHgt /= 2;
    }

    if ((0 == stGfxRect.u32IWth) || (0 == stGfxRect.u32IHgt) || (0 == stGfxRect.u32OWth) || (0 == stGfxRect.u32OHgt)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    drv_hifb_hal_gp_set_layer_reso(g_pstGfxGPDevice[enGpId]->enGpHalId, &stGfxRect);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if (IS_SLAVER_GP(enGpId)) {
        stGfxRect.u32IWth = (hi_u32)g_stGfxWbc2.stInRect.w & 0xfffffffe;
        stGfxRect.u32IHgt = (hi_u32)g_stGfxWbc2.stInRect.h & 0xfffffffe;
        if ((0 == stGfxRect.u32IWth) || (0 == stGfxRect.u32IHgt)) {
            return HI_SUCCESS;
        }

        if (HIFB_STEREO_SIDEBYSIDE_HALF == g_pstGfxGPDevice[OPTM_SLAVER_GPID]->enTriDimMode) {
            stGfxRect.u32IWth /= 2;
        } else if (HIFB_STEREO_TOPANDBOTTOM == g_pstGfxGPDevice[OPTM_SLAVER_GPID]->enTriDimMode) {
            stGfxRect.u32IHgt /= 2;
        }

        if (HI_TRUE == g_pstGfxGPDevice[OPTM_MASTER_GPID]->bNeedExtractLine) {
            stGfxRect.u32IHgt /= 2;
        }

        DRV_HIFB_WBC_SetCropReso(stGfxRect);
    }
#endif
    /* *set zme, if wbc mode, should not set gp1 zme * */

    OPTM_GfxGetPqGpFromDispChannel(g_pstGfxGPDevice[enGpId]->enDispCh, &enPqGfxGp);
    if (enPqGfxGp == HI_DRV_PQ_GFX_LAYER_MAX) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, OPTM_GfxGetPqGpFromDispChannel, enPqGfxGp);
        return HI_FAILURE;
    }

    drv_hifb_hal_gp_set_para_upd(g_pstGfxGPDevice[enGpId]->enGpHalId, VDP_GP_PARA_ZME_HOR);
    drv_hifb_hal_gp_set_para_upd(g_pstGfxGPDevice[enGpId]->enGpHalId, VDP_GP_PARA_ZME_VER);

    gfx_zme_in.zme_width_in = stGfxRect.u32IWth;
    gfx_zme_in.zme_height_in = stGfxRect.u32IHgt;
    gfx_zme_in.zme_width_out = stGfxRect.u32OWth;
    gfx_zme_in.zme_height_out = stGfxRect.u32OHgt;
    gfx_zme_in.is_deflicker = g_pstGfxGPDevice[enGpId]->u32ZmeDeflicker ? HI_FALSE : HI_TRUE;
    gfx_zme_in.zme_frame_in = HI_DRV_PQ_FRM_FRAME;
    gfx_zme_in.zme_frame_out = (g_pstGfxGPDevice[enGpId]->bInterface == HI_TRUE) ? HI_DRV_PQ_FRM_TOP_FIELD
                                                                                    : HI_DRV_PQ_FRM_FRAME;
    gfx_zme_in.is_ogeny = HI_FALSE;

#ifndef HI_BUILD_IN_BOOT
    if (IS_SLAVER_GP(enGpId)) {
        return HI_SUCCESS;
    }
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(gs_pstPqFuncs, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(gs_pstPqFuncs->pq_get_gfx_zme_strategy, HI_FAILURE);
    gs_pstPqFuncs->pq_get_gfx_zme_strategy(enPqGfxGp, &gfx_zme_in, &gfx_zme_out);
    hifb_set_gp_zme(enGpId, &gfx_zme_out);
#endif

#ifdef HI_BUILD_IN_BOOT
    DRV_PQ_SetGfxZme(enPqGfxGp, &stGfxZmePara);
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

hi_s32 hifb_set_wbc_gp_rect(HIFB_GP_ID_E gp_id, const HIFB_RECT *rect)
{
    OPTM_VDP_DISP_RECT_S gp_rect = {0};
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(gp_id, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(rect, HI_FAILURE);

    if ((rect->x > CONFIG_HIFB_LAYER_MAXWIDTH) || (rect->y > CONFIG_HIFB_LAYER_MAXHEIGHT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, rect->x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, rect->y);
        return HI_FAILURE;
    }

    if ((rect->w == 0) || (rect->w > GRAPHIC_DISPLAY_MAX_WIDTH)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, rect->w);
        return HI_FAILURE;
    }

    if ((rect->h == 0) || (rect->h > GRAPHIC_DISPLAY_MAX_HIGHT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, rect->h);
        return HI_FAILURE;
    }

    gp_rect.u32IWth = (hi_u32)rect->w & 0xfffffffe;
    gp_rect.u32IHgt = (hi_u32)rect->h & 0xfffffffe;
    gp_rect.u32OWth = (hi_u32)rect->w & 0xfffffffe;
    gp_rect.u32OHgt = (hi_u32)rect->h & 0xfffffffe;

    gp_rect.u32DXS = (hi_u32)rect->x & 0xfffffffe;
    gp_rect.u32DYS = (hi_u32)rect->y & 0xfffffffe;

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(gp_rect.u32OWth, gp_rect.u32DXS, HI_FAILURE);
    gp_rect.u32DXL = gp_rect.u32OWth + gp_rect.u32DXS;
    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(gp_rect.u32OHgt, gp_rect.u32DYS, HI_FAILURE);
    gp_rect.u32DYL = gp_rect.u32OHgt + gp_rect.u32DYS;

    gp_rect.u32VX = gp_rect.u32DXS;
    gp_rect.u32VY = gp_rect.u32DYS;

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(gp_rect.u32OWth, gp_rect.u32VX, HI_FAILURE);
    gp_rect.u32VXL = gp_rect.u32OWth + gp_rect.u32VX;
    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN(gp_rect.u32OHgt, gp_rect.u32VY, HI_FAILURE);
    gp_rect.u32VYL = gp_rect.u32OHgt + gp_rect.u32VY;

    if (g_pstGfxGPDevice[gp_id]->bNeedExtractLine == HI_TRUE) {
        gp_rect.u32IHgt /= 2;
    }

    drv_hifb_hal_gp_set_layer_reso(g_pstGfxGPDevice[gp_id]->enGpHalId, &gp_rect);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetOutRect
 * description : CNcomment: 获取输出分辨率 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
STATIC hi_s32 DRV_HIFB_ADP_GetOutRect(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOutputRect, HI_FAILURE);
    enGpId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    pstOutputRect->x = 0;
    pstOutputRect->y = 0;
    pstOutputRect->w = g_pstGfxGPDevice[enGpId]->stInRect.rect_w;
    pstOutputRect->h = g_pstGfxGPDevice[enGpId]->stInRect.rect_h;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetLayKeyMask
 * description  : CNcomment:设置图层color key 信息CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
STATIC hi_s32 DRV_HIFB_ADP_SetLayKeyMask(HIFB_LAYER_ID_E enLayerId, const HIFB_COLORKEYEX_S *pstColorkey)
{
    OPTM_VDP_GFX_CKEY_S ckey_info;
    OPTM_VDP_GFX_MASK_S ckey_mask;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    HIFB_CHECK_LAYER_OPEN(enLayerId);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->enGPId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstColorkey, HI_FAILURE);

    memset(&ckey_info, 0, sizeof(ckey_info));
    memset(&ckey_mask, 0, sizeof(ckey_mask));

    memcpy(&g_pstGfxDevice[enLayerId]->stColorkey, pstColorkey, sizeof(HIFB_COLORKEYEX_S));

    if (HI_TRUE == g_pstGfxGPDevice[g_pstGfxDevice[enLayerId]->enGPId]->bMaskFlag) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    ckey_info.bKeyMode = pstColorkey->u32KeyMode;

    if (g_pstGfxDevice[enLayerId]->CscState == OPTM_CSC_SET_PARA_BGR) {
        ckey_info.u32Key_r_min = pstColorkey->u8BlueMin;
        ckey_info.u32Key_g_min = pstColorkey->u8GreenMin;
        ckey_info.u32Key_b_min = pstColorkey->u8RedMin;

        ckey_info.u32Key_r_max = pstColorkey->u8BlueMax;
        ckey_info.u32Key_g_max = pstColorkey->u8GreenMax;
        ckey_info.u32Key_b_max = pstColorkey->u8RedMax;

        ckey_mask.u32Mask_r = pstColorkey->u8BlueMask;
        ckey_mask.u32Mask_g = pstColorkey->u8GreenMask;
        ckey_mask.u32Mask_b = pstColorkey->u8RedMask;
    } else {
        ckey_info.u32Key_r_min = pstColorkey->u8RedMin;
        ckey_info.u32Key_g_min = pstColorkey->u8GreenMin;
        ckey_info.u32Key_b_min = pstColorkey->u8BlueMin;

        ckey_info.u32Key_r_max = pstColorkey->u8RedMax;
        ckey_info.u32Key_g_max = pstColorkey->u8GreenMax;
        ckey_info.u32Key_b_max = pstColorkey->u8BlueMax;

        ckey_mask.u32Mask_r = pstColorkey->u8RedMask;
        ckey_mask.u32Mask_g = pstColorkey->u8GreenMask;
        ckey_mask.u32Mask_b = pstColorkey->u8BlueMask;
    }

    drv_hifb_hal_gfx_set_key_mask(g_pstGfxDevice[enLayerId]->enGfxHalId, &ckey_mask);
    drv_hifb_hal_gfx_set_color_key(g_pstGfxDevice[enLayerId]->enGfxHalId, pstColorkey->bKeyEnable, &ckey_info);

    drv_hifb_hal_gfx_set_reg_up(g_pstGfxDevice[enLayerId]->enGfxHalId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetLayerPreMult
 * description  : CNcomment: 设置图层预乘 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerPreMult(HIFB_LAYER_ID_E enLayerId, hi_bool bEnable)
{
    hi_bool bHdr = HI_FALSE;
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGpId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    g_pstGfxDevice[enLayerId]->bPreMute = bEnable;

    OPTM_CheckGPMask_BYGPID(enGpId);

    bHdr = g_pstGfxGPDevice[enGpId]->bHdr;

    drv_hifb_hal_gfx_set_pre_mult_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, bEnable, bHdr);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_void DRV_HIFB_ADP_GetLayerPreMult(HIFB_LAYER_ID_E enLayerId, hi_bool *pPreMul, hi_bool *pDePreMult)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    drv_hifb_hal_gfx_get_pre_mult_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, pPreMul, pDePreMult);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC hi_void DRV_HIFB_ADP_GetLayerHdr(HIFB_LAYER_ID_E enLayerId, hi_bool *pHdr)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    enGpId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    *pHdr = g_pstGfxGPDevice[enGpId]->bHdr;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC hi_void DRV_HIFB_ADP_GetClosePreMultState(HIFB_LAYER_ID_E LayerId, hi_bool *pbShouldClosePreMult)
{
#ifdef CHIP_TYPE_hi3798cv200
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pbShouldClosePreMult);

#ifdef CHIP_TYPE_hi3798cv200
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId);
    enGpId = HIFB_ADP_GetGpId(LayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);
    *pbShouldClosePreMult = g_pstGfxGPDevice[enGpId]->bHdr;
#else
    *pbShouldClosePreMult = HI_FALSE;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

STATIC hi_void DRV_HIFB_ADP_ReadRegister(hi_u32 Offset, hi_u32 *pRegBuf)
{
    drv_hifb_hal_gfx_read_register(Offset, pRegBuf);
}

STATIC hi_void DRV_HIFB_ADP_WriteRegister(hi_u32 Offset, hi_u32 Value)
{
    drv_hifb_hal_gfx_write_register(Offset, Value);
}

#ifndef HI_BUILD_IN_BOOT
STATIC hi_void DRV_HIFB_ADP_WhetherDiscardFrame(HIFB_LAYER_ID_E enLayerId, hi_bool *pNoDiscardFrame)
{
#ifndef CONFIG_HIFB_VERSION_1_0
    hi_s32 Ret = HI_SUCCESS;
    disp_export_func *DispExportFuncs = NULL;
    hi_disp_display_info stDispInfo;
    hi_drv_display enDisp;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (enLayerId < HIFB_LAYER_SD_0) {
        enDisp = HI_DRV_DISPLAY_0;
    } else {
        enDisp = HI_DRV_DISPLAY_1;
    }

    memset(&stDispInfo, 0x0, sizeof(stDispInfo));
    Ret = osal_exportfunc_get(HI_ID_DISP, (hi_void **)&DispExportFuncs);
    if ((HI_SUCCESS != Ret) || (NULL == DispExportFuncs)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "get disp func failure");
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_drv_module_get_function, Ret);
        return;
    }

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pNoDiscardFrame);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(DispExportFuncs->disp_get_displayinfo);

    Ret = DispExportFuncs->disp_get_displayinfo(enDisp, &stDispInfo);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, pfnDispGetDispInfo, Ret);
        return;
    }

    if ((HI_DRV_DISP_FMT_1080P_24 == stDispInfo.fmt) || (HI_DRV_DISP_FMT_1080P_25 == stDispInfo.fmt) ||
        (HI_DRV_DISP_FMT_1080P_30 == stDispInfo.fmt) || (HI_DRV_DISP_FMT_1080P_23_976 == stDispInfo.fmt) ||
        (HI_DRV_DISP_FMT_1080P_29_97 == stDispInfo.fmt) || (HI_DRV_DISP_FMT_3840X2160_24 == stDispInfo.fmt) ||
        (HI_DRV_DISP_FMT_3840X2160_25 == stDispInfo.fmt) || (HI_DRV_DISP_FMT_3840X2160_30 == stDispInfo.fmt) ||
        (HI_DRV_DISP_FMT_3840X2160_23_976 == stDispInfo.fmt) || (HI_DRV_DISP_FMT_3840X2160_29_97 == stDispInfo.fmt) ||
        (HI_DRV_DISP_FMT_4096X2160_24 == stDispInfo.fmt) || (HI_DRV_DISP_FMT_4096X2160_25 == stDispInfo.fmt) ||
        (HI_DRV_DISP_FMT_4096X2160_30 == stDispInfo.fmt)) {
        *pNoDiscardFrame = HI_FALSE;
    } else {
        *pNoDiscardFrame = HI_TRUE;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#else
#endif
    return;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetOSDData
 * description : CNcomment: 获取对应图层得OSD数据 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
STATIC hi_void DRV_HIFB_ADP_GetOSDData(HIFB_LAYER_ID_E enLayerId, HIFB_OSD_DATA_S *pstLayerData)
{
    hi_u32 u32Enable = HI_FALSE;
    hi_u32 u32KeyEnable = HI_FALSE;
    hi_u32 alpharange = 0;
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP_BUTT;
    OPTM_VDP_DISP_RECT_S stRect = {0};
    OPTM_VDP_DISP_RECT_S stInRect = {0};
    OPTM_VDP_GFX_MASK_S stckey_mask = {0};
    OPTM_VDP_GFX_CKEY_S stKey = {0};
    OPTM_VDP_GFX_IFMT_E enDataFmt = VDP_GFX_IFMT_BUTT;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    enGPId = HIFB_ADP_GetGpId(enLayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGPId);

    drv_hifb_hal_gfx_get_layer_enable(HIFB_ADP_GetGfxHalId(enLayerId), &u32Enable);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstLayerData);
    if (u32Enable) {
        pstLayerData->eState = HIFB_LAYER_STATE_ENABLE;
    } else {
        pstLayerData->eState = HIFB_LAYER_STATE_DISABLE;
    }

    /* * 获取surface 帧buffer地址 * */
    drv_hifb_hal_gfx_get_layer_addr(HIFB_ADP_GetGfxHalId(enLayerId), &pstLayerData->u32RegPhyAddr);

    drv_hifb_hal_gfx_get_layer_stride(HIFB_ADP_GetGfxHalId(enLayerId), &pstLayerData->u32Stride);

    drv_hifb_hal_gfx_get_layer_in_rect(HIFB_ADP_GetGfxHalId(enLayerId), &stInRect);
    pstLayerData->stInRect.x = stInRect.u32DXS;
    pstLayerData->stInRect.y = stInRect.u32DYS;
    pstLayerData->stInRect.w = stInRect.u32IWth;
    pstLayerData->stInRect.h = stInRect.u32IHgt;

    drv_hifb_hal_gp_get_rect(g_pstGfxGPDevice[enGPId]->enGpHalId, &stRect);

    pstLayerData->stOutRect.x = 0;
    pstLayerData->stOutRect.y = 0;
    pstLayerData->stOutRect.w = stRect.u32IWth;
    pstLayerData->stOutRect.h = stRect.u32IHgt;

    pstLayerData->DisplayWidth = stRect.u32OWth;
    pstLayerData->DisplayHeight = stRect.u32OHgt;

    drv_hifb_hal_wbc_gp_get_enable(OPTM_VDP_LAYER_WBC_GP0, &u32Enable);
    if (u32Enable) {
        /* *同源* */
        pstLayerData->eGfxWorkMode = HIFB_GFX_MODE_HD_WBC;
        pstLayerData->enSlaveryLayerID = OPTM_SLAVER_LAYERID;
    } else { /* *非同源* */
        pstLayerData->eGfxWorkMode = HIFB_GFX_MODE_NORMAL;
        pstLayerData->enSlaveryLayerID = HIFB_LAYER_ID_BUTT;
    }

    drv_hifb_hal_gfx_get_palpha(HIFB_ADP_GetGfxHalId(enLayerId), &pstLayerData->stAlpha.bAlphaEnable, &alpharange,
                                &pstLayerData->stAlpha.u8Alpha0, &pstLayerData->stAlpha.u8Alpha1);

    drv_hifb_hal_gfx_get_layer_galpha(HIFB_ADP_GetGfxHalId(enLayerId), &pstLayerData->stAlpha.u8GlobalAlpha);

    drv_hifb_hal_gfx_get_key_mask(HIFB_ADP_GetGfxHalId(enLayerId), &stckey_mask);
    drv_hifb_hal_gfx_get_color_key(HIFB_ADP_GetGfxHalId(enLayerId), &u32KeyEnable, &stKey);

    pstLayerData->stColorKey.u8RedMask = stckey_mask.u32Mask_r;
    pstLayerData->stColorKey.u8GreenMask = stckey_mask.u32Mask_g;
    pstLayerData->stColorKey.u8BlueMask = stckey_mask.u32Mask_b;

    pstLayerData->stColorKey.bMaskEnable = HI_TRUE;
    pstLayerData->stColorKey.bKeyEnable = u32KeyEnable;
    pstLayerData->stColorKey.u32KeyMode = stKey.bKeyMode;

    pstLayerData->stColorKey.u8RedMax = stKey.u32Key_r_max;
    pstLayerData->stColorKey.u8GreenMax = stKey.u32Key_g_max;
    pstLayerData->stColorKey.u8BlueMax = stKey.u32Key_b_max;

    pstLayerData->stColorKey.u8RedMin = stKey.u32Key_r_min;
    pstLayerData->stColorKey.u8GreenMin = stKey.u32Key_g_min;
    pstLayerData->stColorKey.u8BlueMin = stKey.u32Key_b_min;

    drv_hifb_hal_gfx_get_pre_mult_enable(HIFB_ADP_GetGfxHalId(enLayerId), &pstLayerData->bPreMul,
                                         &pstLayerData->bDePreMul);

    drv_hifb_hal_gfx_get_in_data_fmt(HIFB_ADP_GetGfxHalId(enLayerId), &enDataFmt);

    pstLayerData->eFmt = DRV_HIFB_ADP_HalFmtTransferToPixerFmt(enDataFmt);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

STATIC hi_void DRV_HIFB_ADP_GetLogoData(HIFB_LAYER_ID_E enLayerId, HIFB_LOGO_DATA_S *pstLogoData)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 LayerEnable = 0;
    OPTM_VDP_GFX_IFMT_E enDataFmt = VDP_GFX_IFMT_BUTT;
    DRV_HIFB_COLOR_FMT_E enFmt = DRV_HIFB_FMT_BUTT;
    disp_export_func *DispExportFuncs = NULL;
    hi_disp_display_info stDispInfo;
    hi_drv_display enDisp;
    hi_drv_rect stOutRect = {0};
    OPTM_VDP_DISP_RECT_S stInRect = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (enLayerId < HIFB_LAYER_SD_0) {
        enDisp = HI_DRV_DISPLAY_0;
    } else {
        enDisp = HI_DRV_DISPLAY_1;
    }

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstLogoData);

    memset(&stDispInfo, 0x0, sizeof(stDispInfo));

    Ret = osal_exportfunc_get(HI_ID_DISP, (hi_void **)&DispExportFuncs);
    if ((HI_SUCCESS != Ret) || (NULL == DispExportFuncs)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "get disp func failure");
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_drv_module_get_function, Ret);
        return;
    }

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(DispExportFuncs->disp_get_displayinfo);
    Ret = DispExportFuncs->disp_get_displayinfo(enDisp, &stDispInfo);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, pfnDispGetDispInfo, Ret);
        return;
    }
    HIFB_ADP_GetScreenRectFromDispInfo(&stDispInfo.virtaul_screen, (OPTM_GFX_OFFSET_S *)&stDispInfo.offset_info,
                                       &stDispInfo.fmt_resolution, &stDispInfo.pixel_fmt_resolution, &stOutRect);

    drv_hifb_hal_gfx_get_layer_in_rect(HIFB_ADP_GetGfxHalId(enLayerId), &stInRect);
    drv_hifb_hal_gfx_get_layer_enable(HIFB_ADP_GetGfxHalId(enLayerId), &LayerEnable);
    drv_hifb_hal_gfx_get_layer_addr(HIFB_ADP_GetGfxHalId(enLayerId), &pstLogoData->LogoYAddr);
    drv_hifb_hal_gfx_get_in_data_fmt(HIFB_ADP_GetGfxHalId(enLayerId), &enDataFmt);
    enFmt = DRV_HIFB_ADP_HalFmtTransferToPixerFmt(enDataFmt);

    if ((GRAPHIC_LAYER_REALLY_4K_WIDTH_DISPLAY == stInRect.u32IWth ||
         GRAPHIC_LAYER_4K_WIDTH_DISPLAY == stInRect.u32IWth) &&
        (GRAPHIC_LAYER_REALLY_4K_WIDTH_DISPLAY == stDispInfo.fmt_resolution.width ||
         GRAPHIC_LAYER_4K_WIDTH_DISPLAY == stDispInfo.fmt_resolution.width) &&
        (GRAPHIC_LAYER_4K_HEIGHT_DISPLAY == stInRect.u32IHgt) &&
        (GRAPHIC_LAYER_4K_HEIGHT_DISPLAY == stDispInfo.fmt_resolution.height)) {
        pstLogoData->Support4KLogo = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    pstLogoData->LogoEnable = (1 == LayerEnable) ? (HI_TRUE) : (HI_FALSE);
    pstLogoData->LogoCbCrAddr = 0x0;

    if ((enLayerId < HIFB_LAYER_SD_0) && (HI_FALSE == pstLogoData->Support4KLogo)) {
        pstLogoData->LogoWidth = stDispInfo.virtaul_screen.width;
        pstLogoData->LogoHeight = stDispInfo.virtaul_screen.height;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        pstLogoData->LogoWidth = stDispInfo.fmt_resolution.width;
        pstLogoData->LogoHeight = stDispInfo.fmt_resolution.height;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
    pstLogoData->stOutRect.x = stOutRect.rect_x;
    pstLogoData->stOutRect.y = stOutRect.rect_y;
    pstLogoData->stOutRect.w = stOutRect.rect_w;
    pstLogoData->stOutRect.h = stOutRect.rect_h;
    pstLogoData->eLogoPixFmt = HIFB_LOGO_PIX_FMT_ARGB8888;
    drv_hifb_hal_gfx_get_layer_stride(HIFB_ADP_GetGfxHalId(enLayerId), &(pstLogoData->LogoYStride));
    if (enFmt == DRV_HIFB_FMT_ARGB1555) {
        pstLogoData->eLogoPixFmt = HIFB_LOGO_PIX_FMT_ARGB1555;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

/***************************************************************************************
 * func         : DRV_HIFB_ADP_UpLayerReg
 * description  : CNcomment: 更新寄存器 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_UpLayerReg(HIFB_LAYER_ID_E enLayerId)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGpId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    OPTM_CheckGPMask_BYGPID(enGpId);

    drv_hifb_hal_gfx_set_reg_up(g_pstGfxDevice[enLayerId]->enGfxHalId);
    drv_hifb_hal_gp_set_reg_up(g_pstGfxGPDevice[enGpId]->enGpHalId);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if (IS_MASTER_GP(enGpId)) {
        DRV_HIFB_WBC_Reset(HI_TRUE);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void DRV_HIFB_ADP_GetCloseState(HIFB_LAYER_ID_E LayerId, hi_bool *pHasBeenClosedForVoCallBack,
                                          hi_bool *pHasBeenClosedForEndCallBack)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId);
    enGpId = g_pstGfxDevice[LayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    if ((HI_TRUE == g_pstGfxGPDevice[enGpId]->bHasBeenReOpen[0]) && (NULL != pHasBeenClosedForVoCallBack)) {
        *pHasBeenClosedForVoCallBack = HI_TRUE;
        g_pstGfxGPDevice[enGpId]->bHasBeenReOpen[0] = HI_FALSE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if ((HI_TRUE == g_pstGfxGPDevice[enGpId]->bHasBeenReOpen[1]) && (NULL != pHasBeenClosedForEndCallBack)) {
        *pHasBeenClosedForEndCallBack = HI_TRUE;
        g_pstGfxGPDevice[enGpId]->bHasBeenReOpen[1] = HI_FALSE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void DRV_HIFB_ADP_GetDhd0Info(HIFB_LAYER_ID_E LayerId, hi_ulong *pExpectIntLineNumsForVoCallBack,
                                        hi_ulong *pExpectIntLineNumsForEndCallBack,
                                        hi_ulong *pActualIntLineNumsForVoCallBack, hi_ulong *pHardIntCntForVoCallBack)
{
#if !defined(HI_BUILD_IN_BOOT) && !defined(CONFIG_HIFB_VERSION_1_0)
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    drv_hifb_hal_disp_get_dhd0_vtthd3(pExpectIntLineNumsForEndCallBack);
    drv_hifb_hal_disp_get_dhd0_vtthd(pExpectIntLineNumsForVoCallBack);
    drv_hifb_hal_disp_get_dhd0_state(pActualIntLineNumsForVoCallBack, pHardIntCntForVoCallBack);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#else
#endif
    return;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetTriDimMode
 * description  : CNcomment: 设置3D模式 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_SetTriDimMode(HIFB_LAYER_ID_E enLayerId, HIFB_STEREO_MODE_E enMode,
                                         HIFB_STEREO_MODE_E enWbcSteroMode)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGpId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    g_pstGfxDevice[enLayerId]->enTriDimMode = enMode;
    g_pstGfxGPDevice[enGpId]->enTriDimMode = enMode;

    OPTM_CheckGPMask_BYGPID(enGpId);

    drv_hifb_hal_gp_set_reg_up(g_pstGfxGPDevice[enGpId]->enGpHalId);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if (IS_MASTER_GP(enGpId)) {
        g_pstGfxGPDevice[OPTM_SLAVER_GPID]->enTriDimMode = enWbcSteroMode;
        g_pstGfxGPDevice[OPTM_SLAVER_GPID]->stInRect.rect_w = g_pstGfxGPDevice[enGpId]->stInRect.rect_w;
        g_pstGfxGPDevice[OPTM_SLAVER_GPID]->stInRect.rect_h = g_pstGfxGPDevice[enGpId]->stInRect.rect_h;

        if (HIFB_STEREO_MONO == enWbcSteroMode) {
            g_stGfxWbc2.enWbcMode = HIFB_WBC_MODE_MONO;
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        } else {
            g_stGfxWbc2.enWbcMode = HIFB_WBC_MODE_LFET_EYE;
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        }

        g_pstGfxGPDevice[OPTM_SLAVER_GPID]->unUpFlag.bits.WbcMode = 1;

        DRV_HIFB_WBC_Reset(HI_TRUE);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetTriDimAddr
 * description  : CNcomment: 设置3D显示地址 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_SetTriDimAddr(HIFB_LAYER_ID_E enLayerId, hi_u32 u32TriDimAddr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(g_pstGfxDevice[enLayerId]->enGPId, HI_FAILURE);

    g_pstGfxDevice[enLayerId]->u32TriDimAddr = u32TriDimAddr;

    if (HI_TRUE == g_pstGfxGPDevice[g_pstGfxDevice[enLayerId]->enGPId]->bMaskFlag) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    drv_hifb_hal_gfx_set_layer_naddr(g_pstGfxDevice[enLayerId]->enGfxHalId, u32TriDimAddr);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

/***************************************************************************************
 * func          : DRV_HIFB_ADP_GetLayerPriority
 * description   : CNcomment: 获取图层在GP 中的优先级 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_GetLayerPriority(HIFB_LAYER_ID_E enLayerId, hi_u32 *pPriority)
{
    hi_u32 i = 0;
    hi_u32 u32prio = 0;
    hi_u32 u32Index = 0;
    hi_u32 u32LayerPrio = 0;
    hi_u32 u32LayerIdIndex = 0;
    OPTM_VDP_CBM_MIX_E eCbmMixg;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pPriority, HI_FAILURE);

    u32Index = (g_pstGfxDevice[enLayerId]->enGPId == HIFB_ADP_GP0) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;
    eCbmMixg = (g_pstGfxDevice[enLayerId]->enGPId == HIFB_ADP_GP0) ? VDP_CBM_MIXG0 : VDP_CBM_MIXG1;
    drv_hifb_hal_cbm_get_mixer_prio(eCbmMixg, &u32prio);

    if (HIFB_LAYER_HD_2 == enLayerId) {
        u32LayerIdIndex = 3; /* * G3 * */
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
#ifdef CONFIG_HIFB_VERSION_3_0
    else if (HIFB_LAYER_HD_1 == enLayerId) {
        /* * match  to G3 mix * */
        u32LayerIdIndex = 2;
    }
#endif
    else {
        u32LayerIdIndex = enLayerId - OPTM_GP0_GFX_COUNT * g_pstGfxDevice[enLayerId]->enGPId;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    for (i = 0; i < u32Index; i++) {
        u32LayerPrio = u32prio & 0xf;
        u32prio = u32prio >> 4;

        if ((u32LayerPrio - 1) == u32LayerIdIndex) {
            *pPriority = i + 1;
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_SUCCESS;
        }
    }

    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "fail to get zorder of this layer");

    return HI_FAILURE;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetLayerPriority
 * description  : CNcomment: 设置图层在GP 中的优先级 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_SetLayerPriority(HIFB_LAYER_ID_E enLayerId, HIFB_ZORDER_E enZOrder)
{
    hi_u32 u32Prio = 0;
    hi_u32 u32LayerPrio = 0;
    hi_u32 u32MaskUpBit = 0;
    hi_u32 u32MaskDownBit = 0;
    hi_u32 u32SwitchBit = 0;
    hi_u32 u32MaskTopBit = 0;
    hi_u32 u32MaskBit = 0;
    hi_u32 u32LayerCount = 0;
    hi_s32 count = 0;
    OPTM_VDP_CBM_MIX_E eCbmMixg = VDP_CBM_MIX_BUTT;
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (HI_SUCCESS != DRV_HIFB_ADP_GetLayerPriority(enLayerId, &u32LayerPrio)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enLayerId);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_GetLayerPriority, FAILURE_TAG);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGpId = g_pstGfxDevice[enLayerId]->enGPId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    eCbmMixg = (enGpId == HIFB_ADP_GP0) ? VDP_CBM_MIXG0 : VDP_CBM_MIXG1;

    drv_hifb_hal_cbm_get_mixer_prio(eCbmMixg, &u32Prio);
    u32LayerCount = (enGpId == HIFB_ADP_GP0) ? OPTM_GP0_GFX_COUNT : OPTM_GP1_GFX_COUNT;

    u32SwitchBit = (u32Prio >> (4 * (u32LayerPrio - 1))) & 0xf;

    for (count = u32LayerPrio; count > 0; count--) {
        u32MaskUpBit = 0xf | (u32MaskUpBit << 4);
    }

    for (count = u32LayerPrio; count <= u32LayerCount; count++) {
        u32MaskDownBit = 0xf | (u32MaskDownBit << 4);
    }
    u32MaskDownBit <<= 4 * (u32LayerPrio - 1);

    u32MaskBit = 0xf;
    u32MaskTopBit = 0xf;
    for (count = 1; count < u32LayerCount; count++) {
        u32MaskBit = 0xf | (u32MaskBit << 4);
        u32MaskTopBit = u32MaskTopBit << 4;
    }

    u32Prio &= u32MaskBit;
    switch (enZOrder) {
        case HIFB_ZORDER_MOVEBOTTOM: {
            if (u32LayerPrio == 1) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return HI_SUCCESS;
            }
            u32Prio = (u32Prio & (~u32MaskUpBit)) | ((u32MaskUpBit & ((u32Prio & u32MaskUpBit) << 4)) & (~0xf)) |
                      u32SwitchBit;
            break;
        }
        case HIFB_ZORDER_MOVEDOWN: {
            if (u32LayerPrio == 1) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return HI_SUCCESS;
            }

            u32Prio = ((u32Prio & (0xf << 4 * (u32LayerPrio - 1))) >> 4) |
                      ((u32Prio & (0xf << 4 * (u32LayerPrio - 2))) << 4) |
                      (~((0xf << 4 * (u32LayerPrio - 1)) | (0xf << 4 * (u32LayerPrio - 2))) & u32Prio);
            break;
        }
        case HIFB_ZORDER_MOVETOP: {
            if (u32LayerPrio == u32LayerCount) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return HI_SUCCESS;
            }
            u32Prio = (u32Prio & (~u32MaskDownBit)) |
                      ((u32MaskDownBit & ((u32Prio & u32MaskDownBit) >> 4)) & (~u32MaskTopBit)) |
                      (u32SwitchBit << (4 * (u32LayerCount - 1)));
            break;
        }
        case HIFB_ZORDER_MOVEUP: {
            if (u32LayerPrio == u32LayerCount) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return HI_SUCCESS;
            }
            u32Prio = ((u32Prio & (0xf << 4 * (u32LayerPrio - 1))) << 4) |
                      ((u32Prio & (0xf << 4 * u32LayerPrio)) >> 4) |
                      (~((0xf << 4 * (u32LayerPrio - 1)) | (0xf << 4 * u32LayerPrio)) & u32Prio);
            break;
        }
        default: {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enZOrder);
            return HI_FAILURE;
        }
    }

    u32Prio &= u32MaskBit;

    drv_hifb_hal_cbm_set_mixg_prio(eCbmMixg, u32Prio);

    g_pstGfxGPDevice[enGpId]->u32Prior = u32Prio;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : HIFB_ADP_DispInfoUpdate
 * description   : CNcomment: 更新display信息 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
STATIC hi_s32 HIFB_ADP_DispInfoUpdate(HIFB_GP_ID_E enGPId)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 LayerIndex = 0;
    hi_u32 u32LayerCount = 0;
    hi_drv_rect *pstDispRect = NULL;
    HIFB_LAYER_ID_E enInitLayerId;
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    if (HI_FALSE == g_pstGfxGPDevice[enGPId]->bOpen) {
        return HI_SUCCESS;
    }

    if (HI_TRUE == g_pstGfxGPDevice[enGPId]->bMaskFlag) {
        return HI_SUCCESS;
    }

    pstDispRect = &g_pstGfxGPDevice[enGPId]->stOutRect;
    OPTM_GfxSetDispFMTSize((HIFB_GP_ID_E)enGPId, pstDispRect);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP1 == enGPId)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
#endif

    u32LayerCount = OPTM_GP0_GFX_COUNT;
    enInitLayerId = HIFB_LAYER_HD_0;
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE((enInitLayerId + u32LayerCount), HI_FAILURE);

    for (LayerIndex = 0; LayerIndex < u32LayerCount; LayerIndex++) {
        if (HI_FALSE == g_pstGfxDevice[enInitLayerId + LayerIndex]->bOpened) {
            continue;
        }
        if (g_pstGfxGPDevice[enGPId]->bInterface == HI_TRUE) {
            g_pstGfxDevice[enInitLayerId + LayerIndex]->enReadMode = VDP_RMODE_SELF_ADAPTION;
        } else {
            g_pstGfxDevice[enInitLayerId + LayerIndex]->enReadMode = VDP_RMODE_PROGRESSIVE;
        }
        Ret = DRV_HIFB_ADP_SetLayerRect(enInitLayerId + LayerIndex,
                                        &g_pstGfxDevice[enInitLayerId + LayerIndex]->stInRect);
        if (HI_SUCCESS != Ret) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_ADP_SetLayerRect, Ret);
        }
        DRV_HIFB_ADP_SetReadMode(enInitLayerId + LayerIndex, g_pstGfxDevice[enInitLayerId + LayerIndex]->enReadMode);
        drv_hifb_hal_gfx_set_upd_mode(g_pstGfxDevice[enInitLayerId + LayerIndex]->enGfxHalId,
                                      g_pstGfxDevice[enInitLayerId + LayerIndex]->enUpDateMode);
        drv_hifb_hal_gfx_set_reg_up(g_pstGfxDevice[enInitLayerId + LayerIndex]->enGfxHalId);
    }

    drv_hifb_hal_gp_set_reg_up(g_pstGfxGPDevice[enGPId]->enGpHalId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : HIFB_ADP_GP_Recovery
 * description  : CNcomment: 重新设置GP属性 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 HIFB_ADP_GP_Recovery(HIFB_GP_ID_E enGPId)
{
    hi_u32 i = 0;
    hi_u32 u32LayerCount = 0;
    hi_drv_rect *pstDispRect = NULL;
    HIFB_LAYER_ID_E enInitLayerId = HIFB_LAYER_HD_0;
    HIFB_LAYER_ID_E enLayerId = HIFB_LAYER_HD_0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    if (!g_pstGfxGPDevice[enGPId]->bOpen) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    pstDispRect = &g_pstGfxGPDevice[enGPId]->stOutRect;

    OPTM_CheckGPMask_BYGPID(enGPId);

    u32LayerCount = OPTM_GP0_GFX_COUNT;
    enInitLayerId = HIFB_LAYER_HD_0;

    drv_hifb_hal_cbm_set_mixer_bkg(g_pstGfxGPDevice[enGPId]->enMixg, &(g_pstGfxGPDevice[enGPId]->stBkg));
    drv_hifb_hal_cbm_set_mixg_prio(g_pstGfxGPDevice[enGPId]->enMixg, g_pstGfxGPDevice[enGPId]->u32Prior);
    drv_hifb_hal_gp_set_layer_galpha(g_pstGfxGPDevice[enGPId]->enGpHalId, g_pstGfxGPDevice[enGPId]->u32Alpha);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_enOptmGfxWorkMode);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, HIFB_GFX_MODE_HD_WBC);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, enGPId);
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP1 == enGPId)) {
        DRV_HIFB_WBC_Recovery(enGPId);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
#endif

    OPTM_GfxSetDispFMTSize(enGPId, pstDispRect);
    drv_hifb_hal_gp_set_reg_up(g_pstGfxGPDevice[enGPId]->enGpHalId);

    for (i = 0; i < u32LayerCount; i++) {
        enLayerId = enInitLayerId + i;
        if (!g_pstGfxDevice[enLayerId]->bOpened) {
            drv_hifb_hal_gfx_set_layer_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, HI_FALSE);
            drv_hifb_hal_gfx_set_reg_up(g_pstGfxDevice[enLayerId]->enGfxHalId);
            continue;
        }

        DRV_HIFB_ADP_SetLayerAlpha(enLayerId, &g_pstGfxDevice[enLayerId]->stAlpha);
        DRV_HIFB_ADP_SetLayKeyMask(enLayerId, &g_pstGfxDevice[enLayerId]->stColorkey);

#ifdef CONFIG_HIFB_STEREO_SUPPORT
        DRV_HIFB_ADP_SetTriDimMode(enLayerId, g_pstGfxDevice[enLayerId]->enTriDimMode,
                                   g_pstGfxGPDevice[OPTM_SLAVER_GPID]->enTriDimMode);
        DRV_HIFB_ADP_SetTriDimAddr(enLayerId, g_pstGfxDevice[enLayerId]->u32TriDimAddr);
        DRV_HIFB_ADP_SetStereoDepth(enLayerId, g_pstGfxDevice[enLayerId]->s32Depth);
#endif

        DRV_HIFB_ADP_SetLayerPreMult(enLayerId, g_pstGfxDevice[enLayerId]->bPreMute);
        DRV_HIFB_ADP_SetLayerDataFmt(enLayerId, g_pstGfxDevice[enLayerId]->enDataFmt);
        DRV_HIFB_ADP_SetLayerRect(enLayerId, &g_pstGfxDevice[enLayerId]->stInRect);

        drv_hifb_hal_gfx_set_lut_addr(g_pstGfxDevice[enLayerId]->enGfxHalId,
                                      g_pstGfxDevice[enLayerId]->clut_table.phy_addr);
        drv_hifb_hal_gfx_set_para_upd(g_pstGfxDevice[enLayerId]->enGfxHalId, VDP_DISP_COEFMODE_LUT);
        drv_hifb_hal_gfx_set_layer_bkg(g_pstGfxDevice[enLayerId]->enGfxHalId, &(g_pstGfxDevice[enLayerId]->stBkg));
        drv_hifb_hal_gfx_set_bit_extend(g_pstGfxDevice[enLayerId]->enGfxHalId, g_pstGfxDevice[enLayerId]->enBitExtend);

#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT) && !defined(HI_BUILD_IN_BOOT)
        if ((HIFB_LAYER_HD_0 == enLayerId) && (HI_TRUE == g_pstGfxDevice[enLayerId]->bDmpOpened)) {
            OPTM_GFX_DECMP_Open(enLayerId);
        }
#endif

#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT) && !defined(CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE) && \
    !defined(HI_BUILD_IN_BOOT)
        if (HI_TRUE == g_pstGfxDevice[enLayerId]->bDmpOpened) {
            drv_hifb_hal_gfx_set_de_cmp_ddr_info(g_pstGfxDevice[enLayerId]->enGfxHalId,
                                                 g_pstGfxDevice[enLayerId]->ARHeadDdr,
                                                 g_pstGfxDevice[enLayerId]->ARDataDdr,
                                                 g_pstGfxDevice[enLayerId]->GBHeadDdr,
                                                 g_pstGfxDevice[enLayerId]->GBDataDdr,
                                                 g_pstGfxDevice[enLayerId]->CmpStride);
        } else
#endif
        {
            DRV_HIFB_ADP_SetLayerStride(enLayerId, g_pstGfxDevice[enLayerId]->Stride);
            DRV_HIFB_ADP_SetLayerAddr(enLayerId, g_pstGfxDevice[enLayerId]->NoCmpBufAddr);
        }

        DRV_HIFB_ADP_SetReadMode(enLayerId, g_pstGfxDevice[enLayerId]->enReadMode);
        DRV_HIFB_ADP_SetEnable(enLayerId, g_pstGfxDevice[enLayerId]->bEnable);
        drv_hifb_hal_gfx_set_upd_mode(g_pstGfxDevice[enLayerId]->enGfxHalId, g_pstGfxDevice[enLayerId]->enUpDateMode);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func        : OPTM_VO_Callback
 * description : CNcomment: VO CALLBACK CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_s32 OPTM_VO_Callback(hi_void *u32Param0, hi_void *u32Param1)
{
    hi_u32 i;
    hi_u32 u32CTypeFlag;
    hi_u32 u32LayerCount;
    HIFB_LAYER_ID_E enInitLayerId;
    HIFB_GP_ID_E *pEnGpId = (HIFB_GP_ID_E *)u32Param0;
    hi_drv_disp_callback_info *pstDispInfo = (hi_drv_disp_callback_info *)u32Param1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((NULL == pEnGpId) || (NULL == pstDispInfo)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pEnGpId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstDispInfo);
        return HI_FAILURE;
    }
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(*pEnGpId, HI_FAILURE);

    u32LayerCount = OPTM_GP0_GFX_COUNT;
    enInitLayerId = HIFB_LAYER_HD_0;
    for (i = 0; i < u32LayerCount; i++) {
        hi_ulong LockFlag = 0;
        u32CTypeFlag = g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[i].u32CTypeFlag;
        if (HIFB_CALLBACK_TYPE_VO != (u32CTypeFlag & HIFB_CALLBACK_TYPE_VO)) {
            continue;
        }

        /* *ensure the func has not be set null * */
        osal_spin_lock_irqsave(&(g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].FuncLock),
                               &LockFlag);
        if (NULL != g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].pFunc) {
            if (g_pstGfxGPDevice[*pEnGpId]->bHdr != g_pstGfxGPDevice[*pEnGpId]->bPreHdr) {
                g_pstGfxGPDevice[*pEnGpId]->bPreHdr = g_pstGfxGPDevice[*pEnGpId]->bHdr;
                drv_hifb_hal_gfx_set_pre_mult_enable(g_pstGfxDevice[i]->enGfxHalId, g_pstGfxDevice[i]->bPreMute,
                                                     g_pstGfxGPDevice[*pEnGpId]->bHdr);
            }
            g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].pFunc(
                g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].param0, u32Param1);
        }
        osal_spin_unlock_irqrestore(
            &(g_pstGfxGPIrq[*pEnGpId]->stGfxCallBack[i].stGfxIrq[HIFB_CALLBACK_TYPE_VO].FuncLock), &LockFlag);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func          : HIFB_ADP_DistributeCallback
 * description   : CNcomment: 分发中断处理类型 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 HIFB_ADP_DistributeCallback(hi_void *u32Param0, hi_void *u32Param1)
{
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    HIFB_STEREO_MODE_E enTriDimMode;
#endif
    HIFB_GP_ID_E *penGpId = (HIFB_GP_ID_E *)u32Param0;
    hi_drv_disp_callback_info *pstDispInfo = (hi_drv_disp_callback_info *)u32Param1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (penGpId == NULL || pstDispInfo == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, penGpId);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstDispInfo);
        return HI_FAILURE;
    }

    OPTM_VO_Callback(u32Param0, u32Param1);

#ifdef CONFIG_HIFB_STEREO_SUPPORT
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(*penGpId, HI_FAILURE);

    enTriDimMode =
        HIFB_ADP_GetStereoModeFromDisp(drv_hifb_hal_disp_get_disp_mode(g_pstGfxGPDevice[*penGpId]->enGpHalId));

    if ((enTriDimMode != g_pstGfxGPDevice[*penGpId]->enTriDimMode) && (HIFB_STEREO_BUTT > enTriDimMode)) {
        if (HIFB_ADP_CheckGfxCallbackReg((HIFB_GP_ID_E)(*penGpId), HIFB_CALLBACK_TYPE_3DMode_CHG) != HI_SUCCESS) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_SUCCESS;
        }

        g_pstGfxGPDevice[*penGpId]->enTriDimMode = enTriDimMode;

        drv_hifb_hal_gp_set_reg_up(g_pstGfxGPDevice[*penGpId]->enGpHalId);

        g_pstGfxGPDevice[*penGpId]->stStartStereoWork.u32Data = *penGpId;

        if (NULL != g_pstGfxGPDevice[*penGpId]->queue) {
            queue_work(g_pstGfxGPDevice[*penGpId]->queue, &g_pstGfxGPDevice[*penGpId]->stStartStereoWork.work);
        }
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

/***************************************************************************************
* func          : DRV_HIFB_ADP_SetGPMask
* description   : CNcomment: 这个接口只有开机logo才会用到，主要是用来过渡使用的
                        设置GP0和GP1掩码，并且在logo过渡完之后重新设置GP CNend\n
* param[in]     : hi_void
* retval        : NA
* others:       : NA
***************************************************************************************/
STATIC hi_s32 DRV_HIFB_ADP_SetGPMask(HIFB_LAYER_ID_E LayerId, hi_bool bFlag)
{
#ifndef HI_BUILD_IN_BOOT
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP_BUTT;
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    HIFB_OSD_DATA_S stLayerData;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(LayerId, HI_FAILURE);
    enGpId = HIFB_ADP_GetGpId(LayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGpId, HI_FAILURE);

    g_pstGfxGPDevice[enGpId]->bMaskFlag = bFlag;

    if (HI_FALSE == bFlag) {
        HIFB_ADP_GP_Recovery(enGpId);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if (HIFB_ADP_GP0 != enGpId) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    DRV_HIFB_ADP_GetOSDData(HIFB_LAYER_SD_0, &stLayerData);
    if ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) || (HIFB_LAYER_STATE_ENABLE == stLayerData.eState)) {
        g_pstGfxGPDevice[HIFB_ADP_GP1]->bMaskFlag = bFlag;
        if ((HI_FALSE == bFlag) && (HI_TRUE == g_stGfxWbc2.IsOpen)) {
            HIFB_ADP_GP_Recovery(HIFB_ADP_GP1);
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        }
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#else
#endif
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_GetGPMask
 * description  : CNcomment: 获取GP设置掩码 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
STATIC hi_void DRV_HIFB_ADP_GetGPMask(HIFB_LAYER_ID_E LayerId, hi_bool *pbMask)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP_BUTT;

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId);
    enGpId = HIFB_ADP_GetGpId(LayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    if (NULL != pbMask) {
        *pbMask = g_pstGfxGPDevice[enGpId]->bMaskFlag;
    }

    return;
}

/***************************************************************************************
 * func          : DRV_HIFB_ADP_ClearLogoOsd
 * description   : CNcomment: 清logo CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
STATIC hi_s32 DRV_HIFB_ADP_ClearLogoOsd(HIFB_LAYER_ID_E enLayerId)
{
    return HI_SUCCESS;
}

#if !defined(HI_BUILD_IN_BOOT) && defined(CONFIG_HIFB_STEREO_SUPPORT)
/***************************************************************************************
 * func          : DRV_HIFB_ADP_SetStereoDepth
 * description   : CNcomment: 设置3D  景深，与人眼的距离 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_SetStereoDepth(HIFB_LAYER_ID_E enLayerId, hi_s32 s32Depth)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    g_pstGfxDevice[enLayerId]->s32Depth = s32Depth;

    if (g_pstGfxDevice[enLayerId]->s32Depth != 0) {
        drv_hifb_hal_gfx_set_three_dim_dof_enable(HIFB_ADP_GetGfxHalId(enLayerId), HI_TRUE);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    drv_hifb_hal_gfx_set_three_dim_dof_step(HIFB_ADP_GetGfxHalId(enLayerId), s32Depth, (0 - s32Depth));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

/***************************************************************************************
 * func          : DRV_HIFB_ADP_SetTCFlag
 * description   : CNcomment: 设置是不是TC使用场景标识 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetTCFlag(hi_bool bFlag)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    DRV_HIFB_WBC_SetTcFlag(bFlag);
#endif
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_ADP_GetHaltDispStatus
 * description  : CNcomment: 获取显示状态 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_ADP_GetHaltDispStatus(HIFB_LAYER_ID_E enLayerId, hi_bool *pbDispInit)
{
    HIFB_GP_ID_E enGPId = HIFB_ADP_GP0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(enLayerId, HI_FAILURE);
    enGPId = HIFB_ADP_GetGpId(enLayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(enGPId, HI_FAILURE);

    if (NULL != pbDispInit) {
        *pbDispInit = g_pstGfxGPDevice[enGPId]->bGpClose ? HI_FALSE : HI_TRUE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetGpOutputRect
 * description : CNcomment: 获取显示输出的大小 CNend\n
 * param[in]   : eGpId
 * param[in]   : eGpId
 * retval      : HI_SUCCESS
 * retval      : HI_FAILURE
 * others:     : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_GetGpOutputRect(HIFB_GP_ID_E eGpId, HIFB_RECT *pstOutputRect)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_drv_rect stOutRect = {0};
    OPTM_GFX_OFFSET_S stOffset = {0};
    hi_disp_display_info stDispInfo;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(eGpId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOutputRect, HI_FAILURE);

    memset(&stDispInfo, 0x0, sizeof(hi_disp_display_info));

#ifdef HI_BUILD_IN_BOOT
    if (HIFB_ADP_GP0 == eGpId) {
        Ret = HI_DISP_GetDisplayInfo(HI_DRV_DISPLAY_0, &stDispInfo);
    } else {
        Ret = HI_DISP_GetDisplayInfo(HI_DRV_DISPLAY_1, &stDispInfo);
    }
#else
    if ((NULL == ps_DispExportFuncs) || (NULL == ps_DispExportFuncs->disp_get_displayinfo)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "get disp func failure");
        return HI_FAILURE;
    }

    if (HIFB_ADP_GP0 == eGpId) {
        Ret = ps_DispExportFuncs->disp_get_displayinfo(HI_DRV_DISPLAY_0, &stDispInfo);
    } else {
        Ret = ps_DispExportFuncs->disp_get_displayinfo(HI_DRV_DISPLAY_1, &stDispInfo);
    }
#endif
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, pfnDispGetDispInfo, Ret);
        return HI_FAILURE;
    }

    stOffset.u32Left = stDispInfo.offset_info.left;
    stOffset.u32Top = stDispInfo.offset_info.top;
    stOffset.u32Right = stDispInfo.offset_info.right;
    stOffset.u32Bottom = stDispInfo.offset_info.bottom;

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "======================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stOffset.u32Left);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stOffset.u32Top);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stOffset.u32Right);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stOffset.u32Bottom);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.virtaul_screen.x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.virtaul_screen.y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.virtaul_screen.width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.virtaul_screen.height);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.fmt_resolution.x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.fmt_resolution.y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.fmt_resolution.width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.fmt_resolution.height);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.pixel_fmt_resolution.x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.pixel_fmt_resolution.y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.pixel_fmt_resolution.width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stDispInfo.pixel_fmt_resolution.height);

    HIFB_ADP_GetScreenRectFromDispInfo(&stDispInfo.virtaul_screen, (OPTM_GFX_OFFSET_S *)&stOffset,
                                       &stDispInfo.fmt_resolution, &stDispInfo.pixel_fmt_resolution, &stOutRect);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_w);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, stOutRect.rect_h);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "======================================================");

    pstOutputRect->x = stOutRect.rect_x;
    pstOutputRect->y = stOutRect.rect_y;
    pstOutputRect->w = stOutRect.rect_w;
    pstOutputRect->h = stOutRect.rect_h;
    g_pstGfxGPDevice[eGpId]->stOutRect.rect_x = stOutRect.rect_x;
    g_pstGfxGPDevice[eGpId]->stOutRect.rect_y = stOutRect.rect_y;
    g_pstGfxGPDevice[eGpId]->stOutRect.rect_w = stOutRect.rect_w;
    g_pstGfxGPDevice[eGpId]->stOutRect.rect_h = stOutRect.rect_h;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifdef HI_BUILD_IN_BOOT
hi_bool DRV_HIFB_ADP_WhetherSupportWbc(hi_void)
{
    return (HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) ? (HI_TRUE) : (HI_FALSE);
}
#endif

#ifndef HI_BUILD_IN_BOOT
static hi_s32 DRV_HIFB_ADP_ColorConvert(const struct fb_var_screeninfo *pstVar, HIFB_COLORKEYEX_S *pCkey)
{
    hi_u8 rOff, gOff, bOff;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((NULL == pstVar) || (NULL == pCkey)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstVar);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pCkey);
        return HI_FAILURE;
    }

    rOff = pstVar->red.length;
    gOff = pstVar->green.length;
    bOff = pstVar->blue.length;

    pCkey->u8RedMask = (0xff >> rOff);
    pCkey->u8GreenMask = (0xff >> gOff);
    pCkey->u8BlueMask = (0xff >> bOff);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

static hi_void DRV_HIFB_ADP_SetDeCmpSwitch(HIFB_LAYER_ID_E enLayerId, hi_bool bOpen)
{
#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT)
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (bOpen) {
        OPTM_GFX_DECMP_Open(enLayerId);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        OPTM_GFX_DECMP_Close(enLayerId);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#else
#endif
}

static hi_void DRV_HIFB_ADP_GetDeCmpSwitch(HIFB_LAYER_ID_E enLayerId, hi_bool *pbOpen)
{
#if defined(CFG_HI_FB_DECOMPRESS_SUPPORT)
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(enLayerId);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pbOpen);
    drv_hifb_hal_gfx_get_dcmp_enable(g_pstGfxDevice[enLayerId]->enGfxHalId, pbOpen);
#else
#endif
}

static hi_void DRV_HIFB_ADP_OpenMute(HIFB_LAYER_ID_E LayerId)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP_BUTT;
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId);
    enGpId = HIFB_ADP_GetGpId(LayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    drv_hifb_hal_cbm_open_mute_bk(g_pstGfxGPDevice[enGpId]->enGpHalId);

    return;
}

static hi_void DRV_HIFB_ADP_CloseMute(HIFB_LAYER_ID_E LayerId)
{
    HIFB_GP_ID_E enGpId = HIFB_ADP_GP_BUTT;
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId);
    enGpId = HIFB_ADP_GetGpId(LayerId);
    GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(enGpId);

    drv_hifb_hal_cbm_close_mute_bk(g_pstGfxGPDevice[enGpId]->enGpHalId);

    return;
}

static hi_void drv_hifb_adp_set_up_mute(HIFB_LAYER_ID_E layer_id, hi_bool mute)
{
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(layer_id);
    drv_hifb_hal_set_up_mute(g_pstGfxDevice[layer_id]->enGfxHalId, mute);
    return;
}

static hi_void drv_hifb_adp_get_up_mute(HIFB_LAYER_ID_E layer_id, hi_bool *mute)
{
    if ((layer_id < HIFB_LAYER_ID_BUTT) && (mute != NULL)) {
        drv_hifb_hal_get_up_mute(g_pstGfxDevice[layer_id]->enGfxHalId, mute);
    }
    return;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetCallBackFunction
 * description : CNcomment: 获取设备上下文 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void DRV_HIFB_ADP_RegistAdpFunction(HIFB_EXPORT_UT_FUNC_S *RegistAdpFunction)
{
    if (NULL != RegistAdpFunction) {
        RegistAdpFunction->pfnHifbAdpParaCheckSupport = HIFB_PARA_CHECK_SUPPORT;
        RegistAdpFunction->pfnHifbAdpAlign = HIFB_ADP_Align;
        RegistAdpFunction->pfnHifbAdpGetGfxHalId = HIFB_ADP_GetGfxHalId;
        RegistAdpFunction->pfnHifbAdpGetGpId = HIFB_ADP_GetGpId;
        RegistAdpFunction->pfnHifbAdpGetGpHalId = HIFB_ADP_GetGpHalId;
        RegistAdpFunction->pfnHifbAdpGetCscTyeFromDisp = DRV_HIFB_ADP_GetCscTypeFromDisp;
#ifdef CONFIG_HIFB_STEREO_SUPPORT
        RegistAdpFunction->pfnHifbAdpGetStereoModeFromDisp = HIFB_ADP_GetStereoModeFromDisp;
        RegistAdpFunction->pfnHifbAdpStereoCallBack = DRV_HIFB_ADP_StereoCallBack;
#endif
        RegistAdpFunction->pfnHifbAdpGpOpen = DRV_HIFB_ADP_GP_Open;
        RegistAdpFunction->pfnHifbAdpGpClose = DRV_HIFB_ADP_GP_Close;
        RegistAdpFunction->pfnHifbAdpSetReadMode = DRV_HIFB_ADP_SetReadMode;
        RegistAdpFunction->pfnHifbAdpInitLayer = DRV_HIFB_ADP_GFX_InitLayer;
        RegistAdpFunction->pfnHifbAdpReleaseClutBuf = DRV_HIFB_ADP_ReleaseClutBuf;
        RegistAdpFunction->pfnHifbAdpSetGpCsc = HIFB_ADP_SetGpCsc;
        RegistAdpFunction->pfnHifbAdpSetCallback = DRV_HIFB_ADP_SetCallback;
#ifdef CONFIG_HIFB_STEREO_SUPPORT
        RegistAdpFunction->pfnHifbAdpCheckGfxCallbackReg = HIFB_ADP_CheckGfxCallbackReg;
#endif
        RegistAdpFunction->pfnHifbAdpGetScreenRectFromDispInfo = HIFB_ADP_GetScreenRectFromDispInfo;
        RegistAdpFunction->pfnHifbAdpFrameEndCallBack = DRV_HIFB_ADP_FrameEndCallBack;
        RegistAdpFunction->pfnHifbAdpDispInfoProcess = HIFB_ADP_DispInfoProcess;
        RegistAdpFunction->pfnHifbAdpGpSetMask = HIFB_ADP_GP_SetMask;
        RegistAdpFunction->pfnHifbAdpSetLowPowerInfo = DRV_HIFB_ADP_SetLowPowerInfo;
        RegistAdpFunction->pfnHifbAdpSetDeCmpDdrInfo = drv_hifb_adp_set_decmp_info;
        RegistAdpFunction->pfnHifbAdpGetDecompressStatus = DRV_HIFB_ADP_GetDecompressStatus;
#ifdef CFG_HI_FB_DECOMPRESS_SUPPORT
        RegistAdpFunction->pfnHifbAdpDecompressOpen = OPTM_GFX_DECMP_Open;
        RegistAdpFunction->pfnHifbAdpDecompressClose = OPTM_GFX_DECMP_Close;
#endif
        RegistAdpFunction->pfnHifbAdpUpDataCallBack = DRV_HIFB_ADP_UpDataCallBack;
        RegistAdpFunction->pfnHifbAdpReOpen = HIFB_ADP_ReOpen;
        RegistAdpFunction->pfnHifbAdpReClose = HIFB_ADP_ReClose;
        RegistAdpFunction->pfnHifbAdpDispSetting = HIFB_ADP_DispSetting;
        RegistAdpFunction->pfnHifbAdpUpdataLayerInfo = HIFB_ADP_UpdataLayerInfo;
        RegistAdpFunction->pfnHifbAdpGfxChn2DispChn = OPTM_GfxChn2DispChn;
        RegistAdpFunction->pfnHifbAdpSetCallbackToDisp = DRV_HIFB_ADP_SetCallbackToDisp;
        RegistAdpFunction->pfnHifbAdpOpenLayer = DRV_HIFB_ADP_OpenLayer;
        RegistAdpFunction->pfnHifbAdpCloseLayer = DRV_HIFB_ADP_CloseLayer;
        RegistAdpFunction->pfnHifbAdpSetEnable = DRV_HIFB_ADP_SetEnable;
        RegistAdpFunction->pfnHifbAdpGetEnable = DRV_HIFB_ADP_GetEnable;
        RegistAdpFunction->pfnHifbAdpSetLayerAddr = DRV_HIFB_ADP_SetLayerAddr;
        RegistAdpFunction->pfnHifbAdpGetLayerAddr = DRV_HIFB_ADP_GetLayerAddr;
        RegistAdpFunction->pfnHifbAdpGetLayerWorkAddr = DRV_HIFB_ADP_GetLayerWorkAddr;
        RegistAdpFunction->pfnHifbAdpGetLayerWillWorkAddr = DRV_HIFB_ADP_GetLayerWillWorkAddr;
        RegistAdpFunction->pfnHifbAdpSetLayerStride = DRV_HIFB_ADP_SetLayerStride;
        RegistAdpFunction->pfnHifbAdpGetLayerStride = DRV_HIFB_ADP_GetLayerStride;
        RegistAdpFunction->pfnHifbAdpHalFmtTransferToPixerFmt = DRV_HIFB_ADP_HalFmtTransferToPixerFmt;
        RegistAdpFunction->pfnHifbAdpPixerFmtTransferToHalFmt = DRV_HIFB_ADP_PixerFmtTransferToHalFmt;
        RegistAdpFunction->pfnHifbAdpSetLayerDataFmt = DRV_HIFB_ADP_SetLayerDataFmt;
        RegistAdpFunction->pfnHifbAdpSetLayerDataBigEndianFmt = HIFB_ADP_SetLayerDataBigEndianFmt;
        RegistAdpFunction->pfnHifbAdpSetLayerDataLittleEndianFmt = HIFB_ADP_SetLayerDataLittleEndianFmt;
        RegistAdpFunction->pfnHifbAdpSetColorReg = DRV_HIFB_ADP_SetColorReg;
        RegistAdpFunction->pfnHifbAdpGfxWVBCallBack = OPTM_GfxWVBCallBack;
        RegistAdpFunction->pfnHifbAdpWaitVBlank = DRV_HIFB_ADP_WaitVBlank;
        RegistAdpFunction->pfnHifbAdpSetLayerAlpha = DRV_HIFB_ADP_SetLayerAlpha;
        RegistAdpFunction->pfnHifbAdpGetLayerRect = DRV_HIFB_ADP_GetLayerRect;
        RegistAdpFunction->pfnHifbAdpSetLayerRect = DRV_HIFB_ADP_SetLayerRect;
        RegistAdpFunction->pfnHifbAdpGetDispFMTSize = DRV_HIFB_ADP_GetDispFMTSize;
        RegistAdpFunction->pfnHifbAdpSetGpRect = DRV_HIFB_ADP_SetGpRect;
        RegistAdpFunction->pfnHifbAdpGetOutRect = DRV_HIFB_ADP_GetOutRect;
        RegistAdpFunction->pfnHifbAdpSetLayKeyMask = DRV_HIFB_ADP_SetLayKeyMask;
        RegistAdpFunction->pfnHifbAdpSetLayerPreMult = DRV_HIFB_ADP_SetLayerPreMult;
        RegistAdpFunction->pfnHifbAdpGetLayerPreMult = DRV_HIFB_ADP_GetLayerPreMult;
        RegistAdpFunction->pfnHifbAdpGetLayerHdr = DRV_HIFB_ADP_GetLayerHdr;
        RegistAdpFunction->pfnHifbAdpWhetherDiscardFrame = DRV_HIFB_ADP_WhetherDiscardFrame;
        RegistAdpFunction->pfnHifbAdpGetOSDData = DRV_HIFB_ADP_GetOSDData;
        RegistAdpFunction->pfnHifbAdpGetLogoData = DRV_HIFB_ADP_GetLogoData;
        RegistAdpFunction->pfnHifbAdpUpLayerReg = DRV_HIFB_ADP_UpLayerReg;
        RegistAdpFunction->pfnHifbAdpGetCloseState = DRV_HIFB_ADP_GetCloseState;
        RegistAdpFunction->pfnHifbAdpGetDhd0Info = DRV_HIFB_ADP_GetDhd0Info;
        RegistAdpFunction->pfnHifbAdpSetTriDimMode = DRV_HIFB_ADP_SetTriDimMode;
        RegistAdpFunction->pfnHifbAdpSetTriDimAddr = DRV_HIFB_ADP_SetTriDimAddr;
        RegistAdpFunction->pfnHifbAdpGetLayerPriority = DRV_HIFB_ADP_GetLayerPriority;
        RegistAdpFunction->pfnHifbAdpSetLayerPriority = DRV_HIFB_ADP_SetLayerPriority;
        RegistAdpFunction->pfnHifbAdpDispInfoUpdate = HIFB_ADP_DispInfoUpdate;
        RegistAdpFunction->pfnHifbAdpGPRecovery = HIFB_ADP_GP_Recovery;
        RegistAdpFunction->pfnHifbAdpVoCallback = OPTM_VO_Callback;
        RegistAdpFunction->pfnHifbAdpDistributeCallback = HIFB_ADP_DistributeCallback;
        RegistAdpFunction->pfnHifbAdpSetGPMask = DRV_HIFB_ADP_SetGPMask;
        RegistAdpFunction->pfnHifbAdpGetGPMask = DRV_HIFB_ADP_GetGPMask;
#ifdef CONFIG_HIFB_STEREO_SUPPORT
        RegistAdpFunction->pfnHifbAdpSetStereoDepth = DRV_HIFB_ADP_SetStereoDepth;
#endif
        RegistAdpFunction->pfnHifbAdpGetHaltDispStatus = DRV_HIFB_ADP_GetHaltDispStatus;
        RegistAdpFunction->pfnHifbAdpSetDeCmpSwitch = DRV_HIFB_ADP_SetDeCmpSwitch;
        RegistAdpFunction->pfnHifbAdpGetDeCmpSwitch = DRV_HIFB_ADP_GetDeCmpSwitch;
        RegistAdpFunction->pfnHifbAdpOpenMute = DRV_HIFB_ADP_OpenMute;
        RegistAdpFunction->pfnHifbAdpCloseMute = DRV_HIFB_ADP_CloseMute;
        RegistAdpFunction->pfn_hifb_adp_set_up_mute = drv_hifb_adp_set_up_mute;
        RegistAdpFunction->pfn_hifb_adp_get_up_mute = drv_hifb_adp_get_up_mute;
    }
    return;
}
#endif

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetCallBackFunction
 * description : CNcomment: 获取设备上下文 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void DRV_HIFB_ADP_GetCallBackFunction(HIFB_DRV_OPS_S *AdpCallBackFunction)
{
    if (NULL != AdpCallBackFunction) {
        AdpCallBackFunction->DRV_HIFB_ADP_CloseLayer = DRV_HIFB_ADP_CloseLayer;
#ifndef HI_BUILD_IN_BOOT
        AdpCallBackFunction->DRV_HIFB_ADP_ColorConvert = DRV_HIFB_ADP_ColorConvert;
#endif
        AdpCallBackFunction->DRV_HIFB_ADP_SetEnable = DRV_HIFB_ADP_SetEnable;
        AdpCallBackFunction->DRV_HIFB_ADP_GetEnable = DRV_HIFB_ADP_GetEnable;
        AdpCallBackFunction->DRV_HIFB_ADP_GetDevCap = DRV_HIFB_ADP_GetDevCap;
#ifndef HI_BUILD_IN_BOOT
        AdpCallBackFunction->DRV_HIFB_ADP_GetOSDData = DRV_HIFB_ADP_GetOSDData;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLogoData = DRV_HIFB_ADP_GetLogoData;
#endif
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerPriority = DRV_HIFB_ADP_GetLayerPriority;
        AdpCallBackFunction->DRV_HIFB_ADP_DeInit = DRV_HIFB_ADP_DeInit;
        AdpCallBackFunction->DRV_HIFB_ADP_Init = DRV_HIFB_ADP_Init;
        AdpCallBackFunction->DRV_HIFB_ADP_OpenLayer = DRV_HIFB_ADP_OpenLayer;
        AdpCallBackFunction->DRV_HIFB_ADP_SetColorReg = DRV_HIFB_ADP_SetColorReg;
#ifndef HI_BUILD_IN_BOOT
        AdpCallBackFunction->DRV_HIFB_ADP_SetTriDimMode = DRV_HIFB_ADP_SetTriDimMode;
        AdpCallBackFunction->DRV_HIFB_ADP_SetTriDimAddr = DRV_HIFB_ADP_SetTriDimAddr;
#endif
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerAddr = DRV_HIFB_ADP_SetLayerAddr;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerAddr = DRV_HIFB_ADP_GetLayerAddr;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerWorkAddr = DRV_HIFB_ADP_GetLayerWorkAddr;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerWillWorkAddr = DRV_HIFB_ADP_GetLayerWillWorkAddr;
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerAlpha = DRV_HIFB_ADP_SetLayerAlpha;
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerDataFmt = DRV_HIFB_ADP_SetLayerDataFmt;
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerDeFlicker = DRV_HIFB_ADP_SetLayerDeFlicker;
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerPriority = DRV_HIFB_ADP_SetLayerPriority;
        AdpCallBackFunction->DRV_HIFB_ADP_UpLayerReg = DRV_HIFB_ADP_UpLayerReg;
        AdpCallBackFunction->DRV_HIFB_ADP_GetDhd0Info = DRV_HIFB_ADP_GetDhd0Info;
        AdpCallBackFunction->DRV_HIFB_ADP_GetCloseState = DRV_HIFB_ADP_GetCloseState;
        AdpCallBackFunction->DRV_HIFB_ADP_WaitVBlank = DRV_HIFB_ADP_WaitVBlank;
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayKeyMask = DRV_HIFB_ADP_SetLayKeyMask;
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerPreMult = DRV_HIFB_ADP_SetLayerPreMult;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerPreMult = DRV_HIFB_ADP_GetLayerPreMult;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerHdr = DRV_HIFB_ADP_GetLayerHdr;
        AdpCallBackFunction->DRV_HIFB_ADP_GetClosePreMultState = DRV_HIFB_ADP_GetClosePreMultState;
        AdpCallBackFunction->DRV_HIFB_ADP_ReadRegister = DRV_HIFB_ADP_ReadRegister;
        AdpCallBackFunction->DRV_HIFB_ADP_WriteRegister = DRV_HIFB_ADP_WriteRegister;
#ifndef HI_BUILD_IN_BOOT
        AdpCallBackFunction->DRV_HIFB_ADP_WhetherDiscardFrame = DRV_HIFB_ADP_WhetherDiscardFrame;
        AdpCallBackFunction->DRV_HIFB_ADP_SetCallback = DRV_HIFB_ADP_SetCallback;
#endif
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerStride = DRV_HIFB_ADP_SetLayerStride;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerStride = DRV_HIFB_ADP_GetLayerStride;
        AdpCallBackFunction->DRV_HIFB_ADP_SetLayerRect = DRV_HIFB_ADP_SetLayerRect;
        AdpCallBackFunction->DRV_HIFB_ADP_GetOutRect = DRV_HIFB_ADP_GetOutRect;
        AdpCallBackFunction->DRV_HIFB_ADP_GetLayerRect = DRV_HIFB_ADP_GetLayerRect;
        AdpCallBackFunction->DRV_HIFB_ADP_SetGPMask = DRV_HIFB_ADP_SetGPMask;
        AdpCallBackFunction->DRV_HIFB_ADP_GetGPMask = DRV_HIFB_ADP_GetGPMask;
        AdpCallBackFunction->DRV_HIFB_ADP_GetDispFMTSize = DRV_HIFB_ADP_GetDispFMTSize;
        AdpCallBackFunction->DRV_HIFB_ADP_ClearLogoOsd = DRV_HIFB_ADP_ClearLogoOsd;
#if !defined(HI_BUILD_IN_BOOT) && defined(CONFIG_HIFB_STEREO_SUPPORT)
        AdpCallBackFunction->DRV_HIFB_ADP_SetStereoDepth = DRV_HIFB_ADP_SetStereoDepth;
#endif
        AdpCallBackFunction->DRV_HIFB_ADP_SetTCFlag = DRV_HIFB_ADP_SetTCFlag;
        AdpCallBackFunction->DRV_HIFB_ADP_SetDeCmpSwitch = DRV_HIFB_ADP_SetDeCmpSwitch;
        AdpCallBackFunction->DRV_HIFB_ADP_GetDeCmpSwitch = DRV_HIFB_ADP_GetDeCmpSwitch;
        AdpCallBackFunction->drv_hifb_adp_set_decmp_info = drv_hifb_adp_set_decmp_info;
#ifndef HI_BUILD_IN_BOOT
        AdpCallBackFunction->DRV_HIFB_ADP_SetLowPowerInfo = DRV_HIFB_ADP_SetLowPowerInfo;
#endif
        AdpCallBackFunction->DRV_HIFB_ADP_GetDecompressStatus = DRV_HIFB_ADP_GetDecompressStatus;
        AdpCallBackFunction->DRV_HIFB_ADP_GetHaltDispStatus = DRV_HIFB_ADP_GetHaltDispStatus;
        AdpCallBackFunction->DRV_HIFB_ADP_OpenMute = DRV_HIFB_ADP_OpenMute;
        AdpCallBackFunction->DRV_HIFB_ADP_CloseMute = DRV_HIFB_ADP_CloseMute;
        AdpCallBackFunction->drv_hifb_adp_set_up_mute = drv_hifb_adp_set_up_mute;
        AdpCallBackFunction->drv_hifb_adp_get_up_mute = drv_hifb_adp_get_up_mute;
    }
    return;
}
