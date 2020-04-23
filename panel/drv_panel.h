/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __DRV_PANEL_H__
#define __DRV_PANEL_H__

#include "drv_panel_ioctl.h"
#include "hi_type.h"
#include "drv_panel_define.h"
#include "hi_drv_panel.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void drv_panel_get_panel_ctx(drv_panel_contex **panel_ctx);

hi_s32 drv_panel_init(hi_void);

hi_s32 drv_panel_deinit(hi_void);

hi_s32 drv_panel_set_power_on(panel_id_index panel_id, hi_bool enable);

hi_s32 drv_panel_get_power_on(panel_id_index panel_id, hi_bool *enable);

hi_s32 drv_panel_set_backlight_power(panel_id_index panel_id, hi_bool enable);

hi_s32 drv_panel_get_backlight_power(panel_id_index panel_id, hi_bool *enable);

hi_s32 drv_panel_get_aspect(panel_id_index panel_id, panel_aspect_info *aspect);

hi_s32 drv_panel_get_3d_type(panel_id_index panel_id, hi_drv_panel_3d_type *type);

hi_s32 drv_panel_set_dynamic_bl_enable(panel_id_index panel_id, hi_bool enable);

hi_s32 drv_panel_get_dynamic_bl_enable(panel_id_index panel_id, hi_bool *enable);

hi_s32 drv_panel_get_backlight_range(panel_id_index panel_id, hi_drv_panel_range *range);

hi_s32 drv_panel_set_backlight_level(panel_id_index panel_id, hi_u32 level);

hi_s32 drv_panel_get_backlight_level(panel_id_index panel_id, hi_u32 *level);

hi_s32 drv_panel_get_dim_strength_info(panel_id_index panel_id, panel_dim_strength_info *dim_strength_info);

hi_s32 drv_panel_set_dim_strength_level(panel_id_index panel_id, hi_u32 level);

hi_s32 drv_panel_get_dim_strength_level(panel_id_index panel_id, hi_u32 *level);

hi_s32 drv_panel_set_ldm_demo_mode(panel_id_index panel_id, hi_drv_panel_ldm_demo_mode demo_mode);

hi_s32 drv_panel_get_ldm_demo_mode(panel_id_index panel_id, hi_drv_panel_ldm_demo_mode *demo_mode);

hi_s32 drv_panel_get_voltage_range(panel_id_index panel_id, panel_signal_list *list);

hi_s32 drv_panel_set_voltage(panel_id_index panel_id, hi_u32 com_voltage);

hi_s32 drv_panel_get_voltage(panel_id_index panel_id, hi_u32 *com_voltage);

hi_s32 drv_panel_get_current_range(panel_id_index panel_id, panel_signal_list *list);

hi_s32 drv_panel_set_current(panel_id_index panel_id, hi_u32 drv_current);

hi_s32 drv_panel_get_current(panel_id_index panel_id, hi_u32 *drv_current);

hi_s32 drv_panel_get_emphasis_range(panel_id_index panel_id, panel_signal_list *list);

hi_s32 drv_panel_set_emphasis(panel_id_index panel_id, hi_u32 panel_emp);

hi_s32 drv_panel_get_emphasis(panel_id_index panel_id, hi_u32 *panel_emp);

hi_s32 drv_panel_set_spread_enable(panel_id_index panel_id, hi_bool enable);

hi_s32 drv_panel_get_spread_enable(panel_id_index panel_id, hi_bool *enable);

hi_s32 drv_panel_get_spread_ratio_range(panel_id_index panel_id, hi_drv_panel_range *range);

hi_s32 drv_panel_set_spread_ratio(panel_id_index panel_id, hi_u32 spread_ratio);

hi_s32 drv_panel_get_spread_ratio(panel_id_index panel_id, hi_u32 *spread_ratio);

hi_s32 drv_panel_get_spread_freq_range(panel_id_index panel_id, panel_signal_list *list);

hi_s32 drv_panel_set_spread_freq(panel_id_index panel_id, hi_u32 spread_freq);

hi_s32 drv_panel_get_spread_freq(panel_id_index panel_id, hi_u32 *spread_freq);

hi_s32 drv_panel_get_panel_name(panel_id_index panel_id, panel_name *name);

hi_s32 drv_panel_set_bit_depth(panel_id_index panel_id, hi_drv_panel_bit_depth depth);

hi_s32 drv_panel_get_bit_depth(panel_id_index panel_id, hi_drv_panel_bit_depth *depth);

hi_s32 drv_panel_set_lvds_attr(panel_id_index panel_id, hi_drv_panel_lvds_attr *lvds_attr);

hi_s32 drv_panel_get_lvds_attr(panel_id_index panel_id, hi_drv_panel_lvds_attr *lvds_attr);

hi_s32 drv_panel_get_interface_type(panel_id_index panel_id, hi_drv_panel_intf_type *intf_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
