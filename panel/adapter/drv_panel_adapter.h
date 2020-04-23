/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel adapter module
* Author: sdk
* Create: 2019-04-03
*/
#ifndef __DRV_PANEL_ADAPTER_H__
#define __DRV_PANEL_ADAPTER_H__

#include "hi_type.h"
#include "drv_panel_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_panel_set_gpio_output_vol(hi_u32 gpio_num, hi_bool high_voltage);

hi_s32 drv_panel_custom_send_panel_i2c(hi_u32 panel_index, panel_advance_info *advance_info);

hi_s32 drv_panel_custom_pull_gpio(hi_u32 panel_index);

hi_s32 drv_panel_get_time_stamp_ms(hi_u32 *time_ms);

hi_s32 drv_panel_get_gpio_output_vol(hi_u32 gpio_num, hi_u32 *voltage);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

