/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel combophy module
* Author: sdk
* Create: 2019-11-21
*/

#ifndef __DRV_PANEL_COMBOPHY_H__
#define __DRV_PANEL_COMBOPHY_H__

#include "hi_drv_panel.h"
#include "drv_panel_define.h"
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void drv_panel_set_combo_phy_reset(hi_bool phy_reset);

hi_s32 drv_panel_check_combo_phy_pll_lock(hi_void);

hi_void drv_panel_combo_phy_digit_cfg(hi_drv_panel_info *panel_info);

hi_void drv_panel_combo_phy_clk_cfg(drv_panel_contex *panel_ctx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
