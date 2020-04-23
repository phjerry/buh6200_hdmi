/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description:
 */
#ifndef __DRV_PWM_IOCTL_H__
#define __DRV_PWM_IOCTL_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32 freq; /* <frequency(HZ) */ /* < c_ncomment: 频率, 单位HZ */
    /* < duty ratio(calculated to three decimal places) */
    hi_u32 duty_ratio; /* <c_ncomment: 占空比，保留小数点后三位 */
} pwm_para;

typedef struct {
    uintptr_t cfg;
    uintptr_t ctrl;
    uintptr_t state0;
    uintptr_t state1;
} pwm_reg;

typedef struct {
    hi_u32 pwm_no;
    hi_u32 pwm_type;
    pwm_para st_pwm_attr;
} pwm_attr_cmd_para;

typedef struct {
    hi_u32 pwm_no;
    hi_u32 carrier_sig_duration_us;
    hi_u32 low_level_sig_duration_us;
} pwm_signal_cmd_para;

typedef struct {
    hi_u32 pwm_no;
    hi_u32 pwm_type;
    hi_bool b_enable;
} pwm_enable_cmd_para;

typedef enum {
    IOC_PWM_GETATTR,
    IOC_PWM_SETATTR,
    IOC_PWM_SENDSIGNAL,
    IOC_PWM_SETENABLE,
    IOC_PWM_GETSTATE,

    IOC_PWM_BUTT
} ioc_pwm;

#define CMD_PWM_GETATTR    _IOWR(HI_ID_PWM, IOC_PWM_GETATTR, pwm_attr_cmd_para)
#define CMD_PWM_SETATTR    _IOWR(HI_ID_PWM, IOC_PWM_SETATTR, pwm_attr_cmd_para)
#define CMD_PWM_SENDSIGNAL _IOWR(HI_ID_PWM, IOC_PWM_SENDSIGNAL, pwm_signal_cmd_para)
#define CMD_PWM_SETENABLE  _IOWR(HI_ID_PWM, IOC_PWM_SETENABLE, pwm_enable_cmd_para)
#define CMD_PWM_GETSTATE   _IOWR(HI_ID_PWM, IOC_PWM_GETSTATE, pwm_attr_cmd_para)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



