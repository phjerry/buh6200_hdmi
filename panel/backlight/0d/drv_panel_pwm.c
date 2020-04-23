/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel pwm module
* Author: sdk
* Create: 2019-11-23
*/

#include "drv_panel_define.h"
#include "hal_panel_pwm.h"
#include "drv_panel_pwm.h"
#include "linux/hisilicon/securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

//#if PANEL_PWM_SUPPORT
#define PANEL_DRV_PWM_DUTY_MAX 0xff
#define PANEL_DRV_PWM_FREQ_MAX 999
#define PANEL_DRV_PWM_DEF_DUTY 0x80
#define PANEL_DRV_PWM_DEF_FREQ 0x12c

typedef struct {
    hi_bool init;
    hi_drv_panel_pwm_type pwm_type;
    drv_panel_pwm_attr pwm_attr;
} drv_panel_pwm_info;

typedef struct {
    hi_u8 u8_pwm_using_cnt;
    drv_panel_pwm_info pwm_info[HI_DRV_PANEL_PWM_TYPE_BUTT];
} panel_pwm_ctx;

panel_pwm_ctx g_st_pwm_ctx;

hi_void drv_panel_get_pwm_ctx(panel_pwm_ctx **pwm_ctx_p)
{
    *pwm_ctx_p = &g_st_pwm_ctx;
}

hi_bool drv_panel_is_pwm_inited(hi_drv_panel_pwm_type pwm_type, panel_pwm_ctx *pwm_ctx_p, hi_u8 *position_p)
{
    hi_u8 u8_idx = 0;
    hi_bool exist = HI_FALSE;

    if (pwm_ctx_p->u8_pwm_using_cnt == 0) {
        return exist;
    }

    for (u8_idx = 0; u8_idx < pwm_ctx_p->u8_pwm_using_cnt; u8_idx++) {
        if ((pwm_type == pwm_ctx_p->pwm_info[u8_idx].pwm_type) && pwm_ctx_p->pwm_info[u8_idx].init) {
            exist = HI_TRUE;
            if (position_p != HI_NULL) {
                *position_p = u8_idx;
            }
            break;
        }
    }

    return exist;
}

hi_s32 drv_panel_check_pwm_attr(drv_panel_pwm_attr *attr_p)
{
    if (attr_p->pwm_duty > PANEL_DRV_PWM_DUTY_MAX) {
        hi_log_err("pwm duty must in range[0, PANEL_DRV_PWM_DUTY_MAX], not support pwm_duty\n");
        hi_err_print_u32(attr_p->pwm_duty);
        return HI_FAILURE;
    }

    if ((attr_p->enable != HI_TRUE) && (attr_p->enable != HI_FALSE)) {
        hi_log_err("pwm enable must TRUE or FALSE\n");
        hi_err_print_u32((hi_u32)attr_p->enable);
        return HI_FAILURE;
    }

    if ((attr_p->dynamic_mode != HI_TRUE) && (attr_p->dynamic_mode != HI_FALSE)) {
        hi_log_err("pwm dynamic_mode must TRUE or FALSE\n");
        hi_err_print_u32((hi_u32)attr_p->dynamic_mode);
        return HI_FAILURE;
    }

    if ((attr_p->sig_inv != HI_TRUE) && (attr_p->sig_inv != HI_FALSE)) {
        hi_log_err("pwm signal invert must TRUE or FALSE\n");
        hi_err_print_u32((hi_u32)attr_p->sig_inv);
        return HI_FAILURE;
    }

    if ((attr_p->vsync_raise != HI_TRUE) && (attr_p->vsync_raise != HI_FALSE)) {
        hi_log_err("pwm vsync raise must TRUE or FALSE\n");
        hi_err_print_u32((hi_u32)attr_p->vsync_raise);
        return HI_FAILURE;
    }

    if ((attr_p->sync_whole_mode != HI_TRUE) && (attr_p->sync_whole_mode != HI_FALSE)) {
        hi_log_err("pwm sync_whole_mode must TRUE or FALSE\n");
        hi_err_print_u32((hi_u32)attr_p->sync_whole_mode);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*******************************************************************************
                                                PWM export function
*******************************************************************************/
hi_s32 drv_panel_pwm_init(hi_drv_panel_pwm_type pwm_type, drv_panel_pwm_attr *attr_p)
{
    hi_s32 ret = HI_SUCCESS;
    errno_t err;
    hi_bool pwm_inited;
    panel_pwm_ctx *pwm_ctx_p = HI_NULL;

    if (pwm_type >= HI_DRV_PANEL_PWM_TYPE_BUTT) {
        hi_log_err("pwm_type ivalid, must in range[HI_DRV_PANEL_PWM_TYPE_PWM, HI_DRV_PANEL_PWM_TYPE_BUTT)\n");
        hi_err_print_u32((hi_u32)pwm_type);
        return HI_FAILURE;
    }

    drv_panel_get_pwm_ctx(&pwm_ctx_p);

    if (pwm_ctx_p->u8_pwm_using_cnt >= HI_DRV_PANEL_PWM_TYPE_BUTT) {
        hi_log_err("support max pwm number is smaller than u8_pwm_using_cnt!\n");
        hi_err_print_u32(pwm_ctx_p->u8_pwm_using_cnt);
        return HI_FAILURE;
    }

    pwm_inited = drv_panel_is_pwm_inited(pwm_type, pwm_ctx_p, HI_NULL);
    if (!pwm_inited) {
        /* set default attr */
        if (attr_p == HI_NULL) {
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].init = HI_TRUE;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_type = pwm_type;

            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr.enable = HI_TRUE;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr.dynamic_mode = HI_FALSE;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr.pwm_duty = PANEL_DRV_PWM_DEF_DUTY;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr.pwm_freq = PANEL_DRV_PWM_DEF_FREQ;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr.sig_inv = HI_FALSE;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr.vsync_raise = HI_TRUE;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr.sync_whole_mode = HI_TRUE;

            pwm_ctx_p->u8_pwm_using_cnt++;
        } else {
            ret = drv_panel_check_pwm_attr(attr_p);
            if (ret != HI_SUCCESS) {
                hi_log_err("drv_panel_check_pwm_attr failed!\n");
                return HI_FAILURE;
            }

            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].init = HI_TRUE;
            pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_type = pwm_type;

            err = memcpy_s(&pwm_ctx_p->pwm_info[pwm_ctx_p->u8_pwm_using_cnt].pwm_attr,
                sizeof(drv_panel_pwm_attr), attr_p, sizeof(drv_panel_pwm_attr));
            if (err != EOK) {
                hi_log_err("memcpy_s failed!\n");
                return HI_FAILURE;
            }

            pwm_ctx_p->u8_pwm_using_cnt++;
        }
    }

    return ret;
}

hi_s32 drv_panel_set_pwm_cfg(hi_drv_panel_pwm_type pwm_type)
{
    hi_u8 u8_index = 0;
    hi_bool pwm_inited;
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };
    panel_pwm_ctx *pwm_ctx_p = HI_NULL;

    HI_UNUSED(pchar_pwm_type[0]);

    if (pwm_type >= HI_DRV_PANEL_PWM_TYPE_BUTT) {
        hi_log_err("pwm_type:%d ivalid, pwm_type must be less than:%d\n", pwm_type, HI_DRV_PANEL_PWM_TYPE_BUTT);
        return HI_FAILURE;
    }

    drv_panel_get_pwm_ctx(&pwm_ctx_p);

    pwm_inited = drv_panel_is_pwm_inited(pwm_type, pwm_ctx_p, &u8_index);
    if (pwm_inited) {
        hal_panel_set_pwm_dynamic_mode(pwm_type, pwm_ctx_p->pwm_info[u8_index].pwm_attr.dynamic_mode);
        hal_panel_set_pwm_duty((panel_pwm_type)pwm_type, pwm_ctx_p->pwm_info[u8_index].pwm_attr.pwm_duty);
        hal_panel_set_pwm_freq((panel_pwm_type)pwm_type, pwm_ctx_p->pwm_info[u8_index].pwm_attr.pwm_freq);
        hal_panel_set_pwm_inv((panel_pwm_type)pwm_type, pwm_ctx_p->pwm_info[u8_index].pwm_attr.sig_inv);
        hal_panel_set_pwm_vsync_rise_count((panel_pwm_type)pwm_type,
                                           pwm_ctx_p->pwm_info[u8_index].pwm_attr.vsync_raise);
        hal_panel_set_pwm_sync_whole_mode((panel_pwm_type)pwm_type,
                                          pwm_ctx_p->pwm_info[u8_index].pwm_attr.sync_whole_mode);
        hal_panel_set_pwm_enable((panel_pwm_type)pwm_type, pwm_ctx_p->pwm_info[u8_index].pwm_attr.enable);
        hal_panel_set_pwm_refresh((panel_pwm_type)pwm_type);

        /* set open drain as 0 to output backlight PWM */
        hal_panel_set_pwm_signal_to_open_drain();
    } else {
        hi_log_err("pwm_type not init, please init first!\n");
        hi_err_print_str(pchar_pwm_type[pwm_type]);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_get_pwm_attr(hi_drv_panel_pwm_type pwm_type, drv_panel_pwm_attr *attr_p)
{
    hi_u8 u8_index = 0;
    errno_t err;
    hi_bool pwm_inited;
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };
    panel_pwm_ctx *pwm_ctx_p = HI_NULL;

    HI_UNUSED(pchar_pwm_type[0]);

    if (pwm_type >= HI_DRV_PANEL_PWM_TYPE_BUTT) {
        hi_log_err("pwm_type ivalid, must in range[HI_DRV_PANEL_PWM_TYPE_PWM, HI_DRV_PANEL_PWM_TYPE_BUTT)\n");
        hi_err_print_u32((hi_u32)pwm_type);
        return HI_FAILURE;
    }

    drv_panel_get_pwm_ctx(&pwm_ctx_p);

    pwm_inited = drv_panel_is_pwm_inited(pwm_type, pwm_ctx_p, &u8_index);
    if (pwm_inited) {
        err = memcpy_s(attr_p, sizeof(drv_panel_pwm_attr),
            &(pwm_ctx_p->pwm_info[u8_index].pwm_attr), sizeof(drv_panel_pwm_attr));
        if (err != EOK) {
            hi_log_err("memcpy_s failed\n");
            return HI_FAILURE;
        }
    } else {
        hi_log_err("pwm_type not init, please init first!\n");
        hi_err_print_str(pchar_pwm_type[pwm_type]);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_set_pwm_attr(hi_drv_panel_pwm_type pwm_type, drv_panel_pwm_attr *attr_p)
{
    hi_s32 ret;
    errno_t err;
    hi_u8 u8_index = 0;
    hi_bool pwm_inited;
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };
    panel_pwm_ctx *pwm_ctx_p = HI_NULL;

    HI_UNUSED(pchar_pwm_type[0]);

    if (pwm_type >= HI_DRV_PANEL_PWM_TYPE_BUTT) {
        hi_log_err("pwm_type ivalid, must in range[HI_DRV_PANEL_PWM_TYPE_PWM, HI_DRV_PANEL_PWM_TYPE_BUTT)\n");
        hi_err_print_u32((hi_u32)pwm_type);
        return HI_FAILURE;
    }

    if (attr_p == HI_NULL) {
        hi_log_err("param attr is NULL\n");
        return HI_FAILURE;
    }

    ret = drv_panel_check_pwm_attr(attr_p);
    if (ret != HI_SUCCESS) {
        hi_log_err("param attr is illegal\n");
        return ret;
    }

    drv_panel_get_pwm_ctx(&pwm_ctx_p);

    pwm_inited = drv_panel_is_pwm_inited(pwm_type, pwm_ctx_p, &u8_index);
    if (pwm_inited) {
        err = memcpy_s(&(pwm_ctx_p->pwm_info[u8_index].pwm_attr), sizeof(drv_panel_pwm_attr),
            attr_p, sizeof(drv_panel_pwm_attr));
        if (err != EOK) {
            hi_log_err("memcpy_s failed\n");
            return HI_FAILURE;
        }
        drv_panel_set_pwm_cfg(pwm_type);
    } else {
        hi_log_err("pwm_type not init, please init first!\n");
        hi_err_print_str(pchar_pwm_type[pwm_type]);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_pwm_de_init(hi_drv_panel_pwm_type pwm_type)
{
    hi_bool pwm_inited;
    hi_u8 u8_index = 0;
    errno_t err;
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };
    panel_pwm_ctx *pwm_ctx = HI_NULL;

    HI_UNUSED(pchar_pwm_type[0]);

    if (pwm_type >= HI_DRV_PANEL_PWM_TYPE_BUTT) {
        hi_log_err("pwm_type ivalid, must be less then:HI_DRV_PANEL_PWM_TYPE_BUTT.\n");
        hi_err_print_u32((hi_u32)pwm_type);
        return HI_FAILURE;
    }

    drv_panel_get_pwm_ctx(&pwm_ctx);

    pwm_inited = drv_panel_is_pwm_inited(pwm_type, pwm_ctx, &u8_index);
    if (pwm_inited) {
        pwm_ctx->pwm_info[u8_index].init = HI_FALSE;
        pwm_ctx->pwm_info[u8_index].pwm_type = HI_DRV_PANEL_PWM_TYPE_BUTT;

        err = memset_s(&(pwm_ctx->pwm_info[u8_index].pwm_attr), sizeof(drv_panel_pwm_attr), 0,
            sizeof(drv_panel_pwm_attr));
        if (err != EOK) {
            hi_log_err("memset_s failed!\n");
            return HI_FAILURE;
        }
        if (pwm_ctx->u8_pwm_using_cnt > 0) {
            pwm_ctx->u8_pwm_using_cnt--;
        }
    } else {
        hi_log_err("pwm_type not init, please init first!\n");
        hi_err_print_str(pchar_pwm_type[pwm_type]);
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_update_pwm_duty(hi_u32 pwm_duty)
{
    hi_bool pwm_inited;
    hi_u8 u8_index = 0;
    panel_pwm_ctx *pwm_ctx_p = HI_NULL;

    drv_panel_get_pwm_ctx(&pwm_ctx_p);

    pwm_inited = drv_panel_is_pwm_inited(HI_DRV_PANEL_PWM_TYPE_PWM, pwm_ctx_p, &u8_index);
    if (pwm_inited) {
        pwm_ctx_p->pwm_info[u8_index].pwm_attr.pwm_duty = pwm_duty;
    } else {
        hi_log_err("update_pwm_duty failed, please init first!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_update_pwm_dynamic_mode(hi_bool dynamic_mode)
{
    hi_bool pwm_inited;
    hi_u8 u8_index = 0;
    panel_pwm_ctx *pwm_ctx_p = HI_NULL;

    drv_panel_get_pwm_ctx(&pwm_ctx_p);

    pwm_inited = drv_panel_is_pwm_inited(HI_DRV_PANEL_PWM_TYPE_PWM, pwm_ctx_p, &u8_index);
    if (pwm_inited) {
        pwm_ctx_p->pwm_info[u8_index].pwm_attr.dynamic_mode = dynamic_mode;
    } else {
        hi_log_err("update pwm dynamic mode failed, please init first!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

#ifndef __DISP_PLATFORM_BOOT__
hi_void drv_panel_pwm_proc_read(hi_void *s)
{
    hi_u8 u8i = 0;
    hi_char *pchar_bool[] = { "FALSE", "TRUE" };
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };
    panel_pwm_ctx *pwm_ctx_p = HI_NULL;

    drv_panel_get_pwm_ctx(&pwm_ctx_p);

    osal_proc_print(s, "===========================================  panel pwm info  "
        "===========================================\n");

    for (u8i = 0; u8i < pwm_ctx_p->u8_pwm_using_cnt; u8i++) {
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "pwm_type", pchar_pwm_type[pwm_ctx_p->pwm_info[u8i].pwm_type],
            "pwm_init", pchar_bool[pwm_ctx_p->pwm_info[u8i].init],
            "enable", pchar_bool[pwm_ctx_p->pwm_info[u8i].pwm_attr.enable],
            "", "");
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12d|%-12s:%-12d|%-12s:%-12s|\n",
            "dynamic_mod", pchar_bool[pwm_ctx_p->pwm_info[u8i].pwm_attr.dynamic_mode],
            "pwm_duty", pwm_ctx_p->pwm_info[u8i].pwm_attr.pwm_duty,
            "pwm_freq", pwm_ctx_p->pwm_info[u8i].pwm_attr.pwm_freq,
            "signal_inv", pchar_bool[pwm_ctx_p->pwm_info[u8i].pwm_attr.sig_inv]);
        osal_proc_print(s, "%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|%-12s:%-12s|\n",
            "vsync_raise", pchar_bool[pwm_ctx_p->pwm_info[u8i].pwm_attr.vsync_raise],
            "sync_who_mod", pchar_bool[pwm_ctx_p->pwm_info[u8i].pwm_attr.sync_whole_mode],
            "", "", "", "");
    }
}

#ifdef PANEL_TMP
hi_void drv_panel_pwm_proc_help(hi_void)
{
    osal_proc_print("-------------------------------------------------  "
        "pwm_help ---------------------------------------------------\n");
    osal_proc_print ("echo pwmen      [0~3]  [0/1]  >/proc/msp/panel    "
        "set pwm enable     (0:pwm0 1:pwm1 2:pwm2 3:pwm3)(0:disable 1:enable)\n");
    osal_proc_print ("echo pwmdymode  [0~3]  [0/1]  >/proc/msp/panel    "
        "set pwm dynamic_mode (0:pwm0 1:pwm1 2:pwm2 3:pwm3)(0:disable 1:enable)\n");
    osal_proc_print ("echo pwmduty    [0~3]  [0~255]>/proc/msp/panel    "
        "set pwm duty       (0:pwm0 1:pwm1 2:pwm2 3:pwm3)(0~255)\n");
    osal_proc_print ("echo pwmfreq    [0~3]  [0~999]>/proc/msp/panel    "
        "set pwm freq       (0:pwm0 1:pwm1 2:pwm2 3:pwm3)(0~999)\n");
    osal_proc_print ("echo pwminvert  [0~3]  [0/1]  >/proc/msp/panel    "
        "set pwm freq       (0:pwm0 1:pwm1 2:pwm2 3:pwm3)(0:positive 1:invert)\n");
    osal_proc_print ("echo syncwhole  [0~3]  [0/1]  >/proc/msp/panel    "
        "set pwm sync_whole_mod(0:pwm0 1:pwm1 2:pwm2 3:pwm3)(0:sync_whole 1:sync_vsync)\n");
}
#endif

hi_s32 drv_panel_check_bool_value(hi_bool value)
{
    if (((value) != HI_TRUE) && ((value) != HI_FALSE)) {
        hi_log_err("parameter ERROR, correct range [0~1]!\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_check_pwm_type(hi_drv_panel_pwm_type pwm_type)
{
    if ((pwm_type) >= HI_DRV_PANEL_PWM_TYPE_BUTT) {
        hi_log_err("pwm_type ivalid, must in range[HI_DRV_PANEL_PWM_TYPE_PWM, HI_DRV_PANEL_PWM_TYPE_BUTT)\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_panel_pwm_proc_set_enable(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_bool value;
    hi_s32 ret;
    hi_drv_panel_pwm_type pwm_type;
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };

    HI_UNUSED(pchar_pwm_type[0]);

    pwm_type = (hi_drv_panel_pwm_type)(simple_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN));
    ret = drv_panel_check_pwm_type(pwm_type);
    if (ret !=HI_SUCCESS) {
        return HI_FAILURE;
    }

    value = (hi_bool)(simple_strtol(arg1_p[2], NULL, PANEL_PARSE_PROC_LEN));
    ret = drv_panel_check_bool_value(value);
    if (ret !=HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (drv_panel_pwm_init(pwm_type, HI_NULL) != HI_SUCCESS) {
        hi_log_err("panel_drv_pwm_init failed.\n");
        return HI_FAILURE;
    }

    if (drv_panel_set_pwm_cfg(pwm_type) != HI_SUCCESS) {
        hi_log_err("panel_drv_set_pwm_cfg failed.\n");
        return HI_FAILURE;
    }

    hi_log_err("pwm_type_init cfg and enable finish!\n");
    hi_dbg_print_str(pchar_pwm_type[pwm_type]);

    return HI_SUCCESS;
}
hi_s32 drv_panel_pwm_proc_set_duty(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    hi_s32 ret;
    drv_panel_pwm_attr attr = { 0 };
    hi_drv_panel_pwm_type pwm_type;
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };

    HI_UNUSED(pchar_pwm_type[0]);

    pwm_type = (hi_drv_panel_pwm_type)(simple_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN));
    ret = drv_panel_check_pwm_type(pwm_type);
    if (ret !=HI_SUCCESS) {
        return HI_FAILURE;
    }
    value = (hi_u32)simple_strtol(arg1_p[2], NULL, PANEL_PARSE_PROC_LEN);
    if (value > PANEL_DRV_PWM_DUTY_MAX) {
        hi_log_err("pwm duty must in range[0~HI_DRV_PANEL_PWM_TYPE_PWM], not support value\n");
        return HI_FAILURE;
    }

    if (drv_panel_get_pwm_attr(pwm_type, &attr) != HI_SUCCESS) {
        hi_log_err("panel_drv_get_pwm_attr failed.\n");
        return HI_FAILURE;
    }

    attr.pwm_duty = value;
    if (drv_panel_set_pwm_attr(pwm_type, &attr) != HI_SUCCESS) {
        hi_log_err("panel_drv_set_pwm_attr failed.\n");
        return HI_FAILURE;
    }

    hi_log_err("pwm_type set duty:value!\n");
    hi_dbg_print_str(pchar_pwm_type[pwm_type]);
    hi_dbg_print_u32(value);

    return HI_SUCCESS;
}

hi_s32 drv_panel_pwm_proc_set_freq(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 value;
    hi_s32 ret;
    hi_drv_panel_pwm_type pwm_type;
    drv_panel_pwm_attr attr = { 0 };
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };

    HI_UNUSED(pchar_pwm_type[0]);

    pwm_type = (hi_drv_panel_pwm_type)(simple_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN));
    ret = drv_panel_check_pwm_type(pwm_type);
    if (ret !=HI_SUCCESS) {
        return HI_FAILURE;
    }
    value = (hi_u32)simple_strtol(arg1_p[2], NULL, PANEL_PARSE_PROC_LEN);
    if (value > PANEL_DRV_PWM_FREQ_MAX) {
        hi_log_err("not support value, pwm freq must in range[0, PANEL_DRV_PWM_FREQ_MAX]\n");
        hi_err_print_u32(value);
        return HI_FAILURE;
    }

    if (drv_panel_get_pwm_attr(pwm_type, &attr) != HI_SUCCESS) {
        hi_log_err("panel_drv_get_pwm_attr failed.\n");
        return HI_FAILURE;
    }
    attr.pwm_freq = value;
    if (drv_panel_set_pwm_attr(pwm_type, &attr) != HI_SUCCESS) {
        hi_log_err("panel_drv_set_pwm_attr failed.\n");
        return HI_FAILURE;
    }

    hi_log_err("pwm_type set freq:value!\n");
    hi_dbg_print_str(pchar_pwm_type[pwm_type]);
    hi_dbg_print_u32(value);

    return HI_SUCCESS;
}

hi_s32 drv_panel_pwm_proc_set_pwm_attr(hi_u32 arg_p,
    char (*arg1_p)[PANEL_PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private, drv_panel_pwm_attr_mode attr_mdoe)
{
    hi_u32 value;
    hi_s32 ret;
    drv_panel_pwm_attr attr = { 0 };
    hi_drv_panel_pwm_type pwm_type;
    hi_char *pchar_pwm_type[] = { "PWM0", "PWM1", "PWM2", "PWM3", "LRSYNC", "LRGLASS", "BUTT" };
    hi_char *attr_mode_str[] = {"dynamic_mode", "SYNC_WHOLE_MODE", "SIG_INV_MODE", "BUTT"};

    HI_UNUSED(pchar_pwm_type[0]);
    HI_UNUSED(attr_mode_str[0]);

    pwm_type = (hi_drv_panel_pwm_type)(simple_strtol(arg1_p[1], NULL, PANEL_PARSE_PROC_LEN));
    ret = drv_panel_check_pwm_type(pwm_type);
    if (ret !=HI_SUCCESS) {
        return HI_FAILURE;
    }
    value = (hi_bool)(simple_strtol(arg1_p[2], NULL, PANEL_PARSE_PROC_LEN));
    ret = drv_panel_check_bool_value(value);
    if (ret !=HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (drv_panel_get_pwm_attr(pwm_type, &attr) != HI_SUCCESS) {
        hi_log_err("panel_drv_get_pwm_attr failed.\n");
        return HI_FAILURE;
    }
    switch (attr_mdoe) {
        case DYNAMIC_MODE:
            attr.dynamic_mode = value;
            break;
        case SYNC_WHOLE_MODE:
            attr.sync_whole_mode = value;
            break;
        case SIG_INV_MODE:
            attr.sig_inv = value;
            break;
        default:
            break;
    }

    if (drv_panel_set_pwm_attr(pwm_type, &attr) != HI_SUCCESS) {
        hi_log_err("panel_drv_set_pwm_attr failed.\n");
        return HI_FAILURE;
    }

    hi_log_err("pwm_type set %s:value!\n", attr_mode_str[attr_mdoe]);
    hi_dbg_print_str(pchar_pwm_type[pwm_type]);
    hi_dbg_print_u32(value);

    return HI_SUCCESS;
}

#endif
//#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

