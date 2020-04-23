/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Definition of hdcp functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-3
 */
#ifndef __HAL_HDMIRX_HDCP_H__
#define __HAL_HDMIRX_HDCP_H__

#include "hal_hdmirx_comm.h"

hi_void hal_hdcp_set_crc_en(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type, hi_bool en);
hi_bool hal_hdcp_get_crc_result(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type);
hi_bool hal_hdcp_get_done(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type);
hdmirx_hdcp_err_cnt hal_hdcp_get_bch_err_cnt(hi_drv_hdmirx_port port);
hi_drv_hdmirx_hdcp_type hal_hdcp_check_type(hi_drv_hdmirx_port port);
hi_u32 hal_hdcp_get_bksv(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type, hi_u32 *bksv, hi_u32 max_len);
hi_u32 hal_hdcp_get_aksv(hi_drv_hdmirx_port port, hi_u32 *aksv, hi_u32 max_len);
hi_u32 hal_hdcp_get_enc_en_cnt(hi_drv_hdmirx_port port);
hi_s32 hal_hdcp2x_load_mcu_pram(hi_drv_hdmirx_port port, hi_u32 *pram, hi_u32 max_len);
hi_void hal_hdcp2x_start_mcu(hi_drv_hdmirx_port port, hi_bool start);
hi_void hal_hdcp2x_set_reauth(hi_drv_hdmirx_port port);

#endif /* __HAL_HDMIRX_HDCP_H__ */
