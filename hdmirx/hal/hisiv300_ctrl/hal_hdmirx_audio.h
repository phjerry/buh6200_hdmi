/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Definition of audio functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/3
 */
#ifndef __HAL_HDMIRX_AUDIO_H__
#define __HAL_HDMIRX_AUDIO_H__

#include "hal_hdmirx_comm.h"

hi_void hal_audio_clear_audio_int(hi_drv_hdmirx_port port);
hi_void hal_audio_set_res_change_aec_en(hi_drv_hdmirx_port port, hi_bool en);
hi_u32 hal_audio_get_audp_state(hi_drv_hdmirx_port port);
hi_u32 hal_audio_get_cts(hi_drv_hdmirx_port port);
hi_u32 hal_audio_get_n(hi_drv_hdmirx_port port);
hi_u32 hal_audio_get_hw_sample_rate(hi_drv_hdmirx_port port);
hi_u32 hal_audio_get_channel_state(hi_drv_hdmirx_port port, hi_u32 *data, hi_u32 len);
hi_u32 hal_audio_get_layout(hi_drv_hdmirx_port port);
hi_u32 hal_audio_get_intr(hi_drv_hdmirx_port port);
hi_void hal_audio_set_intr(hi_drv_hdmirx_port port, hi_u32 value);
hi_bool hal_audio_get_hbra_intr(hi_drv_hdmirx_port port);
hi_void hal_audio_clear_hbra_intr(hi_drv_hdmirx_port port);
hi_bool hal_audio_is_audio_packet_got(hi_drv_hdmirx_port port);
hi_void hal_audio_clear_audio_packet(hi_drv_hdmirx_port port);
hi_bool hal_audio_is_cts_got(hi_drv_hdmirx_port port);
hi_bool hal_audio_is_cts_stable(hi_drv_hdmirx_port port);
hi_bool hal_audio_is_fifo_stable(hi_drv_hdmirx_port port);
hi_void hal_audio_set_aac_mute_en(hi_drv_hdmirx_port port, hi_bool en);
hi_void hal_audio_set_aac_unmute(hi_drv_hdmirx_port port);
hi_bool hal_audio_get_aac_mute_en(hi_drv_hdmirx_port port);
hi_void hal_audio_set_mclk(hi_drv_hdmirx_port port, hi_u32 code);
hi_void hal_audio_set_sw_n(hi_drv_hdmirx_port port, hi_u32 value);
hi_void hal_audio_set_sw_cts(hi_drv_hdmirx_port port, hi_u32 value);
hi_void hal_audio_clr_sw_acr(hi_drv_hdmirx_port port);
hi_void hal_audio_set_i2s_out_cfg(hi_drv_hdmirx_port port, hi_u32 cfg);
hi_void hal_audio_set_i2s_out_dft(hi_drv_hdmirx_port port);
hi_void hal_audio_set_acr_start(hi_drv_hdmirx_port port);
hi_u32 hal_audio_get_audio_fifo_diff(hi_drv_hdmirx_port port);
hi_void hal_audio_set_sys_mute_en(hi_drv_hdmirx_port port, hi_bool en);
#endif /* __HAL_HDMIRX_AUDIO_H__ */
