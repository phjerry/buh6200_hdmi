/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: gfx2d ctl
 * Author: sdk
 * Create: 2018-01-01
 */

#include "hi_osal.h"
#include "drv_gfx2d_struct.h"
#include "drv_gfx2d_errcode.h"

#include "drv_gfx2d_hal.h"
#include "drv_gfx2d_list.h"
#include "drv_gfx2d_mem.h"
#include "drv_gfx2d_ctl.h"
#include "drv_gfx2d_fence.h"
#include "drv_gfx2d_hal_hwc_adp.h"
#include "drv_gfx2d_debug.h"

#include "hi_gfx_comm_k.h"

#ifdef GFX2D_FENCE_SUPPORT
#include <linux/sync_file.h>
#endif

/* ********************* Global Variable declaration ********************************************* */
static hi_u32 g_dev_ref = 0;
static osal_mutex g_dev_ref_mutex;

hi_s32 gfx2d_ctl_init_mutex(hi_void)
{
    hi_s32 ret;

    ret = osal_mutex_init(&g_dev_ref_mutex);
    if (ret != 0) {
        g_dev_ref_mutex.mutex = HI_NULL;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void gfx2d_ctl_deinit_mutex(hi_void)
{
    if (g_dev_ref_mutex.mutex != HI_NULL) {
        osal_mutex_destory(&g_dev_ref_mutex);
    }
}

hi_s32 GFX2D_CTL_Init(hi_void)
{
    hi_s32 s32Ret;
    hi_u32 u32MemSize;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    u32MemSize = GFX2D_CONFIG_GetMemSize();
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32MemSize);

    s32Ret = GFX2D_MEM_Init(u32MemSize);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32MemSize);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_MEM_Init, FAILURE_TAG);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "GFX2D_MEM_Init failure");
        return HI_FAILURE;
    }

    s32Ret = GFX2D_HAL_Init();
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_HAL_Init, FAILURE_TAG);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "GFX2D_HAL_Init failure");
        goto ERR;
    }

    s32Ret = GFX2D_LIST_Init();
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_LIST_Init, FAILURE_TAG);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "GFX2D_LIST_Init failure");
        goto ERR1;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;

ERR1:
    GFX2D_HAL_Deinit();
ERR:
    GFX2D_MEM_Deinit();

    return s32Ret;
}

hi_void GFX2D_CTL_Deinit(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GFX2D_LIST_Deinit();
    GFX2D_HAL_Deinit();
    GFX2D_MEM_Deinit();

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 GFX2D_CTL_Open(hi_void)
{
    hi_s32 s32Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    osal_mutex_lock(&g_dev_ref_mutex);

    if (g_dev_ref > 0) {
        g_dev_ref++;
        osal_mutex_unlock(&g_dev_ref_mutex);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_dev_ref);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    s32Ret = GFX2D_HAL_Open();
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_HAL_Open, FAILURE_TAG);
        goto ERR;
    }

    s32Ret = GFX2D_LIST_Open();
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_LIST_Open, FAILURE_TAG);
        goto ERR1;
    }

#ifdef GFX2D_FENCE_SUPPORT
    s32Ret = GFX2D_FENCE_Open();
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_FENCE_Open, FAILURE_TAG);
        goto ERR2;
    }
#endif

    g_dev_ref++;

    osal_mutex_unlock(&g_dev_ref_mutex);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_dev_ref);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;

#ifdef GFX2D_FENCE_SUPPORT
ERR2:
    GFX2D_LIST_Close();
#endif
ERR1:
    GFX2D_HAL_Close();
ERR:
    osal_mutex_unlock(&g_dev_ref_mutex);
    return s32Ret;
}

hi_s32 GFX2D_CTL_Close(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    osal_mutex_lock(&g_dev_ref_mutex);

    if (0 == g_dev_ref) {
        osal_mutex_unlock(&g_dev_ref_mutex);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    g_dev_ref--;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_dev_ref);

    /* still used by some module! */
    if (g_dev_ref > 0) {
        osal_mutex_unlock(&g_dev_ref_mutex);
        (hi_void) gfx2d_wait_done(DRV_GFX2D_DEV_ID_0, 0);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    (hi_void) gfx2d_wait_done(DRV_GFX2D_DEV_ID_0, 0);

#ifdef GFX2D_FENCE_SUPPORT
    GFX2D_FENCE_Close();
#endif

    GFX2D_LIST_Close();

    GFX2D_HAL_Close();

    osal_mutex_unlock(&g_dev_ref_mutex);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void save_address_to_node(const drv_gfx2d_compose_list *compose_list,
                                    const drv_gfx2d_compose_surface *dst_surface, hi_bool work_sync,
                                    GFX2D_LIST_NODE_S *list_node)
{
    hi_u32 i, j;
    if (work_sync == HI_TRUE) {
        return;
    }
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(compose_list);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(dst_surface);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(list_node);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_NOVALUE(compose_list->compose_cnt, GFX2D_MAX_LAYERS);

    for (i = 0; i < DRV_GFX2D_MAX_SURFACE_NUM; i++) {
        for (j = 0; j < compose_list->compose_cnt; j++) {
            list_node->src_phy_addr[j][i] = compose_list->compose[j].compose_surface.mem_handle[i].mem_handle;
        }
    }

    return;
}

#ifdef GFX2D_FENCE_SUPPORT
static hi_s32 save_fence_to_node(const drv_gfx2d_compose_list *compose_list,
                                 const drv_gfx2d_compose_surface *dst_surface, hi_bool work_sync,
                                 GFX2D_LIST_NODE_S *list_node)
{
    hi_u32 i;
    hi_s32 release_fence_fd = -1;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(compose_list, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(list_node, HI_FAILURE);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(compose_list->compose_cnt, GFX2D_MAX_LAYERS, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, work_sync);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, dst_surface->acquire_fence_fd);

    for (i = 0; i < compose_list->compose_cnt; i++) {
        if ((compose_list->compose[i].compose_surface.acquire_fence_fd >= 0) &&
            (compose_list->compose[i].compose_surface.surface_type != DRV_GFX2D_SURFACE_TYPE_COLOR) &&
            (work_sync == HI_FALSE)) {
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose_list->compose[i].compose_surface.surface_type);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose_list->compose[i].compose_surface.acquire_fence_fd);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose_list->compose_cnt);
            list_node->src_acquire_fd[i] = compose_list->compose[i].compose_surface.acquire_fence_fd;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
            list_node->src_fence[i] = sync_fence_fdget(compose_list->compose[i].compose_surface.acquire_fence_fd);
#else
            list_node->src_fence[i] = sync_file_get_fence(compose_list->compose[i].compose_surface.acquire_fence_fd);
#endif
        } else {
            list_node->src_acquire_fd[i] = -1;
            list_node->src_fence[i] = NULL;
        }
    }

    if (work_sync == HI_FALSE) {
        release_fence_fd = GFX2D_FENCE_Create("hi_gfx2d");
        if (release_fence_fd < 0) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_FENCE_Create, FAILURE_TAG);
            return DRV_GFX2D_ERR_SYS;
        }
    }

    list_node->release_fence_fd = release_fence_fd;
    return HI_SUCCESS;
}
#endif

hi_s32 GFX2D_CTL_Compose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                         drv_gfx2d_compose_surface *pstDstSurface, hi_bool work_sync, const hi_u32 time_out)
{
    hi_s32 ret;
#ifdef GFX2D_FENCE_SUPPORT
    hi_u32 i = 0;
#endif
    hi_void *pNode = NULL;
    GFX2D_HAL_DEV_TYPE_E enNodeType = GFX2D_HAL_DEV_TYPE_HWC;
    GFX2D_LIST_NODE_S stListNode = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(compose_list, DRV_GFX2D_ERR_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(compose_list->compose, DRV_GFX2D_ERR_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDstSurface, DRV_GFX2D_ERR_NULL_PTR);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(compose_list->compose_cnt, GFX2D_MAX_LAYERS,
                                                 DRV_GFX2D_ERR_INVALID_COMPOSECNT);

    save_address_to_node(compose_list, pstDstSurface, work_sync, &stListNode);

#ifdef GFX2D_FENCE_SUPPORT
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    if (pstDstSurface->acquire_fence_fd >= 0 && (work_sync == HI_FALSE)) {
        GFX2D_FENCE_Wait(sync_fence_fdget(pstDstSurface->acquire_fence_fd), pstDstSurface->acquire_fence_fd);
    }
#else
    if (pstDstSurface->acquire_fence_fd >= 0 && (work_sync == HI_FALSE)) {
        GFX2D_FENCE_Wait(sync_file_get_fence(pstDstSurface->acquire_fence_fd), pstDstSurface->acquire_fence_fd);
    }
#endif
#endif

#ifdef GFX2D_FENCE_SUPPORT
    ret = save_fence_to_node(compose_list, pstDstSurface, work_sync, &stListNode);
    if (ret != HI_SUCCESS) {
        return ret;
    }
#endif

    ret = GFX2D_HAL_Compose(dev_id, compose_list, pstDstSurface, &pNode, &enNodeType);
    if (ret != HI_SUCCESS) {
#ifdef GFX2D_FENCE_SUPPORT
        if (stListNode.release_fence_fd >= 0) {
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
            GFX2D_FENCE_Destroy(stListNode.release_fence_fd);
        }
#endif
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stListNode.release_fence_fd);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_HAL_Compose, FAILURE_TAG);
        return ret;
    }

    stListNode.dev_id = dev_id;
    stListNode.pNode = pNode;
    stListNode.enNodeType = enNodeType;
    stListNode.work_sync = work_sync;
    stListNode.time_out = time_out;
    ret = GFX2D_LIST_SubNode(&stListNode, &(pstDstSurface->ext_info));
    if (ret != HI_SUCCESS) {
#ifdef GFX2D_FENCE_SUPPORT
        if (stListNode.release_fence_fd >= 0) {
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
            GFX2D_FENCE_Destroy(stListNode.release_fence_fd);
        }
#endif
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_LIST_SubNode, FAILURE_TAG);
        return ret;
    }

#ifdef GFX2D_FENCE_SUPPORT
    for (i = 0; i < compose_list->compose_cnt; i++) {
        compose_list->compose[i].compose_surface.release_fence_fd = stListNode.release_fence_fd;
    }

    pstDstSurface->release_fence_fd = stListNode.release_fence_fd;
#else
    pstDstSurface->release_fence_fd = -1;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 gfx2d_wait_done(const drv_gfx2d_dev_id dev_id, const hi_u32 time_out)
{
    return GFX2D_LIST_WaitAllDone(dev_id, time_out);
}

hi_s32 GFX2D_CTL_Resume(hi_void)
{
    return GFX2D_LIST_Resume();
}

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_s32 gfx2d_read_ctl_proc(hi_void *p, hi_void *v)
{
    GFX2D_MEM_ReadProc(p, v);

    GFX2D_LIST_ReadProc(p, v);

    GFX2D_HAL_ReadProc(p, v);

#ifdef GFX2D_FENCE_SUPPORT
    GFX2D_FENCE_ReadProc(p, v);
#endif

    return HI_SUCCESS;
}
#endif

hi_bool gfx2d_check_open(hi_void)
{
    return (g_dev_ref > 0) ? HI_TRUE : HI_FALSE;
}
