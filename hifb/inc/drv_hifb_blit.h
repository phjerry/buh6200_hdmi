/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb blit header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_BLIT_H__
#define __DRV_HIFB_BLIT_H__

/* ********************************add include here************************************************ */
#include <linux/fb.h>

#include "drv_tde_struct.h"
#include "drv_hifb_type.h"
#include "drv_hifb_common.h"

/**************************************************************************************************/

/**************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************** */
#define CONFIG_BLOCK_TIME 100

/* ************************** Structure Definition ************************************************ */
typedef struct {
    hi_bool bInRegionClip;
    hi_bool bClip;
    HIFB_RECT stClipRect;
} HIFB_CLIP_S;

typedef struct {
    hi_bool bScale;
    hi_bool bBlock;
    hi_bool bCallBack;
    hi_bool bRegionDeflicker;
    hi_void *pParam;
    hi_u8 *pu8CmapAddr;
    HIFB_LAYER_ANTIFLICKER_LEVEL_E enAntiflickerLevel;
    IntCallBack pfnCallBack;
    HIFB_ALPHA_S stAlpha;
    HIFB_COLORKEYEX_S stCKey;
    HIFB_CLIP_S stClip;
} HIFB_BLIT_OPT_S;

typedef struct {
    hi_u32 YAddr;
    hi_u32 CbCrAddr;
    hi_u32 Width;
    hi_u32 Height;
    hi_u32 YStride;
    hi_u32 CbCrStride;
    DRV_HIFB_COLOR_FMT_E enFmt;
} HIFB_BLIT_SURFACE_S;

typedef struct {
    HIFB_BLIT_SURFACE_S stCanvas;
    HIFB_RECT UpdateRect;
} HIFB_BLIT_BUFFER_S;

typedef struct {
    hi_s32 (*DRV_HIFB_GFX2D_QuickCopy)(HIFB_BUFFER_S *pSrcImg, HIFB_BUFFER_S *pDstImg, hi_bool mmz_to_smmu);
    hi_s32 (*DRV_HIFB_GFX2D_Blit)(HIFB_BUFFER_S *pSrcImg, HIFB_BUFFER_S *pDstImg, HIFB_BLIT_OPT_S *pstOpt,
                                  hi_bool bScreenRefresh);
    hi_s32 (*DRV_HIFB_GFX2D_MbBlitEx)(HIFB_BLIT_BUFFER_S *pSrcImg, HIFB_BLIT_BUFFER_S *pDstImg,
                                      HIFB_BLIT_OPT_S *pstOpt);
    hi_s32 (*DRV_HIFB_GFX2D_ClearRect)(HIFB_SURFACE_S *pDstImg, HIFB_BLIT_OPT_S *pstOpt);
    hi_s32 (*DRV_HIFB_GFX2D_WaitAllDone)(hi_bool bSync);
    hi_bool (*DRV_HIFB_GFX2D_SupportFmt)(DRV_HIFB_COLOR_FMT_E fmt);
    hi_s32 (*DRV_HIFB_GFX2D_CalScaleRect)(const hi_tde_rect *pstSrcRect, const hi_tde_rect *pstDstRect,
                                          hi_tde_rect *pstRectInSrc, hi_tde_rect *pstRectInDst);
    hi_s32 (*DRV_HIFB_GFX2D_WaitForDone)(hi_s32 s32Handle, hi_u32 u32TimeOut);
    hi_s32 (*DRV_HIFB_GFX2D_Open)(hi_void);
    hi_void (*DRV_HIFB_GFX2D_Close)(hi_void);
} HIFB_DRV_TDEOPS_S;

/* ********************* Global Variable declaration ********************************************** */

/* ****************************** API declaration ************************************************* */
hi_void DRV_HIFB_GFX2D_GetCallBackFunction(HIFB_DRV_TDEOPS_S *Gfx2dCallBackFunction);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
