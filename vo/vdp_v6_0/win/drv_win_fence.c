/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: functions define
* Author: VDP
* Create: 2019-10-11
*/

#include <linux/sync_file.h>
#include <linux/dma-buf.h>

#include "osal_list.h"
#include "drv_xdp_osal.h"
#include "drv_win_fence.h"
#include "hi_errno.h"
#include "hi_module.h"
#include "hi_drv_mem.h"
#include "hi_osal.h"

#define SYNC_INSTANCE_PREFIX 0x0ABC0000
typedef struct {
    hi_u64             frame_addr;
    struct dma_fence   *fence_node;
    struct osal_list_head   list_node;
} fence_node;

typedef struct {
    drv_win_timeline  fence_instance[WINDOW_MAX_NUMBER];
    osal_spinlock lock;
    hi_bool is_fence_init;
} sync_fence_manage;

sync_fence_manage g_fence_instance_mng;

static hi_void _deinit_fence_list(common_list *frame_list)
{
    fence_node *_fence_node;
    struct osal_list_head *list_node;

    while (1) {
        common_list_get_del_node(frame_list, &list_node);
        if (list_node == HI_NULL) {
            break;
        }

        COMMON_GET_NODE_CONTENT(list_node, fence_node, _fence_node);
        osal_kfree(HI_ID_WIN, _fence_node);
    }

    return;
}

static hi_s32  _insert_fence_to_list(drv_win_timeline *timeline_info,
                                     struct dma_fence *fence_info,
                                     hi_u64 frame_addr)
{
    fence_node *_fence_node;
    struct osal_list_head *list_node = HI_NULL;
    hi_ulong lock_flags = 0;

    osal_spin_lock_irqsave(&timeline_info->fence_array.fence_lock, &lock_flags);
    common_list_get_del_node(&timeline_info->fence_array.fence_empty_list, &list_node);
    if (list_node != HI_NULL) {
        COMMON_GET_NODE_CONTENT(list_node, fence_node, _fence_node);
        _fence_node->fence_node = fence_info;
        _fence_node->frame_addr = frame_addr;
        common_list_add_node(&timeline_info->fence_array.fence_full_list, list_node);
    }

    osal_spin_unlock_irqrestore(&timeline_info->fence_array.fence_lock, &lock_flags);
    if (list_node == HI_NULL) {
        WIN_ERROR("not enough node to fill.\n");
        return HI_ERR_VO_BUFQUE_FULL;
    }

    return HI_SUCCESS;
}

static hi_s32 delete_fence_from_list(drv_win_timeline *timeline_info,
                                     struct dma_fence *fence_info)
{
    fence_node *_fence_node;
    struct osal_list_head *list_node = HI_NULL;
    hi_ulong lock_flags = 0;

    osal_spin_lock_irqsave(&timeline_info->fence_array.fence_lock, &lock_flags);

    common_list_get_del_node(&timeline_info->fence_array.fence_full_list, &list_node);

    if (list_node != HI_NULL) {
        COMMON_GET_NODE_CONTENT(list_node, fence_node, _fence_node);
        if (fence_info != _fence_node->fence_node) {
            WIN_ERROR("fence release out of order.\n");
        }

        _fence_node->fence_node = HI_NULL_PTR;
        common_list_add_node(&timeline_info->fence_array.fence_empty_list, list_node);
    }

    osal_spin_unlock_irqrestore(&timeline_info->fence_array.fence_lock, &lock_flags);

    if (list_node == HI_NULL) {
        return HI_ERR_VO_BUFQUE_EMPTY;
    }

    return HI_SUCCESS;
}


static drv_win_timeline *fence_to_sync_info(struct dma_fence *fence)
{
    return container_of(fence->lock, drv_win_timeline, fence_lock);
}

static const char *fence_get_driver_name(struct dma_fence *fence)
{
    return DRV_HI_WIN_FENCE_NAME;
}

static const char *fence_get_timeline_name(struct dma_fence *fence)
{
    drv_win_timeline *sync_info = fence_to_sync_info(fence);

    return sync_info->timeline_name;
}

static bool fence_enable_signaling(struct dma_fence *fence)
{
    return true;
}

static const struct dma_fence_ops g_fence_ops = {
    .get_driver_name   = fence_get_driver_name,
    .get_timeline_name = fence_get_timeline_name,
    .enable_signaling  = fence_enable_signaling,
    .wait              = dma_fence_default_wait,
};

static struct dma_fence *fence_create(spinlock_t *fence_lock,
                                      hi_u64 fence_context, hi_u32 *pfence_seqno)
{
    struct dma_fence *fence;

    fence = osal_kmalloc(HI_ID_WIN, sizeof(*fence), OSAL_GFP_KERNEL);
    if (!fence) {
        return NULL;
    }
    memset(fence, 0, sizeof(*fence));
    dma_fence_init(fence, &g_fence_ops, fence_lock, fence_context, ++(*pfence_seqno));
    return fence;
}

static hi_void fence_destroy(struct dma_fence *fence)
{
    if (fence == HI_NULL) {
        return;
    }

    osal_kfree(HI_ID_WIN, fence);
    return;
}

static hi_s32 fence_get_fd(struct dma_fence *fence)
{
    hi_s32 fence_fd;
    struct sync_file *sync_file_fence = NULL;

    fence_fd = get_unused_fd_flags(0);

    sync_file_fence = sync_file_create(fence);
    if (!sync_file_fence) {
        return -ENOMEM;
    } else {
        fd_install(fence_fd, sync_file_fence->file);
    }

    return fence_fd;
}


hi_s32 drv_fence_init(hi_void)
{
    hi_s32 ret;

    if (g_fence_instance_mng.is_fence_init == HI_TRUE) {
        WIN_WARN("fence has been inited\n");
        return HI_SUCCESS;
    }

    memset(&g_fence_instance_mng, 0, sizeof(g_fence_instance_mng));
    ret = osal_spin_lock_init(&(g_fence_instance_mng.lock));
    if (ret != HI_SUCCESS) {
        WIN_ERROR("fence lock initiation failed\n");
        return HI_FAILURE;
    }
    g_fence_instance_mng.is_fence_init = HI_TRUE;

    return HI_SUCCESS;
}


hi_s32 drv_fence_deinit(hi_void)
{
    hi_u32 i;

    if (g_fence_instance_mng.is_fence_init != HI_TRUE) {
        WIN_ERROR("fence has not been initiated\n");
        return HI_SUCCESS;
    }

    for (i = 0; i < WINDOW_MAX_NUMBER; i ++){
        if (g_fence_instance_mng.fence_instance[i].is_busy == HI_TRUE) {
            WIN_ERROR("some fence not destroyed.\n");
        }
    }
    osal_spin_lock_destory(&(g_fence_instance_mng.lock));
    g_fence_instance_mng.is_fence_init = HI_FALSE;
    memset(&g_fence_instance_mng, 0, sizeof(g_fence_instance_mng));
    return HI_SUCCESS;
}

hi_s32 drv_fence_create_instance(hi_void)
{
    hi_u32 i;
    hi_ulong lock_flags = 0;
    fence_node *_fence_node;
    drv_win_timeline *fence_instance;
    hi_handle fence_instance_handle;
    hi_u32 ret;

    osal_spin_lock_irqsave(&g_fence_instance_mng.lock, &lock_flags);
    for (i = 0; i < WINDOW_MAX_NUMBER; i ++){
        if (g_fence_instance_mng.fence_instance[i].is_busy == HI_FALSE) {
            break;
        }
    }

    if (i >= WINDOW_MAX_NUMBER) {
        WIN_ERROR(" fence instance reach max.\n");
        osal_spin_unlock_irqrestore(&g_fence_instance_mng.lock, &lock_flags);
        return HI_INVALID_HANDLE;
    }

    g_fence_instance_mng.fence_instance[i].is_busy = HI_TRUE;
    fence_instance = &g_fence_instance_mng.fence_instance[i];
    fence_instance_handle = SYNC_INSTANCE_PREFIX | i;

    osal_spin_unlock_irqrestore(&g_fence_instance_mng.lock, &lock_flags);

    ret = osal_spin_lock_init(&(fence_instance->fence_array.fence_lock));
    if (ret != HI_SUCCESS) {
        g_fence_instance_mng.fence_instance[i].is_busy = HI_FALSE;
        WIN_ERROR("fence instance initiation failed\n");
        return HI_INVALID_HANDLE;
    }
    common_list_init(&(fence_instance->fence_array.fence_empty_list));
    common_list_init(&(fence_instance->fence_array.fence_full_list));

    for (i = 0; i < WINDOW_MAX_NUMBER; i++) {
        _fence_node = osal_kmalloc(HI_ID_WIN, sizeof(fence_node), OSAL_GFP_KERNEL);
        if (_fence_node == HI_NULL) {
            goto __malloc_error;
        }
        memset(_fence_node, 0, sizeof(fence_node));
        common_list_add_node(&(fence_instance->fence_array.fence_empty_list),
                             &_fence_node->list_node);
    }

    return fence_instance_handle;
__malloc_error:
    WIN_ERROR("frame list create failed\n");
    _deinit_fence_list(&(fence_instance->fence_array.fence_empty_list));
    return HI_INVALID_HANDLE;
}

static drv_win_timeline *_get_fence_instance(hi_handle fence_instance_handle)
{
    hi_ulong lock_flags = 0;
    hi_u32 i;
    drv_win_timeline *fence_instance;

    osal_spin_lock_irqsave(&g_fence_instance_mng.lock, &lock_flags);
    i = fence_instance_handle & 0xffff;
    fence_instance = &g_fence_instance_mng.fence_instance[i];
    osal_spin_unlock_irqrestore(&g_fence_instance_mng.lock, &lock_flags);
    return fence_instance;
}

hi_s32 drv_fence_destroy_instance(hi_handle fence_instance_handle)
{
    hi_ulong lock_flags = 0;
    hi_u32 i;
    drv_win_timeline *fence_instance;


    osal_spin_lock_irqsave(&g_fence_instance_mng.lock, &lock_flags);

    i = fence_instance_handle & 0xffff;
    if (i > WINDOW_MAX_NUMBER) {
        WIN_ERROR("pass invalid instance handle.\n");
        osal_spin_unlock_irqrestore(&g_fence_instance_mng.lock, &lock_flags);
        return HI_ERR_VO_INVALID_PARA;
    }

    fence_instance = &g_fence_instance_mng.fence_instance[i];

    if (fence_instance->is_busy == HI_FALSE) {
        osal_spin_unlock_irqrestore(&g_fence_instance_mng.lock, &lock_flags);
        return HI_SUCCESS;
    }

    fence_instance->is_busy = HI_FALSE;
    osal_spin_unlock_irqrestore(&g_fence_instance_mng.lock, &lock_flags);

    osal_spin_lock_irqsave(&fence_instance->fence_array.fence_lock, &lock_flags);
    _deinit_fence_list(&(fence_instance->fence_array.fence_empty_list));
    _deinit_fence_list(&(fence_instance->fence_array.fence_full_list));
    osal_spin_unlock_irqrestore(&fence_instance->fence_array.fence_lock, &lock_flags);

    osal_spin_lock_destory(&(fence_instance->fence_array.fence_lock));

    return HI_SUCCESS;
}


int drv_fence_create_fence(hi_handle fence_instance_handle,
                          hi_u32 frame_addr,
                          hi_u64 *fence_descp)
{
    int fd = 0;
    hi_s32 ret;
    struct dma_fence *fence = HI_NULL;
    drv_win_timeline *fence_instance;

    fence_instance = _get_fence_instance(fence_instance_handle);
    if (fence_instance == HI_NULL) {
        WIN_ERROR("get fence_instance failed.\n");
        return HI_ERR_VO_NO_SOURCE;
    }

    fence = fence_create(&fence_instance->fence_lock, fence_instance->fence_context,
                         &fence_instance->fence_seqno);
    if (fence == HI_NULL) {
        WIN_ERROR("create fence failed.\n");
        return HI_ERR_VO_NO_SOURCE;
    }

    fd = fence_get_fd(fence);
    if (fd <= 0) {
        WIN_ERROR("fd resources not enough.\n");
        goto _FENCE_DESTROY_;
    }

    ret = _insert_fence_to_list(fence_instance,fence, frame_addr);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("timeline list full.\n");
        goto _FENCE_DESTROY_;
    }

    *fence_descp = (hi_u64)fence;

    return fd;
 _FENCE_DESTROY_:
    if (fd > 0) {
        put_unused_fd(fd);
    }

    if (fence != HI_NULL) {
        fence_destroy(fence);
    }

    return HI_ERR_VO_NO_SOURCE;
}

hi_s32 drv_fence_signal(hi_handle fence_instance_handle, hi_void *fence_descp)
{
    struct dma_fence *fence;
    hi_s32 ret;
    drv_win_timeline *fence_instance;

    fence_instance = _get_fence_instance(fence_instance_handle);
    if (fence_instance == HI_NULL) {
        WIN_ERROR("get fence_instance failed.\n");
        return HI_ERR_VO_NO_SOURCE;
    }

    fence = (struct dma_fence *)fence_descp;

    ret = dma_fence_signal(fence);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("Signal error.\n");
        return HI_ERR_VO_INVALID_PARA;
    }

    /* attention:  put fence here. when signaled,
     * fence is useless and should  refcnt --, fence will be kfree when fd close in userspace. */
    dma_fence_put(fence);

    (hi_void)delete_fence_from_list(fence_instance, fence);
    return HI_SUCCESS;
}

hi_s32 drv_fence_flush(hi_void)
{
    return HI_SUCCESS;
}
