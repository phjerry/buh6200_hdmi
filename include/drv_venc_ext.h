/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-07-18
 */
#ifndef __DRV_VENC_EXT_H__
#define __DRV_VENC_EXT_H__

#include "hi_drv_video.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_mod_id source_mode;
    hi_handle source_handle;

    hi_u32    reserved;
} drv_venc_source_handle;

typedef hi_s32  (*fn_drv_venc_queue_frame)(hi_handle handle, hi_drv_video_frame* frame);

typedef struct {
    fn_drv_venc_queue_frame   fn_venc_queue_frame;
} drv_venc_export_func;

hi_s32 drv_venc_init_module(hi_void);
hi_void drv_venc_exit_module(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
