/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: logo control header
 * Author: sdk
 * Create: 2019-08-17
 */

#ifndef __DRV_HIFB_LOGO_H__
#define __DRV_HIFB_LOGO_H__

#include "hi_osal.h"
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_bool is_mute;
    hi_u32 stride;
    hi_u32 size;
    hi_char *screen_base;
    hi_ulong smem_start;
} hifb_logo_surface;

typedef struct {
    hi_bool need_wait_up;
    hi_bool has_been_inited;
    hi_bool finish_transition;
    hi_u32 disp_id;
    osal_wait wait_queue;
    osal_task *pthread_task;
    hifb_logo_surface surface;
} drv_hifb_logo_info;

hi_void drv_hifb_logo_init(hi_ulong smem_start, hi_char *screen_base, hi_u32 stride, hi_u32 size, hi_u32 layer_id);
hi_void drv_hifb_logo_deinit(hi_u32 layer_id);
hi_void drv_hifb_logo_get_base_info(hi_u32 layer_id);
hi_s32 drv_init_logo_wait_queue(hi_void);
hi_void drv_deinit_logo_wait_queue(hi_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_HIFB_LOGO_H__ */
