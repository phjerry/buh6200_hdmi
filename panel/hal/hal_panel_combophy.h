/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel vbo module
* Author: sdk
* Create: 2019-11-21
*/

#ifndef __HAL_PANEL_COMBO_PHY_H__
#define __HAL_PANEL_COMBO_PHY_H__

#include "hi_type.h"
#include "hal_panel_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void hal_panel_set_combo_dphy_reset(hi_bool phy_srst);
hi_void hal_panel_set_aphy_intf_power_mode(hal_panel_aphy_power_mode aphy_mode);
hi_void hal_panel_set_combo_aphy_clock_port(hi_u32 clock_port);
hi_void hal_panel_set_combo_aphy_clk_mode(hal_panel_intftype intf_type);

hi_void hal_panel_set_combo_aphy_over_sample(hal_panel_phy_over_sample over_sample);
hi_void hal_panel_set_combo_phy_atop(hal_panel_intftype intf_type);
hi_void hal_panel_set_combo_phy_txpll_div_fb(hal_panel_phy_div_feedback aphy_div_fb);
hi_void hal_panel_set_combo_phy_txpll_div_in(hal_panel_phy_div_in aphy_div_in);
hi_void hal_panel_set_combo_phy_txpll_icp_current(hal_panel_phy_icp_current aphy_icp_current);
hi_void hal_panel_set_combo_phy_current(hal_panel_intftype intf_type, const hi_u8 *drv_current_p);
hi_void hal_panel_set_combo_phy_pre_emphasis(hal_panel_intftype intf_type, const hi_u8 *drv_current_p,
                                             const hi_u8 *pre_emphasis_p);
hi_void hal_panel_set_current_control_signal(hal_panel_phy_predriver phy_pre_driver);
hi_void hal_panel_set_pre_emp_control_signal(hal_panel_phy_preemp_predriver pre_emp_driver);
hi_void hal_panel_set_dphy_port_pn_swap(hi_u32 port_pn_swap);
hi_void hal_panel_set_dphy_port_sort(hal_panel_sorttype sort_type, hi_u32 first_group_line,
                                     hi_u32 second_group_line, hi_u32 third_group_line, hi_u32 fourth_group_line);
hi_void hal_panel_set_combo_dphy_over_sample(hal_panel_phy_over_sample over_sample);
hi_void hal_panel_set_combo_dphy_intf_type(hal_panel_intftype intf_type);
hi_void hal_panel_set_dphy_bit_width(hal_panel_dphy_bitwidth dphy_bit_width);
hi_void hal_panel_set_combo_dphy_clk_cfg(hal_panel_intftype intf_type);
hi_void hal_panel_set_combo_aphy_enable(hi_bool power_on);
hi_void hal_panel_set_combo_aphy_pll(hi_void);
hi_s32 hal_panel_check_combo_phy_pll_lock(hi_void);
hi_void hal_panel_set_aphy_pre_emp_power_on(hi_bool pre_emphasis_on);
hi_void hal_panel_set_aphy_lane_power_on(hi_bool lane_power_on, hi_u32 lane_used);

hi_void hal_panel_set_aphy_power_down_31_to_0(hi_u32 value);
hi_void hal_panel_set_aphy_power_down_63_to_32(hi_u32 value);

hi_void hal_panel_set_ioshare(hi_void);
hi_void hal_panel_set_lockn(hi_void);
hi_void hal_panel_set_htpdn(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

