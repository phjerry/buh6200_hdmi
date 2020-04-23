/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb blit
 * Author: sdk
 * Create: 2016-01-01
 */

/* **************************** add include here*************************************************** */
#include "hi_drv_module.h"
#include "hi_drv_module.h"
#include "hi_drv_tde.h"
#include "drv_hifb_mem.h"
#include "drv_hifb_blit.h"
#include "drv_hifb_config.h"
#include "drv_hifb_debug.h"
#include "drv_hifb_adp.h"
#include <linux/hisilicon/securec.h>
/* **************************** Macro Definition ************************************************** */
#define DRV_HIFB_GET_GFX2D_EXPORTFUNCS(GetGfx2dExportFuncs)                           \
    {                                                                                 \
        hi_s32 Ret = HI_SUCCESS;                                                      \
        Ret = osal_exportfunc_get(HI_ID_TDE, (hi_void **)&GetGfx2dExportFuncs);       \
        if ((HI_SUCCESS != Ret) || (NULL == GetGfx2dExportFuncs)) {                   \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "get tde func failure"); \
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_exportfunc_get, Ret); \
            return HI_FAILURE;                                                        \
        }                                                                             \
    }
/* **************************** Structure Definition ********************************************** */

/* **************************** Global Variable declaration *************************************** */
static hi_bool g_is_gfx2d_color_fmt[DRV_HIFB_FMT_BUTT] = {
    HI_TRUE, /* DRV_HIFB_FMT_RGB565  */
    HI_TRUE, /* DRV_HIFB_FMT_RGB888  */
    HI_TRUE, /* DRV_HIFB_FMT_KRGB444 */
    HI_TRUE, /* DRV_HIFB_FMT_KRGB555 */

    HI_FALSE, /* DRV_HIFB_FMT_KRGB888  */
    HI_TRUE,  /* DRV_HIFB_FMT_ARGB4444 */
    HI_TRUE,  /* DRV_HIFB_FMT_ARGB1555 */
    HI_TRUE,  /* DRV_HIFB_FMT_ARGB8888 */

    HI_TRUE, /* DRV_HIFB_FMT_ARGB8565 */
    HI_TRUE, /* DRV_HIFB_FMT_RGBA4444 */
    HI_TRUE, /* DRV_HIFB_FMT_RGBA5551 */
    HI_TRUE, /* DRV_HIFB_FMT_RGBA5658 */

    HI_TRUE, /* DRV_HIFB_FMT_RGBA8888 */
    HI_TRUE, /**< BGR565 */
    HI_TRUE, /**< BGR888 */
    HI_TRUE, /**< ABGR4444 */

    HI_TRUE, /**< ABGR1555 */
    HI_TRUE, /**< ABGR8888 */
    HI_TRUE, /**< ABGR8565 */
    HI_TRUE, /**< BGR444 16bpp */

    HI_TRUE, /**< BGR555 16bpp  */
    HI_TRUE, /**< BGR888 32bpp  */
    HI_TRUE, /* DRV_HIFB_FMT_1BPP    */
    HI_TRUE, /* DRV_HIFB_FMT_2BPP    */

    HI_TRUE, /* DRV_HIFB_FMT_4BPP    */
    HI_TRUE, /* DRV_HIFB_FMT_8BPP    */
    HI_TRUE, /* DRV_HIFB_FMT_ACLUT44 */
    HI_TRUE, /* DRV_HIFB_FMT_ACLUT88 */
};

/* **************************** API forward declarations ****************************************** */
static inline hi_void DRV_HIFB_PrintBlitInfo(hi_tde_surface *pSrcSurface, hi_tde_rect *pstSrcRect,
                                             hi_tde_surface *pDstSurface, hi_tde_rect *pstDstRect, hi_tde_opt *pstOpt);

static inline hi_tde_deflicker_level HIFB_BLIT_GetAntiflickerLevel(HIFB_LAYER_ANTIFLICKER_LEVEL_E enAntiflickerLevel,
                                                                   hi_tde_deflicker_mode *penDeflickerMode);

static inline hi_tde_color_fmt HIFB_GFX2D_GetXRGBFmt(DRV_HIFB_COLOR_FMT_E Fmt);
static inline hi_tde_color_fmt HIFB_GFX2D_GetRGBFmt(DRV_HIFB_COLOR_FMT_E Fmt);
static inline hi_tde_color_fmt HIFB_GFX2D_GetClutFmt(DRV_HIFB_COLOR_FMT_E Fmt);
/* **************************** API realization *************************************************** */

/***************************************************************************************
 * func          : DRV_HIFB_GFX2D_SupportFmt
 * description   : CNcomment: 判断TDE是否支持该像素格式操作 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static hi_bool DRV_HIFB_GFX2D_SupportFmt(DRV_HIFB_COLOR_FMT_E fmt)
{
    if (fmt < DRV_HIFB_FMT_BUTT) {
        return g_is_gfx2d_color_fmt[fmt];
    }

    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, fmt);
    GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, DRV_HIFB_FMT_BUTT);
    return HI_FALSE;
}

/***************************************************************************************
 * func          : HIFB_GFX2D_ConvFmt
 * description   : CNcomment:FB像素格式转成TDE像素格式 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_tde_color_fmt HIFB_GFX2D_ConvFmt(DRV_HIFB_COLOR_FMT_E Fmt)
{
    hi_tde_color_fmt eTdeColorFmt = HI_TDE_COLOR_FMT_MAX;

    eTdeColorFmt = HIFB_GFX2D_GetRGBFmt(Fmt);
    if (HI_TDE_COLOR_FMT_MAX != eTdeColorFmt) {
        return eTdeColorFmt;
    }

    eTdeColorFmt = HIFB_GFX2D_GetXRGBFmt(Fmt);
    if (HI_TDE_COLOR_FMT_MAX != eTdeColorFmt) {
        return eTdeColorFmt;
    }

    eTdeColorFmt = HIFB_GFX2D_GetClutFmt(Fmt);
    if (HI_TDE_COLOR_FMT_MAX != eTdeColorFmt) {
        return eTdeColorFmt;
    }

    return HI_TDE_COLOR_FMT_MAX;
}

static inline hi_tde_color_fmt HIFB_GFX2D_GetXRGBFmt(DRV_HIFB_COLOR_FMT_E Fmt)
{
    switch (Fmt) {
        case DRV_HIFB_FMT_KRGB888:
            return HI_TDE_COLOR_FMT_ARGB8888;
        case DRV_HIFB_FMT_ARGB4444:
            return HI_TDE_COLOR_FMT_ARGB4444;
        case DRV_HIFB_FMT_ARGB1555:
            return HI_TDE_COLOR_FMT_ARGB1555;
        case DRV_HIFB_FMT_ARGB8888:
            return HI_TDE_COLOR_FMT_ARGB8888;
        case DRV_HIFB_FMT_ARGB8565:
            return HI_TDE_COLOR_FMT_ARGB8565;
        case DRV_HIFB_FMT_ABGR1555:
            return HI_TDE_COLOR_FMT_ABGR1555;
        case DRV_HIFB_FMT_ABGR4444:
            return HI_TDE_COLOR_FMT_ABGR4444;
        case DRV_HIFB_FMT_ABGR8888:
            return HI_TDE_COLOR_FMT_ABGR8888;
        case DRV_HIFB_FMT_ABGR8565:
            return HI_TDE_COLOR_FMT_ABGR8565;
        case DRV_HIFB_FMT_KBGR888:
            return HI_TDE_COLOR_FMT_ABGR8888;
        default:
            return HI_TDE_COLOR_FMT_MAX;
    }
}

static inline hi_tde_color_fmt HIFB_GFX2D_GetRGBFmt(DRV_HIFB_COLOR_FMT_E Fmt)
{
    switch (Fmt) {
        case DRV_HIFB_FMT_RGB565:
            return HI_TDE_COLOR_FMT_RGB565;
        case DRV_HIFB_FMT_RGB888:
            return HI_TDE_COLOR_FMT_RGB888;
        case DRV_HIFB_FMT_KRGB444:
            return HI_TDE_COLOR_FMT_RGB444;
        case DRV_HIFB_FMT_KRGB555:
            return HI_TDE_COLOR_FMT_RGB555;
        case DRV_HIFB_FMT_BGR565:
            return HI_TDE_COLOR_FMT_BGR565;
        case DRV_HIFB_FMT_KBGR555:
            return HI_TDE_COLOR_FMT_BGR555;
        case DRV_HIFB_FMT_KBGR444:
            return HI_TDE_COLOR_FMT_BGR444;
        case DRV_HIFB_FMT_BGR888:
            return HI_TDE_COLOR_FMT_BGR888;
        default:
            return HI_TDE_COLOR_FMT_MAX;
    }
}

static inline hi_tde_color_fmt HIFB_GFX2D_GetClutFmt(DRV_HIFB_COLOR_FMT_E Fmt)
{
    switch (Fmt) {
        case DRV_HIFB_FMT_1BPP:
            return HI_TDE_COLOR_FMT_CLUT1;
        case DRV_HIFB_FMT_2BPP:
            return HI_TDE_COLOR_FMT_CLUT2;
        case DRV_HIFB_FMT_4BPP:
            return HI_TDE_COLOR_FMT_CLUT4;
        case DRV_HIFB_FMT_8BPP:
            return HI_TDE_COLOR_FMT_CLUT8;
        case DRV_HIFB_FMT_ACLUT44:
            return HI_TDE_COLOR_FMT_ACLUT44;
        case DRV_HIFB_FMT_ACLUT88:
            return HI_TDE_COLOR_FMT_ACLUT88;
        default:
            return HI_TDE_COLOR_FMT_MAX;
    }
}

static inline hi_bool HIFB_GFX2D_IsColorFmtClut(hi_tde_color_fmt enColorFmt)
{
    if ((enColorFmt >= HI_TDE_COLOR_FMT_CLUT1) && (enColorFmt <= HI_TDE_COLOR_FMT_ACLUT88)) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

/***************************************************************************************
 * func         : DRV_HIFB_GFX2D_QuickCopy
 * description  : CNcomment: tde quickcopy CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_GFX2D_QuickCopy(HIFB_BUFFER_S *pSrcImg, HIFB_BUFFER_S *pDstImg, hi_bool mmz_to_smmu)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_s32 handle = 0;
    hi_tde_surface stSrcSur = {0}, stDstSur = {0};
    hi_tde_rect stSrcRect = {0}, stDstRect = {0};
    hi_tde_export_func *Gfx2dExportFuncs = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pSrcImg, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pDstImg, HI_FAILURE);

    stSrcSur.phy_addr = pSrcImg->stCanvas.u32PhyAddr;
    stSrcSur.width = pSrcImg->stCanvas.u32Width;
    stSrcSur.height = pSrcImg->stCanvas.u32Height;
    stSrcSur.stride = pSrcImg->stCanvas.u32Pitch;
    stSrcSur.alpha_max_is_255 = HI_TRUE;
    stSrcSur.is_ycbcr_clut = HI_FALSE;
    stSrcSur.color_fmt = HIFB_GFX2D_ConvFmt(pSrcImg->stCanvas.enFmt);
    stSrcSur.alpha0 = 0;
    stSrcSur.alpha1 = 0;
    stSrcRect.pos_x = pSrcImg->UpdateRect.x;
    stSrcRect.pos_y = pSrcImg->UpdateRect.y;
    stSrcRect.width = pSrcImg->UpdateRect.w;
    stSrcRect.height = pSrcImg->UpdateRect.h;

    stDstSur.phy_addr = pDstImg->stCanvas.u32PhyAddr;
    stDstSur.width = pDstImg->stCanvas.u32Width;
    stDstSur.height = pDstImg->stCanvas.u32Height;
    stDstSur.stride = pDstImg->stCanvas.u32Pitch;
    stDstSur.alpha_max_is_255 = HI_TRUE;
    stDstSur.is_ycbcr_clut = HI_FALSE;
    stDstSur.color_fmt = HIFB_GFX2D_ConvFmt(pDstImg->stCanvas.enFmt);
    stDstSur.alpha0 = 0;
    stDstSur.alpha1 = 0;
    stDstRect.pos_x = pDstImg->UpdateRect.x;
    stDstRect.pos_y = pDstImg->UpdateRect.y;
    stDstRect.width = pDstImg->UpdateRect.w;
    stDstRect.height = pDstImg->UpdateRect.h;

    DRV_HIFB_PrintBlitInfo(&stSrcSur, &stSrcRect, &stDstSur, &stDstRect, NULL);

    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_begin_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_quick_copy, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_end_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_cancel_job, HI_FAILURE);

    Ret = Gfx2dExportFuncs->drv_tde_module_begin_job(&handle);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_begin_job, Ret);
        return Ret;
    }

    if (HI_FALSE == mmz_to_smmu) {
        Ret = Gfx2dExportFuncs->drv_tde_module_quick_copy(handle, &stSrcSur, &stSrcRect, &stDstSur, &stDstRect);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        Ret = Gfx2dExportFuncs->drv_tde_module_quick_copyex(handle, &stSrcSur, &stSrcRect, &stDstSur, &stDstRect,
                                                            HI_TRUE, HI_FALSE);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_quick_copy, Ret);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_quick_copyex, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(handle);
        return Ret;
    }

    Ret = Gfx2dExportFuncs->drv_tde_module_end_job(handle, HI_TRUE, 2000, HI_FALSE, NULL, NULL);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_end_job, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(handle);
        return Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func           : DRV_HIFB_GFX2D_Blit
 * description    : CNcomment: tde blit CNend\n
 * param[in]      : hi_void
 * retval         : NA
 * others:        : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_GFX2D_Blit(HIFB_BUFFER_S *pSrcImg, HIFB_BUFFER_S *pDstImg, HIFB_BLIT_OPT_S *pstOpt,
                                  hi_bool bRefreshScreen)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_s32 handle = 0;
    hi_tde_surface stSrcSur = {0}, stDstSur = {0};
    hi_tde_rect stSrcRect = {0}, stDstRect = {0};
    hi_tde_opt stOpt = {0};
    hi_tde_deflicker_level enTdeDflkLevel = HI_TDE_DEFLICKER_LEVEL_MAX;
    hi_tde_export_func *Gfx2dExportFuncs = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pSrcImg, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pDstImg, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOpt, HI_FAILURE);

    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_set_deflicker_level, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_enable_region_deflicker, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_begin_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_blit, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_end_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_cancel_job, HI_FAILURE);

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, pSrcImg->stCanvas.u32PhyAddr, HI_FAILURE);

    stSrcSur.phy_addr = pSrcImg->stCanvas.u32PhyAddr;
    stSrcSur.width = pSrcImg->stCanvas.u32Width;
    stSrcSur.height = pSrcImg->stCanvas.u32Height;
    stSrcSur.stride = pSrcImg->stCanvas.u32Pitch;
    stSrcSur.alpha_max_is_255 = HI_TRUE;
    stSrcSur.is_ycbcr_clut = HI_FALSE;
    stSrcSur.color_fmt = HIFB_GFX2D_ConvFmt(pSrcImg->stCanvas.enFmt);
    stSrcSur.alpha0 = pstOpt->stAlpha.u8Alpha0;
    stSrcSur.alpha1 = pstOpt->stAlpha.u8Alpha1;

    if (!((stSrcSur.alpha0 == 0) && (stSrcSur.alpha1 == 0))) {
        stSrcSur.support_alpha_ext_1555 = HI_TRUE;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    stSrcRect.pos_x = pSrcImg->UpdateRect.x;
    stSrcRect.pos_y = pSrcImg->UpdateRect.y;
    stSrcRect.width = pSrcImg->UpdateRect.w;
    stSrcRect.height = pSrcImg->UpdateRect.h;

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, pDstImg->stCanvas.u32PhyAddr, HI_FAILURE);

    stDstSur.phy_addr = pDstImg->stCanvas.u32PhyAddr;
    stDstSur.width = pDstImg->stCanvas.u32Width;
    stDstSur.height = pDstImg->stCanvas.u32Height;
    stDstSur.stride = pDstImg->stCanvas.u32Pitch;
    stDstSur.alpha_max_is_255 = HI_TRUE;
    stDstSur.is_ycbcr_clut = HI_FALSE;
    stDstSur.color_fmt = HIFB_GFX2D_ConvFmt(pDstImg->stCanvas.enFmt);
    stDstSur.alpha0 = pstOpt->stAlpha.u8Alpha0;
    stDstSur.alpha1 = pstOpt->stAlpha.u8Alpha1;

    stDstRect.pos_x = pDstImg->UpdateRect.x;
    stDstRect.pos_y = pDstImg->UpdateRect.y;
    stDstRect.width = pDstImg->UpdateRect.w;
    stDstRect.height = pDstImg->UpdateRect.h;

    stOpt.resize = pstOpt->bScale;

    if (HIFB_GFX2D_IsColorFmtClut(stSrcSur.color_fmt)) {
        stOpt.clut_reload = HI_TRUE;
        stSrcSur.clut_phy_addr = (hi_u64)(uintptr_t)pstOpt->pu8CmapAddr;
        stDstSur.clut_phy_addr = (hi_u64)(uintptr_t)pstOpt->pu8CmapAddr;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    enTdeDflkLevel = HIFB_BLIT_GetAntiflickerLevel(pstOpt->enAntiflickerLevel, &stOpt.deflicker_mode);
    if (HI_TDE_DEFLICKER_LEVEL_MAX != enTdeDflkLevel) {
        Gfx2dExportFuncs->drv_tde_module_set_deflicker_level(enTdeDflkLevel);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if (pstOpt->stCKey.bKeyEnable) {
        if (HIFB_GFX2D_IsColorFmtClut(stSrcSur.color_fmt)) {
            stOpt.color_key_mode = HI_TDE_COLOR_KEY_MODE_FOREGROUND;
            stOpt.color_key_value.clut_color_key.alpha.is_component_ignore = HI_TRUE;
            stOpt.color_key_value.clut_color_key.clut.is_component_out = pstOpt->stCKey.u32KeyMode;
            stOpt.color_key_value.clut_color_key.clut.component_max = pstOpt->stCKey.u8BlueMax;
            stOpt.color_key_value.clut_color_key.clut.component_min = pstOpt->stCKey.u8BlueMin;
            stOpt.color_key_value.clut_color_key.clut.component_mask = 0xff;
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        } else {
            stOpt.color_key_mode = HI_TDE_COLOR_KEY_MODE_FOREGROUND;
            stOpt.color_key_value.argb_color_key.alpha.is_component_ignore = HI_TRUE;
            stOpt.color_key_value.argb_color_key.red.component_max = pstOpt->stCKey.u8RedMax;
            stOpt.color_key_value.argb_color_key.red.component_min = pstOpt->stCKey.u8RedMin;
            stOpt.color_key_value.argb_color_key.red.is_component_out = pstOpt->stCKey.u32KeyMode;
            stOpt.color_key_value.argb_color_key.red.component_mask = 0xff;

            stOpt.color_key_value.argb_color_key.green.component_max = pstOpt->stCKey.u8GreenMax;
            stOpt.color_key_value.argb_color_key.green.component_min = pstOpt->stCKey.u8GreenMin;
            stOpt.color_key_value.argb_color_key.green.is_component_out = pstOpt->stCKey.u32KeyMode;
            stOpt.color_key_value.argb_color_key.green.component_mask = 0xff;

            stOpt.color_key_value.argb_color_key.blue.component_max = pstOpt->stCKey.u8BlueMax;
            stOpt.color_key_value.argb_color_key.blue.component_min = pstOpt->stCKey.u8BlueMin;
            stOpt.color_key_value.argb_color_key.blue.is_component_out = pstOpt->stCKey.u32KeyMode;
            stOpt.color_key_value.argb_color_key.blue.component_mask = 0xff;
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        }
    }

    stOpt.global_alpha = 255;

    if (pstOpt->stAlpha.bAlphaEnable) {
        stOpt.alpha_blending_cmd = HI_TDE_ALPHA_BLENDING_BLEND;
        stOpt.global_alpha = pstOpt->stAlpha.u8GlobalAlpha;
        stOpt.out_alpha_from = HI_TDE_OUT_ALPHA_FROM_NORM;
        stOpt.blend_opt.global_alpha_en = HI_TRUE;
        stOpt.blend_opt.pixel_alpha_en = HI_TRUE;
        stOpt.blend_opt.src1_alpha_premulti = HI_TRUE;
        stOpt.blend_opt.src2_alpha_premulti = HI_TRUE;
        stOpt.blend_opt.blend_cmd = HI_TDE_BLEND_CMD_SRC;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        stOpt.out_alpha_from = HI_TDE_OUT_ALPHA_FROM_FOREGROUND;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    Ret = Gfx2dExportFuncs->drv_tde_module_enable_region_deflicker(pstOpt->bRegionDeflicker);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_enable_region_deflicker, Ret);
        return Ret;
    }

    if (pstOpt->stClip.bClip) {
        stOpt.clip_mode = pstOpt->stClip.bInRegionClip ? HI_TDE_CLIP_MODE_INSIDE : HI_TDE_CLIP_MODE_OUTSIDE;
        stOpt.clip_rect.pos_x = pstOpt->stClip.stClipRect.x;
        stOpt.clip_rect.pos_y = pstOpt->stClip.stClipRect.y;
        stOpt.clip_rect.width = pstOpt->stClip.stClipRect.w;
        stOpt.clip_rect.height = pstOpt->stClip.stClipRect.h;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    DRV_HIFB_PrintBlitInfo(&stSrcSur, &stSrcRect, &stDstSur, &stDstRect, &stOpt);

    Ret = Gfx2dExportFuncs->drv_tde_module_begin_job(&handle);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_begin_job, Ret);
        return Ret;
    }

    Ret = Gfx2dExportFuncs->drv_tde_module_blit(handle, NULL, NULL, &stSrcSur, &stSrcRect, &stDstSur, &stDstRect,
                                                &stOpt);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_blit, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(handle);
        return Ret;
    }

    if (pstOpt->bCallBack) {
        Ret = Gfx2dExportFuncs->drv_tde_module_end_job(handle, pstOpt->bBlock, CONFIG_BLOCK_TIME, HI_FALSE,
                                                       (drv_tde_func_callback)pstOpt->pfnCallBack, pstOpt->pParam);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        Ret = Gfx2dExportFuncs->drv_tde_module_end_job(handle, pstOpt->bBlock, CONFIG_BLOCK_TIME, HI_FALSE, NULL, NULL);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_end_job, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(handle);
        return Ret;
    }

    if (pstOpt->bRegionDeflicker) {
        Ret = Gfx2dExportFuncs->drv_tde_module_enable_region_deflicker(HI_FALSE);
        if (HI_SUCCESS != Ret) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_enable_region_deflicker, Ret);
            return Ret;
        }
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return handle;
}

static inline hi_tde_deflicker_level HIFB_BLIT_GetAntiflickerLevel(HIFB_LAYER_ANTIFLICKER_LEVEL_E enAntiflickerLevel,
                                                                   hi_tde_deflicker_mode *penDeflickerMode)
{
    switch (enAntiflickerLevel) {
        case HIFB_LAYER_ANTIFLICKER_NONE:
            if (NULL != penDeflickerMode) {
                *penDeflickerMode = HI_TDE_DEFLICKER_MODE_NONE;
            }
            return HI_TDE_DEFLICKER_LEVEL_MAX;
        case HIFB_LAYER_ANTIFLICKER_LOW:
            if (NULL != penDeflickerMode) {
                *penDeflickerMode = HI_TDE_DEFLICKER_MODE_BOTH;
            }
            return HI_TDE_DEFLICKER_LEVEL_LOW;
        case HIFB_LAYER_ANTIFLICKER_MIDDLE:
            if (NULL != penDeflickerMode) {
                *penDeflickerMode = HI_TDE_DEFLICKER_MODE_BOTH;
            }
            return HI_TDE_DEFLICKER_LEVEL_MIDDLE;
        case HIFB_LAYER_ANTIFLICKER_HIGH:
            if (NULL != penDeflickerMode) {
                *penDeflickerMode = HI_TDE_DEFLICKER_MODE_BOTH;
            }
            return HI_TDE_DEFLICKER_LEVEL_HIGH;
        case HIFB_LAYER_ANTIFLICKER_AUTO:
            if (NULL != penDeflickerMode) {
                *penDeflickerMode = HI_TDE_DEFLICKER_MODE_BOTH;
            }
            return HI_TDE_DEFLICKER_LEVEL_AUTO;
        default:
            if (NULL != penDeflickerMode) {
                *penDeflickerMode = HI_TDE_DEFLICKER_MODE_NONE;
            }
            return HI_TDE_DEFLICKER_LEVEL_MAX;
    }
}

static hi_s32 DRV_HIFB_GFX2D_MbBlitEx(HIFB_BLIT_BUFFER_S *pSrcImg, HIFB_BLIT_BUFFER_S *pDstImg, HIFB_BLIT_OPT_S *pstOpt)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_s32 BlitHandle = 0;
    hi_tde_surface stSrcSur = {0}, stDstSur = {0};
    hi_tde_rect stSrcRect = {0}, stDstRect = {0};
    hi_tde_opt stOpt = {0};
    hi_tde_export_func *Gfx2dExportFuncs = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pSrcImg, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pDstImg, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOpt, HI_FAILURE);

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, pSrcImg->stCanvas.YAddr, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, pSrcImg->stCanvas.CbCrAddr, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, pDstImg->stCanvas.YAddr, HI_FAILURE);

    memset_s(&stSrcSur, sizeof(stSrcSur), 0x0, sizeof(stSrcSur));
    stSrcSur.phy_addr = pSrcImg->stCanvas.YAddr;
    stSrcSur.cbcr_phy_addr = pSrcImg->stCanvas.CbCrAddr;
    stSrcSur.width = pSrcImg->stCanvas.Width;
    stSrcSur.height = pSrcImg->stCanvas.Height;
    stSrcSur.stride = pSrcImg->stCanvas.YStride;
    stSrcSur.cbcr_stride = pSrcImg->stCanvas.CbCrStride;
    stSrcSur.color_fmt = HIFB_GFX2D_ConvFmt(pSrcImg->stCanvas.enFmt);
    stSrcSur.alpha_max_is_255 = HI_TRUE;

    stSrcRect.pos_x = pSrcImg->UpdateRect.x;
    stSrcRect.pos_y = pSrcImg->UpdateRect.y;
    stSrcRect.width = pSrcImg->UpdateRect.w;
    stSrcRect.height = pSrcImg->UpdateRect.h;

    memset_s(&stDstSur, sizeof(stDstSur), 0x0, sizeof(stDstSur));
    stDstSur.phy_addr = pDstImg->stCanvas.YAddr;
    stDstSur.width = pDstImg->stCanvas.Width;
    stDstSur.height = pDstImg->stCanvas.Height;
    stDstSur.stride = pDstImg->stCanvas.YStride;
    stDstSur.color_fmt = HIFB_GFX2D_ConvFmt(pDstImg->stCanvas.enFmt);
    stDstSur.alpha_max_is_255 = HI_TRUE;

    stDstRect.pos_x = pDstImg->UpdateRect.x;
    stDstRect.pos_y = pDstImg->UpdateRect.y;
    stDstRect.width = pDstImg->UpdateRect.w;
    stDstRect.height = pDstImg->UpdateRect.h;

    memset_s(&stOpt, sizeof(stOpt), 0x0, sizeof(stOpt));
    stOpt.resize = pstOpt->bScale;

    DRV_HIFB_PrintBlitInfo(&stSrcSur, &stSrcRect, &stDstSur, &stDstRect, &stOpt);

    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_begin_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_single_blitex, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_end_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_cancel_job, HI_FAILURE);

    Ret = Gfx2dExportFuncs->drv_tde_module_begin_job(&BlitHandle);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_begin_job, Ret);
        return Ret;
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    Ret = Gfx2dExportFuncs->drv_tde_module_single_blitex(BlitHandle, &stSrcSur, &stSrcRect, &stDstSur, &stDstRect,
                                                         &stOpt, HI_TRUE, HI_FALSE);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
#else
    Ret = Gfx2dExportFuncs->drv_tde_module_single_blitex(BlitHandle, &stSrcSur, &stSrcRect, &stDstSur, &stDstRect,
                                                         &stOpt, HI_TRUE, HI_TRUE);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
#endif
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_single_blitex, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(BlitHandle);
        return Ret;
    }

    Ret = Gfx2dExportFuncs->drv_tde_module_end_job(BlitHandle, HI_TRUE, 100, HI_FALSE, NULL, NULL);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_end_job, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(BlitHandle);
        return Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return BlitHandle;
}

/***************************************************************************************
 * func         : DRV_HIFB_GFX2D_ClearRect
 * description  : CNcomment:清surface CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_GFX2D_ClearRect(HIFB_SURFACE_S *pDstImg, HIFB_BLIT_OPT_S *pstOpt)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_surface TDESurface = {0};
    hi_tde_rect Rect = {0};
    hi_s32 s32Handle = 0;
    hi_tde_export_func *Gfx2dExportFuncs = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOpt, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pDstImg, HI_FAILURE);

    /* * confing dst */
    TDESurface.phy_addr = pDstImg->u32PhyAddr;
    TDESurface.width = pDstImg->u32Width;
    TDESurface.height = pDstImg->u32Height;
    TDESurface.stride = pDstImg->u32Pitch;
    TDESurface.alpha_max_is_255 = HI_TRUE;
    TDESurface.is_ycbcr_clut = HI_FALSE;
    TDESurface.color_fmt = HIFB_GFX2D_ConvFmt(pDstImg->enFmt);
    TDESurface.alpha0 = pstOpt->stAlpha.u8Alpha0;
    TDESurface.alpha1 = pstOpt->stAlpha.u8Alpha1;

    Rect.pos_x = 0;
    Rect.pos_y = 0;
    Rect.width = pDstImg->u32Width;
    Rect.height = pDstImg->u32Height;

    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_begin_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_quick_fill, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_end_job, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_cancel_job, HI_FAILURE);

    Ret = Gfx2dExportFuncs->drv_tde_module_begin_job(&s32Handle);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_begin_job, Ret);
        return Ret;
    }

    Ret = Gfx2dExportFuncs->drv_tde_module_quick_fill(s32Handle, &TDESurface, &Rect, 0x0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_quick_fill, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(s32Handle);
        return Ret;
    }

    Ret = Gfx2dExportFuncs->drv_tde_module_end_job(s32Handle, pstOpt->bBlock, 100, HI_FALSE, NULL, NULL);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_end_job, Ret);
        Gfx2dExportFuncs->drv_tde_module_cancel_job(s32Handle);
        return Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return s32Handle;
}

static hi_s32 DRV_HIFB_GFX2D_WaitAllDone(hi_bool bSync)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_export_func *Gfx2dExportFuncs = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_wait_all_done, HI_FAILURE);

    Ret = Gfx2dExportFuncs->drv_tde_module_wait_all_done(bSync);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_wait_all_done, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_GFX2D_CalScaleRect
 * description  : CNcomment: 计算缩放更新区域 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 DRV_HIFB_GFX2D_CalScaleRect(const hi_tde_rect *pstSrcRect, const hi_tde_rect *pstDstRect,
                                          hi_tde_rect *pstRectInSrc, hi_tde_rect *pstRectInDst)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_export_func *Gfx2dExportFuncs = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSrcRect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDstRect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRectInSrc, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRectInDst, HI_FAILURE);
    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_calc_scale_rect, HI_FAILURE);

    Ret = Gfx2dExportFuncs->drv_tde_module_calc_scale_rect(pstSrcRect, pstDstRect, pstRectInSrc, pstRectInDst);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_calc_scale_rect, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GFX2D_WaitForDone(hi_s32 s32Handle, hi_u32 u32TimeOut)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_export_func *Gfx2dExportFuncs = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(Gfx2dExportFuncs->drv_tde_module_wait_for_done, HI_FAILURE);

    Ret = Gfx2dExportFuncs->drv_tde_module_wait_for_done(s32Handle, u32TimeOut);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_wait_for_done, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 DRV_HIFB_GFX2D_Open(hi_void)
{
    hi_s32 Ret = HI_FAILURE;
    hi_tde_export_func *Gfx2dExportFuncs = NULL;

    DRV_HIFB_GET_GFX2D_EXPORTFUNCS(Gfx2dExportFuncs);
    if (NULL != Gfx2dExportFuncs->drv_tde_module_open) {
        Ret = Gfx2dExportFuncs->drv_tde_module_open();
    }
    if (Ret < 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_open, Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void DRV_HIFB_GFX2D_Close(hi_void)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_export_func *Gfx2dExportFuncs = NULL;

    Ret = osal_exportfunc_get(HI_ID_TDE, (hi_void **)&Gfx2dExportFuncs);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_exportfunc_get, Ret);
        return;
    }

    if ((NULL != Gfx2dExportFuncs) && (NULL != Gfx2dExportFuncs->drv_tde_module_close)) {
        Gfx2dExportFuncs->drv_tde_module_close();
    }

    return;
}

hi_void DRV_HIFB_GFX2D_GetCallBackFunction(HIFB_DRV_TDEOPS_S *Gfx2dCallBackFunction)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (NULL != Gfx2dCallBackFunction) {
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_Open = DRV_HIFB_GFX2D_Open;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_Close = DRV_HIFB_GFX2D_Close;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_QuickCopy = DRV_HIFB_GFX2D_QuickCopy;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_Blit = DRV_HIFB_GFX2D_Blit;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_MbBlitEx = DRV_HIFB_GFX2D_MbBlitEx;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_CalScaleRect = DRV_HIFB_GFX2D_CalScaleRect;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_ClearRect = DRV_HIFB_GFX2D_ClearRect;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_SupportFmt = DRV_HIFB_GFX2D_SupportFmt;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_WaitAllDone = DRV_HIFB_GFX2D_WaitAllDone;
        Gfx2dCallBackFunction->DRV_HIFB_GFX2D_WaitForDone = DRV_HIFB_GFX2D_WaitForDone;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void DRV_HIFB_PrintBlitInfo(hi_tde_surface *pSrcSurface, hi_tde_rect *pstSrcRect,
                                             hi_tde_surface *pDstSurface, hi_tde_rect *pstDstRect, hi_tde_opt *pstOpt)
{
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    if (NULL != pstSrcRect) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstSrcRect->pos_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstSrcRect->pos_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSrcRect->width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSrcRect->height);
    }

    if (NULL != pSrcSurface) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pSrcSurface->phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pSrcSurface->width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pSrcSurface->height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pSrcSurface->stride);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pSrcSurface->cbcr_phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pSrcSurface->cbcr_stride);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pSrcSurface->color_fmt);
    }

    if (NULL != pstDstRect) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstDstRect->pos_x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstDstRect->pos_y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstDstRect->width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstDstRect->height);
    }

    if (NULL != pDstSurface) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pDstSurface->phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pDstSurface->width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pDstSurface->height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pDstSurface->stride);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pDstSurface->cbcr_phy_addr);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pDstSurface->cbcr_stride);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pDstSurface->color_fmt);
    }

    if (NULL != pstOpt) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstOpt->resize);
    }

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    return;
}
