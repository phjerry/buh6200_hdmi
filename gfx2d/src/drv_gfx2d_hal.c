/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d hal layer
 * Author: sdk
 * Create: 2019-05-13
 */

/* ********************************add include here*********************************************** */
#include <linux/fs.h>
#include "hi_osal.h"

#include "drv_gfx2d_struct.h"
#include "drv_gfx2d_hal.h"
#include "drv_gfx2d_hal_hwc.h"
#include "drv_gfx2d_debug.h"

/* ****************************** API realization ************************************************ */
hi_s32 GFX2D_HAL_Init(hi_void)
{
    hi_s32 Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = GFX2D_HAL_HWC_Init();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

hi_void GFX2D_HAL_Deinit(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GFX2D_HAL_HWC_Deinit();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

hi_s32 GFX2D_HAL_Open(hi_void)
{
    hi_s32 Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = GFX2D_HAL_HWC_Open();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

hi_void GFX2D_HAL_Close(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GFX2D_HAL_HWC_Close();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

hi_s32 GFX2D_HAL_Compose(const drv_gfx2d_dev_id dev_id, const drv_gfx2d_compose_list *compose_list,
                         const drv_gfx2d_compose_surface *pstDstSurface, hi_void **ppNode,
                         GFX2D_HAL_DEV_TYPE_E *penNodeType)
{
    hi_s32 Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = GFX2D_HAL_HWC_Compose(dev_id, compose_list, pstDstSurface, ppNode, penNodeType);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

hi_void GFX2D_HAL_GetNodeOps(const GFX2D_HAL_DEV_TYPE_E enDevType, GFX2D_HAL_NODE_OPS_S **ppstNodeOps)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (enDevType == GFX2D_HAL_DEV_TYPE_HWC) {
        GFX2D_HAL_HWC_GetNodeOps(ppstNodeOps);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 GFX2D_HAL_GetIntStatus(const GFX2D_HAL_DEV_TYPE_E enDevType)
{
    hi_s32 Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = GFX2D_HAL_HWC_GetIntStatus();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

hi_u32 GFX2D_HAL_GetIsrNum(const drv_gfx2d_dev_id dev_id, const GFX2D_HAL_DEV_TYPE_E enDevType)
{
    hi_s32 Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = GFX2D_HAL_HWC_GetIsrNum();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

hi_u32 GFX2D_HAL_GetBaseAddr(const drv_gfx2d_dev_id dev_id, const GFX2D_HAL_DEV_TYPE_E enDevType)
{
    hi_s32 Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = GFX2D_HAL_HWC_GetBaseAddr();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

#ifdef GFX2D_ALPHADETECT_SUPPORT
hi_void GFX2D_HAL_GetTransparent(drv_gfx2d_compose_ext_info *ext_info)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GFX2D_HAL_HWC_GetTransparent(ext_info);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_HAL_ReadProc(hi_void *p, hi_void *v)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GFX2D_HAL_HWC_ReadProc(p, v);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif
