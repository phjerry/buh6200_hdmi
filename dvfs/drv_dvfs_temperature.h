/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: temperature control
 */

#ifndef __DRV_DVFS_TEMPERATURE_H__
#define __DRV_DVFS_TEMPERATURE_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DELAY_20MS 20
#define DELAY_100MS 100

typedef enum {
    STATUS_NORMAL = 0,
    STATUS_ENTER_LOW_TEMP_COMP,
    STATUS_QUIT_LOW_TEMP_COMP,
    STATUS_ENTER_LIMITING_FREQ,
    STATUS_QUIT_LIMITING_FREQ,
    STATUS_ENTER_STANDBY,
    STATUS_START_UP,
    STATUS_MAX
} dvfs_temperature_status;

typedef struct {
    hi_u32 enter_low_temp_comp;
    hi_u32 quit_low_temp_comp;
    hi_u32 enter_limiting_freq;
    hi_u32 quit_limiting_freq;
    hi_u32 enter_standby;
} dvfs_temperature_ctrl_threshold;

hi_u32 dvfs_get_temperature_status(hi_void);
hi_void dvfs_enable_temperature_ctrl(hi_bool enable);
hi_void dvfs_get_temp_ctrl_threshold(dvfs_temperature_ctrl_threshold *threshold);
hi_void dvfs_set_temp_ctrl_threshold(dvfs_temperature_status status, hi_u32 threshold);
hi_s32 dvfs_temperature_ctrl_init(hi_void);
hi_void dvfs_temperature_ctrl_deinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_DVFS_TEMPERATURE_H__ */
