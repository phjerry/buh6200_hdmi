/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Definition of controller functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/3
 */
#ifndef __HAL_HDMIRX_CTRL_H__
#define __HAL_HDMIRX_CTRL_H__

#include "hal_hdmirx_comm.h"

hi_void hal_ctrl_init(hi_drv_hdmirx_port port);
hi_void hal_ctrl_set_pwd_iso_en(hi_drv_hdmirx_port port, hi_bool en);
hi_void hal_ctrl_set_hpd_det_mode(hi_drv_hdmirx_port port, hi_u32 mode);
hi_void hal_ctrl_set_hpd_low_thresh(hi_drv_hdmirx_port port, hi_u32 thresh);
hi_bool hal_ctrl_get_5v_status(hi_drv_hdmirx_port port);
hi_void hal_ctrl_set_hpd_pol(hi_drv_hdmirx_port port, hi_bool invert);
hi_void hal_ctrl_set_hpd_level(hi_drv_hdmirx_port port, hi_bool high);
hi_void hal_ctrl_set_video_intr_en(hi_drv_hdmirx_port port, hdmirx_ctrl_video_event_type type, hi_bool en);
hi_u32 hal_ctrl_get_pwd_interrupt(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len);
hi_u32 hal_ctrl_get_pwd_intr_status(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_type type,
    hi_u32 *intr, hi_u32 max_len);
hi_void hal_ctrl_clear_pwd_interrupt(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_type type,
    hi_u32 *intr, hi_u32 len);
hi_u32 hal_ctrl_get_aon_interrupt(hi_drv_hdmirx_port port, hi_u32 *intr, hi_u32 max_len);
hi_u32 hal_ctrl_get_aon_intr_status(hi_drv_hdmirx_port port, hdmirx_ctrl_aon_intr_type type,
    hi_u32 *intr, hi_u32 max_len);
hi_void hal_ctrl_clear_aon_interrupt(hi_drv_hdmirx_port port, hdmirx_ctrl_aon_intr_type type,
    hi_u32 *intr, hi_u32 len);
hi_void hal_ctrl_clear_interrupt(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_intr_idx type);
hi_void hal_ctrl_clear_t4_error(hi_drv_hdmirx_port port);
hi_bool hal_ctrl_get_ckdt(hi_drv_hdmirx_port port);
hi_bool hal_ctrl_get_scdt(hi_drv_hdmirx_port port);
hi_void hal_ctrl_set_aon_reset(hi_drv_hdmirx_port port, hdmirx_ctrl_aon_reset_type type);
hi_void hal_ctrl_set_aon_reset_en(hi_drv_hdmirx_port port, hdmirx_ctrl_aon_reset_type type, hi_bool en);
hi_void hal_ctrl_set_pwd_reset(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_reset_type type);
hi_void hal_ctrl_set_pwd_reset_en(hi_drv_hdmirx_port port, hdmirx_ctrl_pwd_reset_type type,  hi_bool en);
hi_void hal_ctrl_set_auto_reset_en(hi_drv_hdmirx_port port, hdmirx_ctrl_auto_reset_type auto_rst, hi_bool en);
hi_void hal_ctrl_set_ddc_slave_en(hi_drv_hdmirx_port port, hi_bool en);
hi_void hal_ctrl_set_ddc_scdc_en(hi_drv_hdmirx_port port, hi_bool en);
hi_void hal_ctrl_set_ddc_hdcp_en(hi_drv_hdmirx_port port, hi_bool en);
hi_void hal_ctrl_set_ddc_edid_en(hi_drv_hdmirx_port port, hi_bool en);
hi_bool hal_ctrl_get_tmds_ratio(hi_drv_hdmirx_port port);
hi_bool hal_ctrl_get_scrbl_en(hi_drv_hdmirx_port port);
hi_void hal_ctrl_overwrite_scramble(hi_drv_hdmirx_port port, hi_bool scdc_en);
hi_void hal_ctrl_overwrite_hdmi2_mode(hi_drv_hdmirx_port port, hi_bool hdmi_mode);
hi_void hal_ctrl_load_edid(hi_drv_hdmirx_port port, hi_drv_hdmirx_edid *edid);
hi_u32 hal_ctrl_get_packet_content(hi_drv_hdmirx_port port, hdmirx_ctrl_packet_type type,
    hi_u8 *data, hi_u32 max_len);
hi_bool hal_ctrl_check_avi_color_chg(hi_drv_hdmirx_port port, hdmirx_color_space color_space);
hi_u32 hal_ctrl_get_packet_type(hi_drv_hdmirx_port port, hi_u32 addr);
hi_void hal_ctrl_set_mute_en(hi_drv_hdmirx_port port, hdmirx_ctrl_mute_type type, hi_bool en);
hi_bool hal_ctrl_get_mute_en(hi_drv_hdmirx_port port, hdmirx_ctrl_mute_type type);
hi_void hal_ctrl_set_pwd_hw_mute(hi_drv_hdmirx_port port, hdmirx_ctrl_hw_mute_type type,
    hdmirx_ctrl_event_type event_type, hi_bool en);
hi_void hal_ctrl_set_sys_mute(hi_drv_hdmirx_port port, hi_bool en);
hi_bool hal_ctrl_is_pwd_clk_stable(hi_drv_hdmirx_port port, hdmirx_ctrl_clock_type type);
hi_u32 hal_ctrl_get_pwd_clock(hi_drv_hdmirx_port port, hdmirx_ctrl_clock_type type);
hi_bool hal_ctrl_get_av_mute(hi_drv_hdmirx_port port);
hi_bool hal_ctrl_get_dsc_en(hi_drv_hdmirx_port port);
hi_void hal_ctrl_set_pp_phase(hi_drv_hdmirx_port port, hi_u32 value);
hi_void hal_ctrl_set_emp_en(hi_drv_hdmirx_port port, hdmirx_ctrl_emp_type type);
hi_void hal_ctrl_set_emp_test_en(hi_drv_hdmirx_port port, hdmirx_ctrl_emp_type type);
hi_void hal_ctrl_get_dsc_emp(hi_drv_hdmirx_port port, hi_u8 *emp_data, hi_u32 len);
hi_void hal_ctrl_get_vrr_fva_emp(hi_drv_hdmirx_port port, hi_u8 *emp_data, hi_u32 len);
hi_void hal_ctrl_set_fapa_end(hi_drv_hdmirx_port port, hi_u32 vic);

#endif /* __HAL_HDMIRX_CTRL_H__ */
