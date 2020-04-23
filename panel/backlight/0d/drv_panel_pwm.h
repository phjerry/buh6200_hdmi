/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel pwm module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __DRV_PANEL_PWM_H__
#define __DRV_PANEL_PWM_H__

#include "drv_panel_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PANEL_PROC_CMD_SINGEL_LENGTH_MAX 16

typedef struct {
    hi_bool enable;
    hi_bool dynamic_mode;
    hi_u32 pwm_duty;
    hi_u32 pwm_freq;
    hi_bool sig_inv;
    hi_bool vsync_raise;
    hi_bool sync_whole_mode;
} drv_panel_pwm_attr;

typedef enum {
    DYNAMIC_MODE,
    SYNC_WHOLE_MODE,
    SIG_INV_MODE,
    ATTR_BUTT_MODE
} drv_panel_pwm_attr_mode;

hi_s32 drv_panel_pwm_init(hi_drv_panel_pwm_type pwm_type, drv_panel_pwm_attr *attr_p);

hi_s32 drv_panel_set_pwm_cfg(hi_drv_panel_pwm_type pwm_type);

hi_s32 drv_panel_get_pwm_attr(hi_drv_panel_pwm_type pwm_type, drv_panel_pwm_attr *attr_p);

hi_s32 drv_panel_set_pwm_attr(hi_drv_panel_pwm_type pwm_type, drv_panel_pwm_attr *attr_p);

hi_s32 drv_panel_update_pwm_duty(hi_u32 pwm_duty);

hi_s32 drv_panel_update_pwm_dynamic_mode(hi_bool dynamic_mode);

#ifdef PANEL_PWM_SUPPORT
#ifndef __DISP_PLATFORM_BOOT__
hi_void drv_panel_pwm_proc_read(hi_void *s);

hi_void drv_panel_pwm_proc_help(hi_void);

hi_s32 drv_panel_pwm_proc_set_enable(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);

hi_s32 drv_panel_pwm_proc_set_duty(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);

hi_s32 drv_panel_pwm_proc_set_freq(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
hi_s32 drv_panel_pwm_proc_set_pwm_attr(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private, drv_panel_pwm_attr_mode attr_mdoe);

#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

