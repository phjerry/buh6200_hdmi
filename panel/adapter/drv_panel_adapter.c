/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel adapter module
* Author: sdk
* Create: 2019-04-03
*/

#include "drv_panel_adapter.h"
#include "hi_drv_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_panel_set_gpio_output_vol(hi_u32 gpio_num, hi_bool high_voltage)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_custom_send_panel_i2c(hi_u32 panel_index, panel_advance_info *advance_info)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_custom_pull_gpio(hi_u32 panel_index)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_time_stamp_ms(hi_u32 *time_ms)
{
    return HI_SUCCESS;
}

hi_s32 drv_panel_get_gpio_output_vol(hi_u32 gpio_num, hi_u32 *voltage)
{
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

