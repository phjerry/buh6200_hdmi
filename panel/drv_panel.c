/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __DISP_PLATFORM_BOOT__
#include "hi_drv_sys.h"
#endif

#include "drv_panel.h"
#include "drv_panel_ctrl.h"
#include "linux/hisilicon/securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


drv_panel_contex g_st_panel_ctx = { 0 };

hi_void drv_panel_get_panel_ctx(drv_panel_contex **panel_ctx)
{
    *panel_ctx = &g_st_panel_ctx;
}

/************************************************************************************
                                panel init and deinit
************************************************************************************/
hi_s32 drv_panel_init(hi_void)
{
    hi_s32 ret;
    drv_panel_contex *panel_ctx_p = HI_NULL;

    hi_info_func_enter();

    drv_panel_get_panel_ctx(&panel_ctx_p);

    ret = drv_panel_init_ctx(panel_ctx_p);
    if (ret != HI_SUCCESS) {
        hi_log_err("PANEL drv_update_panel_ctx failed!\n");
        return HI_FAILURE;
    }

    ret = drv_panel_config_hardware(panel_ctx_p);
    if (ret != HI_SUCCESS) {
        hi_log_err("PANEL drv_config_hard_ware failed!\n");
        return HI_FAILURE;
    }

    hi_info_func_exit();

    return HI_SUCCESS;
}

hi_s32 drv_panel_deinit(hi_void)
{
    return HI_SUCCESS;
}

/************************************************************************************
                                                ioctrl export function
************************************************************************************/
hi_s32 drv_panel_set_power_on(panel_id_index panel_id, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_power_on(panel_id_index panel_id, hi_bool *enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_backlight_power(panel_id_index panel_id, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_backlight_power(panel_id_index panel_id, hi_bool *enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_aspect(panel_id_index panel_id, panel_aspect_info *aspect)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_3d_type(panel_id_index panel_id, hi_drv_panel_3d_type *type)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_dynamic_bl_enable(panel_id_index panel_id, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_dynamic_bl_enable(panel_id_index panel_id, hi_bool *enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_backlight_range(panel_id_index panel_id, hi_drv_panel_range *range)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_backlight_level(panel_id_index panel_id, hi_u32 level)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_backlight_level(panel_id_index panel_id, hi_u32 *level)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_dim_strength_info(panel_id_index panel_id, panel_dim_strength_info *dim_strength_info)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_dim_strength_level(panel_id_index panel_id, hi_u32 level)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_dim_strength_level(panel_id_index panel_id, hi_u32 *level)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_ldm_demo_mode(panel_id_index panel_id, hi_drv_panel_ldm_demo_mode demo_mode)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_ldm_demo_mode(panel_id_index panel_id, hi_drv_panel_ldm_demo_mode *demo_mode)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_voltage_range(panel_id_index panel_id, panel_signal_list *list)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_voltage(panel_id_index panel_id, hi_u32 com_voltage)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_voltage(panel_id_index panel_id, hi_u32 *com_voltage)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_current_range(panel_id_index panel_id, panel_signal_list *list)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_current(panel_id_index panel_id, hi_u32 drv_current)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_current(panel_id_index panel_id, hi_u32 *drv_current)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_emphasis_range(panel_id_index panel_id, panel_signal_list *list)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_emphasis(panel_id_index panel_id, hi_u32 panel_emp)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_emphasis(panel_id_index panel_id, hi_u32 *panel_emp)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_spread_enable(panel_id_index panel_id, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_spread_enable(panel_id_index panel_id, hi_bool *enable)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_spread_ratio_range(panel_id_index panel_id, hi_drv_panel_range *range)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_spread_ratio(panel_id_index panel_id, hi_u32 spread_ratio)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_spread_ratio(panel_id_index panel_id, hi_u32 *spread_ratio)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_spread_freq_range(panel_id_index panel_id, panel_signal_list *list)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_spread_freq(panel_id_index panel_id, hi_u32 spread_freq)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_spread_freq(panel_id_index panel_id, hi_u32 *spread_freq)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_panel_name(panel_id_index panel_id, panel_name *name)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_bit_depth(panel_id_index panel_id, hi_drv_panel_bit_depth depth)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_bit_depth(panel_id_index panel_id, hi_drv_panel_bit_depth *depth)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_set_lvds_attr(panel_id_index panel_id, hi_drv_panel_lvds_attr *lvds_attr)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_lvds_attr(panel_id_index panel_id, hi_drv_panel_lvds_attr *lvds_attr)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_interface_type(panel_id_index panel_id, hi_drv_panel_intf_type *intf_type)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_panel_isr_main(hi_drv_panel_disp_info *disp_info, hi_drv_panel_cfg *panel_config)
{
    drv_panel_contex *panel_ctx = HI_NULL;
    errno_t err;

    hi_dbg_func_enter();

    if (disp_info == HI_NULL || panel_config == HI_NULL) {
        hi_log_err("null point, disp_info = %p, panel_config = %p", disp_info, panel_config);
    }

    drv_panel_get_panel_ctx(&panel_ctx);
    /* WORKING */
    err = memcpy_s(panel_config, sizeof(hi_drv_panel_cfg), &panel_ctx->panel_cfg, sizeof(panel_ctx->panel_cfg));
    if (err != EOK) {
        hi_log_err("memcpy_s failed!\n");
        return HI_FAILURE;
    }
    /* WORKING */
    hi_dbg_func_exit();

    return HI_SUCCESS;
}

hi_s32 hi_drv_panel_backlight_change(hi_u32 out_frm_rate)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_panel_localdimming_regist(hi_drv_disp_intf *intf, hi_bool enable)
{
    return HI_SUCCESS;
}
hi_s32 hi_drv_panel_change_mode(hi_void)
{
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
