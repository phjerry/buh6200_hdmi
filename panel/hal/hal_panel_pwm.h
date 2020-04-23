/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal pwm module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __HAL_PANEL_PWM_H__
#define __HAL_PANEL_PWM_H__

#include "hal_panel_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    PANEL_PWM_TYPE_PWM = 0x0,
    PANEL_PWM_TYPE_PWM1D1,
    PANEL_PWM_TYPE_PWM1D2,
    PANEL_PWM_TYPE_PWM1D3,
    PANEL_PWM_TYPE_LRSYNC,
    PANEL_PWM_TYPE_LRGLASS,
    PANEL_PWM_TYPE_BUTT,
} panel_pwm_type;

hi_void hal_panel_set_pwm_enable(panel_pwm_type pwm_type, hi_bool enable);

hi_void hal_panel_set_pwm_inv(panel_pwm_type pwm_type, hi_bool inv);

hi_void hal_panel_set_pwm_duty(panel_pwm_type pwm_type, hi_u32 duty);

hi_void hal_panel_set_pwm_freq(panel_pwm_type pwm_type, hi_u32 freq);

hi_void hal_panel_set_pwm_vsync_rise_count(panel_pwm_type pwm_type, hi_bool enable);

hi_void hal_panel_set_pwm_init_high_level(panel_pwm_type pwm_type, hi_bool high_level);

hi_void hal_panel_set_pwm_refresh(panel_pwm_type pwm_type);

hi_void hal_panel_set_pwm_lr_glass_signal_in(hi_bool b3d_sync_out);

hi_void hal_panel_set3d_sg_mode(panel_pwm_type pwm_type, hi_bool enable);

hi_void hal_panel_set_pwm_bl_mode(panel_pwm_type pwm_type, hi_bool dynamic_bl);

hi_void hal_panel_set_pwm_sync_whole_mode(panel_pwm_type pwm_type, hi_bool enable);

hi_void hal_panel_set_pwm3d_sg_phase(panel_pwm_type pwm_type, hi_u32 m, hi_u32 p, hi_u32 q);

hi_void hal_panel_set_pwm_dynamic_mode(hi_drv_panel_pwm_type pwm_type, hi_bool enable);

hi_void hal_panel_set_pwm_signal_to_open_drain(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

