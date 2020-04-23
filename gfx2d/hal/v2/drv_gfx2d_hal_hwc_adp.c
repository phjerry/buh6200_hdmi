/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d hal hwc adapter
 * Author: sdk
 * Create: 2019-05-13
 */

/* ********************************add include here*********************************************** */
#include "hi_type.h"
#include "drv_gfx2d_hal_hwc_adp.h"
#include "hi_gfx_sys_k.h"
#include "drv_gfx2d_debug.h"
#include "drv_gfx2d_hal.h"
#include <linux/hisilicon/securec.h>

/* ********************* Global Variable declaration ********************************************* */
static gfx2d_capability_info g_gfx2d_capability_info = {
    GFX2D_MAX_LAYERS,
    7,
    1,

    { 0xFFFFFFF, 0xFF7E0 },

    { 0xDC1DCF0, 0x0 },

    1,
    0xffff,
    16,
    1,
#if HWC_VERSION == 0x500
    16384,
#elif HWC_VERSION == 0x400
    2560,
#elif HWC_VERSION == 0x300
    3840,
#else
    4096,
#endif

    1,
#if HWC_VERSION == 0x500
    16384,
#elif HWC_VERSION == 0x400
    1600,
#elif HWC_VERSION == 0x300
    2160,
#else
    2160,
#endif

    32,
    32,

    16,
    16,

    2,
    { 0xC400000, 0x0 },

    HI_TRUE,
    HI_FALSE,
    3,
    2,
};

/* ****************************** API realization ************************************************ */

hi_void HWC_ADP_GetCapability(gfx2d_capability_info *pstCapability)
{
    if (pstCapability != NULL) {
        if (memcpy_s(pstCapability, sizeof(gfx2d_capability_info), &g_gfx2d_capability_info,
                     sizeof(gfx2d_capability_info)) != EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "call memcpy_s failed");
            return;
        }
    }

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u8MaxLayerNum);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u8MaxGLayerNum);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u8MaxVLayerNum);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.au32InputFmtBitmap[0]);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.au32InputFmtBitmap[1]);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.au32OutputFmtBitmap[0]);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.au32OutputFmtBitmap[1]);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MinStride);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MaxStride);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u8StrideAlign);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MinWidth);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MaxWidth);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MinHeight);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MaxHeight);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MinVWidth);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MinVHeight);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MaxHorizonZmeRatio);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32MaxVerticalZmeRatio);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u32CmpWidthAlign);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.au32CmpFmtBitmap[0]);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.au32CmpFmtBitmap[1]);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.bGZmeSupport);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.bClipSupport);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u8ZmeLayerNum);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability_info.u8ZmeAlign);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");

    return;
}
