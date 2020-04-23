/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel 0ddim module
* Author: sdk
* Create: 2019-11-23
*/

#include "hal_panel_dim.h"
#include "drv_panel_0ddim.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

//#if PANEL_0DDIM_SUPPORT
#define PANEL_DIM0D_STRENGTH_MIN 1
#define PANEL_DIM0D_STRENGTH_MAX 1

#define PANEL_DIM0D_LIGHT_LEVEL_MAX 0xff

panel_0d_dim_contex g_st_0d_dim_ctx = { 0 };

hi_void drv_panel_get_0d_dim_ctx(panel_0d_dim_contex **p_st_0d_dim_ctx)
{
    *p_st_0d_dim_ctx = &g_st_0d_dim_ctx;
}

/*******************************************************************************
                                                0_d_dim export function
*******************************************************************************/
hi_void drv_panel_set_0d_dim_init(hi_u32 bl_level)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    panel_0d_dim_ctx->st_0d_dim_info.p_0d_dim_en = HI_FALSE;
    panel_0d_dim_ctx->st_0d_dim_info.bl_level = bl_level;
    panel_0d_dim_ctx->st_0d_dim_info.str_level = 0;

    panel_0d_dim_ctx->init = HI_TRUE;
}

hi_s32 drv_panel_set_0d_dim_cfg(hi_u32 panel_width, hi_u32 panel_height)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (!panel_0d_dim_ctx->init) {
        hi_log_err("please init 0D dim first!\n");
        return HI_FAILURE;
    }

    hal_panel_set_dim_led_enable(HI_TRUE);
    hal_panel_set_dim_enable(panel_0d_dim_ctx->st_0d_dim_info.p_0d_dim_en);
    hal_panel_set_dim_glb_norm_unit((panel_width / 2) * (panel_height / 2));  /* 2: default parm */
    hal_panel_set_dynamic_bl_gain_level(panel_0d_dim_ctx->st_0d_dim_info.bl_level);
#ifdef PANEL_LDM_SUPPORT
    hal_panel_set_dim_y_mul_value(panel_width, panel_height);
#endif
    panel_0d_dim_ctx->cfg = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 drv_panel_check_0d_dim_ctx(panel_0d_dim_contex *panel_0d_dim_ctx)
{
    if (panel_0d_dim_ctx == HI_NULL) {
        hi_log_err("panel_0d_dim_ctx is null!\n");
        return HI_FAILURE;
    }
    if (!panel_0d_dim_ctx->init) {
        hi_log_err("please init 0D dim first!\n");
        return HI_FAILURE;
    }

    if (!panel_0d_dim_ctx->cfg) {
        hi_log_err("please config 0D dim hardware first!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_0d_dim_enable(hi_bool enable)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (drv_panel_check_0d_dim_ctx(panel_0d_dim_ctx) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if ((enable != HI_TRUE) && (enable != HI_FALSE)) {
        hi_log_err("set 0D dim enable param illegal!\n");
        return HI_FAILURE;
    }

    hal_panel_set_dim_enable(enable);
    panel_0d_dim_ctx->st_0d_dim_info.p_0d_dim_en = enable;

    return HI_SUCCESS;
}

hi_s32 drv_panel_get_0d_dim_enable(hi_bool *enable)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (drv_panel_check_0d_dim_ctx(panel_0d_dim_ctx) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (enable == HI_NULL) {
        hi_log_err("get 0D dim enable param illegal!\n");
        return HI_FAILURE;
    }

    *enable = panel_0d_dim_ctx->st_0d_dim_info.p_0d_dim_en;

    return HI_SUCCESS;
}

hi_s32 drv_panel_get_0d_dim_strength_range(hi_drv_panel_range *str_range)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (drv_panel_check_0d_dim_ctx(panel_0d_dim_ctx) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (str_range == HI_NULL) {
        hi_log_err("get 0D dim strength range param illegal!\n");
        return HI_FAILURE;
    }

    str_range->min = PANEL_DIM0D_STRENGTH_MIN;
    str_range->max = PANEL_DIM0D_STRENGTH_MAX;

    return HI_SUCCESS;
}

hi_s32 drv_panel_set_0d_dim_strength_level(hi_u32 level)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (drv_panel_check_0d_dim_ctx(panel_0d_dim_ctx) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if ((level > PANEL_DIM0D_STRENGTH_MAX) || (level < PANEL_DIM0D_STRENGTH_MIN)) {
        hi_log_err("0D dim strength illegal!\n");
        return HI_FAILURE;
    }

    panel_0d_dim_ctx->st_0d_dim_info.str_level = level;
    return HI_SUCCESS;
}
hi_s32 drv_panel_check_0d_dim_ctx_and_level(panel_0d_dim_contex *panel_0d_dim_ctx, hi_u32 *level)
{
    if (panel_0d_dim_ctx == HI_NULL) {
        hi_log_err("panel_0d_dim_ctx is NULL!\n");
        return HI_FAILURE;
    }
    if (drv_panel_check_0d_dim_ctx(panel_0d_dim_ctx) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (level == HI_NULL) {
        hi_log_err("get 0D dim strength param illegal!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
hi_s32 drv_panel_get_0d_dim_strength_level(hi_u32 *level)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (drv_panel_check_0d_dim_ctx_and_level(panel_0d_dim_ctx, level) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    *level = panel_0d_dim_ctx->st_0d_dim_info.str_level;
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_0d_dim_bl_level(hi_u32 level)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (drv_panel_check_0d_dim_ctx(panel_0d_dim_ctx) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    panel_0d_dim_ctx->st_0d_dim_info.bl_level = level;
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_0d_dim_bl_level(hi_u32 *level)
{
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;

    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    if (drv_panel_check_0d_dim_ctx_and_level(panel_0d_dim_ctx, level) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    *level = panel_0d_dim_ctx->st_0d_dim_info.bl_level;
    return HI_SUCCESS;
}

#ifndef __DISP_PLATFORM_BOOT__
hi_void drv_panel_0d_dim_proc_read(hi_void *s)
{
    struct seq_file *sign;
    panel_0d_dim_contex *panel_0d_dim_ctx = HI_NULL;
    hi_char *bool_p[] = { "FALSE", "TRUE" };
    sign = (struct seq_file *)s;
    drv_panel_get_0d_dim_ctx(&panel_0d_dim_ctx);

    osal_proc_print(sign, "=========================================== 0d_dim info state "
        "==========================================\n");
    osal_proc_print(sign, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12d|\n",
        "0d_dim_init", bool_p[panel_0d_dim_ctx->init],
        "hw_config", bool_p[panel_0d_dim_ctx->cfg],
        "0d_dim_en", bool_p[panel_0d_dim_ctx->st_0d_dim_info.p_0d_dim_en],
        "0d_bl_level", panel_0d_dim_ctx->st_0d_dim_info.bl_level);
}
#endif
//#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

