/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HI_AIAO_FUNC_H__
#define __HI_AIAO_FUNC_H__

#include "hal_aiao_common.h"
#include "hal_aiao_priv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* global function */
hi_s32 i_hal_aiao_init(hi_void);
hi_void i_hal_aiao_de_init(hi_void);
hi_void i_hal_aiao_get_hw_capability(hi_u32 *capability);
hi_void i_hal_aiao_get_hw_version(hi_u32 *version);
hi_void i_hal_aiao_dbg_rw_reg(aiao_dbg_reg *reg);
hi_void i_hal_aiao_set_top_int(hi_u32 multibit);
hi_u32 i_hal_aiao_get_top_int_raw_status(hi_void);
hi_u32 i_hal_aiao_get_top_int_status(hi_void);

/* description: AIAO TX/RX port DSP control HAL API */
hi_void i_hal_aiao_set_int(aiao_port_id port_id, hi_u32 multibit);
hi_void i_hal_aiao_clr_int(aiao_port_id port_id, hi_u32 multibit);
hi_u32 i_hal_aiao_get_int_status_raw(aiao_port_id port_id);
hi_u32 i_hal_aiao_get_int_status(aiao_port_id port_id);

/* global port function */
hi_s32 i_hal_aiao_open(const aiao_port_id port_id,
                       const aiao_port_user_cfg *config,
                       aiao_port **p_port,
                       aiao_isr_func **isr);

hi_void i_hal_aiao_close(aiao_port *port);
hi_s32 i_hal_aiao_start(aiao_port *port);
hi_s32 i_hal_aiao_stop(aiao_port *port, aiao_port_stopmode stop_mode);
hi_s32 i_hal_aiao_mute(aiao_port *port, hi_bool mute);
hi_s32 i_hal_aiao_mute_fade(aiao_port *port, hi_bool mute_fade);
hi_s32 i_hal_aiao_set_volume(aiao_port *port, hi_u32 volume_db);
hi_s32 i_hal_aiao_set_spdif_category_code(aiao_port *port, aiao_spdif_categorycode category_code);
hi_s32 i_hal_aiao_set_spdif_scms_mode(aiao_port *port, aiao_spdif_scms_mode scms_mode);
hi_s32 i_hal_aiao_set_track_mode(aiao_port *port, aiao_track_mode track_mode);
hi_s32 i_hal_aiao_set_bypass(aiao_port *port, hi_bool by_bass);
hi_s32 i_hal_aiao_set_mode(aiao_port *port, hi_bool mode);
hi_s32 i_hal_aiao_get_user_congfig(aiao_port *port, aiao_port_user_cfg *user_config);
hi_s32 i_hal_aiao_get_status(aiao_port *port, aiao_port_stauts *proc_info);
hi_s32 i_hal_aiao_select_spdif_source(aiao_port *port, aiao_spdifport_source src_chn_id);
hi_s32 i_hal_aiao_set_spdif_out_port(aiao_port *port, hi_s32 en);
hi_s32 i_hal_aiao_set_i2s_sd_select(aiao_port *port, aiao_i2s_data_sel *sd_sel);
hi_s32 i_hal_aiao_set_attr(aiao_port *port, aiao_port_attr *attr);
hi_s32 i_hal_aiao_get_attr(aiao_port *port, aiao_port_attr *attr);
hi_void i_hal_aiao_proc_statistics(aiao_port *port, hi_u32 int_status);
hi_s32 i_hal_aiao_set_i2s_master_clk(aiao_port_id port_id, aiao_if_attr *if_attr);
hi_s32 i_hal_aiao_set_i2_slave_clk(aiao_port_id port_id, aiao_if_attr *if_attr);
hi_void i_hal_aiao_set_spdif_port_en(aiao_port_id port_id, hi_s32 en);

/* port buffer function */
hi_u32 i_hal_aiao_read_data_not_up_rptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size, hi_u32 *rptr, hi_u32 *wptr);
hi_u32 i_hal_aiao_read_data(aiao_port *port, hi_u8 *dest, hi_u32 dest_size);
hi_u32 i_hal_aiao_write_data(aiao_port *port, hi_u8 *src, hi_u32 src_len);
hi_u32 i_hal_aiao_prepare_data(aiao_port *port, hi_u8 *src, hi_u32 src_len);
hi_u32 i_hal_aiao_query_buf_data_provide_rptr(aiao_port *port, hi_u32 *rptr);
hi_u32 i_hal_aiao_query_buf_data(aiao_port *port);
hi_u32 i_hal_aiao_query_buf_free(aiao_port *port);
hi_u32 i_hal_aiao_update_rptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size);
hi_u32 i_hal_aiao_update_wptr(aiao_port *port, hi_u8 *src, hi_u32 src_len);
hi_s32 i_hal_aiao_get_rbf_attr(aiao_port *port, aiao_rbuf_attr *rbf_attr);
hi_void i_hal_aiao_get_delay_ms(aiao_port *port, hi_u32 *delayms);

#ifdef HI_ALSA_AI_SUPPORT
hi_u32 i_hal_aiao_alsa_update_rptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size);
hi_u32 i_hal_aiao_alsa_query_write_pos(aiao_port *port);
hi_u32 i_hal_aiao_alsa_query_read_pos(aiao_port *port);

hi_u32 i_hal_aiao_alsa_update_wptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size);
hi_u32 i_hal_aiao_alsa_flash(aiao_port *port);
#endif

hi_s32 i_hal_aiao_set_op_type(aiao_port_id port_id, aiao_op_type op_type);

#ifdef HI_AIAO_TIMER_SUPPORT
hi_s32 i_hal_aiao_t_create(aiao_timer_id timer_id, const aiao_timer_create *param,
    aiao_timer *ppst_port, aiao_timer_isr_func **isr);
hi_void i_hal_aiao_t_destroy(aiao_timer port);
hi_s32 i_hal_aiao_t_set_timer_attr(aiao_timer port, const aiao_timer_attr *attr);
hi_s32 i_hal_aiao_t_set_timer_enable(aiao_timer port, hi_bool enable);
hi_s32 i_hal_aiao_t_get_status(aiao_timer port, aiao_timer_status *status);
hi_void i_hal_aiao_t_timer_process(aiao_timer port);
hi_void i_hal_timer_clear_timer(aiao_timer_id timer_id);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HI_AIAO_FUNC_H__ */
