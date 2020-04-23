/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel combophy module
* Author: sdk
* Create: 2019-04-03
*/

#include "drv_panel_vbo.h"
#include "drv_panel_combophy.h"
#include "hal_panel_combophy.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_COMBOPHY_CLOCK_PORT_ONE 0x01
#define HI_COMBOPHY_CLOCK_PORT_TWO 0x02

hi_void drv_panel_set_combo_phy_reset(hi_bool phy_reset)
{
    hal_panel_set_combo_dphy_reset(phy_reset);
}

hi_s32 drv_panel_check_combo_phy_pll_lock(hi_void)
{
    return hal_panel_check_combo_phy_pll_lock();
}

hi_void drv_panel_combo_phy_digit_cfg(hi_drv_panel_info *panel_info)
{
    hal_panel_intftype panel_intf_type = (hal_panel_intftype)panel_info->intf_type;

    hal_panel_set_aphy_power_down_31_to_0(0);
    hal_panel_set_aphy_power_down_63_to_32(0);
    hal_panel_set_combo_aphy_enable(HI_TRUE);
    hal_panel_set_combo_aphy_clk_mode(panel_intf_type);

#ifdef PANEL_TMP
    hal_panel_set_combo_dphy_intf_type(panel_intf_type);
    hal_panel_set_combo_dphy_clk_cfg(panel_intf_type);
    hal_panel_set_current_control_signal(HAL_PANEL_PHY_PREDRIVER_2POINT5MA);
    hal_panel_set_pre_emp_control_signal(HAL_PANEL_PHY_PREEMP_PREDRIVER_1POINT25MA);
    hal_panel_set_combo_phy_atop(panel_intf_type);
#endif

    switch (panel_intf_type) {
        case HAL_PANEL_INTFTYPE_LVDS:
            break;
        case HAL_PANEL_INTFTYPE_VBONE:
#ifdef PANEL_TMP
            drv_panel_vbo_phy_init(panel_info);
            hal_panel_set_combo_aphy_clock_port(VBO_PHY_CLK_CHANNEL);
#endif
            break;
        case HAL_PANEL_INTFTYPE_MINILVDS:
            break;
        case HAL_PANEL_INTFTYPE_EPI:
            break;
        case HAL_PANEL_INTFTYPE_ISP:
        case HAL_PANEL_INTFTYPE_CSPI:
            break;
        case HAL_PANEL_INTFTYPE_CEDS:
            break;
        case HAL_PANEL_INTFTYPE_CHPI:
            break;
        case HAL_PANEL_INTFTYPE_USIT:
            break;
        case HAL_PANEL_INTFTYPE_CMPI:
            break;
        case HAL_PANEL_INTFTYPE_MIPI:
        default:
            break;
    }
}

hi_void drv_panel_combo_phy_clk_cfg(drv_panel_contex *panel_ctx)
{
#ifdef PANEL_TMP
    hi_drv_panel_intf_type panel_intf_type = panel_ctx->image_info.base_info.intf_type;
    hal_combo_phy_clkcfg phy_clk_cfg = { 0 };

    switch (panel_intf_type) {
        case HAL_PANEL_INTFTYPE_LVDS:
            break;
        case HAL_PANEL_INTFTYPE_VBONE:
            drv_panel_set_vbo_phy_clk(&phy_clk_cfg, panel_ctx);
            break;
        case HAL_PANEL_INTFTYPE_MINILVDS:
            break;
        case HAL_PANEL_INTFTYPE_EPI:
        case HAL_PANEL_INTFTYPE_ISP:
        case HAL_PANEL_INTFTYPE_CEDS:
        case HAL_PANEL_INTFTYPE_CSPI:
        case HAL_PANEL_INTFTYPE_CHPI:
        case HAL_PANEL_INTFTYPE_USIT:
            break;
        default:
            break;
    }

    hal_panel_set_combo_aphy_pll();
    hal_panel_set_combo_dphy_over_sample(phy_clk_cfg.dphy_over_sample);
    hal_panel_set_combo_aphy_over_sample(phy_clk_cfg.aphy_over_sample);
    hal_panel_set_combo_phy_txpll_div_in(phy_clk_cfg.aphy_div_in);
    hal_panel_set_combo_phy_txpll_div_fb(phy_clk_cfg.aphy_div_fb);
    hal_panel_set_combo_phy_txpll_icp_current(phy_clk_cfg.aphy_icp_current);
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
