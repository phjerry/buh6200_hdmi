/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: handle manage
 * Author: sdk
 * Create: 2019-03-18
 */

#ifndef HI_BUILD_IN_BOOT
#include "hi_osal.h"
#include "hi_gfx_comm_k.h"
#else
#include "hi_gfx_list.h"
#include "wmalloc.h"
#include "hi_gfx_mem.h"
#endif
#include "tde_define.h"
#include "tde_handle.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_gfx_comm.h"
#include "hi_gfx_debug.h"
#else
#include "tde_debug.h"
#endif

/* ********************* Global Variable declaration *************************************** */
tde_handle_mgr *g_pstTdeHandleList = HI_NULL;

STATIC hi_s32 g_handle = 1;

#ifndef HI_BUILD_IN_BOOT
STATIC osal_spinlock g_handlelock;
#endif

/* ****************************** API Definition ********************************************** */
hi_bool initial_handle(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (g_pstTdeHandleList == HI_NULL) {
#ifndef HI_BUILD_IN_BOOT
        g_pstTdeHandleList = (tde_handle_mgr *)HI_GFX_KMALLOC(HIGFX_TDE_ID, sizeof(tde_handle_mgr), GFP_KERNEL);
#else
        g_pstTdeHandleList = (tde_handle_mgr *)hi_gfx_sys_mem_malloc(sizeof(tde_handle_mgr), "hi_s32");
#endif
        if (g_pstTdeHandleList == HI_NULL) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_gfx_sys_mem_malloc, FAILURE_TAG);
            return HI_FALSE;
        }

        /* Initialize list head */
        OSAL_INIT_LIST_HEAD(&g_pstTdeHandleList->list_head);

#ifndef HI_BUILD_IN_BOOT
        osal_spin_lock_init(&g_pstTdeHandleList->lock);
        osal_spin_lock_init(&g_handlelock);
#endif
    }

    g_handle = 1;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_TRUE;
}

hi_void get_handle(tde_handle_mgr *res, hi_s32 *handle)
{
#ifndef HI_BUILD_IN_BOOT
    hi_size_t lockflags = 0;
    hi_size_t handlockflags = 0;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(res);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(handle);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(g_pstTdeHandleList);

    tde_lock(&g_handlelock, handlockflags);

    if (g_handle == TDE_MAX_HANDLE_VALUE) {
        g_handle = 1;
    }

    res->handle = g_handle;
    *handle = g_handle++;
    tde_unlock(&g_handlelock, handlockflags);

    tde_lock(&g_pstTdeHandleList->lock, lockflags);
    osal_list_add_tail(&res->list_head, &g_pstTdeHandleList->list_head);
    tde_unlock(&g_pstTdeHandleList->lock, lockflags);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

hi_bool query_handle(hi_s32 handle, tde_handle_mgr **res)
{
#ifndef HI_BUILD_IN_BOOT
    hi_size_t lockflags = 0;
#endif
    tde_handle_mgr *pstHandle = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(res, HI_FALSE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_pstTdeHandleList, HI_FALSE);

    tde_lock(&g_pstTdeHandleList->lock, lockflags);

    pstHandle = osal_list_entry(g_pstTdeHandleList->list_head.prev, tde_handle_mgr, list_head);
    if (pstHandle == HI_NULL) {
        tde_unlock(&g_pstTdeHandleList->lock, lockflags);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_list_entry, FAILURE_TAG);
        return HI_FALSE;
    }

    while (pstHandle != g_pstTdeHandleList) {
        if (pstHandle->handle == handle) {
            *res = pstHandle;
            tde_unlock(&g_pstTdeHandleList->lock, lockflags);
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TRUE;
        }

        pstHandle = osal_list_entry(pstHandle->list_head.prev, tde_handle_mgr, list_head);
        if (pstHandle == HI_NULL) {
            tde_unlock(&g_pstTdeHandleList->lock, lockflags);
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_list_entry, FAILURE_TAG);
            return HI_FALSE;
        }
    }

    tde_unlock(&g_pstTdeHandleList->lock, lockflags);

    return HI_FALSE;
}

hi_bool release_handle(hi_s32 handle)
{
#ifndef HI_BUILD_IN_BOOT
    hi_size_t lockflags;
#endif
    tde_handle_mgr *pstHandle = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_pstTdeHandleList, HI_FALSE);

    tde_lock(&g_pstTdeHandleList->lock, lockflags);
    pstHandle = osal_list_entry(g_pstTdeHandleList->list_head.next, tde_handle_mgr, list_head);
    if (pstHandle == HI_NULL) {
        tde_unlock(&g_pstTdeHandleList->lock, lockflags);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_list_entry, FAILURE_TAG);
        return HI_FALSE;
    }

    while (pstHandle != g_pstTdeHandleList) {
        if (pstHandle->handle == handle) {
            osal_list_del_init(&pstHandle->list_head);
            tde_unlock(&g_pstTdeHandleList->lock, lockflags);
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TRUE;
        }
        pstHandle = osal_list_entry(pstHandle->list_head.next, tde_handle_mgr, list_head);
        if (pstHandle == HI_NULL) {
            tde_unlock(&g_pstTdeHandleList->lock, lockflags);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_list_entry, FAILURE_TAG);
            return HI_FALSE;
        }
    }

    tde_unlock(&g_pstTdeHandleList->lock, lockflags);

    GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, FAILURE_TAG, FAILURE_TAG);
    return HI_FALSE;
}

hi_void destroy_handle(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (g_pstTdeHandleList != HI_NULL) {
        osal_spin_lock_destory(&g_pstTdeHandleList->lock);
#ifndef HI_BUILD_IN_BOOT
        HI_GFX_KFREE(HIGFX_TDE_ID, g_pstTdeHandleList);
#else
        hi_gfx_sys_mem_free((hi_char *)g_pstTdeHandleList);
#endif
    }

    g_pstTdeHandleList = HI_NULL;
    osal_spin_lock_destory(&g_handlelock);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}
