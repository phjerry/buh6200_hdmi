/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Definition of video functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-3
 */
#ifndef __HAL_HDMIRX_VIDEO_H__
#define __HAL_HDMIRX_VIDEO_H__

#include "hal_hdmirx_comm.h"

hi_u32 hal_video_get_pixel_repetition(hi_drv_hdmirx_port port);
hi_u32 hal_video_get_htotal(hi_drv_hdmirx_port port, hi_bool from_dsc);
hi_u32 hal_video_get_vtotal(hi_drv_hdmirx_port port, hi_bool from_dsc);
hi_u32 hal_video_get_vtotal4_odd(hi_drv_hdmirx_port port, hi_bool from_dsc);
hi_u32 hal_video_get_vactive(hi_drv_hdmirx_port port, hi_bool from_dsc);
hi_u32 hal_video_get_hactive(hi_drv_hdmirx_port port, hi_bool from_dsc);
hi_u32 hal_video_get_pix_clk(hi_drv_hdmirx_port port);
hi_bool hal_video_get_interlance(hi_drv_hdmirx_port port);
hi_bool hal_video_get_hpol(hi_drv_hdmirx_port port);
hi_bool hal_video_get_vpol(hi_drv_hdmirx_port port);
hi_void hal_video_set_hpol(hi_drv_hdmirx_port port, hi_bool hpol);
hi_void hal_video_set_vpol(hi_drv_hdmirx_port port, hi_bool vpol);
hi_void hal_video_clr_pol(hi_drv_hdmirx_port port, hi_bool vpol);
hdmirx_input_width hal_video_get_deep_color(hi_drv_hdmirx_port port);
hi_void hal_video_set_channel_map(hi_drv_hdmirx_port port, hdmirx_color_space color_space);
hi_bool hal_video_get_hdmi_mode(hi_drv_hdmirx_port port);
hi_void hal_video_set_mute_value(hi_drv_hdmirx_port port, hdmirx_color_space color_space);
hi_void hal_video_set_crc_enable(hi_drv_hdmirx_port port, hi_bool enable);
hi_u32 hal_video_get_crc_y_value(hi_drv_hdmirx_port port, hi_u32 *value, hi_u32 len);
hi_u32 hal_video_get_crc_cb_value(hi_drv_hdmirx_port port, hi_u32 *value, hi_u32 len);
hi_u32 hal_video_get_crc_cr_value(hi_drv_hdmirx_port port, hi_u32 *value, hi_u32 len);
hi_void hal_video_set_mute_type_en(hi_drv_hdmirx_port port, hdmirx_mute_type type, hi_bool en);
hi_void hal_video_set_mute_mode_en(hi_drv_hdmirx_port port, hdmirx_mute_mode mode);
hi_void hal_video_clr_frame_count(hi_drv_hdmirx_port port);
hi_u32 hal_video_get_frame_count(hi_drv_hdmirx_port port);
hi_u32 hal_video_get_frame_rate(hi_drv_hdmirx_port port);

#endif /* __HAL_HDMIRX_VIDEO_H__ */