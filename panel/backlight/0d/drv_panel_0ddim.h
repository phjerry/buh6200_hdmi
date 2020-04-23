/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel oddim module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __DRV_PANEL_0DDIM_H__
#define __DRV_PANEL_0DDIM_H__

#include "drv_panel_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool p_0d_dim_en;
    hi_u32 bl_level;
    hi_u32 str_level;
} panel_0d_dim_info;

typedef struct {
    hi_bool init;
    hi_bool cfg;

    panel_0d_dim_info st_0d_dim_info;
} panel_0d_dim_contex;

hi_void drv_panel_set_0d_dim_init(hi_u32 bl_level);

hi_s32 drv_panel_set_0d_dim_cfg(hi_u32 panel_width, hi_u32 panel_height);

hi_s32 drv_panel_check_0d_dim_ctx(panel_0d_dim_contex *panel_0d_dim_ctx);

hi_s32 drv_panel_set_0d_dim_enable(hi_bool enable);

hi_s32 drv_panel_get_0d_dim_enable(hi_bool *enable);

hi_s32 drv_panel_get_0d_dim_strength_range(hi_drv_panel_range *str_range);

hi_s32 drv_panel_set_0d_dim_strength_level(hi_u32 level);

hi_s32 drv_panel_check_0d_dim_ctx_and_level(panel_0d_dim_contex *panel_0d_dim_ctx, hi_u32 *level);

hi_s32 drv_panel_get_0d_dim_strength_level(hi_u32 *level);

hi_s32 drv_panel_set_0d_dim_bl_level(hi_u32 level);

hi_s32 drv_panel_get_0d_dim_bl_level(hi_u32 *level);

#ifndef __DISP_PLATFORM_BOOT__
hi_void drv_panel_0d_dim_proc_read(hi_void *s);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

