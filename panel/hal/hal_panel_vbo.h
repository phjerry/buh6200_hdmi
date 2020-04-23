/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal vbone module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __HAL_PANEL_VBO_H__
#define __HAL_PANEL_VBO_H__

#include "hi_type.h"
#include "hi_drv_panel.h"
#include "drv_panel_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void hal_panel_set_vbo_enable(hi_bool enable);
hi_void hal_panel_get_vbo_enable(hi_bool *enable);
hi_void hal_panel_set_vbo_data_mode(panel_vbo_data_mode data_mode);
hi_void hal_panel_set_vbo_byte_num(hi_drv_panel_byte_num byte_num);
hi_void hal_panel_set_vbo_lockn_sw_mode(hi_bool enable);
hi_void hal_panel_set_vbo_lockn_high(hi_bool bhigh);
hi_void hal_panel_set_vbo_msb_mode(hi_bool msb_mode);
hi_void hal_panel_set_vbo_lockn_sel(hi_void);
hi_void hal_panel_set_vbo_htpn_mode(hi_void);
hi_void hal_panel_set_vbo_frm_mask(hi_void);
hi_void hal_panel_set_vbo_lane_num(hi_drv_panel_link_type link_mode);
hi_void hal_panel_set_vbo_lane_swap(hi_drv_panel_division_type division_type, hi_drv_panel_link_type link_type);
hi_void hal_panel_set_vbo_channel_sel(hi_drv_panel_division_type division_type, hi_drv_panel_link_type link_type);
hi_void hal_panel_set_vbo_partiton_sel(hi_drv_panel_division_type division_type, hi_drv_panel_link_type link_type);
hi_void hal_panel_set_vbo_pn_swap(hi_bool pn_swap);
hi_void hal_panel_set_vbo_clk(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

