/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ioctl definitions
 */

#ifndef __DRV_PMOC_IOCTL_H__
#define __DRV_PMOC_IOCTL_H__

#include "hi_drv_pmoc.h"
#include "hi_drv_module.h"
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_pmoc_wakeup_src source;
    hi_bool enable;
    hi_pmoc_suspend_param param;
} pmoc_cmd_suspend_attr;

enum {
    PMOC_SET_SUSPEND_ATTR_CMD_ID,
    PMOC_GET_SUSPEND_ATTR_CMD_ID,
    PMOC_SET_WAKEUP_TYPE_CMD_ID,
    PMOC_STANDBY_READY_CMD_ID,
    PMOC_GET_WAKEUP_ATTR_CMD_ID,
    PMOC_GET_STANDBY_PERIOD_CMD_ID,
    PMOC_CLEAN_WAKEUP_PARAM_CMD_ID,
    PMOC_ENTER_ACTIVE_STANDBY_CMD_ID,
    PMOC_QUIT_ACTIVE_STANDBY_CMD_ID,
    PMOC_SET_DISPLAY_PARAM_CMD_ID,
    PMOC_SET_GPIO_POWEROFF_CMD_ID,
    PMOC_GET_CHIP_TEMPERATURE_CMD_ID,

    PMOC_MAX_CMD_ID,
};

/* Ioctl definitions */
#define CMD_PMOC_SET_SUSPEND_ATTR        _IOW(HI_ID_PM, PMOC_SET_SUSPEND_ATTR_CMD_ID, pmoc_cmd_suspend_attr)
#define CMD_PMOC_GET_SUSPEND_ATTR        _IOWR(HI_ID_PM, PMOC_GET_SUSPEND_ATTR_CMD_ID, pmoc_cmd_suspend_attr)
#define CMD_PMOC_SET_WAKEUP_TYPE         _IOW(HI_ID_PM, PMOC_SET_WAKEUP_TYPE_CMD_ID, hi_pmoc_wakeup_type)
#define CMD_PMOC_STANDBY_READY           _IO(HI_ID_PM, PMOC_STANDBY_READY_CMD_ID)
#define CMD_PMOC_GET_WAKEUP_ATTR         _IOR(HI_ID_PM, PMOC_GET_WAKEUP_ATTR_CMD_ID, hi_pmoc_wakeup_attr)
#define CMD_PMOC_GET_STANDBY_PERIOD      _IOR(HI_ID_PM, PMOC_GET_STANDBY_PERIOD_CMD_ID, hi_u32)
#define CMD_PMOC_CLEAN_WAKEUP_PARAM      _IO(HI_ID_PM, PMOC_CLEAN_WAKEUP_PARAM_CMD_ID)
#define CMD_PMOC_ENTER_ACTIVE_STANDBY    _IOW(HI_ID_PM, PMOC_ENTER_ACTIVE_STANDBY_CMD_ID, hi_pmoc_active_standby_modle)
#define CMD_PMOC_QUIT_ACTIVE_STANDBY     _IO(HI_ID_PM, PMOC_QUIT_ACTIVE_STANDBY_CMD_ID)
#define CMD_PMOC_SET_DISPLAY_PARAM       _IOW(HI_ID_PM, PMOC_SET_DISPLAY_PARAM_CMD_ID, hi_pmoc_display_param)
#define CMD_PMOC_SET_GPIO_POWEROFF       _IOW(HI_ID_PM, PMOC_SET_GPIO_POWEROFF_CMD_ID, hi_pmoc_poweroff_gpio_param)
#define CMD_PMOC_GET_CHIP_TEMPERATURE    _IOR(HI_ID_PM, PMOC_GET_CHIP_TEMPERATURE_CMD_ID, hi_s32)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_PMOC_IOCTL_H__*/
