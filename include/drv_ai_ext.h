/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ai driver external head file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AI_EXT_H__
#define __DRV_AI_EXT_H__

#include "hi_drv_ai.h"
#include "hi_drv_ao.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    hi_s32 (*ai_drv_resume)(hi_void *private_data);
    hi_s32 (*ai_drv_suspend)(hi_void *private_data);
} ai_export_func;

hi_s32  ai_drv_mod_init(hi_void);
hi_void ai_drv_mod_exit(hi_void);

hi_s32 hi_drv_ai_init(hi_void);
hi_void hi_drv_ai_deinit(hi_void);
hi_s32 hi_drv_ai_open(void *private_data);
hi_s32 hi_drv_ai_release(void *private_data);
hi_s32 hi_drv_ai_get_default_attr(hi_ai_port ai_port, hi_ai_attr *attr);
hi_s32 hi_drv_ai_create(ai_drv_create_param *ai_para, void *private_data);
hi_s32 hi_drv_ai_destroy(hi_handle h_ai);
hi_s32 hi_drv_ai_set_enable(hi_handle h_ai, hi_bool enable);
hi_s32 hi_drv_ai_get_enable(hi_handle h_ai, hi_bool *enable);
hi_s32 hi_drv_ai_get_attr(hi_handle h_ai, hi_ai_attr *attr);
hi_s32 hi_drv_ai_get_attr(hi_handle h_ai, hi_ai_attr *attr);
hi_s32 hi_drv_ai_acquire_frame(hi_handle h_ai, ao_frame *frame);
hi_s32 hi_drv_ai_release_frame(hi_handle h_ai);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_AI_EXT_H__ */
