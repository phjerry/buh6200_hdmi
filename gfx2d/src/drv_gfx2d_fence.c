/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: gfx2d fence
 * Author: sdk
 * Create: 2018-12-10
 */

/* ********************************add include here*********************************************** */
#ifdef GFX2D_FENCE_SUPPORT
#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/time.h>
#include <linux/fb.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <asm/types.h>
#include <asm/stat.h>
#include <asm/fcntl.h>
#include "hi_osal.h"

#include "drv_gfx2d_fence.h"
#include "drv_gfx2d_debug.h"

#include <linux/sync_file.h>

#include "hi_gfx_comm_k.h"
#include "linux/hisilicon/securec.h"

/* ********************* Global Variable declaration ********************************************* */
#define DRV_GFX2D_FENCE_NAME "gfx2d_fence"
#define DRV_GFX2D_FENCE_ROTATE_NUM 1000
#define TIMELINE_NAME_NUM 32
typedef struct {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    hi_char timeline_name[TIMELINE_NAME_NUM];
    hi_s32 create_fence_index;
    hi_s32 destroy_fence_index;
    hi_u32 fence_seqno;
    hi_u64 fence_context;
    spinlock_t fence_lock;
    struct dma_fence *fence_array[DRV_GFX2D_FENCE_ROTATE_NUM];
#endif
    hi_u8 reg_update_cnt;
    hi_u32 fence_value;
    hi_u32 time_line;
    hi_s32 release_fence_fd;
    hi_u32 frame_end_flag;
    hi_u32 pre_time_ms;
    hi_u32 max_time_ms;
    hi_u32 run_max_time_ms;
    hi_ulong end_soft_callback_cnt;
    hi_ulong actual_int_line_num_end_callback;
    hi_ulong hard_int_cnt_end_callback;
    osal_spinlock lock;
    osal_wait frame_end_event;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    struct sw_sync_timeline *time_line;
#endif
} drv_gfx2d_fence;

static drv_gfx2d_fence *fence_to_sync_info(struct dma_fence *fence)
{
    return container_of(fence->lock, drv_gfx2d_fence, fence_lock);
}

static const char *fence_get_driver_name(struct dma_fence *fence)
{
    return DRV_GFX2D_FENCE_NAME;
}

static const char *fence_get_timeline_name(struct dma_fence *fence)
{
    drv_gfx2d_fence *sync_info = fence_to_sync_info(fence);

    return sync_info->timeline_name;
}

static bool fence_enable_signaling(struct dma_fence *fence)
{
    return true;
}

hi_s32 gfx2d_fence_get_fd(struct dma_fence *fence)
{
    hi_s32 fence_fd;
    struct sync_file *sync_file_fence = NULL;
    fence_fd = get_unused_fd_flags(0);
    sync_file_fence = sync_file_create(fence);
    if (!sync_file_fence) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "gfx2d_fence_get_fd fail");
        return -ENOMEM;
    } else {
        fd_install(fence_fd, sync_file_fence->file);
    }
    return fence_fd;
}

static const struct dma_fence_ops g_fence_ops = {
    .get_driver_name = fence_get_driver_name,
    .get_timeline_name = fence_get_timeline_name,
    .enable_signaling = fence_enable_signaling,
    .wait = dma_fence_default_wait,
};

typedef struct {
    hi_void (*fence_put)(struct dma_fence *fence);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    hi_void (*fence_init)(struct dma_fence *fence, const struct dma_fence_ops *ops, spinlock_t *lock, u64 context,
                          unsigned seqno);
#else
    hi_void (*fence_init)(struct dma_fence *fence, const struct fence_ops *ops, spinlock_t *lock, u64 context,
                          unsigned seqno);
#endif
    hi_u64 (*fence_context_alloc)(unsigned num);
    hi_slong (*fence_wait_timeout)(struct dma_fence *fence, bool intr, signed long timeout);
    hi_s32 (*fence_signal)(struct dma_fence *fence);
} gfx2d_fence_func;

gfx2d_fence_func g_gfx2d_fence_func;

drv_gfx2d_fence g_fence_sync_info;

static hi_u32 g_fence_value = 0;
static hi_u32 g_timeline_value = 0;

/* ****************************** API realization ************************************************ */
hi_s32 GFX2D_FENCE_Open(hi_void)
{
    hi_s32 ret;
    g_fence_sync_info.time_line = 0;
    g_fence_sync_info.fence_value = 1;
    g_fence_sync_info.reg_update_cnt = 0;
    g_fence_sync_info.release_fence_fd = -1;
    g_fence_sync_info.frame_end_flag = 0;
    g_fence_sync_info.create_fence_index = 0;
    g_fence_sync_info.destroy_fence_index = 0;
    g_fence_sync_info.fence_seqno = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    ret = snprintf_s(g_fence_sync_info.timeline_name, TIMELINE_NAME_NUM, TIMELINE_NAME_NUM - 1, DRV_GFX2D_FENCE_NAME);
    if (ret == -1) {
        return HI_FAILURE;
    }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    g_gfx2d_fence_func.fence_put = dma_fence_put;
    g_gfx2d_fence_func.fence_signal = dma_fence_signal;
    g_gfx2d_fence_func.fence_wait_timeout = dma_fence_wait_timeout;
    g_gfx2d_fence_func.fence_context_alloc = dma_fence_context_alloc;
    g_gfx2d_fence_func.fence_init = dma_fence_init;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    g_gfx2d_fence_func.fence_put = fence_put;
    g_gfx2d_fence_func.fence_signal = fence_signal;
    g_gfx2d_fence_func.fence_wait_timeout = fence_wait_timeout;
    g_gfx2d_fence_func.fence_context_alloc = fence_context_alloc;
    g_gfx2d_fence_func.fence_init = fence_init;
#endif
    g_fence_sync_info.fence_context = g_gfx2d_fence_func.fence_context_alloc(1);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static struct dma_fence *fence_create(spinlock_t *fence_lock, hi_u64 fence_context, hi_u32 *pfence_seqno)
{
    struct dma_fence *fence;

    fence = kzalloc(sizeof(*fence), GFP_KERNEL);
    if (!fence) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "fence_create fail");
        return NULL;
    }

    g_gfx2d_fence_func.fence_init(fence, &g_fence_ops, fence_lock, fence_context, ++(*pfence_seqno));

    return fence;
}

hi_void GFX2D_FENCE_Close(hi_void)
{
    return;
}

hi_s32 gfx2d_init_fence_lock(hi_void)
{
    if (osal_spin_lock_init(&g_fence_sync_info.lock) != 0) {
        g_fence_sync_info.lock.lock = HI_NULL;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void gfx2d_deinit_fence_lock(hi_void)
{
    if (g_fence_sync_info.lock.lock != HI_NULL) {
        osal_spin_lock_destory(&g_fence_sync_info.lock);
    }
}

hi_s32 GFX2D_FENCE_Create(const char *name)
{
    hi_s32 fd;
    hi_ulong lock_flag;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    osal_spin_lock_irqsave(&g_fence_sync_info.lock, &lock_flag);
    g_fence_sync_info.create_fence_index++;
    g_fence_sync_info.create_fence_index %= DRV_GFX2D_FENCE_ROTATE_NUM;

    g_fence_sync_info.fence_array[g_fence_sync_info.create_fence_index] =
        fence_create(&g_fence_sync_info.fence_lock, g_fence_sync_info.fence_context, &g_fence_sync_info.fence_seqno);
    if (g_fence_sync_info.fence_array == NULL) {
        osal_printk("dma fence is NULL!\n");
    }
    fd = gfx2d_fence_get_fd(g_fence_sync_info.fence_array[g_fence_sync_info.create_fence_index]);

    osal_spin_unlock_irqrestore(&g_fence_sync_info.lock, &lock_flag);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return fd;
}

hi_void GFX2D_FENCE_Destroy(hi_s32 fd)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (fd >= 0) {
        put_unused_fd(fd);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#define GFX2D_FENCE_TIMEOUT 3000

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
hi_void GFX2D_FENCE_Wait(struct sync_fence *fence, hi_s32 fence_fd)
#else
hi_void GFX2D_FENCE_Wait(struct dma_fence *fence, hi_s32 fence_fd)
#endif
{
    hi_s32 ret;

    if (fence == NULL) {
        return;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    ret = sync_fence_wait(fence, GFX2D_FENCE_TIMEOUT);
#else
    ret = g_gfx2d_fence_func.fence_wait_timeout(fence, true, GFX2D_FENCE_TIMEOUT);
#endif
    if (ret < 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, wait_fence, ret);
    }

    return;
}

hi_s32 GFX2D_FENCE_WakeUp(hi_void)
{
    hi_ulong lock_flag;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    osal_spin_lock_irqsave(&g_fence_sync_info.lock, &lock_flag);
    g_fence_sync_info.destroy_fence_index++;
    g_fence_sync_info.destroy_fence_index %= DRV_GFX2D_FENCE_ROTATE_NUM;

    if (g_fence_sync_info.fence_array[g_fence_sync_info.destroy_fence_index] != NULL) {
        g_gfx2d_fence_func.fence_signal(g_fence_sync_info.fence_array[g_fence_sync_info.destroy_fence_index]);
        g_gfx2d_fence_func.fence_put(g_fence_sync_info.fence_array[g_fence_sync_info.destroy_fence_index]);
        g_fence_sync_info.fence_array[g_fence_sync_info.destroy_fence_index] = NULL;
    }
    g_timeline_value++;
    osal_spin_unlock_irqrestore(&g_fence_sync_info.lock, &lock_flag);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_timeline_value);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void GFX2D_FENCE_ReadProc(hi_void *p, hi_void *v)
{
    if (p != NULL) {
        osal_proc_print(p, "---------------------GFX2D Fence Info--------------------\n");
        osal_proc_print(p, "FenceValue\t:%u\n", g_fence_value);
        osal_proc_print(p, "TimeLineValue\t:%u\n", g_timeline_value);
    }

    return;
}

#endif
