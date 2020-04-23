/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Implementation of phy functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/3
 */
#ifndef __HAL_HDMIRX_PHY_H__
#define __HAL_HDMIRX_PHY_H__

#include "hal_hdmirx_comm.h"

hi_void hal_phy_init(hi_drv_hdmirx_port port);
hi_void hal_phy_set_power(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_set_term_mode(hi_drv_hdmirx_port port, hdmirx_term_sel mode);
hi_u32 hal_dphy_get_ced_err_cnt(hi_drv_hdmirx_port port, hi_u8 channel);
hi_void hal_dphy_clear_ced_erro_cnt(hi_drv_hdmirx_port port);
hi_void hal_damix_set_handle_eq_code_single(hi_drv_hdmirx_port port, hi_u8 u8_ch_sel, hi_u8 eq_code);
hi_void hal_damix_set_handle_eq_code(hi_drv_hdmirx_port port, hi_u8 eq_code);
hi_void hal_damix_get_eq_result(hi_drv_hdmirx_port port, hi_u32 *eq_result, hi_u32 len);
hi_bool hal_damix_get_eq_mode(hi_drv_hdmirx_port port);
hi_void hal_damix_set_handle_def_code_single(hi_drv_hdmirx_port port, hi_u8 u8_ch_sel, hi_u8 dfe_code);
hi_void hal_damix_set_handle_def_code(hi_drv_hdmirx_port port, hi_u8 dfe_code);
hi_void hal_damix_clr_handle_eq_code(hi_drv_hdmirx_port port);
hi_void hal_damix_get_handle_auto_def_result(hi_drv_hdmirx_port port, hi_u32 *dfe_result, hi_u32 len);
hi_bool hal_damix_get_def_mode(hi_drv_hdmirx_port port);
hi_void hal_damix_set_auto_def_en(hi_drv_hdmirx_port port, hi_bool auto_dfe_en, hdmirx_input_type type);
hdmirx_damix_status hal_damix_get_status(hi_drv_hdmirx_port port, hdmirx_input_type input_type,
                                         hdmirx_damix_type damix_type);
hi_u32 hal_phy_get_cfg(hi_drv_hdmirx_port port, hdmirx_damix_cfg cfgtype, int *cfg, int maxlen);
hi_void hal_phy_set_cfg(hi_drv_hdmirx_port port, hdmirx_damix_cfg cfgtype, int *cfg, int len);
hi_s32 hal_dphy_get_status(hi_drv_hdmirx_port port, hdmirx_dphy_type type);
hi_s32 hal_dphy_get_ltp_status(hi_drv_hdmirx_port port);
hi_bool hal_dphy_get_inter_align(hi_drv_hdmirx_port port);
hi_void hal_dphy_set_in_fifo_rst(hi_drv_hdmirx_port port);
hi_bool hal_dphy_get_scramber_status(hi_drv_hdmirx_port port);
hi_void hal_damix_pre_set(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_bool hal_damix_need_autoeq_mode(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_fix_eq_init(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_set_fix_eq_en(hi_drv_hdmirx_port port, hdmirx_input_type type, hi_bool en);
hi_bool hal_damix_get_fix_eq_result(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_set_force_eq(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_auto_eq_init(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_set_auto_eq_en(hi_drv_hdmirx_port port, hdmirx_input_type type, hi_bool en);
hi_bool hal_damix_get_auto_eq_result(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_bool hal_damix_need_dfe(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_dfe_init(hi_drv_hdmirx_port port, hdmirx_input_type type);
hi_void hal_damix_set_dfe_mode_en(hi_drv_hdmirx_port port, hdmirx_dfe_mode mode, hi_bool en);
hi_bool hal_damix_get_dfe_result(hi_drv_hdmirx_port port);

#endif /* __HAL_HDMIRX_PHY_H__ */
