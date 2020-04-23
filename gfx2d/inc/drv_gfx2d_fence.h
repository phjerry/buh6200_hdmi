/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d fence header
 * Author: sdk
 * Create: 2019-05-13
 */

#ifndef _GFX2D_FENCE_H_
#define _GFX2D_FENCE_H_

/* ********************************add include here******************************************** */

#include "hi_type.h"
#ifdef GFX2D_FENCE_SUPPORT
#include <linux/sync_file.h>
#endif
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* ****************************** API declaration ********************************************* */
#ifdef GFX2D_FENCE_SUPPORT
hi_s32 GFX2D_FENCE_Open(hi_void);
hi_void GFX2D_FENCE_Close(hi_void);
hi_s32 GFX2D_FENCE_Create(const hi_char *name);
hi_void GFX2D_FENCE_Destroy(hi_s32 fd);
hi_s32 GFX2D_FENCE_WakeUp(hi_void);
hi_void GFX2D_FENCE_ReadProc(hi_void *p, hi_void *v);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
hi_void GFX2D_FENCE_Wait(struct sync_fence *fence, hi_s32 fence_fd);
#else
hi_void GFX2D_FENCE_Wait(struct dma_fence *fence, hi_s32 fence_fd);
#endif
hi_s32 gfx2d_init_fence_lock(hi_void);
hi_void gfx2d_deinit_fence_lock(hi_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* *_GFX2D_FENCE_H_* */
