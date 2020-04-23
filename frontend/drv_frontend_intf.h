/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend interface
 * Author: SDK
 * Create: 2017-05-09
 */

#ifndef __DRV_FRONTEND_INTF_H__
#define __DRV_FRONTEND_INTF_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 drv_fe_intf_connect(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_close(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_status(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_signal_strength(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_ber(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_per(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_snr(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_attr(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_freq_symb_offset(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_signal_info(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_standby(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_quality(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_attr(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_monitor_layers_config(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_ts_out(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_ts_out(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_ter_attr(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_plp_num(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_plp_para(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_plp_info(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_sat_attr(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_blindscan_init(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_blindscan_action(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_blindscan_abort(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_lnb_out(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_send_continuous_22k(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_send_tone(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_diseqc_send_msg(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_diseqc_recv_msg(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_stream_num(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_get_isi_id(unsigned int cmd, void *arg, void *private_data);
hi_s32 drv_fe_intf_set_isi_id(unsigned int cmd, void *arg, void *private_data);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif
