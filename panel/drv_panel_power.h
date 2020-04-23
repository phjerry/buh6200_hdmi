/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel power module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __DRV_PANEL_POWER_H__
#define __DRV_PANEL_POWER_H__

#include "drv_panel_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_panel_set_panel_tcon_power(hi_bool power_on);

hi_s32 drv_panel_set_panel_bl_power(hi_bool ldm_scene, hi_bool power_on);

hi_s32 drv_panel_get_panel_tcon_power(hi_bool *power_on);

hi_s32 drv_panel_get_panel_bl_power(hi_bool *power_on);

hi_s32 drv_panel_set_panel_power(drv_panel_image *panel_image_p, hi_bool power_on);

hi_s32 drv_panel_get_panel_power_on(hi_drv_panel_intf_type intf_type, hi_bool *poweron_p);

hi_s32 drv_panel_get_power_on_recovery(hi_drv_panel_intf_type intf_type, hi_bool *poweron_p);

hi_s32 drv_panel_set_intf_power(hi_drv_panel_intf_type intf_type, hi_bool power_on);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

