/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: list operation header
 * Author: sdk
 * Create: 2019-05-17
 */

#ifndef _GFX2D_LIST_H_
#define _GFX2D_LIST_H_

/* ********************************add include here******************************************** */

#include "hi_osal.h"

#include "drv_gfx2d_config.h"
#include "drv_gfx2d_hal.h"
#include "drv_gfx2d_hal_hwc_adp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* ************************** Structure Definition ******************************************** */
typedef struct {
    drv_gfx2d_dev_id dev_id;
    hi_void *pNode;
    GFX2D_HAL_DEV_TYPE_E enNodeType;
    hi_bool work_sync;
    hi_u32 time_out;
    hi_mem_handle_t src_phy_addr[GFX2D_MAX_LAYERS][DRV_GFX2D_MAX_SURFACE_NUM];
    hi_s32 src_acquire_fd[GFX2D_MAX_LAYERS];
    hi_s32 release_fence_fd;
#ifdef GFX2D_FENCE_SUPPORT
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    struct sync_fence *src_fence[GFX2D_MAX_LAYERS];
#else
    struct dma_fence *src_fence[GFX2D_MAX_LAYERS];
#endif
#endif
} GFX2D_LIST_NODE_S;

/* ****************************** API declaration ********************************************* */
hi_s32 GFX2D_LIST_Init(hi_void);

hi_void GFX2D_LIST_Deinit(hi_void);

hi_s32 GFX2D_LIST_Open(hi_void);

hi_void GFX2D_LIST_Close(hi_void);

hi_s32 GFX2D_LIST_SubNode(GFX2D_LIST_NODE_S *pstNode, drv_gfx2d_compose_ext_info *ext_info);

hi_s32 GFX2D_LIST_WaitAllDone(const drv_gfx2d_dev_id dev_id, const hi_u32 time_out);

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_LIST_ReadProc(hi_void *p, hi_void *v);
#endif

hi_s32 GFX2D_LIST_Resume(hi_void);

hi_s32 gfx2d_init_task_mem(hi_void);

hi_void gfx2d_deinit_task_mem(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _GFX2D_LIST_H_ */
