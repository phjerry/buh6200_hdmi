/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HI_HAL_AIAO_H__
#define __HI_HAL_AIAO_H__

#include "hi_drv_audio.h"
#include "hal_aiao_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PERIOND_NUM (4)

/***************************** macro definition ******************************/
/* global function */
hi_s32 hal_aiao_suspend(hi_void);
hi_s32 hal_aiao_resume(hi_void);
hi_s32 hal_aiao_power_on(hi_void);
hi_void hal_aiao_power_off(hi_void);
hi_s32 hal_aiao_request_isr(hi_void);
hi_void hal_aiao_free_isr(hi_void);
hi_s32 hal_aiao_init(hi_void);
hi_void hal_aiao_de_init(hi_void);
hi_void hal_aiao_get_hw_capability(hi_u32 *capability);
hi_void hal_aiao_get_hw_version(hi_u32 *version);
hi_void hal_aiao_dbg_rw_reg(aiao_dbg_reg *reg);
hi_void hal_aiao_set_top_int(hi_u32 multibit);
hi_u32 hal_aiao_get_top_int_raw_status(hi_void);
hi_u32 hal_aiao_get_top_int_status(hi_void);

/* port function */
hi_s32 hal_aiao_open(aiao_port_id port_id, const aiao_port_user_cfg *config);

hi_void hal_aiao_close(aiao_port_id port_id);
hi_s32 hal_aiao_start(aiao_port_id port_id);
hi_s32 hal_aiao_stop(aiao_port_id port_id, aiao_port_stopmode stop_mode);
hi_s32 hal_aiao_mute(aiao_port_id port_id, hi_bool mute);
hi_s32 hal_aiao_set_volume(aiao_port_id port_id, hi_u32 volume_db);
hi_s32 hal_aiao_set_spdif_category_code(aiao_port_id port_id, aiao_spdif_categorycode category_code);
hi_s32 hal_aiao_set_spdif_scms_mode(aiao_port_id port_id, aiao_spdif_scms_mode scms_mode);
hi_s32 hal_aiao_set_track_mode(aiao_port_id port_id, aiao_track_mode track_mode);
hi_s32 hal_aiao_set_bypass(aiao_port_id port_id, hi_bool by_bass);
hi_s32 hal_aiao_set_mode(aiao_port_id port_id, hi_bool mode);
hi_s32 hal_aiao_get_user_congfig(aiao_port_id port_id, aiao_port_user_cfg *user_config);
hi_s32 hal_aiao_get_status(aiao_port_id port_id, aiao_port_stauts *proc_info);
hi_s32 hal_aiao_select_spdif_source(aiao_port_id port_id, aiao_spdifport_source src_chn_id);
hi_s32 hal_aiao_set_spdif_out_port(aiao_port_id port_id, hi_s32 en);
hi_s32 hal_aiao_set_i2s_sd_select(aiao_port_id port_id, aiao_i2s_data_sel *sd_sel);
hi_s32 hal_aiao_set_attr(aiao_port_id port_id, aiao_port_attr *attr);
hi_s32 hal_aiao_get_attr(aiao_port_id port_id, aiao_port_attr *attr);

hi_void hal_aiao_get_tx_i2s_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr);
hi_void hal_aiao_set_tx_i2s_df_attr(aiao_port_id port_id, aiao_isr_func *isr_func);
hi_void hal_aiao_get_hdmi_hbr_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr);
hi_void hal_aiao_get_hdmi_i2s_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr);
hi_void hal_aiao_get_tx_spd_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr);
hi_void hal_aiao_get_rx_adc_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr);
hi_void hal_aiao_get_rx_sif_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr);
hi_void hal_aiao_get_rx_hdmi_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr);

hi_void hal_aiao_set_spdif_port_en(aiao_port_id port_id, hi_s32 en);

/* port function for verification */
hi_s32 hal_aiao_open_veri(aiao_port_id port_id, const aiao_port_user_cfg *config);
hi_void hal_aiao_close_veri(aiao_port_id port_id);

#ifdef HI_ALSA_AI_SUPPORT
hi_u32 hal_aiao_alsa_query_read_pos(aiao_port_id port_id);
hi_u32 hal_aiao_alsa_query_write_pos(aiao_port_id port_id);
hi_u32 hal_aiao_p_alsa_flush(aiao_port_id port_id);
hi_u32 hal_aiao_alsa_update_rptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size);
hi_u32 hal_aiao_alsa_update_wptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size);
#endif
hi_void hal_aiao_proc_statistics(aiao_port_id port_id, hi_u32 int_status, hi_void *pst);

/* port buffer function */
hi_u32 hal_aiao_read_data_not_up_rptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size, hi_u32 *rptr, hi_u32 *wptr);
hi_u32 hal_aiao_read_data(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size);
hi_u32 hal_aiao_write_data(aiao_port_id port_id, hi_u8 *src, hi_u32 src_len);
hi_u32 hal_aiao_prepare_data(aiao_port_id port_id, hi_u8 *src, hi_u32 src_len);
hi_u32 hal_aiao_query_buf_data_provide_rptr(aiao_port_id port_id, hi_u32 *rptr);
hi_u32 hal_aiao_query_buf_data(aiao_port_id port_id);
hi_u32 hal_aiao_query_buf_free(aiao_port_id port_id);
hi_u32 hal_aiao_update_rptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size);
hi_u32 hal_aiao_update_wptr(aiao_port_id port_id, hi_u8 *src, hi_u32 src_len);
hi_void hal_aiao_get_delay_ms(aiao_port_id port_id, hi_u32 *delayms);
hi_s32 hal_aiao_get_rbf_attr(aiao_port_id port_id, aiao_rbuf_attr *rbf_attr);
hi_s32 hal_aiao_set_op_type(aiao_port_id port_id, aiao_op_type op_type);

#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
hi_void hal_aiao_get_board_tx_i2s_df_attr(hi_u32 board_i2s_num,
    hi_i2s_attr *i2s_attr, aiao_port_id *port_id, aiao_port_user_cfg *attr);
hi_void hal_aiao_get_board_rx_i2s_df_attr(hi_u32 board_i2s_num, aiao_port_id *port_id, aiao_port_user_cfg *attr);
hi_s32 hal_aiao_check_board_i2s_open_attr(aiao_port_id port_id, const aiao_if_attr *new_if_attr);
hi_void hal_aiao_set_board_i2s_open_attr(aiao_port_id port_id, const aiao_if_attr *new_if_attr);
hi_void hal_aiao_destroy_board_i2s_open_attr(aiao_port_id port_id);
#endif

#ifdef HI_AIAO_TIMER_SUPPORT
hi_s32 hal_aiao_t_create(aiao_timer_id timer_id, const aiao_timer_create *param);
hi_void hal_aiao_t_destroy(aiao_timer_id timer_id);
hi_s32 hal_aiao_t_set_timer_attr(aiao_timer_id timer_id, const aiao_timer_attr *attr_param);
hi_s32 hal_aiao_t_set_timer_enable(aiao_timer_id timer_id, hi_bool enable);
hi_s32 hal_aiao_t_get_status(aiao_timer_id timer_id, aiao_timer_status *param);
hi_void hal_aiao_t_timer_process(aiao_timer_id timer_id, hi_void *pst);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HI_HAL_AIAO_H__ */
