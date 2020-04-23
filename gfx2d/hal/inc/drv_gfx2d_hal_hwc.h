/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d hal hwc
 * Author: sdk
 * Create: 2019-05-13
 */

#ifndef _GFX2D_HAL_HWC_H_
#define _GFX2D_HAL_HWC_H_

/* ********************************add include here******************************************** */

#include "drv_gfx2d_struct.h"
#include "drv_gfx2d_config.h"
#include "drv_gfx2d_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* ****************************** API declaration ********************************************* */
hi_s32 GFX2D_HAL_HWC_Init(hi_void);

hi_void GFX2D_HAL_HWC_Deinit(hi_void);

hi_s32 GFX2D_HAL_HWC_Open(hi_void);

hi_void GFX2D_HAL_HWC_Close(hi_void);

hi_s32 GFX2D_HAL_HWC_Compose(const drv_gfx2d_dev_id dev_id, const drv_gfx2d_compose_list *compose_list,
                             const drv_gfx2d_compose_surface *pstDstSurface, hi_void **ppNode,
                             GFX2D_HAL_DEV_TYPE_E *penNodeType);

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_s32 GFX2D_HAL_HWC_ReadProc(hi_void *p, hi_void *v);
hi_s32 GFX2D_HAL_HWC_WriteProc(struct seq_file *p, hi_void *v);
hi_void gfx2d_set_debug_level(hi_u32 debug_level);
#endif

hi_void GFX2D_HAL_HWC_GetNodeOps(GFX2D_HAL_NODE_OPS_S **ppstNodeOps);

hi_u32 GFX2D_HAL_HWC_GetIntStatus(hi_void);

hi_u32 GFX2D_HAL_HWC_GetIsrNum(hi_void);

hi_u32 GFX2D_HAL_HWC_GetBaseAddr(hi_void);

#ifdef GFX2D_ALPHADETECT_SUPPORT
hi_void GFX2D_HAL_HWC_GetTransparent(drv_gfx2d_compose_ext_info *ext_info);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _GFX2D_HAL_HWC_H_ */
