/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel vbo module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __DRV_PANEL_VBO_H__
#define __DRV_PANEL_VBO_H__


#include "drv_panel_define.h"
#include "hal_panel_comm.h"
#include "hi_drv_panel.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    /* < 0: lane0~3 output left half screen data?��?1: lane4~7 output right half screen data */
    hi_bool group_swap;
    hi_bool left_internal_swap;              /* < lane0~3 internal swap */
    hi_bool right_internal_swap;             /* < lane4~7 internal swap */
    hi_bool bit0_at_high_bit;                 /* < if big endian or little endian, default false, little endian */
    panel_vbo_data_mode data_mode; /* < vbone date mode */
    hi_drv_panel_byte_num vbo_byte_num; /* < vbone byte per pixel */
    panel_current_type vbo_current;    /* < vbone swing */
    panel_emp_type vbo_emphasis;       /* < vbone pre-emphasis */
    hi_u32 vbo_spread_ratio;               /* < vb1 spread ratio from 0 to 31 */
    panel_vbo_ssfreq vbo_spread_freq;
} hi_drv_panel_vbo_attr;

hi_s32  drv_panel_check_vbo_phy(hi_drv_panel_intf_attr *intf_attr_p);

hi_s32  drv_panel_check_vbo_attr(hi_drv_panel_vbo_attr *vbo_attr_p);

hi_void drv_panel_update_vbo_attr(hi_drv_panel_vbo_attr *vbo_attr_p,
                                  hi_drv_panel_intf_attr *intf_attr_p);

hi_void drv_panel_update_complete_vbo_attr(hi_drv_panel_info *base_info,
                                           panel_vbo_comp_attr *vbo_comp_attr_p);

hi_void drv_panel_set_vbo_cfg(panel_vbo_comp_attr *vb1_attr_p, drv_panel_combo_attr *combo_attr_p);

hi_s32  drv_panel_set_vbo_power(hi_bool enable);

hi_s32  drv_panel_get_vbo_power(hi_bool *enable);

hi_s32  drv_panel_set_vbo_atrr(hi_drv_panel_intf_attr *old_attr_p, hi_drv_panel_intf_attr *new_attr_p);

hi_void drv_panel_vbo_phy_init(hi_drv_panel_info *panel_info_p);

hi_void drv_panel_set_vbo_phy_clk(hal_combo_phy_clkcfg *phy_clk_cfg_p, drv_panel_contex *panel_ctx_p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif


