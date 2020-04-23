/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal dim module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __HAL_PANEL_DIM_H__
#define __HAL_PANEL_DIM_H__

#include "hal_panel_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void hal_panel_set_dim_final_gain(hi_u32 u32gain_final_0d);

hi_void hal_panel_set_dim_lcd_comp0dk_value(hi_u32 value);

hi_void hal_panel_set_dim_enable(hi_bool enable);

hi_void hal_panel_set_2d_dim_final_gain(hi_u32 u32gain_final_2d);

hi_void hal_panel_set_dim_led_enable(hi_bool enable);

hi_void hal_panel_set_dim_lcd_enable(hi_bool enable);

hi_void hal_panel_set_2d_dim_enable(hi_bool enable);

hi_void hal_panel_set_dim_led_num(hi_u32 hor_num, hi_u32 ver_num);

hi_void hal_panel_set_dim_segment_size(hi_u32 hor_size, hi_u32 ver_size);

hi_void hal_panel_set_dim_state_size(hi_u32 hor_size, hi_u32 ver_size);

hi_void hal_panel_set_dim_glb_norm_unit(hi_u32 glb_norm_unit);

hi_void hal_panel_set_dim_seg_norm_unit(hi_u32 seg_norm_unit);

hi_void hal_panel_set_dim_ver_scl_ratio(hi_u32 ver_scl_ratio);

hi_void hal_panel_set_dim_hor_scl_ratio(hi_u32 hor_scl_ratio);

hi_void hal_panel_set_dim_init_value(hi_u32 init_val);

hi_void hal_panel_set_dynamic_bl_gain_level(hi_u32 bl_level);

hi_void hal_panel_set_dim_demo_mode_enable(hi_bool enable);

hi_void hal_panel_set_dim_demo_mode(hal_panel_2_ddim_demo_mode demo_mode);

hi_void hal_panel_set_dim_reg_up(hi_void);

hi_s32 hal_panel_set_dim_gradule_change_step(hi_u32 change_step);

hi_s32 hal_panel_dim_gradule_change_step_print(hi_void);

#ifdef PANEL_LDM_SUPPORT
hi_void hal_panel_set_data_bit_width(hal_panel_ldm_bit_width_sel bit_width);

hi_void hal_panel_set_dim_scd_enable(hi_bool enable);

hi_void hal_panel_set_dim_scdflag(hi_u32 scd_flag);

hi_void hal_panel_get_2d_dim_data(hal_panel_ldm_bit_width_sel bit_width, hi_u32 data_cnt, hi_u32 *buf_p);

hi_void hal_panel_set_dim_y_mul_value(hi_bool uhd_flag);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

