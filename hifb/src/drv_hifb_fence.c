/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb fence
 * Author: sdk
 * Create: 2016-01-01
 */

#ifdef CONFIG_HIFB_FENCE_SUPPORT

/* ********************************add include here*********************************************** */
#include <linux/sync_file.h>
#include "hi_osal.h"

#include "drv_hifb_mem.h"
#include "drv_hifb_fence.h"
#include "drv_hifb_config.h"
#include "drv_hifb_debug.h"

/* ********************* Global Variable declaration ********************************************* */
drv_hifb_fence g_sync_info;
extern HIFB_DRV_OPS_S g_stDrvAdpCallBackFunction;

/* ****************************** API declaration ************************************************ */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
typedef struct {
    hi_void (*fence_put)(struct hifb_fence *fence);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    hi_void (*fence_init)(struct hifb_fence *fence, const struct dma_fence_ops *ops, spinlock_t *lock, u64 context,
                          unsigned seqno);
#else
    hi_void (*fence_init)(struct hifb_fence *fence, const struct fence_ops *ops, spinlock_t *lock, u64 context,
                          unsigned seqno);
#endif
    hi_u64 (*fence_context_alloc)(unsigned num);
    hi_slong (*fence_wait_timeout)(struct hifb_fence *fence, bool intr, signed long timeout);
    hi_s32 (*fence_signal)(struct hifb_fence *fence);
} hifb_fence_func;

hifb_fence_func g_hifb_fence_func;

static drv_hifb_fence *fence_to_sync_info(struct hifb_fence *fence)
{
    return container_of(fence->lock, drv_hifb_fence, fence_lock);
}

static const char *fence_get_driver_name(struct hifb_fence *fence)
{
    return DRV_HIFB_FENCE_NAME;
}

static const char *fence_get_timeline_name(struct hifb_fence *fence)
{
    drv_hifb_fence *sync_info = fence_to_sync_info(fence);

    return sync_info->timeline_name;
}

static bool fence_enable_signaling(struct hifb_fence *fence)
{
    return true;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
static const struct dma_fence_ops g_fence_ops = {
    .get_driver_name = fence_get_driver_name,
    .get_timeline_name = fence_get_timeline_name,
    .enable_signaling = fence_enable_signaling,
    .wait = dma_fence_default_wait,
};
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
static const struct fence_ops g_fence_ops = {
    .get_driver_name = fence_get_driver_name,
    .get_timeline_name = fence_get_timeline_name,
    .enable_signaling = fence_enable_signaling,
    .wait = fence_default_wait,
};
#endif

struct hifb_fence *fence_get_fence(hi_s32 fd)
{
    struct hifb_fence *fence;
    fence = sync_file_get_fence(fd);
    if (!fence) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, fd);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "hi_xdp_sync_file_get_fence fail");
        return NULL;
    }
    return fence;
}

static struct hifb_fence *fence_create(spinlock_t *fence_lock, hi_u64 fence_context, hi_u32 *pfence_seqno)
{
    struct hifb_fence *fence;

    fence = kzalloc(sizeof(*fence), GFP_KERNEL);
    if (!fence) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "fence_create fail");
        return NULL;
    }

    g_hifb_fence_func.fence_init(fence, &g_fence_ops, fence_lock, fence_context, ++(*pfence_seqno));

    return fence;
}

hi_s32 fence_get_fd(struct hifb_fence *fence)
{
    hi_s32 fence_fd;
    struct sync_file *sync_file_fence = NULL;
    fence_fd = get_unused_fd_flags(0);
    sync_file_fence = sync_file_create(fence);
    if (!sync_file_fence) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "fence_get_fd fail");
        return -ENOMEM;
    } else {
        fd_install(fence_fd, sync_file_fence->file);
    }
    return fence_fd;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
hi_s32 hifb_fence_create(hi_u32 layer_id)
{
    hi_s32 fence_fd;
    hi_u32 fence_value;
    hi_ulong lock_flag;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;

    info = s_stLayer[layer_id].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    fence_value = ++(pstPar->fence_sync_info.fence_value);

    osal_spin_lock_irqsave(&pstPar->fence_sync_info.lock, &lock_flag);
    pstPar->fence_sync_info.create_fence_index++;
    pstPar->fence_sync_info.create_fence_index %= DRV_HIFB_FENCE_ROTATE_NUM;

    pstPar->fence_sync_info.fence_array[pstPar->fence_sync_info.create_fence_index] =
        fence_create(&pstPar->fence_sync_info.fence_lock, pstPar->fence_sync_info.fence_context,
                     &pstPar->fence_sync_info.fence_seqno);
    osal_spin_unlock_irqrestore(&pstPar->fence_sync_info.lock, &lock_flag);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstPar->fence_sync_info.create_fence_index);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, fence_value);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "fb fence create");

    fence_fd = fence_get_fd(pstPar->fence_sync_info.fence_array[pstPar->fence_sync_info.create_fence_index]);
    pstPar->fence_sync_info.release_fence_fd = fence_fd;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return fence_fd;
}

hi_s32 hifb_fence_wait(struct hifb_fence *fence, long timeout)
{
    hi_slong ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fence, HI_FAILURE);
    ret = g_hifb_fence_func.fence_wait_timeout(fence, true, timeout);
    if (ret <= 0) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the fence wait timed out");
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void hifb_fence_signal(hi_u32 layer_id)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;

    info = s_stLayer[layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstPar->fence_sync_info.destroy_fence_index);
    if (pstPar->fence_sync_info.fence_array[pstPar->fence_sync_info.destroy_fence_index] != NULL) {
        g_hifb_fence_func.fence_signal(
            pstPar->fence_sync_info.fence_array[pstPar->fence_sync_info.destroy_fence_index]);
        g_hifb_fence_func.fence_put(pstPar->fence_sync_info.fence_array[pstPar->fence_sync_info.destroy_fence_index]);
        pstPar->fence_sync_info.fence_array[pstPar->fence_sync_info.destroy_fence_index] = NULL;
        pstPar->fence_sync_info.destroy_fence_index++;
        pstPar->fence_sync_info.destroy_fence_index %= DRV_HIFB_FENCE_ROTATE_NUM;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "fb fence signal and put");
    } else {
        pstPar->fence_sync_info.destroy_fence_index++;
        pstPar->fence_sync_info.destroy_fence_index %= DRV_HIFB_FENCE_ROTATE_NUM;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

hi_void hifb_fence_init(hi_u32 layer_id)
{
    hi_s32 fence_index;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;

    info = s_stLayer[layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);

    pstPar->fence_sync_info.time_line = 0;
    pstPar->fence_sync_info.fence_value = 1;
    pstPar->fence_sync_info.reg_update_cnt = 0;
    pstPar->fence_sync_info.release_fence_fd = -1;
    pstPar->fence_sync_info.frame_end_flag = 0;
    pstPar->fence_sync_info.create_fence_index = 0;
    pstPar->fence_sync_info.destroy_fence_index = 0;
    pstPar->fence_sync_info.fence_seqno = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    sprintf(pstPar->fence_sync_info.timeline_name, DRV_HIFB_FENCE_NAME);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    g_hifb_fence_func.fence_put = dma_fence_put;
    g_hifb_fence_func.fence_signal = dma_fence_signal;
    g_hifb_fence_func.fence_wait_timeout = dma_fence_wait_timeout;
    g_hifb_fence_func.fence_context_alloc = dma_fence_context_alloc;
    g_hifb_fence_func.fence_init = dma_fence_init;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    g_hifb_fence_func.fence_put = fence_put;
    g_hifb_fence_func.fence_signal = fence_signal;
    g_hifb_fence_func.fence_wait_timeout = fence_wait_timeout;
    g_hifb_fence_func.fence_context_alloc = fence_context_alloc;
    g_hifb_fence_func.fence_init = fence_init;
#endif
    pstPar->fence_sync_info.fence_context = g_hifb_fence_func.fence_context_alloc(1);
    for (fence_index = 0; fence_index < DRV_HIFB_FENCE_ROTATE_NUM; ++fence_index) {
        pstPar->fence_sync_info.fence_array[fence_index] = NULL;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

hi_void hifb_fence_deinit(hi_void)
{
    return;
}

struct hifb_fence *hifb_fence_get_fence(hi_s32 fd)
{
    return fence_get_fence(fd);
}
#endif
hi_s32 drv_hifb_fence_init(HIFB_PAR_S *par)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    hifb_fence_init(par->stBaseInfo.u32LayerID);
    if (osal_spin_lock_init(&par->fence_sync_info.lock) != 0) {
        return HI_FAILURE;
    }
    if (osal_wait_init(&par->fence_sync_info.frame_end_event) != 0) {
        return HI_FAILURE;
    }

    par->FenceRefreshCount = 0;
    par->bEndFenceRefresh = HI_FALSE;

    par->pFenceRefreshWorkqueue = create_singlethread_workqueue(HIFB_HWC_REFRESH_WORK_QUEUE);
    if (par->pFenceRefreshWorkqueue == NULL) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, create_singlethread_workqueue, FAILURE_TAG);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void drv_hifb_fence_deinit(HIFB_PAR_S *par)
{
    hi_ulong lock_flag;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(par);

    if (HIFB_LAYER_HD_0 != par->stBaseInfo.u32LayerID) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
    osal_spin_lock_irqsave(&par->stBaseInfo.lock, &lock_flag);
    while (par->hwc_refresh_cnt > 0) {
        par->FenceRefreshCount++;
        par->hwc_refresh_cnt--;
    }

    while (par->FenceRefreshCount > 0) {
        drv_hifb_fence_inc_refresh_time(HI_FALSE, par->stBaseInfo.u32LayerID);
        par->FenceRefreshCount--;
    }
    osal_spin_unlock_irqrestore(&par->stBaseInfo.lock, &lock_flag);

    hifb_fence_deinit();
    if (par->pFenceRefreshWorkqueue != NULL) {
        destroy_workqueue(par->pFenceRefreshWorkqueue);
    }
    par->pFenceRefreshWorkqueue = NULL;

    par->bEndFenceRefresh = HI_FALSE;

    if (par->fence_sync_info.lock.lock != HI_NULL) {
        osal_spin_lock_destory(&par->fence_sync_info.lock);
    }
    osal_wait_destroy(&par->fence_sync_info.frame_end_event);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void drv_hifb_fence_signal(hi_u32 layer_id)
{
    hifb_fence_signal(layer_id);
}

hi_void drv_hifb_fence_get_sync_info(drv_hifb_fence *sync_info)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(sync_info);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    memcpy(sync_info, &g_sync_info, sizeof(drv_hifb_fence));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void drv_hifb_fence_count_times_start(hi_u32 layer_id)
{
    hi_bool has_been_closed = HI_FALSE;
    osal_timeval tv_cur;
    hi_u32 start_time;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;

    info = s_stLayer[layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);
    osal_get_timeofday(&tv_cur);
    start_time = tv_cur.tv_sec * 1000 + tv_cur.tv_usec / 1000; /* 1 second is 1000ms ,1ms is 1000us */
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetCloseState(layer_id, NULL, &has_been_closed);
    if (has_been_closed == HI_TRUE) {
        pstPar->fence_sync_info.pre_time_ms = 0;
        pstPar->fence_sync_info.max_time_ms = 0;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    if (pstPar->fence_sync_info.pre_time_ms == 0) {
        pstPar->fence_sync_info.pre_time_ms = start_time;
    }
    if ((start_time - pstPar->fence_sync_info.pre_time_ms) > pstPar->fence_sync_info.max_time_ms) {
        pstPar->fence_sync_info.max_time_ms = start_time - pstPar->fence_sync_info.pre_time_ms;
    }
    pstPar->fence_sync_info.pre_time_ms = start_time;
}

static hi_void drv_hifb_fence_count_times_end(hi_void)
{
    hi_u32 end_time;
    osal_timeval tv_cur;
    osal_get_timeofday(&tv_cur);
    end_time = tv_cur.tv_sec * 1000 + tv_cur.tv_usec / 1000; /* 1 second is 1000ms ,1ms is 1000us */
    if ((end_time - g_sync_info.pre_time_ms) > g_sync_info.run_max_time_ms) {
        g_sync_info.run_max_time_ms = end_time - g_sync_info.pre_time_ms;
    }
}

static hi_void drv_hifb_fence_count_int_info(hi_u32 layer_id)
{
    hi_ulong actual_int_line_num = 0;
    hi_ulong hard_int_cnt_end_callback = 0;

    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    info = s_stLayer[layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDhd0Info);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDhd0Info(layer_id, NULL, NULL, &actual_int_line_num,
                                                        &hard_int_cnt_end_callback);

    pstPar->fence_sync_info.actual_int_line_num_end_callback = actual_int_line_num;
    pstPar->fence_sync_info.hard_int_cnt_end_callback = hard_int_cnt_end_callback;
    pstPar->fence_sync_info.end_soft_callback_cnt++;
}

hi_void drv_hifb_fence_frame_end_callback(hi_void *para1, hi_void *para2)
{
    hi_u32 *layer_id = NULL;
    hi_ulong lock_flag = 0;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* ************************** check para ********************************** */
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(para1);
    layer_id = (hi_u32 *)para1;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(layer_id);

    info = s_stLayer[*layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);

    drv_hifb_fence_count_times_start(*layer_id);
    drv_hifb_fence_count_int_info(*layer_id);

    /* ************************** release fence ******************************* */
    if (pstPar->fence_sync_info.reg_update_cnt == 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    osal_spin_lock_irqsave(&pstPar->fence_sync_info.lock, &lock_flag);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    if (g_sync_info.time_line == NULL) {
        osal_wait_wakeup(&g_sync_info.frame_end_event);
        osal_spin_unlock_irqrestore(&g_sync_info.lock, &lock_flag);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
#endif

    while (pstPar->fence_sync_info.reg_update_cnt > 0) {
        pstPar->fence_sync_info.reg_update_cnt--;
        drv_hifb_fence_signal(*layer_id);
        pstPar->fence_sync_info.time_line++;
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstPar->fence_sync_info.time_line);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    pstPar->fence_sync_info.frame_end_flag = 1;
    osal_wait_wakeup(&pstPar->fence_sync_info.frame_end_event);

    osal_spin_unlock_irqrestore(&pstPar->fence_sync_info.lock, &lock_flag);

    drv_hifb_fence_count_times_end();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 drv_hifb_fence_create(hi_u32 layer_id)
{
    return hifb_fence_create(layer_id);
}

struct hifb_fence *drv_hifb_fd_get_fence(hi_s32 fd)
{
    return hifb_fence_get_fence(fd);
}

hi_s32 drv_hifb_get_end_flag(const hi_void *param)
{
    return g_sync_info.frame_end_flag;
}

hi_void drv_hifb_fence_wait_refresh_end(hi_bool should_wait)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (should_wait == HI_TRUE) {
        g_sync_info.frame_end_flag = 0;
        osal_wait_timeout_interruptible(&g_sync_info.frame_end_event, drv_hifb_get_end_flag, HI_NULL, HZ);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void drv_hifb_fence_inc_refresh_time(hi_bool enable, hi_u32 layer_id)
{
    hi_ulong lock_flag;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    info = s_stLayer[layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    osal_spin_lock_irqsave(&pstPar->fence_sync_info.lock, &lock_flag);
    pstPar->fence_sync_info.reg_update_cnt++;
    osal_spin_unlock_irqrestore(&pstPar->fence_sync_info.lock, &lock_flag);

    if (enable == HI_TRUE) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    if (pstPar->fence_sync_info.reg_update_cnt == 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    if (g_sync_info.time_line == NULL) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
#endif

    while (pstPar->fence_sync_info.reg_update_cnt > 0) {
        pstPar->fence_sync_info.reg_update_cnt--;
        drv_hifb_fence_signal(layer_id);
        pstPar->fence_sync_info.time_line++;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 drv_hifb_fence_wait(struct hifb_fence *fence, long timeout)
{
    return hifb_fence_wait(fence, timeout);
}

hi_void drv_hifb_fence_put(struct hifb_fence *fence)
{
    /* for sync_fence put */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    sync_fence_put(fence);
#endif
}

hi_void drv_hifb_fence_get_fence_value(hi_u32 *fence_value, hi_u32 layer_id)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    info = s_stLayer[layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);
    if (fence_value != NULL) {
        *fence_value = pstPar->fence_sync_info.fence_value;
    }
}

hi_void drv_hifb_fence_get_timeline_value(hi_u32 *time_line, hi_u32 layer_id)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    info = s_stLayer[layer_id].pstInfo;
    pstPar = (HIFB_PAR_S *)(info->par);
    if (time_line != NULL) {
        *time_line = pstPar->fence_sync_info.time_line;
    }
}
#endif
